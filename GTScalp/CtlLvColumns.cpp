#include "StdAfx.h"
#include "CtlLvColumns.h"

// http://www.codeproject.com/listctrl/lvcustomdraw.asp

// CLVColumn

CLVColumn::CLVColumn( const char *Name, int Alignment, int Width ) {
  m_sName = Name;
  m_col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
  m_col.fmt = Alignment;
  m_col.cx = Width;
  m_col.pszText = (LPSTR) m_sName.c_str();
}

CLVColumn::~CLVColumn(void) {
}

// CLVColumns

CLVColumns::CLVColumns() {
}

CLVColumns::~CLVColumns() {
  while ( !m_vColumns.empty() ) {
    LPLVCOLUMN &c = m_vColumns.back();
    delete c;
    m_vColumns.pop_back();
  }
}

void CLVColumns::Add(int position, CLVColumn *column) {
  m_vColumns.insert( m_vColumns.begin() + position, column );
}

void CLVColumns::Append(CLVColumn *column) {
  m_vColumns.push_back( column );
}

void CLVColumns::Draw(CListCtrl *pLC) {
  int nColumnCount = pLC->GetHeaderCtrl()->GetItemCount();
  for ( int i = 0; i < nColumnCount; i++ ) {
    pLC->DeleteColumn( 0 );
  }

  int ix = 0;
  CLVColumn* col;

  for ( 
    std::vector<CLVColumn*>::iterator iter = m_vColumns.begin();
    iter != m_vColumns.end();
    iter++ ) {
      col = *iter;
      pLC->InsertColumn( ix, col->GetStructure()  );
      ix++;
  }
}

