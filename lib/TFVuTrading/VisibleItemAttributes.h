#pragma once

class CVisibleItemAttributes {
public:
  CVisibleItemAttributes( 
    int nFontHeight, 
    COLORREF colourBackground = ::GetSysColor( COLOR_WINDOW ), 
    COLORREF colourForeground = ::GetSysColor( COLOR_WINDOWTEXT ) );
  ~CVisibleItemAttributes(void);

  const CPen& Pen( void ) { return m_pen; };
  const CBrush& Brush( void ) { return m_brush; };
  const CFont& Font( void ) { return m_font; };
  COLORREF DefaultBackgroundColour( void ) { return m_colourDefaultBackground; };
  COLORREF DefaultForegroundColour( void ) { return m_colourDefaultForeground; };
  
protected:
private:

  CVisibleItemAttributes( void );  // no default constructor

  // graphic objects
  CPen m_penNull;
  CPen m_pen;
  CBrush m_brush;
  CFont m_font;
  CBitmap m_bitmap;
  CPalette m_palette;
  CRgn m_rgn;

  COLORREF m_colourDefaultBackground;
  COLORREF m_colourDefaultForeground;

};
