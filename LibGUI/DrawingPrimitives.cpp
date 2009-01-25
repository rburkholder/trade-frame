#include "StdAfx.h"
#include "DrawingPrimitives.h"

#include "Colour.h"

CDrawingPrimitives::CDrawingPrimitives( int nFontHeight, COLORREF colourBackground, COLORREF colourForeground ) 
: m_colourDefaultBackground( colourBackground ), m_colourDefaultForeground( colourForeground )
{

  m_font.CreateFontA( nFontHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, 
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
    DEFAULT_PITCH | FF_SWISS, _T("Arial") );

  m_brush.CreateSolidBrush( colourBackground );
  m_penNull.CreatePen(PS_NULL, 0, colourForeground );
  m_pen.CreatePen(PS_SOLID, 1, colourForeground );
}

CDrawingPrimitives::~CDrawingPrimitives(void) {
  
}
