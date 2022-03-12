
#include <rdaf/TH2.h>

int macro_initial() {

   // historgram used for iterative calculations
  TH2F* h2 = new TH2F( "h2", "histogram 2", 100, 2470, 2475, 100, 1646000, 1647000 );

  return 0;
}
