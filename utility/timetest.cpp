#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>

  union U {
    int64_t num;
    boost::posix_time::ptime dt;
    U( int64_t n ): num( n ) {}
  };

int main() {
  U u( 212520427364229830ll );
  std::cout << u.dt << std::endl;
}

// g++ -l boost_date_time-gcc12-mt-x64-1_77 timetest.cpp
