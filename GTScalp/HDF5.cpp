#include "StdAfx.h"
#include "HDF5.h"

#include <iostream>
#include <string>

#include "H5Cpp.h"
using namespace H5;

CHDF5::CHDF5(void) {
}

CHDF5::~CHDF5(void) {
}

int CHDF5::Test() {

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the files COPYING and Copyright.html.  COPYING can be found at the root   *
 * of the source code distribution tree; Copyright.html can be found at the  *
 * root level of an installed copy of the electronic HDF5 document set and   *
 * is linked from the top-level documents page.  It can also be found at     *
 * http://hdf.ncsa.uiuc.edu/HDF5/doc/Copyright.html.  If you do not have     *
 * access to either file, you may request a copy from hdfhelp@ncsa.uiuc.edu. *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 *  This example writes a dataset to a new HDF5 file.
 */

const H5std_string      FILE_NAME( "SDS.h5" );
const H5std_string      DATASET_NAME( "IntArray" );
const int       NX = 5;                    // dataset dimensions
const int       NY = 6;
const int       RANK = 2;

   /*
    * Data initialization.
    */
   int i, j;
   int data[NX][NY];          // buffer for data to write
   for (j = 0; j < NX; j++)
   {
      for (i = 0; i < NY; i++)
         data[j][i] = i + j;
   }
   /*
    * 0 1 2 3 4 5
    * 1 2 3 4 5 6
    * 2 3 4 5 6 7
    * 3 4 5 6 7 8
    * 4 5 6 7 8 9
    */

   // Try block to detect exceptions raised by any of the calls inside it
   try
   {
      /*
       * Turn off the auto-printing when failure occurs so that we can
       * handle the errors appropriately
       */
      Exception::dontPrint();

      /*
       * Create a new file using H5F_ACC_TRUNC access,
       * default file creation properties, and default file
       * access properties.
       */
      H5File file( FILE_NAME, H5F_ACC_TRUNC );

      // H5Fcreate accepts H5F_ACC_TRUNC or H5F_ACC_EXCL.
      // H5Fopen accepts H5F_ACC_RDONLY or H5F_ACC_RDWR.
/*
      H5F_ACC_EXCL  If file already exists, H5Fcreate fails. If file does not exist, it is created
                    and opened with read-write access.
      H5F_ACC_TRUNC If file already exists, file is opened with read-write access and new data
                    overwrites existing data, destroying all prior content, i.e., file content is
                    truncated upon opening. If file does not exist, it is created and opened with
                    read-write access.
      H5F_ACC_RDONLY Existing file is opened with read-only access. If file does not exist,
                    H5Fopen fails.
      H5F_ACC_RDWR  Existing file is opened with read-write access. If file does not exist,
                    H5Fopen fails.
*/

//hid_t plist, file;
//plist = H5Pcreate (H5P_FILE_ACCESS);
//H5Pset_family (plist, 1<<30, H5P_DEFAULT);
//file = H5Fcreate ("big%03d.h5", H5F_ACC_TRUNC, H5P_DEFAULT, plist);
      // 2,147,483,647



      /*
       * Define the size of the array and create the data space for fixed
       * size dataset.
       */
      hsize_t     dimsf[2];              // dataset dimensions
      dimsf[0] = NX;
      dimsf[1] = NY;
      DataSpace dataspace( RANK, dimsf );

      /*
       * Define datatype for the data in the file.
       * We will store little endian INT numbers.
       */
      IntType datatype( PredType::NATIVE_INT );
      datatype.setOrder( H5T_ORDER_LE );

      /*
       * Create a new dataset within the file using defined dataspace and
       * datatype and default dataset creation properties.
       */
      DataSet dataset = file.createDataSet( DATASET_NAME, datatype, dataspace );

      /*
       * Write the data to the dataset using default memory space, file
       * space, and transfer properties.
       */
      dataset.write( data, PredType::NATIVE_INT );
   }  // end of try block

   // catch failure caused by the H5File operations
   catch( FileIException error )
   {
      error.printError();
      return -1;
   }

   // catch failure caused by the DataSet operations
   catch( DataSetIException error )
   {
      error.printError();
      return -1;
   }

   // catch failure caused by the DataSpace operations
   catch( DataSpaceIException error )
   {
      error.printError();
      return -1;
   }

   // catch failure caused by the DataSpace operations
   catch( DataTypeIException error )
   {
      error.printError();
      return -1;
   }

   return 0;  // successfully terminated
}

