#pragma once

#include "HDF5TimeSeriesIterator.h"

#include "DataManager.h"
using namespace H5;

#include <string>
using namespace std;

class CHDF5TimeSeriesContainer {
public:
  CHDF5TimeSeriesContainer( string &sFilename );
  ~CHDF5TimeSeriesContainer(void);
protected:
  string m_sFilename;
    CDataManager dm;
DataSet *pdset;
DataSpace *pdspace;
private:
};
