// TestSimulator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <TFTimeSeries/TimeSeries.h>

int _tmain(int argc, _TCHAR* argv[]) {

  ou::tf::Quotes quotes;
  ou::tf::Trades trades;

  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 11,  0 ) ), 10, 1, 11, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 11, 20 ) ), 10, 1, 11, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 11, 40 ) ), 10, 1, 11, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 11, 60 ) ), 10, 1, 12, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 11, 80 ) ), 10, 1, 13, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 12,  0 ) ), 12, 1, 14, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 12, 20 ) ), 14, 1, 15, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 12, 40 ) ), 15, 1, 16, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 12, 60 ) ), 16, 1, 17, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 12, 80 ) ), 16, 1, 18, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 13,  0 ) ), 17, 1, 19, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 13, 20 ) ), 19, 1, 20, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 13, 40 ) ), 14, 1, 19, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 13, 60 ) ), 17, 1, 18, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 13, 80 ) ), 16, 1, 17, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 14,  0 ) ), 16, 1, 17, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 14, 20 ) ), 15, 1, 16, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 14, 40 ) ), 14, 1, 16, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 14, 60 ) ), 15, 1, 17, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 14, 80 ) ), 16, 1, 17, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 15,  0 ) ), 16, 1, 18, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 15, 20 ) ), 17, 1, 19, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 15, 40 ) ), 14, 1, 15, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 15, 60 ) ), 15, 1, 17, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 15, 80 ) ), 18, 1, 19, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 16,  0 ) ), 19, 1, 21, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 16, 20 ) ), 22, 1, 23, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 16, 40 ) ), 16, 1, 17, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 16, 60 ) ), 15, 1, 16, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 16, 80 ) ), 14, 1, 16, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 17,  0 ) ), 13, 1, 17, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 17, 20 ) ), 15, 1, 17, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 17, 40 ) ), 16, 1, 17, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 17, 60 ) ), 16, 1, 17, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 17, 80 ) ), 15, 1, 17, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 18,  0 ) ), 17, 1, 18, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 18, 20 ) ), 16, 1, 18, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 18, 40 ) ), 17, 1, 18, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 18, 60 ) ), 17, 1, 18, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 18, 80 ) ), 16, 1, 18, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 19,  0 ) ), 18, 1, 19, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 19, 20 ) ), 18, 1, 19, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 19, 40 ) ), 18, 1, 19, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 19, 60 ) ), 17, 1, 19, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 19, 80 ) ), 18, 1, 19, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 20,  0 ) ), 19, 1, 20, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 20, 20 ) ), 19, 1, 20, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 20, 40 ) ), 18, 1, 20, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 20, 60 ) ), 19, 1, 20, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 20, 80 ) ), 19, 1, 20, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 21,  0 ) ), 20, 1, 21, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 21, 20 ) ), 20, 1, 21, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 21, 40 ) ), 19, 1, 21, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 21, 60 ) ), 20, 1, 21, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 21, 80 ) ), 20, 1, 21, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 22,  0 ) ), 21, 1, 22, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 22, 20 ) ), 21, 1, 22, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 22, 40 ) ), 21, 1, 22, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 22, 60 ) ), 20, 1, 22, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 22, 80 ) ), 21, 1, 22, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 23,  0 ) ), 22, 1, 23, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 23, 20 ) ), 22, 1, 23, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 23, 40 ) ), 21, 1, 23, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 23, 60 ) ), 21, 1, 23, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 23, 80 ) ), 22, 1, 23, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 24,  0 ) ), 22, 1, 24, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 24, 20 ) ), 23, 1, 24, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 24, 40 ) ), 22, 1, 24, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 24, 60 ) ), 23, 1, 24, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 24, 80 ) ), 21, 1, 24, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 25,  0 ) ), 23, 1, 25, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 25, 20 ) ), 24, 1, 25, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 25, 40 ) ), 23, 1, 25, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 25, 60 ) ), 22, 1, 25, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 25, 80 ) ), 23, 1, 25, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 26,  0 ) ), 25, 1, 26, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 26, 20 ) ), 23, 1, 26, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 26, 40 ) ), 25, 1, 26, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 26, 60 ) ), 24, 1, 26, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 26, 80 ) ), 25, 1, 26, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 27,  0 ) ), 26, 1, 27, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 27, 20 ) ), 25, 1, 27, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 27, 40 ) ), 25, 1, 27, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 27, 60 ) ), 24, 1, 27, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 27, 80 ) ), 26, 1, 27, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 28,  0 ) ), 27, 1, 28, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 28, 20 ) ), 26, 1, 28, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 28, 40 ) ), 27, 1, 28, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 28, 60 ) ), 28, 1, 28, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 28, 80 ) ), 25, 1, 28, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 29,  0 ) ), 27, 1, 29, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 29, 20 ) ), 28, 1, 29, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 29, 40 ) ), 26, 1, 29, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 29, 60 ) ), 27, 1, 29, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 29, 80 ) ), 28, 1, 29, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 30,  0 ) ), 29, 1, 30, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 30, 20 ) ), 28, 1, 30, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 30, 40 ) ), 29, 1, 30, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 30, 60 ) ), 29, 1, 30, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 30, 80 ) ), 28, 1, 30, 1 ) );
  quotes.Append( ou::tf::Quote( ptime( date( 2012, 7, 29 ), time_duration( 8, 0, 31,  0 ) ), 30, 1, 31, 1 ) );

	return 0;
}

