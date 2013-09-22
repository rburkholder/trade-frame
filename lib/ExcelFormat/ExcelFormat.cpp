/*
	ExcelFormat.cpp

	Copyright (c) 2009, 2011 Martin Fuchs <martin-fuchs@gmx.net>

	License: CPOL

	THE WORK (AS DEFINED BELOW) IS PROVIDED UNDER THE TERMS OF THIS CODE PROJECT OPEN LICENSE ("LICENSE").
	THE WORK IS PROTECTED BY COPYRIGHT AND/OR OTHER APPLICABLE LAW. ANY USE OF THE WORK OTHER THAN AS
	AUTHORIZED UNDER THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
	BY EXERCISING ANY RIGHTS TO THE WORK PROVIDED HEREIN, YOU ACCEPT AND AGREE TO BE BOUND BY THE TERMS
	OF THIS LICENSE. THE AUTHOR GRANTS YOU THE RIGHTS CONTAINED HEREIN IN CONSIDERATION OF YOUR ACCEPTANCE
	OF SUCH TERMS AND CONDITIONS. IF YOU DO NOT AGREE TO ACCEPT AND BE BOUND BY THE TERMS OF THIS LICENSE,
	YOU CANNOT MAKE ANY USE OF THE WORK.
*/

#include "ExcelFormat.h"

using namespace ExcelFormat;


XLSFormatManager::XLSFormatManager(BasicExcel& xls)
 :	_xls(xls),
	_next_fmt_idx(FIRST_USER_FORMAT_IDX)	// above last reserved format index 163
{
	 // initialize predefined formats
	_formats[0] = XLS_FORMAT_GENERAL;		// "General"								// General
	_formats[1] = XLS_FORMAT_INTEGER;		// "0"										// Decimal
	_formats[2] = XLS_FORMAT_DECIMAL;		// "0.00"									// Decimal
	_formats[3] = L"#,##0";																// Decimal
	_formats[4] = L"#,##0.00";															// Decimal
	_formats[5] = L"\"$\"#,##0_);(\"$\"#,##0)";											// Currency
	_formats[6] = L"\"$\"#,##0_);[Red](\"$\"#,##0)";									// Currency
	_formats[7] = L"\"$\"#,##0.00_);(\"$\"#,##0.00)";									// Currency
	_formats[8] = L"\"$\"#,##0.00_);[Red](\"$\"#,##0.00)";								// Currency
	_formats[9] = XLS_FORMAT_PERCENT;		// "0%"										// Percent
	_formats[10] = L"0.00%";															// Percent
	_formats[11] = L"0.00E+00";															// Scientific
	_formats[12] = L"# ?/?";															// Fraction
	_formats[13] = L"# ?\?/?\?";															// Fraction
	_formats[14] = XLS_FORMAT_DATE;			// "M/D/YY"									// Date
	_formats[15] = L"D-MMM-YY";															// Date
	_formats[16] = L"D-MMM";															// Date
	_formats[17] = L"MMM-YY";															// Date
	_formats[18] = L"h:mm AM/PM";														// Time
	_formats[19] = L"h:mm:ss AM/PM";													// Time
	_formats[20] = L"h:mm";																// Time
	_formats[21] = XLS_FORMAT_TIME;			// "h:mm:ss"								// Time
	_formats[22] = XLS_FORMAT_DATETIME;		// "M/D/YY h:mm"							// Date/Time
	_formats[37] = L"_(#,##0_);(#,##0)";												// Account.
	_formats[38] = L"_(#,##0_);[Red](#,##0)";											// Account.
	_formats[39] = L"_(#,##0.00_);(#,##0.00)";											// Account.
	_formats[40] = L"_(#,##0.00_);[Red](#,##0.00)";										// Account.
	_formats[41] = L"_(* #,##0_);_(* (#,##0);_(* \"-\"_);_(@_)";						// Currency
	_formats[42] = L"_($* #,##0_);_($* (#,##0);_($* \"-\"_);_(@_)";						// Currency
	_formats[43] = L"_(* #,##0.00_);_(* (#,##0.00);_(* \"-\"??_);_(@_)";				// Currency
	_formats[44] = L"_($* #,##0.00_);_($* (#,##0.00);_($* \"-\"??_);_(@_)";				// Currency
	_formats[45] = L"mm:ss";															// Time
	_formats[46] = L"[h]:mm:ss";														// Time
	_formats[47] = L"mm:ss.0";															// Time
	_formats[48] = L"##0.0E+0";															// Scientific
	_formats[49] = XLS_FORMAT_TEXT;			// "@"										// Text

	 // overwrite formats from workbook
	size_t maxFormats = xls.workbook_.formats_.size();
	for(size_t i=0; i<maxFormats; ++i) {
		int idx = xls.workbook_.formats_[i].index_;

		_formats[idx] = wstringFromLargeString(xls.workbook_.formats_[i].fmtstring_);

		 // adjust index for the next user defined format
		if (idx >= _next_fmt_idx)
			_next_fmt_idx = idx + 1;
	}

	 // create reverse format map
	for(FormatMap::const_iterator it=_formats.begin(); it!=_formats.end(); ++it)
		_formats_rev[it->second] = it->first;
}


int XLSFormatManager::get_font_idx(const ExcelFont& font)
{
	int i = 0;
	for(vector<Workbook::Font>::const_iterator it=_xls.workbook_.fonts_.begin(); it!=_xls.workbook_.fonts_.end(); ++it,++i)
		if (font.matches(*it))
			return i;

	int font_idx = (int) _xls.workbook_.fonts_.size();
	_xls.workbook_.fonts_.push_back(Workbook::Font());
	Workbook::Font& new_font = _xls.workbook_.fonts_[font_idx];

	new_font.height_ = font._height;
	new_font.options_ = font._options;
	new_font.colourIndex_ = font._color_index;
	new_font.weight_ = font._weight;
	new_font.escapementType_ = font._escapement_type;
	new_font.underlineType_ = font._underline_type;
	new_font.family_ = font._font_family;
	new_font.characterSet_ = font._character_set;

	new_font.name_ = font._name.c_str();

	 // The font with index 4 is omitted in all BIFF versions. This means the first four fonts have zero-based indexes,
	 // and the fifth font and all following fonts are referenced with one-based indexes.
	if (font_idx >= 4)
		++font_idx;

	return font_idx;
}

const Workbook::Font& XLSFormatManager::get_font(const CellFormat& fmt) const
{
	size_t font_idx = fmt.get_font_idx();

	if (font_idx > 4)
		--font_idx;

	if (font_idx < _xls.workbook_.fonts_.size())
		return _xls.workbook_.fonts_[font_idx];
	else {
//		assert(0); // font_idx out of range
		return _xls.workbook_.fonts_[0];
	}
}

wstring XLSFormatManager::get_format_string(const CellFormat& fmt) const
{
	int fmt_idx = fmt.get_fmt_idx();

	FormatMap::const_iterator found = _formats.find(fmt_idx);

	if (found != _formats.end())
		return found->second;
	else
		return XLS_FORMAT_GENERAL;
}

int XLSFormatManager::get_format_idx(const wstring& fmt_str)
{
	FormatRevMap::const_iterator found = _formats_rev.find(fmt_str);

	if (found != _formats_rev.end())
		return found->second;

	 // register a new format string
	int fmt_idx = _next_fmt_idx++;

	_formats[fmt_idx] = fmt_str;
	_formats_rev[fmt_str] = fmt_idx;

	_xls.workbook_.formats_.push_back(Workbook::Format());
	Workbook::Format& format = _xls.workbook_.formats_.back();

	format.index_ = fmt_idx;
	format.fmtstring_ = fmt_str.c_str();

	return fmt_idx;
}

int XLSFormatManager::get_xf_idx(const CellFormat& fmt)
{
	int i = 0;
	for(vector<Workbook::XF>::const_iterator it=_xls.workbook_.XFs_.begin(); it!=_xls.workbook_.XFs_.end(); ++it,++i)
		if (fmt.matches(*it))
			return i;

	 // create a new XF
	int xf_idx = (int) _xls.workbook_.XFs_.size();
	_xls.workbook_.XFs_.push_back(Workbook::XF());
	Workbook::XF& xf = _xls.workbook_.XFs_[xf_idx];

	fmt.get_xf(xf);

	xf.protectionType_ = 0 | (15 << 3);		// cell not locked, formula not hidden, type=Cell XF, parent style 15 (default)
//	xf.usedAttributes_ = (unsigned char)(0x3F << 2);	// XF_USED_ATTRIB: use attributes from parent style: font (0x02), ...

	return xf_idx;
}
