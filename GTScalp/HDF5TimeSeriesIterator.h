#pragma once

#include "DatedDatum.h"

#include "DataManager.h"
using namespace H5;

#include <iterator>
//using namespace std;

class CHDF5TimeSeriesContainer;

class CHDF5TimeSeriesIterator: 
  public std::iterator<std::random_access_iterator_tag, const CDatedDatum> {
public:
  explicit CHDF5TimeSeriesIterator( CHDF5TimeSeriesContainer *pContainer ); // end() init
  explicit CHDF5TimeSeriesIterator( CHDF5TimeSeriesContainer *pContainer, hsize_t Index );  // begin() or later init
  ~CHDF5TimeSeriesIterator(void);
protected:
  CHDF5TimeSeriesContainer *m_pContainer;
  bool m_bPointingSomewhere;  // m_ItemIndex is valid ie, is .end()
  hsize_t m_ItemIndex; // index into specific item
  CDatedDatum *m_pDatum; // the currently retrieved datum
private:
};

