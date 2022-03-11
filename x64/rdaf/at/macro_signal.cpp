
#include <rdaf/TH2.h>
#include <rdaf/TDirectory.h>

//function to run to get trading signal, 1 means go long.
int macro_signal( double time, double price ) {

  // attach to historgram created in macro_initial
  TH2F* h2 = (TH2F*)gDirectory->GetList()->FindObject( "h2" );

  //input of this function is the time, which is the current time
  //therefore we need to find a suitable range of bins along Y-axis (remember time is stored in y-axis)
  // and find the projection of h2. the result is a 1-D hist with x-axis being price
  // and value being volume

  //find the bin that the time given belongs to:
  Int_t bin_y = h2->GetYaxis()->FindBin(time);

  //if bin_y is valid and larger than 1 then proceed, else abort (since there is not enought data)
  if (bin_y < 1) return -1;

  //now find projection of h2 from the beginning till now:
  auto h2_x = h2->ProjectionX("_x", 1, bin_y);

  // now that h2_x is calculated, fit a gaussian to the it (i.e volume distribution)
  auto b = h2_x->Fit("gaus","S");

  //if fit is valid proceed, else abort
  if (!b->IsValid()) return -1;

  //finally, if price is within 1 sigma from the mean, it's a good signal and go long:
  if (abs( price - b->Parameter(1)) < b->Parameter(2)) return 1;

  return -1;

}
