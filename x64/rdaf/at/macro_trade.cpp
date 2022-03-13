//
// macro_trade.cpp
// run by rdaf/at/Strategy.cpp to process latest Trade
//
// Supplied by Andrew Ross, March 2022
//

#include <rdaf/TH2.h>
#include <rdaf/TTree.h>

#include <rdaf/TDirectory.h>

int macro_trade() {

  TH2F* pHisto2 {};
  //TTree* pTreeQuotes;
  TTree* pTreeTrades {};

  TList* pList = gDirectory->GetList();

  for( const auto&& obj: *pList ) {

    TClass* class_ = (TClass*) obj;

    //std::cout
    //  << "name=" << class_->GetName()
    //  << ",title=" << class_->GetTitle()
    //  << ",class" << class_->ClassName()
    //  << std::endl;

    //class_->Dump();

    //if ( 0 == strcmp( "quotes", class_->GetName() ) ) {
    //  pTreeQuotes = (TTree*)obj;
    //}

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

  pHisto2->Fill( trade.price, trade.time, trade.vol );

  return cntEntries; // diagnose cntEntries
}