#pragma once

// class DT: primitive data type (self-assignment not checked)
// class GUI:  GUI type to be used for drawing

// 
// contains a single data point
// emits event when data point changes
// provides ability to draw data point in owner draw device context
// currently not used anywhere
// possibly superceded by CVisibleItem
// 

// to reduce code size, some common methods could be factored out into a 
//  base class and inherited

// 2009/06/14 possibly inherit from CVisibleItem

#include <sstream>

#include "FastDelegate.h"
using namespace fastdelegate;

#include <boost/utility.hpp>

#include "VisibleItemAttributes.h"

template<class DT, class GUI> class CVisibleItemInDevContext: boost::noncopyable {
public:
  CVisibleItemInDevContext<DT,GUI>( 
    GUI* pWin, 
    CVisibleItemAttributes* pItemAttributes,
    DT dtBlank // initialized with this, GUI shows blank with this value
    );
  ~CVisibleItemInDevContext<DT,GUI>(void);

  DT& operator=( const DT& rhs );
  DT& Value( void ) { return m_dtItem; };

  typedef FastDelegate0<> OnUpdateHandler;
  void SetOnUpdate( OnUpdateHandler function ) {
    OnUpdate = function;
  }

  void Paint( CPaintDC* pdc );

protected:
  OnUpdateHandler OnUpdate;
private:
  GUI* m_pWin;  // destroyed elsewhere
  DT m_dtBlank;
  DT m_dtItem;
  CVisibleItemAttributes* m_pItemAttributes;
  COLORREF colourBackground;
  COLORREF colourForeground;
  //std::string m_sText;
  std::stringstream m_ss;
  CVisibleItemInDevContext<DT,GUI>( void );  // default constructor unavailable
};

template<class DT, class GUI> CVisibleItemInDevContext<DT,GUI>::CVisibleItemInDevContext( 
  GUI *pWin, 
  CVisibleItemAttributes* pItemAttributes,
  DT dtBlank
  ) 
: m_pWin( pWin ), m_dtBlank( dtBlank ), m_dtItem( dtBlank ), m_pItemAttributes( pItemAttributes )
{
  // pre-constructed window on to which we'll draw our stuff
  assert( NULL != pWin );
}

template<class DT, class GUI> CVisibleItemInDevContext<DT,GUI>::~CVisibleItemInDevContext() {
}

template<class DT, class GUI> DT& CVisibleItemInDevContext<DT,GUI>::operator=( const DT& rhs ) {
  if ( m_dtItem != rhs ) {
    m_dtItem = rhs;
    m_ss.str( "" );
    if ( m_dtBlank != m_dtItem ) {
      m_ss << m_dtItem;
    }
    if ( NULL != OnUpdate ) OnUpdate();
  }
  return m_dtItem;
}

template<class DT, class GUI> void CVisibleItemInDevContext<DT,GUI>::Paint( CPaintDC* pdc ) {
  // should be called from within Win's OnPaint handler
  // commands as they are used in CVisibleElement
  // OnEraseBkgnd should be no-op'd
  CRect rc;
  pWin->GetClientRect( &rc );
  pdc->SelectObject( m_pItemAttributes->Brush() );
  pdc->SelectObject( m_pItemAttributes->Pen() );
  pdc->FillSolidRect( &rc, colourBackground );
  // possibly draw a focus rectangle:
  // pdc->Rectangle( &rc );
  pdc->SelectObject( m_pItemAttributes->Font() );
  pdc->SetBkMode( TRANSPARENT );
  pdc->DrawText( m_ss.str().c_str(), m_ss.str().size(), &rc, DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_NOCLIP );
}
