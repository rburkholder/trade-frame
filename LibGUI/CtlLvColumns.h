#pragma once

#include <string>
#include <vector>
using namespace std;

// used by CtlKeyValues, CtlMarketDepth, and other ListCtrl's

// CLVColumn

class CLVColumn {

public:

  CLVColumn( const char *Name, int Alignment, int Width );
  virtual ~CLVColumn(void);

  LPLVCOLUMN GetStructure( void ) { return &m_col; };

private:
  LVCOLUMN m_col;
  string m_sName;
};

// CLVColumns

class CLVColumns {
public:
  CLVColumns(void);
  virtual ~CLVColumns(void);

  typedef CLVColumn* LPLVCOLUMN;

  void Add( int position, CLVColumn *column );
  void Append( CLVColumn *column );
  void Draw( CListCtrl *ctrl );

private:
  std::vector<LPLVCOLUMN> m_vColumns; // vector of column headers
};

