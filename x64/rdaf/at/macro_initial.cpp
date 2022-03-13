//
// macro_initial.cpp
// run by rdaf/at/Strategy.cpp to construct structures used in the variuous macros
//
// Supplied by Andrew Ross, March 2022
//


#include <rdaf/TH2.h>

int macro_initial() {

   // historgram used for iterative calculations
  TH2F* h2 = new TH2F( "h2", "histogram 2", 100, 2470, 2475, 100, 1646000, 1647000 );

  return 0;
}
