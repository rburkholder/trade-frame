{
  struct TreeTrade {
    double time;
    double price;
    uint64_t vol;
    int64_t direction;
  };

  TreeTrade treeTrade; // need to define this data type to retrieve data stored in trades branch i

  TFile *f = new TFile("<raw data filename>.root", "READ"); //reading the raw data file
  TTree *tree = (TTree*)f->Get("trades");//tree pointing to trades

  // setting tree to point to trade branch,
  //and tell ROOT every data point is an instance of TreeTrade
  tree->SetBranchAddress("trade", &treeTrade);

 //defining a 2-D histogram, and will populate it by the data read from trade branch:
  TH2F* h2 = new TH2F("h2", "histogram name", 100,2470,2475, 100, 1646000, 1647000);

 for (int i=0; i<tree->GetEntries(); i++)//looping over all entries in trade branch
 {
   tree->GetEntry(i);//getting next row of data and store it in treeTrade variable

   //fill h2, x-coordinate is price, y-coordinate is time and vol is the weight of the entry
   h2->Fill(treeTrade.price,treeTrade.time, treeTrade.vol);
 }
 //open a new file to write the histogram to it.
 TFile *f2 = new TFile("<first macro output>.root", "RECREATE");
 h2->Write();
 f2->Close();
 f->Close();
}
