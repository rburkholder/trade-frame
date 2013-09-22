/*
	ExcelFormat.h

	Copyright (c) 2009, 2010, 2011 Martin Fuchs <martin-fuchs@gmx.net>

	License: CPOL

	THE WORK (AS DEFINED BELOW) IS PROVIDED UNDER THE TERMS OF THIS CODE PROJECT OPEN LICENSE ("LICENSE").
	THE WORK IS PROTECTED BY COPYRIGHT AND/OR OTHER APPLICABLE LAW. ANY USE OF THE WORK OTHER THAN AS
	AUTHORIZED UNDER THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
	BY EXERCISING ANY RIGHTS TO THE WORK PROVIDED HEREIN, YOU ACCEPT AND AGREE TO BE BOUND BY THE TERMS
	OF THIS LICENSE. THE AUTHOR GRANTS YOU THE RIGHTS CONTAINED HEREIN IN CONSIDERATION OF YOUR ACCEPTANCE
	OF SUCH TERMS AND CONDITIONS. IF YOU DO NOT AGREE TO ACCEPT AND BE BOUND BY THE TERMS OF THIS LICENSE,
	YOU CANNOT MAKE ANY USE OF THE WORK.
*/

#ifndef EXCELFORMAT_H
#define EXCELFORMAT_H

#include "BasicExcel.hpp"

namespace ExcelFormat {

using namespace YExcel;


#define XLS_FORMAT_GENERAL		L"General"
#define XLS_FORMAT_TEXT			L"@"
#define XLS_FORMAT_INTEGER		L"0"
#define XLS_FORMAT_DECIMAL		L"0.00"
#define XLS_FORMAT_PERCENT		L"0%"
#define XLS_FORMAT_DATE			L"M/D/YY"
#define XLS_FORMAT_TIME			L"h:mm:ss"
#define XLS_FORMAT_DATETIME		L"M/D/YY h:mm"

#define	FIRST_USER_FORMAT_IDX	164


 // Font option flags
enum EXCEL_FONT_OPTIONS {
	EXCEL_FONT_BOLD			= 0x01,	// redundant in BIFF5-BIFF8, see ExcelFont::_weight
	EXCEL_FONT_ITALIC		= 0x02,
	EXCEL_FONT_UNDERLINED	= 0x04,	// redundant in BIFF5-BIFF8, see ExcelFont::_underline_type
	EXCEL_FONT_STRUCK_OUT	= 0x08,
	EXCEL_FONT_OUTLINED		= 0x10,
	EXCEL_FONT_SHADOWED		= 0x20,
	EXCEL_FONT_CONDENSED	= 0x40,
	EXCEL_FONT_EXTENDED		= 0x80
};

 // Font escapement types
enum EXCEL_ESCAPEMENT {
	EXCEL_ESCAPEMENT_NONE		 = 0,
	EXCEL_ESCAPEMENT_SUPERSCRIPT = 1,
	EXCEL_ESCAPEMENT_SUBSCRIPT	 = 2
};

 // Font underline types
enum EXCEL_UNDERLINE_FLAGS {
	EXCEL_UNDERLINE_NONE				= 0x00,
	EXCEL_UNDERLINE_SINGLE				= 0x01,
	EXCEL_UNDERLINE_DOUBLE				= 0x02,
	EXCEL_UNDERLINE_SINGLE_ACCOUNTING	= 0x21,
	EXCEL_UNDERLINE_DOUBLE_ACCOUNTING	= 0x22
};

 // Excel font family constants, see also FF_DONTCARE, ... in wingdi.h
enum EXCEL_FONT_FAMILY {
	EXCEL_FONT_FAMILY_DONTCARE	 = 0,
	EXCEL_FONT_FAMILY_ROMAN		 = 1,
	EXCEL_FONT_FAMILY_SWISS		 = 2,
	EXCEL_FONT_FAMILY_MODERN	 = 3,
	EXCEL_FONT_FAMILY_SCRIPT	 = 4,
	EXCEL_FONT_FAMILY_DECORATIVE = 5
};


struct ExcelFont
{
	ExcelFont()
	 :	_height(200),
		_options(0),
		_color_index(0x7FFF),
		_weight(400), // FW_NORMAL
		_escapement_type(EXCEL_ESCAPEMENT_NONE),
		_underline_type(EXCEL_UNDERLINE_NONE),
		_font_family(EXCEL_FONT_FAMILY_DONTCARE),
		_character_set(0), // ANSI_CHARSET
		_name(L"Arial")
	{
	}

	ExcelFont(const Workbook::Font& font)
	 :	_height(font.height_),
		_options(font.options_),
		_color_index(font.colourIndex_),
		_weight(font.weight_),
		_escapement_type(font.escapementType_),
		_underline_type(font.underlineType_),
		_font_family(font.family_),
		_character_set(font.characterSet_),
		_name(wstringFromSmallString(font.name_))
	{
	}

	wstring	_name;
	short	_height;			// font height in twips (1/20 of a point)
	short	_weight;			// FW_NORMAL, FW_BOLD, ...
	short	_options;			// see EXCEL_FONT_OPTIONS
	short	_color_index;		// font colour index
	short	_escapement_type;	// see EXCEL_ESCAPEMENT
	char	_underline_type;	// see EXCEL_UNDERLINE_FLAGS
	char	_font_family;		// see EXCEL_FONT_FAMILY
	char	_character_set;		// ANSI_CHARSET, DEFAULT_CHARSET, SYMBOL_CHARSET, ...

	ExcelFont& set_height(int height)
	{
		_height = height;
		return *this;
	}

	ExcelFont& set_weight(int weight)
	{
		_weight = weight;
		return *this;
	}

	ExcelFont& set_italic(bool italic)
	{
		_options = (_options&~EXCEL_FONT_ITALIC) | (italic? EXCEL_FONT_ITALIC: 0);
		return *this;
	}

	ExcelFont& set_color_index(int color_idx)
	{
		_color_index = color_idx;
		return *this;
	}

	ExcelFont& set_underline_type(char underline_type)
	{
		_underline_type = underline_type;
		return *this;
	}

	ExcelFont& set_escapement(short escapement_type)
	{
		_escapement_type = escapement_type;
		return *this;
	}

	ExcelFont& set_font_name(const wchar_t* name)
	{
		_name = name;
		return *this;
	}

	bool matches(const Workbook::Font& font) const
	{
		if (wstringFromSmallString(font.name_) != _name)
			return false;

		if (font.height_ != _height)
			return false;

		if (font.weight_ != _weight)
			return false;

		if (font.options_ != _options)
			return false;

		if (font.colourIndex_ != _color_index)
			return false;

		if (font.escapementType_ != _escapement_type)
			return false;

		if (font.underlineType_ != _underline_type)
			return false;

		if (font.family_ != _font_family)
			return false;

		if (font.characterSet_ != _character_set)
			return false;

		return true;
	}
};


struct CellFormat;

 // Excel cell format manager class
struct XLSFormatManager
{
	XLSFormatManager(BasicExcel& xls);

	const Workbook::Font& get_font(const CellFormat& fmt) const;
	wstring get_format_string(const CellFormat& fmt) const;
	const Workbook::XF& get_XF(int xf_idx) const {return _xls.workbook_.XFs_[xf_idx];}

	int get_font_idx(const ExcelFont& font);
	int get_format_idx(const wstring& fmt_str);
	int get_xf_idx(const CellFormat& fmt);

private:
	BasicExcel&	_xls;

	int _next_fmt_idx;

	typedef map<int, wstring> FormatMap;
	typedef map<wstring, int> FormatRevMap;
	FormatMap _formats;
	FormatRevMap _formats_rev;
};


enum XF_USED_ATTRIB {
	XF_USED_ATTRIB_NUMBER_FORMAT	= 0x01,
	XF_USED_ATTRIB_FONT				= 0x02,
	XF_USED_ATTRIB_ALIGN			= 0x04,	// horizontal and vertical alignment, text wrap, indentation, orientation, rotation, and text direction
	XF_USED_ATTRIB_BORDER_LINES		= 0x08,
	XF_USED_ATTRIB_BKGND_AREA_STYLE	= 0x10,
	XF_USED_ATTRIB_CELL_PROTECTION	= 0x20
};


/*
 Bit Mask Contents of alignment byte
 2- 0 07H XF_HOR_ALIGN: Horizontal alignment
 3    08H 1 = Text is wrapped at right border
 6- 4 70H XF_VERT_ALIGN: Vertical alignment
 7    80H 1 = Justify last line in justified or distibuted text
*/
enum EXCEL_ALIGNMENT {
	 // horizontal aligmment
	EXCEL_HALIGN_GENERAL		= 0x00,
	EXCEL_HALIGN_LEFT			= 0x01,
	EXCEL_HALIGN_CENTRED		= 0x02,
	EXCEL_HALIGN_RIGHT			= 0x03,
	EXCEL_HALIGN_FILLED			= 0x04,
	EXCEL_HALIGN_JUSITFIED		= 0x05,
	EXCEL_HALIGN_SEL_CENTRED	= 0x06,	// centred across selection
	EXCEL_HALIGN_DISTRIBUTED	= 0x07,	// available in Excel 10.0 (Excel XP) and later only

	 // vertical alignment
	EXCEL_VALIGN_TOP			= 0x00,
	EXCEL_VALIGN_CENTRED		= 0x10,
	EXCEL_VALIGN_BOTTOM			= 0x20,
	EXCEL_VALIGN_JUSTIFIED		= 0x30,
	EXCEL_VALIGN_DISTRIBUTED	= 0x40,	// available in Excel 10.0 (Excel XP) and later only

	EXCEL_JUSTIFY_LAST_LINE		= 0x80	// justify last line in justified or distibuted text
};


/*
Indentation, shrink to cell size, and text direction:
 Bit Mask Contents
 3- 0 0FH Indent level
 4    10H 1 = Shrink content to fit into cell
 7- 6 C0H Text direction: 0 = According to context; 1 = Left-to-right; 2 = Right-to-left
*/
#define MAKE_TEXT_PROPS(indent, flags)	((indent) | (flags))

enum EXCEL_TEXT_PROP_FLAGS {
	 // shrink flag
	EXCEL_TEXT_PROP_SHRINK			= 0x10,

	 // text direction
	EXCEL_TEXT_PROP_DEFAULT			= 0x00,
	EXCEL_TEXT_PROP_LEFT_TO_RIGHT	= 0x40,
	EXCEL_TEXT_PROP_RIGHT_TO_LEFT	= 0x80
};


/*
  Bit      Mask Contents of cell border lines and background area:
 3- 0 0000000FH Left line style
 7- 4 000000F0H Right line style
11- 8 00000F00H Top line style
15-12 0000F000H Bottom line style
22-16 007F0000H Color index for left line colour
29-23 3F800000H Color index for right line colour
30    40000000H 1 = Diagonal line from top left to right bottom
31    80000000H 1 = Diagonal line from bottom left to right top
*/
#define MAKE_BORDERSTYLE(left, right, top, bottom, lcol, rcol) \
	((left) | ((right)<<4) | ((top)<<8) | ((bottom)<<12) | ((lcol)<<16) | ((rcol)<<23))

 // line style constants
enum EXCEL_LS {
	EXCEL_LS_NO_LINE					= 0x00,
	EXCEL_LS_THIN						= 0x01,
	EXCEL_LS_MEDIUM						= 0x02,
	EXCEL_LS_DASHED						= 0x03,
	EXCEL_LS_DOTTED						= 0x04,
	EXCEL_LS_THICK						= 0x05,
	EXCEL_LS_DOUBLE						= 0x06,
	EXCEL_LS_HAIR						= 0x07,
	EXCEL_LS_MEDIUM_DASHED				= 0x08,
	EXCEL_LS_THIN_DASH_DOTTED			= 0x09,
	EXCEL_LS_MEDIUM_DASH_DOTTED			= 0x0A,
	EXCEL_LS_THIN_DASH_DOT_DOTTED		= 0x0B,
	EXCEL_LS_MEDIUM_DASH_DOT_DOTTED		= 0x0C,
	EXCEL_LS_SLANTED_MEDIUM_DASH_DOTTED	= 0x0D
};

 // borderline flags for diagonal lines
enum BORDERLINE_FLAGS {
	BORDERLINE_DIAG1	= 0x40000000,
	BORDERLINE_DIAG2	= 0x80000000
};


/*
  Bit      Mask Contents of COLOR1
06- 0 0000007FH Color index for top line colour
13- 7 00003F80H Color index for bottom line colour
20-14 001FC000H Color index for diagonal line colour
24-21 01E00000H Diagonal line style
31-26 FC000000H Fill pattern
*/
#define MAKE_COLOR1(top, bottom, diag, pat)	((top) | ((bottom)<<7) | ((diag)<<14) | pat)

 // line style constants for COLOR1
enum COLOR1_LS { // EXCEL_LS << 21
	COLOR1_LS_NO_LINE						= 0x0000000,
	COLOR1_LS_THIN							= 0x0200000,
	COLOR1_LS_MEDIUM						= 0x0400000,
	COLOR1_LS_DASHED						= 0x0600000,
	COLOR1_LS_DOTTED						= 0x0800000,
	COLOR1_LS_THICK							= 0x0A00000,
	COLOR1_LS_DOUBLE						= 0x0C00000,
	COLOR1_LS_HAIR							= 0x0E00000,
	COLOR1_LS_MEDIUM_DASHED					= 0x1000000,
	COLOR1_LS_THIN_DASH_DOTTED				= 0x1200000,
	COLOR1_LS_MEDIUM_DASH_DOTTED			= 0x1400000,
	COLOR1_LS_THIN_DASH_DOT_DOTTED			= 0x1600000,
	COLOR1_LS_MEDIUM_DASH_DOT_DOTTED		= 0x1800000,
	COLOR1_LS_SLANTED_MEDIUM_DASH_DOTTED	= 0x1A00000
};

 // pattern constants
enum COLOR1_PAT {
	COLOR1_PAT_EMPTY		= 0x00000000,
	COLOR1_PAT_SOLID		= 0x04000000,
	COLOR1_PAT_2			= 0x08000000,
	COLOR1_PAT_3			= 0x0C000000,
	COLOR1_PAT_4			= 0x10000000,
	COLOR1_PAT_5			= 0x14000000,
	COLOR1_PAT_6			= 0x18000000,
	COLOR1_PAT_7			= 0x1C000000,
	COLOR1_PAT_8			= 0x20000000,
	COLOR1_PAT_9			= 0x24000000,
	COLOR1_PAT_10			= 0x28000000,
	COLOR1_PAT_11			= 0x2C000000,
	COLOR1_PAT_12			= 0x30000000,
	COLOR1_PAT_13			= 0x34000000,
	COLOR1_PAT_14			= 0x38000000,
	COLOR1_PAT_15			= 0x3C000000,
	COLOR1_PAT_16			= 0x40000000,
	COLOR1_PAT_17			= 0x44000000,
	COLOR1_PAT_18			= 0x48000000
};

/*
   Bit Mask Contents of COLOR2
 6- 0 007FH Color index for pattern colour
13- 7 3F80H Color index for pattern background
*/
#define MAKE_COLOR2(pc, pbc)	((pc) | ((pbc)<<7))


 // predefined Excel color definitions
enum EXCEL_COLORS {
	EGA_BLACK	= 0,	// 000000H
	EGA_WHITE	= 1,	// FFFFFFH
	EGA_RED		= 2,	// FF0000H
	EGA_GREEN	= 3,	// 00FF00H
	EGA_BLUE	= 4,	// 0000FFH
	EGA_YELLOW	= 5,	// FFFF00H
	EGA_MAGENTA	= 6,	// FF00FFH
	EGA_CYAN	= 7		// 00FFFFH
};


 // Excel cell format
struct CellFormat
{
	CellFormat(XLSFormatManager& mgr)
	 :	_mgr(mgr)
	{
		init();
	}

	CellFormat(XLSFormatManager& mgr, const ExcelFont& font)
	 :	_mgr(mgr)
	{
		init();
		set_font(font);
	}

	CellFormat(XLSFormatManager& mgr, const Workbook::XF& xf, int xf_idx)
	 :	_mgr(mgr)
	{
		init(xf, xf_idx);
	}

	CellFormat(XLSFormatManager& mgr, const BasicExcelCell* cell)
	 :	_mgr(mgr)
	{
		int xf_idx = cell->GetXFormatIdx();
		init(mgr.get_XF(xf_idx), xf_idx);
	}

	XLSFormatManager& get_format_manager() const
	{
		return _mgr;
	}

	 // return XF index for all current attributes
	int get_xf_idx() const
	{
		if (_xf_idx == -1)
			_xf_idx = _mgr.get_xf_idx(*this);

		return _xf_idx;
	}

	void flush()
	{
		_xf_idx = -1;	// invalidate XF index
	}

	int get_font_idx() const
	{
		return _font_idx;
	}

	int get_fmt_idx() const
	{
		return _fmt_idx;
	}

	ExcelFont get_font()
	{
		return ExcelFont(_mgr.get_font(*this));
	}
	CellFormat& set_font(const ExcelFont& font)
	{
		_font_idx = _mgr.get_font_idx(font);
		flush();
		return *this;
	}

	wstring get_format_string() const
	{
		return _mgr.get_format_string(*this);
	}
	CellFormat& set_format_string(const char* fmt_str)
	{
		return set_format_string(widen_string(fmt_str));
	}
	CellFormat& set_format_string(const wstring& fmt_str)
	{
		_fmt_idx = _mgr.get_format_idx(fmt_str);
		flush();
		return *this;
	}

	int get_color1() const
	{
		return _color1;
	}
	CellFormat& set_color1(unsigned color)
	{
		_color1 = color;
		flush();
		return *this;
	}

	unsigned short get_color2() const
	{
		return _color2;
	}
	CellFormat& set_color2(unsigned short color)
	{
		_color2 = color;
		flush();
		return *this;
	}

	 // convenience function to set color1 and color2 at the same time
	CellFormat& set_background(unsigned short color2, unsigned color1=COLOR1_PAT_SOLID)
	{
		return set_color1(color1).set_color2(color2);
	}

	char get_alignment() const
	{
		return _alignment;
	}
	CellFormat& set_alignment(char alignment)
	{
		_alignment = alignment;
		flush();
		return *this;
	}

	bool is_wrapping()
	{
		return (_alignment & 0x08)? true: false;
	}
	CellFormat& set_wrapping(bool wrap)
	{
		return set_alignment((_alignment&~0x08) | (wrap? 0x08: 0x00));
	}

	char get_rotation() const
	{
		return _rotation;
	}
	CellFormat& set_rotation(char rotation)
	{
		_rotation = rotation;
		flush();
		return *this;
	}

	char get_text_props() const
	{
		return _text_props;
	}
	CellFormat& set_text_props(char text_props)
	{
		_text_props = text_props;
		flush();
		return *this;
	}

	int get_borderlines() const
	{
		return _borderlines;
	}
	CellFormat& set_borderlines(int borderlines)
	{
		_borderlines = borderlines;
		flush();
		return *this;
	}

	 // set borderlines using EXCEL_LS enumeration constants and colour indices
	CellFormat& set_borderlines(
		EXCEL_LS left, EXCEL_LS right, EXCEL_LS top, EXCEL_LS bottom,
		unsigned idxLclr, unsigned idxRclr
	)
	{
		_borderlines = MAKE_BORDERSTYLE(left, right, top, bottom, idxLclr, idxRclr);
		flush();
		return *this;
	}

	 // set borderlines including the top and bottom colour
	CellFormat& set_borderlines(
		EXCEL_LS left, EXCEL_LS right, EXCEL_LS top, EXCEL_LS bottom,
		unsigned idxLclr, unsigned idxRclr, unsigned idxTclr, unsigned idxBclr,
		EXCEL_LS diag,		// diagonal line style
		COLOR1_PAT pattern	// fill pattern
	)
	{
		_borderlines = MAKE_BORDERSTYLE(left, right, top, bottom, idxLclr, idxRclr);
		_color1 = MAKE_COLOR1(idxTclr, idxBclr, diag, pattern);
		flush();
		return *this;
	}

	bool matches(const Workbook::XF& xf) const
	{
		if (xf.fontRecordIndex_ != _font_idx)
			return false;

		if (xf.formatRecordIndex_ != _fmt_idx)
			return false;

		if (xf.alignment_ != _alignment)
			return false;

		if (xf.rotation_ != _rotation)
			return false;

		if (xf.textProperties_ != _text_props)
			return false;

		if (xf.borderLines_ != _borderlines)
			return false;

		if (xf.colour1_ != _color1)
			return false;

		if (xf.colour2_ != _color2)
			return false;

		return true;
	}

	void get_xf(Workbook::XF& xf) const
	{
		xf.fontRecordIndex_ = _font_idx;
		xf.formatRecordIndex_ = _fmt_idx;
		xf.alignment_ = _alignment;
		xf.rotation_= _rotation;
		xf.textProperties_= _text_props;
		xf.borderLines_= _borderlines;
		xf.colour1_ = _color1;
		xf.colour2_ = _color2;
	}

private:
	XLSFormatManager& _mgr;

	mutable int _xf_idx;	// cached XF index

	int		_font_idx;
	int		_fmt_idx;

//	char	_protectionType;
	char	_alignment;			// alignment and text break, see EXCEL_ALIGNMENT
	char	_rotation;			// text rotation angle in degrees
	char	_text_props;		// see MAKE_TEXT_PROPS() and EXCEL_TEXT_PROP_FLAGS
//	char	_usedAttributes;
	int		_borderlines;		// see MAKE_BORDERSTYLE(), EXCEL_LS and BORDERLINE_FLAGS
	unsigned _color1;			// see MAKE_COLOR1(), COLOR1_LS and COLOR1_PAT
	unsigned short _color2;		// see MAKE_COLOR2()

	void init()
	{
		_font_idx = 0;
		_fmt_idx = 0;
		_alignment = EXCEL_VALIGN_BOTTOM;
		_rotation = 0;
		_text_props = MAKE_TEXT_PROPS(0, EXCEL_TEXT_PROP_DEFAULT);
		_borderlines = 0;
		_color1 = 0;
		_color2 = MAKE_COLOR2(64, 65); // 0x20C0

		_xf_idx = 0;
	}

	void init(const Workbook::XF& xf, int xf_idx)
	{
		_font_idx = xf.fontRecordIndex_;
		_fmt_idx = xf.formatRecordIndex_;
		_alignment = xf.alignment_;
		_rotation = xf.rotation_;
		_text_props = xf.textProperties_;
		_borderlines = xf.borderLines_;
		_color1 = xf.colour1_;
		_color2 = xf.colour2_;
		_xf_idx = xf_idx;
	}
};

} // namespace ExcelFormat


namespace YExcel {

inline void BasicExcelCell::SetFormat(const ExcelFormat::CellFormat& fmt)
{
	_xf_idx = fmt.get_xf_idx();
}

}

#endif	// EXCELFORMAT_H
