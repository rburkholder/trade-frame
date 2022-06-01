```
$ cat x64/debug/rdaf/download.cfg
exchange=NYSE
exchange=NYSE_ARCA
exchange=NYSE_AMERICAN
exchange=NASDAQ
exchange=NCM
exchange=NGM
exchange=NGSM

security_type=EQUITY
minimum_price=5.0
number_of_days=1
```

To read the string branches, something like:

```
std::string sSecurityName, sListedMarket;
TTree* pTreeStats;

sProjnameIn = "rdaf_dl.root";

tTShowerHeader->GetBranch("name")->GetEntries();//will return the # of entries
tTShowerHeader->GetBranch("name")->GetEntry(0);//return the first entry
// change the index to cycle through the indexes
// excerpt from: https://stackoverflow.com/questions/7010587/write-read-string-in-a-ttree-cern-root
```
