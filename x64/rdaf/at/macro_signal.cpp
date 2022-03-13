
//
// macro_trade.cpp
// run by rdaf/at/Strategy.cpp once a second to generate trading signal
//  -1: invalid data
//   0: no change
//   1: enter/stay long
//
// Supplied by Andrew Ross, March 2022
//

//#include <iostream>

#include <rdaf/TH2.h>
#include <rdaf/TTree.h>

#include <rdaf/TDirectory.h>

//function to run to get trading signal, 1 means go long.
//int macro_signal( double time, double price ) {
int macro_signal() {

  TH2F* pHisto2;
  TTree* pTreeQuotes;
  TTree* pTreeTrades;

  TList* pList = gDirectory->GetList();

  for( const auto&& obj: *pList ) {

    TClass* class_ = (TClass*) obj;

    //std::cout
    //  << "name=" << class_->GetName()
    //  << ",title=" << class_->GetTitle()
    //  << ",class" << class_->ClassName()
    //  << std::endl;

    //class_->Dump();

    if ( 0 == strcmp( "quotes", class_->GetName() ) ) {
      pTreeQuotes = (TTree*)obj;
    }

    if ( 0 == strcmp( "trades", class_->GetName() ) ) {
      pTreeTrades = (TTree*)obj;
    }

    if ( 0 == strcmp( "h2", class_->GetName() ) ) {
      pHisto2 = (TH2F*)obj;
    }

  }

  // attach to historgram created in macro_initial
  if ( nullptr != pHisto2 ) {
    //std::cout << "found h2" << std::endl;
  }

  TBranch* pTrades {};

  if ( nullptr != pTreeTrades ) {
    //std::cout << "have a branch" << std::endl;
    //treeTrade->SetBranchAddress( "trade", &trade, &branchTrade );
    pTrades = pTreeTrades->GetBranch( "trade" );
  }

  struct Trade {
    double time;
    double price;
    uint64_t vol;
    int64_t direction;
  } trade;

  pTrades->SetAddress( &trade );
  int cntEntries = pTrades->GetEntries();
  pTrades->GetEntry( cntEntries - 1 );

  //std::cout << "in macro_signal: " << pTreeTrades->GetEntries() << std::endl;


  //input of this function is the time, which is the current time
  //therefore we need to find a suitable range of bins along Y-axis (remember time is stored in y-axis)
  // and find the projection of h2. the result is a 1-D hist with x-axis being price
  // and value being volume

  //find the bin that the time given belongs to:
  Int_t bin_y = pHisto2->GetYaxis()->FindBin( trade.time );

  //if bin_y is valid and larger than 1 then proceed, else abort (since there is not enought data)
  if (bin_y < 1) return -1;

  //now find projection of h2 from the beginning till now:
  auto h2_x = pHisto2->ProjectionX( "_x", 1, bin_y );

  // now that h2_x is calculated, fit a gaussian to the it (i.e volume distribution)
  auto b = h2_x->Fit( "gaus", "S" );

  //if fit is valid proceed, else abort
  if ( !b->IsValid() ) return -1;

  //finally, if price is within 1 sigma from the mean, it's a good signal and go long:
  if ( abs( trade.price - b->Parameter(1)) < b->Parameter(2) ) return 1;

  return 0;

}
