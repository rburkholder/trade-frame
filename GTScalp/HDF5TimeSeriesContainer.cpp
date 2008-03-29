#include "StdAfx.h"
#include "HDF5TimeSeriesContainer.h"

CHDF5TimeSeriesContainer::CHDF5TimeSeriesContainer( string &sFilename ):
  m_sFilename( sFilename ) {
  //CDataManager dm;
  pdset = new DataSet( dm.GetH5File()->openDataSet( sFilename.c_str() ) );
  pdspace = new DataSpace( pdset->getSpace() );
  hsize_t t1, t2;
  pdspace->getSimpleExtentDims( &t1, &t2  );  //current, max

  DataSpace dspaceSelection( pdset->getSpace() );

}

CHDF5TimeSeriesContainer::~CHDF5TimeSeriesContainer(void) {
  pdspace->close();
  pdset->close();
}