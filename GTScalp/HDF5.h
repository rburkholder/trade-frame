#pragma once

#include "H5Cpp.h"

#ifndef H5_NO_NAMESPACE
    using namespace H5;
#endif

//HDF5_EXT_ZLIB
//HDF5_EXT_SZIP

/*      Click "Start", click "Control Panel", and then double-click "System".  
      On the "Advanced" tab, click "Environment Variables".

      HDF5_EXT_SZIP environment variable as szlibdll.lib

      In release 1.8.0, all.zip is deprecated. Users should go to hdf5/windows 
      directory, run copy_hdf.bat first and then open all.dsw under 
      hdf5/windows/proj/all to start building process.

      For users who would like to build Muti-threaded version of HDF5 library, 
      please read Section VII.
      */

class CHDF5 {
public:
  CHDF5(void);
  virtual ~CHDF5(void);
  int Test( void );
};
