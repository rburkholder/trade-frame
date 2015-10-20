/*
 * Copyright (C) 2012 Advanced Software Engineering Limited.
 *
 * This file is part of the ChartDirector software. Usage of this file is
 * subjected to the ChartDirector license agreement. See the LICENSE.TXT
 * file that comes with the ChartDirector software release, or visit
 * Advanced Software Engineering's web site at www.advsofteng.com or
 * email to support@advsofteng.com for information.
 *
 */

#ifndef BCHARTDIR_HDR
#define BCHARTDIR_HDR

#ifndef CHARTDIR_DLLAPI
#define CHARTDIR_DLLAPI
#endif

#if !defined(_WIN32) && !defined(__cdecl)
#define __cdecl
#endif

extern "C" {


////////////////////////////////////////////////////////////////////////////////////////
//	chartdir.h
////////////////////////////////////////////////////////////////////////////////////////
CHARTDIR_DLLAPI int __cdecl CChart_getVersion();
CHARTDIR_DLLAPI const char * __cdecl CChart_getDescription();
CHARTDIR_DLLAPI const char * __cdecl CChart_getCopyright();
CHARTDIR_DLLAPI void __cdecl CChart_getBootLog(char *buffer);

CHARTDIR_DLLAPI bool __cdecl CChart_testFont(const char *font, int fontIndex, double fontHeight,
		double fontWidth, double angle, char *buffer);
CHARTDIR_DLLAPI void __cdecl CChart_setFontSearchPath(const char *path);

CHARTDIR_DLLAPI bool __cdecl CChart_isLicensed();
CHARTDIR_DLLAPI bool __cdecl CChart_getLicenseAttr(const char *key, char *buffer);
CHARTDIR_DLLAPI bool __cdecl CChart_setLicenseFile(const char *filename, char *buffer);
CHARTDIR_DLLAPI bool __cdecl CChart_setLicenseCode(const char *licCode, char *buffer);

CHARTDIR_DLLAPI double __cdecl CChart_chartTime(int y, int m, int d, int h, int n, int s);
CHARTDIR_DLLAPI double __cdecl CChart_chartTime2(int t);
CHARTDIR_DLLAPI int __cdecl CChart_getChartYMD(double t);

CHARTDIR_DLLAPI int __cdecl CChart_metalColor(int c, int angle);
CHARTDIR_DLLAPI	int __cdecl CChart_encodeFilter(int filterTag, int labelStep, double initialMargin);

CHARTDIR_DLLAPI int __cdecl CChart_glassEffect(int glareSize, int glareDirection, int raisedEffect);
CHARTDIR_DLLAPI	int __cdecl CChart_softLighting(int direction, int raisedEffect);
CHARTDIR_DLLAPI int __cdecl CChart_barLighting(double startBrightness, double endBrightness);
CHARTDIR_DLLAPI	int __cdecl CChart_cylinderEffect(int orientation, double ambientIntensity, 
	double diffuseIntensity, double specularIntensity, int shininess);
CHARTDIR_DLLAPI	int __cdecl CChart_flatBorder(int thickness);
CHARTDIR_DLLAPI int __cdecl CChart_arrowShape(double angle, double widthRatio, double stemWidthRatio, double stemLengthRatio);

CHARTDIR_DLLAPI	double __cdecl CChart_bSearch(const double *a, int len, double v);
CHARTDIR_DLLAPI	int __cdecl CChart_ClearTypeMono(double gamma);
CHARTDIR_DLLAPI	int __cdecl CChart_ClearTypeColor(double gamma);
CHARTDIR_DLLAPI	int __cdecl CChart_phongLighting(double ambientIntensity, double diffuseIntensity, double specularIntensity, int shininess);


////////////////////////////////////////////////////////////////////////////////////////
//	libgraphics.h
////////////////////////////////////////////////////////////////////////////////////////
class TTFTextInternal;
CHARTDIR_DLLAPI void __cdecl CTTFText_destroy(TTFTextInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CTTFText_getWidth(TTFTextInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CTTFText_getHeight(TTFTextInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CTTFText_getLineHeight(TTFTextInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CTTFText_getLineDistance(TTFTextInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CTTFText_draw(TTFTextInternal *ptr, int x, int y, int color, int align);

class DrawAreaInternal;
CHARTDIR_DLLAPI DrawAreaInternal * __cdecl CDrawArea_create();
CHARTDIR_DLLAPI void __cdecl CDrawArea_destroy(DrawAreaInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CDrawArea_enableVectorOutput(DrawAreaInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CDrawArea_setSize(DrawAreaInternal *ptr, int width, int height, int bgColor);
CHARTDIR_DLLAPI void __cdecl CDrawArea_resize(DrawAreaInternal *ptr, int newWidth, int newHeight, int f, double blur);
CHARTDIR_DLLAPI int __cdecl CDrawArea_getWidth(DrawAreaInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CDrawArea_getHeight(DrawAreaInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CDrawArea_setClipRect(DrawAreaInternal *ptr, int left, int top, int right, int bottom);
CHARTDIR_DLLAPI void __cdecl CDrawArea_setBgColor(DrawAreaInternal *ptr, int c);

CHARTDIR_DLLAPI void __cdecl CDrawArea_move(DrawAreaInternal *ptr, double xOffset, double yOffset, int bgColor, int ft, double blur);
CHARTDIR_DLLAPI void __cdecl CDrawArea_rotate(DrawAreaInternal *ptr, double angle, int bgColor, double cx, double cy, int ft, double blur);
CHARTDIR_DLLAPI void __cdecl CDrawArea_hFlip(DrawAreaInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CDrawArea_vFlip(DrawAreaInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CDrawArea_clone(DrawAreaInternal *ptr, DrawAreaInternal *d, int x, int y, int align, int newWidth, int newHeight, int ft, double blur);

CHARTDIR_DLLAPI void __cdecl CDrawArea_pixel(DrawAreaInternal *ptr, int x, int y, int c);
CHARTDIR_DLLAPI int __cdecl CDrawArea_getPixel(DrawAreaInternal *ptr, int x, int y);

CHARTDIR_DLLAPI void __cdecl CDrawArea_hline(DrawAreaInternal *ptr, int x1, int x2, int y, int c);
CHARTDIR_DLLAPI void __cdecl CDrawArea_vline(DrawAreaInternal *ptr, int y1, int y2, int x, int c);
CHARTDIR_DLLAPI void __cdecl CDrawArea_line(DrawAreaInternal *ptr, double x1, double y1, double x2, double y2, int c, int lineWidth);
CHARTDIR_DLLAPI void __cdecl CDrawArea_arc(DrawAreaInternal *ptr, int cx, int cy, int rx, int ry, double a1, double a2, int c);

CHARTDIR_DLLAPI void __cdecl CDrawArea_rect(DrawAreaInternal *ptr, int x1, int y1, int x2, int y2, int edgeColor, int fillColor, int raisedEffect);
CHARTDIR_DLLAPI void __cdecl CDrawArea_polygon2(DrawAreaInternal *ptr, const int *xData, int xLen, const int *yData, int yLen, int edgeColor, int fillColor);
CHARTDIR_DLLAPI void __cdecl CDrawArea_polygon(DrawAreaInternal *ptr, const double *xData, int xLen, const double *yData, int yLen, int edgeColor, int fillColor);
CHARTDIR_DLLAPI void __cdecl CDrawArea_surface(DrawAreaInternal *ptr, double x1, double y1, double x2, double y2, int depthX, int depthY,
	int edgeColor, int fillColor);
CHARTDIR_DLLAPI void __cdecl CDrawArea_sector(DrawAreaInternal *ptr, int cx, int cy, int rx, int ry, double a1, double a2, int edgeColor, int fillColor);
CHARTDIR_DLLAPI void __cdecl CDrawArea_cylinder(DrawAreaInternal *ptr, int cx, int cy, int rx, int ry, double a1, double a2,
	int depthX, int depthY, int edgeColor, int fillColor);
CHARTDIR_DLLAPI void __cdecl CDrawArea_circle(DrawAreaInternal *ptr, int cx, int cy, int rx, int ry, int edgeColor, int fillColor);
CHARTDIR_DLLAPI void __cdecl CDrawArea_ringSector(DrawAreaInternal *ptr, int cx, int cy, int rx, int ry, int rx2, int ry2, double a1, double a2, int edgeColor, int fillColor);
CHARTDIR_DLLAPI void __cdecl CDrawArea_ring(DrawAreaInternal *ptr, int cx, int cy, int rx, int ry, int rx2, int ry2, int edgeColor, int fillColor);

CHARTDIR_DLLAPI void __cdecl CDrawArea_fill(DrawAreaInternal *ptr, int x, int y, int color);
CHARTDIR_DLLAPI void __cdecl CDrawArea_fill2(DrawAreaInternal *ptr, int x, int y, int color, int borderColor);

CHARTDIR_DLLAPI void __cdecl CDrawArea_text2(DrawAreaInternal *ptr, const char *str, const char *font, int fontIndex, double fontHeight, double fontWidth,
	double angle, bool vertical, int x, int y, int color, int align);
CHARTDIR_DLLAPI void __cdecl CDrawArea_text2w(DrawAreaInternal *ptr, const wchar_t *str, const char *font, int fontIndex, double fontHeight, double fontWidth,
	double angle, bool vertical, int x, int y, int color, int align);
CHARTDIR_DLLAPI void __cdecl CDrawArea_text(DrawAreaInternal *ptr, const char *str, const char *font, double fontSize,
	int x, int y, int color);
CHARTDIR_DLLAPI void __cdecl CDrawArea_textw(DrawAreaInternal *ptr, const wchar_t *str, const char *font, double fontSize,
	int x, int y, int color);
CHARTDIR_DLLAPI TTFTextInternal* __cdecl CDrawArea_text4w(DrawAreaInternal *ptr, const wchar_t *text, const char *font, int fontIndex,
	double fontHeight, double fontWidth, double angle, bool vertical);
CHARTDIR_DLLAPI TTFTextInternal* __cdecl CDrawArea_text4(DrawAreaInternal *ptr, const char *text, const char *font, int fontIndex,
	double fontHeight, double fontWidth, double angle, bool vertical);
CHARTDIR_DLLAPI TTFTextInternal* __cdecl CDrawArea_text3(DrawAreaInternal *ptr, const char *str, const char *font, double fontSize);
CHARTDIR_DLLAPI TTFTextInternal* __cdecl CDrawArea_text3w(DrawAreaInternal *ptr, const wchar_t *str, const char *font, double fontSize);
CHARTDIR_DLLAPI void __cdecl CDrawArea_close(DrawAreaInternal *ptr, TTFTextInternal *text);

CHARTDIR_DLLAPI void __cdecl CDrawArea_merge(DrawAreaInternal *ptr, DrawAreaInternal *d, int x, int y, int align, int transparency);
CHARTDIR_DLLAPI void __cdecl CDrawArea_tile(DrawAreaInternal *ptr, DrawAreaInternal *d, int transparency);

CHARTDIR_DLLAPI void __cdecl CDrawArea_setSearchPath(DrawAreaInternal *ptr, const char *path);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_loadGIF(DrawAreaInternal *ptr, const char *filename);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_loadPNG(DrawAreaInternal *ptr, const char *filename);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_loadJPG(DrawAreaInternal *ptr, const char *filename);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_loadWMP(DrawAreaInternal *ptr, const char *filename);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_load(DrawAreaInternal *ptr, const char *filename);

CHARTDIR_DLLAPI void __cdecl CDrawArea_rAffineTransform(DrawAreaInternal *ptr, double a, double b, double c, double d, double e, double f, int bgColor, int ft, double blur);
CHARTDIR_DLLAPI void __cdecl CDrawArea_affineTransform(DrawAreaInternal *ptr, double a, double b, double c, double d, double e, double f, int bgColor, int ft, double blur);
CHARTDIR_DLLAPI void __cdecl CDrawArea_sphereTransform(DrawAreaInternal *ptr, int xDiameter, int yDiameter, int bgColor, int ft, double blur);
CHARTDIR_DLLAPI void __cdecl CDrawArea_hCylinderTransform(DrawAreaInternal *ptr, int yDiameter, int bgColor, int ft, double blur);
CHARTDIR_DLLAPI void __cdecl CDrawArea_vCylinderTransform(DrawAreaInternal *ptr, int xDiameter, int bgColor, int ft, double blur);
CHARTDIR_DLLAPI void __cdecl CDrawArea_vTriangleTransform(DrawAreaInternal *ptr, int tHeight, int bgColor, int ft, double blur);
CHARTDIR_DLLAPI void __cdecl CDrawArea_hTriangleTransform(DrawAreaInternal *ptr, int tWidth, int bgColor, int ft, double blur);
CHARTDIR_DLLAPI void __cdecl CDrawArea_shearTransform(DrawAreaInternal *ptr, double xShear, double yShear, int bgColor, int ft, double blur);
CHARTDIR_DLLAPI void __cdecl CDrawArea_waveTransform(DrawAreaInternal *ptr, int period, double amplitude, double direction, double startAngle, bool longitudinal, int bgColor, int ft, double blur);

CHARTDIR_DLLAPI bool __cdecl CDrawArea_out(DrawAreaInternal *ptr, const char *filename);

CHARTDIR_DLLAPI bool __cdecl CDrawArea_outGIF(DrawAreaInternal *ptr, const char *filename);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outPNG(DrawAreaInternal *ptr, const char *filename);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outJPG(DrawAreaInternal *ptr, const char *filename, int quality);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outWMP(DrawAreaInternal *ptr, const char *filename);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outBMP(DrawAreaInternal *ptr, const char *filename);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outSVG(DrawAreaInternal *ptr, const char *filename, const char *options);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outPDF(DrawAreaInternal *ptr, const char *filename);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outAGF(DrawAreaInternal *ptr, const char *filename);

CHARTDIR_DLLAPI bool __cdecl CDrawArea_outGIF2(DrawAreaInternal *ptr, const char **data, int *len);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outPNG2(DrawAreaInternal *ptr, const char **data, int *len);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outJPG2(DrawAreaInternal *ptr, const char **data, int *len, int quality);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outWMP2(DrawAreaInternal *ptr, const char **data, int *len);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outBMP2(DrawAreaInternal *ptr, const char **data, int *len);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outSVG2(DrawAreaInternal *ptr, const char **data, int *len, const char *options);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outPDF2(DrawAreaInternal *ptr, const char **data, int *len);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_outAGF2(DrawAreaInternal *ptr, const char **data, int *len);

CHARTDIR_DLLAPI void __cdecl CDrawArea_setOutputOptions(DrawAreaInternal *ptr, const char *options);
CHARTDIR_DLLAPI bool __cdecl CDrawArea_cacheOutput(DrawAreaInternal *ptr, const char *id);

CHARTDIR_DLLAPI void __cdecl CDrawArea_setPaletteMode(DrawAreaInternal *ptr, int p);
CHARTDIR_DLLAPI void __cdecl CDrawArea_setDitherMethod(DrawAreaInternal *ptr, int m);
CHARTDIR_DLLAPI void __cdecl CDrawArea_setTransparentColor(DrawAreaInternal *ptr, int c);
CHARTDIR_DLLAPI void __cdecl CDrawArea_setAntiAliasText(DrawAreaInternal *ptr, int a);
CHARTDIR_DLLAPI void __cdecl CDrawArea_setAntiAlias(DrawAreaInternal *ptr, bool shapeAntiAlias, int textAntiAlias);
CHARTDIR_DLLAPI void __cdecl CDrawArea_setInterlace(DrawAreaInternal *ptr, bool i);

CHARTDIR_DLLAPI void __cdecl CDrawArea_setColorTable(DrawAreaInternal *ptr, const int *colors, int noOfColors, int offset);
CHARTDIR_DLLAPI int __cdecl CDrawArea_getARGBColor(DrawAreaInternal *ptr, int c);
CHARTDIR_DLLAPI int __cdecl CDrawArea_halfColor(DrawAreaInternal *ptr, int c);
CHARTDIR_DLLAPI int __cdecl CDrawArea_adjustBrightness(DrawAreaInternal *ptr, int c, double brightness);

CHARTDIR_DLLAPI int __cdecl CDrawArea_dashLineColor(DrawAreaInternal *ptr, int color, int dashPattern);
CHARTDIR_DLLAPI int __cdecl CDrawArea_patternColor(DrawAreaInternal *ptr, const int *cData, int cLen, int h, int startX, int startY);
CHARTDIR_DLLAPI int __cdecl CDrawArea_patternColor2(DrawAreaInternal *ptr, const char *filename, int startX, int startY);
CHARTDIR_DLLAPI int __cdecl CDrawArea_gradientColor2a(DrawAreaInternal *ptr, const int *c, double angle, double scale, int startX, int startY);
CHARTDIR_DLLAPI int __cdecl CDrawArea_gradientColor2(DrawAreaInternal *ptr, const int *cData, int cLen, double angle, double scale, int startX, int startY);
CHARTDIR_DLLAPI int __cdecl CDrawArea_gradientColor(DrawAreaInternal *ptr, int startX, int startY, int endX, int endY, int startColor, int endColor);
CHARTDIR_DLLAPI int __cdecl CDrawArea_linearGradientColor(DrawAreaInternal *ptr, int startX, int startY, int endX, int endY, int startColor, int endColor, bool periodic);
CHARTDIR_DLLAPI int __cdecl CDrawArea_linearGradientColor2(DrawAreaInternal *ptr, int startX, int startY, int endX, int endY, const int *cData, int cLen, bool periodic);
CHARTDIR_DLLAPI int __cdecl CDrawArea_radialGradientColor(DrawAreaInternal *ptr, int cx, int cy, int rx, int ry, int startColor, int endColor, bool periodic);
CHARTDIR_DLLAPI int __cdecl CDrawArea_radialGradientColor2(DrawAreaInternal *ptr, int cx, int cy, int rx, int ry, const int *cData, int cLen, bool periodic);

CHARTDIR_DLLAPI int __cdecl CDrawArea_reduceColors(DrawAreaInternal *ptr, int colorCount, bool blackAndWhite);

CHARTDIR_DLLAPI void __cdecl CDrawArea_setDefaultFonts(DrawAreaInternal *ptr, const char *normal, const char *bold, const char *italic, const char *boldItalic);
CHARTDIR_DLLAPI void __cdecl CDrawArea_setFontTable(DrawAreaInternal *ptr, int index, const char *font);

CHARTDIR_DLLAPI void __cdecl CDrawArea_initDynamicLayer(DrawAreaInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CDrawArea_removeDynamicLayer(DrawAreaInternal *ptr, bool keepOriginal);


////////////////////////////////////////////////////////////////////////////////////////
//	drawobj.h
////////////////////////////////////////////////////////////////////////////////////////
class DrawObjInternal;
CHARTDIR_DLLAPI void __cdecl CDrawObj_paint(DrawObjInternal *ptr, DrawAreaInternal *d);
CHARTDIR_DLLAPI void __cdecl CDrawObj_setZOrder(DrawObjInternal *ptr, int z);

class BoxInternal;
CHARTDIR_DLLAPI DrawObjInternal * __cdecl Box2DrawObj(BoxInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CBox_setPos(BoxInternal *ptr, int x, int y);
CHARTDIR_DLLAPI void __cdecl CBox_setSize(BoxInternal *ptr, int w, int h);
CHARTDIR_DLLAPI void __cdecl CBox_setBackground(BoxInternal *ptr, int color, int edgeColor, int raisedEffect);
CHARTDIR_DLLAPI void __cdecl CBox_setRoundedCorners(BoxInternal *ptr, int r1, int r2, int r3, int r4);

CHARTDIR_DLLAPI int __cdecl CBox_getLeftX(BoxInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CBox_getTopY(BoxInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CBox_getWidth(BoxInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CBox_getHeight(BoxInternal *ptr);
CHARTDIR_DLLAPI const char * __cdecl CBox_getImageCoor(BoxInternal *ptr, int offsetX, int offsetY);

class TextBoxInternal;
CHARTDIR_DLLAPI BoxInternal * __cdecl TextBox2Box(TextBoxInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CTextBox_setText(TextBoxInternal *ptr, const char *text);
CHARTDIR_DLLAPI void __cdecl CTextBox_setAlignment(TextBoxInternal *ptr, int a);
CHARTDIR_DLLAPI void __cdecl CTextBox_setFontStyle(TextBoxInternal *ptr, const char *font, int fontIndex);
CHARTDIR_DLLAPI void __cdecl CTextBox_setFontSize(TextBoxInternal *ptr, double fontHeight, double fontWidth);
CHARTDIR_DLLAPI void __cdecl CTextBox_setFontAngle(TextBoxInternal *ptr, double angle, bool vertical);
CHARTDIR_DLLAPI void __cdecl CTextBox_setFontColor(TextBoxInternal *ptr, int color);
CHARTDIR_DLLAPI void __cdecl CTextBox_setMargin2(TextBoxInternal *ptr, int leftMargin, int rightMargin, int topMargin, int bottomMargin);
CHARTDIR_DLLAPI void __cdecl CTextBox_setMargin(TextBoxInternal *ptr, int m);
CHARTDIR_DLLAPI void __cdecl CTextBox_setWidth(TextBoxInternal *ptr, int width);	
CHARTDIR_DLLAPI void __cdecl CTextBox_setHeight(TextBoxInternal *ptr, int height);
CHARTDIR_DLLAPI void __cdecl CTextBox_setMaxWidth(TextBoxInternal *ptr, int maxWidth);
CHARTDIR_DLLAPI void __cdecl CTextBox_setTruncate(TextBoxInternal *ptr, int maxWidth, int maxLines);

class LineInternal;
CHARTDIR_DLLAPI DrawObjInternal * __cdecl Line2DrawObj(LineInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CLine_setPos(LineInternal *ptr, int x1, int y1, int x2, int y2);
CHARTDIR_DLLAPI void __cdecl CLine_setColor(LineInternal *ptr, int c);
CHARTDIR_DLLAPI void __cdecl CLine_setWidth(LineInternal *ptr, int w);

class CDMLTableInternal;
CHARTDIR_DLLAPI DrawObjInternal * __cdecl CDMLTable2DrawObj(CDMLTableInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CCDMLTable_setPos(CDMLTableInternal *ptr, int x, int y, int alignment);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CCDMLTable_insertCol(CDMLTableInternal *ptr, int col);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CCDMLTable_appendCol(CDMLTableInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CCDMLTable_getColCount(CDMLTableInternal *ptr);
	
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CCDMLTable_insertRow(CDMLTableInternal *ptr, int row); 
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CCDMLTable_appendRow(CDMLTableInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CCDMLTable_getRowCount(CDMLTableInternal *ptr);

CHARTDIR_DLLAPI TextBoxInternal * __cdecl CCDMLTable_setText(CDMLTableInternal *ptr, int col, int row, const char *text);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CCDMLTable_setCell(CDMLTableInternal *ptr, int col, int row, int width, int height, const char *text);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CCDMLTable_getCell(CDMLTableInternal *ptr, int col, int row);

CHARTDIR_DLLAPI TextBoxInternal * __cdecl CCDMLTable_getColStyle(CDMLTableInternal *ptr, int col);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CCDMLTable_getRowStyle(CDMLTableInternal *ptr, int row);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CCDMLTable_getStyle(CDMLTableInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CCDMLTable_layout(CDMLTableInternal *ptr);
	
CHARTDIR_DLLAPI int __cdecl CCDMLTable_getColWidth(CDMLTableInternal *ptr, int col);
CHARTDIR_DLLAPI int __cdecl CCDMLTable_getRowHeight(CDMLTableInternal *ptr, int row);
CHARTDIR_DLLAPI int __cdecl CCDMLTable_getWidth(CDMLTableInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CCDMLTable_getHeight(CDMLTableInternal *ptr);


////////////////////////////////////////////////////////////////////////////////////////
//	basechart.h
////////////////////////////////////////////////////////////////////////////////////////
class LegendBoxInternal;
CHARTDIR_DLLAPI TextBoxInternal * __cdecl LegendBox2TextBox(LegendBoxInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CLegendBox_setCols(LegendBoxInternal *ptr, int noOfCols);
CHARTDIR_DLLAPI void __cdecl CLegendBox_setReverse(LegendBoxInternal *ptr, bool b);

CHARTDIR_DLLAPI void __cdecl CLegendBox_addKey(LegendBoxInternal *ptr, const char *text, int color, int lineWidth, const DrawAreaInternal *symbol);
CHARTDIR_DLLAPI void __cdecl CLegendBox_addKey2(LegendBoxInternal *ptr, int pos, const char *text, int color, int lineWidth, const DrawAreaInternal *symbol);
CHARTDIR_DLLAPI void __cdecl CLegendBox_setKeySize(LegendBoxInternal *ptr, int width, int height, int gap);
CHARTDIR_DLLAPI void __cdecl CLegendBox_setKeySpacing(LegendBoxInternal *ptr, int keySpacing, int lineSpacing);
CHARTDIR_DLLAPI void __cdecl CLegendBox_setKeyBorder(LegendBoxInternal *ptr, int edgeColor, int raisedEffect);
CHARTDIR_DLLAPI void __cdecl CLegendBox_setLineStyleKey(LegendBoxInternal *ptr, bool b);

CHARTDIR_DLLAPI const char * __cdecl CLegendBox_getImageCoor(LegendBoxInternal *ptr, int dataItem, int offsetX, int offsetY);
CHARTDIR_DLLAPI const char * __cdecl CLegendBox_getHTMLImageMap(LegendBoxInternal *ptr, const char *url, const char *queryFormat, const char *extraAttr, int offsetX, int offsetY);

class BaseChartInternal;
CHARTDIR_DLLAPI void __cdecl CBaseChart_destroy(BaseChartInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CBaseChart_enableVectorOutput(BaseChartInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CBaseChart_setSize(BaseChartInternal *ptr, int width, int height);
CHARTDIR_DLLAPI int __cdecl CBaseChart_getWidth(BaseChartInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CBaseChart_getHeight(BaseChartInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setBackground(BaseChartInternal *ptr, int bgColor, int edgeColor, int raisedEffect);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setBorder(BaseChartInternal *ptr, int color);	//deprecated - use setBackground
CHARTDIR_DLLAPI void __cdecl CBaseChart_setRoundedFrame(BaseChartInternal *ptr, int extColor, int r1, int r2, int r3, int r4);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setThickFrame(BaseChartInternal *ptr, int thickness, int frameColor, int outerEdgeColor, int innerEdgeColor);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setWallpaper(BaseChartInternal *ptr, const char *img);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setBgImage(BaseChartInternal *ptr, const char *img, int align);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setDropShadow(BaseChartInternal *ptr, int color, int offsetX, int offsetY, int blurRadius);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setThickFrame(BaseChartInternal *ptr, int thickness, int frameColor, int outerEdgeColor, int innerEdgeColor);

CHARTDIR_DLLAPI void __cdecl CBaseChart_setTransparentColor(BaseChartInternal *ptr, int c);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setAntiAlias(BaseChartInternal *ptr, bool antiAliasShape, int antiAliasText);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setSearchPath(BaseChartInternal *ptr, const char *path);

CHARTDIR_DLLAPI TextBoxInternal * __cdecl CBaseChart_addTitle2(BaseChartInternal *ptr, int alignment, const char *text, const char *font, double fontSize, int fontColor, int bgColor, int edgeColor);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CBaseChart_addTitle(BaseChartInternal *ptr, const char *text, const char *font, double fontSize, int fontColor, int bgColor, int edgeColor);

CHARTDIR_DLLAPI LegendBoxInternal * __cdecl CBaseChart_addLegend(BaseChartInternal *ptr, int x, int y, bool vertical, const char *font, double fontHeight);
CHARTDIR_DLLAPI LegendBoxInternal * __cdecl CBaseChart_addLegend2(BaseChartInternal *ptr, int x, int y, int noOfCols, const char *font, double fontHeight);
CHARTDIR_DLLAPI LegendBoxInternal * __cdecl CBaseChart_getLegend(BaseChartInternal *ptr);

CHARTDIR_DLLAPI DrawAreaInternal * __cdecl CBaseChart_getDrawArea(BaseChartInternal *ptr);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CBaseChart_addText(BaseChartInternal *ptr, int x, int y, const char *text, const char *font, double fontSize, int fontColor, int alignment, double angle, bool vertical);
CHARTDIR_DLLAPI LineInternal * __cdecl CBaseChart_addLine(BaseChartInternal *ptr, int x1, int y1, int x2, int y2, int color, int lineWidth);
CHARTDIR_DLLAPI CDMLTableInternal * __cdecl CBaseChart_addTable(BaseChartInternal *ptr, int x, int y, int alignment, int col, int row);
CHARTDIR_DLLAPI void __cdecl CBaseChart_addExtraField(BaseChartInternal *ptr, const char * const * strData, int arrayLen);
CHARTDIR_DLLAPI void __cdecl CBaseChart_addExtraField2(BaseChartInternal *ptr, const double *dblData, int arrayLen);

CHARTDIR_DLLAPI void __cdecl CBaseChart_setColors(BaseChartInternal *ptr, int paletteEntry, const int *colorsData, int colorsLen);
CHARTDIR_DLLAPI int __cdecl CBaseChart_getColor(BaseChartInternal *ptr, int paletteEntry);
CHARTDIR_DLLAPI int __cdecl CBaseChart_halfColor(BaseChartInternal *ptr, int c);
CHARTDIR_DLLAPI int __cdecl CBaseChart_adjustBrightness(BaseChartInternal *ptr, int c, double brightness);

CHARTDIR_DLLAPI int __cdecl CBaseChart_dashLineColor(BaseChartInternal *ptr, int color, int patternCode);
CHARTDIR_DLLAPI int __cdecl CBaseChart_patternColor(BaseChartInternal *ptr, const int *cData, int cLen, int h, int startX, int startY);
CHARTDIR_DLLAPI int __cdecl CBaseChart_patternColor2(BaseChartInternal *ptr, const char *filename, int startX, int startY);
CHARTDIR_DLLAPI int __cdecl CBaseChart_gradientColor2a(BaseChartInternal *ptr, const int *c, double angle, double scale, int startX, int startY);
CHARTDIR_DLLAPI int __cdecl CBaseChart_gradientColor2(BaseChartInternal *ptr, const int *cData, int cLen, double angle, double scale, int startX, int startY);
CHARTDIR_DLLAPI int __cdecl CBaseChart_gradientColor(BaseChartInternal *ptr, int startX, int startY, int endX, int endY, int startColor, int endColor);
CHARTDIR_DLLAPI int __cdecl CBaseChart_linearGradientColor(BaseChartInternal *ptr, int startX, int startY, int endX, int endY, int startColor, int endColor, bool periodic);
CHARTDIR_DLLAPI int __cdecl CBaseChart_linearGradientColor2(BaseChartInternal *ptr, int startX, int startY, int endX, int endY, const int *cData, int cLen, bool periodic);
CHARTDIR_DLLAPI int __cdecl CBaseChart_radialGradientColor(BaseChartInternal *ptr, int cx, int cy, int rx, int ry, int startColor, int endColor, bool periodic);
CHARTDIR_DLLAPI int __cdecl CBaseChart_radialGradientColor2(BaseChartInternal *ptr, int cx, int cy, int rx, int ry, const int *cData, int cLen, bool periodic);

CHARTDIR_DLLAPI void __cdecl CBaseChart_setDefaultFonts(BaseChartInternal *ptr, const char *normal, const char *bold, const char *italic, const char *boldItalic);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setFontTable(BaseChartInternal *ptr, int index, const char *font);

CHARTDIR_DLLAPI void __cdecl CBaseChart_setNumberFormat(BaseChartInternal *ptr, char thousandSeparator, char decimalPointChar, char signChar);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setMonthNames(BaseChartInternal *ptr, const char * const *namesData, int namesLen);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setWeekDayNames(BaseChartInternal *ptr, const char * const *namesData, int namesLen);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setAMPM(BaseChartInternal *ptr, const char *AM, const char *PM);

CHARTDIR_DLLAPI const char * __cdecl CBaseChart_formatValue(BaseChartInternal *ptr, double value, const char *formatString);

CHARTDIR_DLLAPI LegendBoxInternal * __cdecl CBaseChart_layoutLegend(BaseChartInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CBaseChart_layout(BaseChartInternal *ptr);
CHARTDIR_DLLAPI bool __cdecl CBaseChart_makeChart(BaseChartInternal *ptr, const char *filename);
CHARTDIR_DLLAPI bool __cdecl CBaseChart_makeChart2(BaseChartInternal *ptr, int format, const char **data, int *len);
CHARTDIR_DLLAPI DrawAreaInternal * __cdecl CBaseChart_makeChart3(BaseChartInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CBaseChart_setOutputOptions(BaseChartInternal *ptr, const char *options);

CHARTDIR_DLLAPI const char * __cdecl CBaseChart_getHTMLImageMap(BaseChartInternal *ptr, const char *url, const char *queryFormat, const char *extraAttr, int offsetX, int offsetY);
CHARTDIR_DLLAPI const char * __cdecl CBaseChart_getChartMetrics(BaseChartInternal *ptr);

class MultiChartInternal;
CHARTDIR_DLLAPI BaseChartInternal * __cdecl MultiChart2BaseChart(MultiChartInternal *ptr);
CHARTDIR_DLLAPI MultiChartInternal * __cdecl CMultiChart_create(int width, int height, int bgColor, int edgeColor, int raisedEffect);
CHARTDIR_DLLAPI void __cdecl CMultiChart_addChart(MultiChartInternal *ptr, int x, int y, BaseChartInternal *c);
CHARTDIR_DLLAPI void __cdecl CMultiChart_setMainChart(MultiChartInternal *ptr, const BaseChartInternal *c);

CHARTDIR_DLLAPI int __cdecl CBaseChart_getAbsOffsetX(BaseChartInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CBaseChart_getAbsOffsetY(BaseChartInternal *ptr);
CHARTDIR_DLLAPI DrawAreaInternal *__cdecl CBaseChart_initDynamicLayer(BaseChartInternal *ptr) ;
CHARTDIR_DLLAPI void __cdecl CBaseChart_removeDynamicLayer(BaseChartInternal *ptr);
CHARTDIR_DLLAPI const char * __cdecl CBaseChart_getJsChartModel(BaseChartInternal *ptr, const char *options);


////////////////////////////////////////////////////////////////////////////////////////
//	piechart.h
////////////////////////////////////////////////////////////////////////////////////////
class SectorInternal;
CHARTDIR_DLLAPI void __cdecl CSector_setExplode(SectorInternal *ptr, int distance);
CHARTDIR_DLLAPI void __cdecl CSector_setStyle(SectorInternal *ptr, int shadingMethod, int edgeColor, int edgeWidth);
CHARTDIR_DLLAPI void __cdecl CSector_setLabelFormat(SectorInternal *ptr, const char *formatString);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CSector_setLabelStyle(SectorInternal *ptr, const char *font, double fontSize, int fontColor);
CHARTDIR_DLLAPI void __cdecl CSector_setLabelPos(SectorInternal *ptr, int pos, int joinLineColor);
CHARTDIR_DLLAPI void __cdecl CSector_setLabelLayout(SectorInternal *ptr, int layoutMethod, int pos);
CHARTDIR_DLLAPI void __cdecl CSector_setJoinLine(SectorInternal *ptr, int joinLineColor, int joinLineWidth);
CHARTDIR_DLLAPI void __cdecl CSector_setColor(SectorInternal *ptr, int color, int edgeColor, int joinLineColor);
CHARTDIR_DLLAPI const char * __cdecl CSector_getImageCoor(SectorInternal *ptr, int offsetX, int offsetY);
CHARTDIR_DLLAPI const char * __cdecl CSector_getLabelCoor(SectorInternal *ptr, int offsetX, int offsetY);

class PieChartInternal;
CHARTDIR_DLLAPI BaseChartInternal * __cdecl PieChart2BaseChart(PieChartInternal *ptr);
CHARTDIR_DLLAPI PieChartInternal * __cdecl CPieChart_create(int width, int height, int bgColor, int edgeColor, int raisedEffect);

CHARTDIR_DLLAPI void __cdecl CPieChart_setPieSize(PieChartInternal *ptr, int x, int y, int r);
CHARTDIR_DLLAPI void __cdecl CPieChart_setDonutSize(PieChartInternal *ptr, int x, int y, int r, int r2);
CHARTDIR_DLLAPI void __cdecl CPieChart_set3D(PieChartInternal *ptr, int depth, double angle, bool shadowMode);
CHARTDIR_DLLAPI void __cdecl CPieChart_set3D2(PieChartInternal *ptr, const double *depthsData, int depthsLen, double angle, bool shadowMode);
CHARTDIR_DLLAPI void __cdecl CPieChart_setSectorStyle(PieChartInternal *ptr, int shadingMethod, int edgeColor, int edgeWidth);
CHARTDIR_DLLAPI void __cdecl CPieChart_setStartAngle(PieChartInternal *ptr, double startAngle, bool clockWise);
CHARTDIR_DLLAPI void __cdecl CPieChart_setExplode(PieChartInternal *ptr, int sectorNo, int distance);
CHARTDIR_DLLAPI void __cdecl CPieChart_setExplodeGroup(PieChartInternal *ptr, int startSector, int endSector, int distance);

CHARTDIR_DLLAPI void __cdecl CPieChart_setLabelFormat(PieChartInternal *ptr, const char *formatString);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CPieChart_setLabelStyle(PieChartInternal *ptr, const char *font, double fontSize, int fontColor);
CHARTDIR_DLLAPI void __cdecl CPieChart_setLabelPos(PieChartInternal *ptr, int pos, int joinLineColor);
CHARTDIR_DLLAPI void __cdecl CPieChart_setLabelLayout(PieChartInternal *ptr, int layoutMethod, int pos, int topBound, int bottomBound);
CHARTDIR_DLLAPI void __cdecl CPieChart_setJoinLine(PieChartInternal *ptr, int joinLineColor, int joinLineWidth);
CHARTDIR_DLLAPI void __cdecl CPieChart_setLineColor(PieChartInternal *ptr, int edgeColor, int joinLineColor);

CHARTDIR_DLLAPI void __cdecl CPieChart_setData(PieChartInternal *ptr, const double *dataData, int dataLen, const char * const * labelsData, int labelsLen);

CHARTDIR_DLLAPI SectorInternal * __cdecl CPieChart_sector(PieChartInternal *ptr, int sectorNo);


////////////////////////////////////////////////////////////////////////////////////////
//	axis.h
////////////////////////////////////////////////////////////////////////////////////////
class MarkInternal;
class LineObjInternal;
CHARTDIR_DLLAPI TextBoxInternal * __cdecl Mark2TextBox(MarkInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CMark_setValue(MarkInternal *ptr, double value);
CHARTDIR_DLLAPI void __cdecl CMark_setMarkColor(MarkInternal *ptr, int lineColor, int textColor, int tickColor);
CHARTDIR_DLLAPI void __cdecl CMark_setLineWidth(MarkInternal *ptr, int w);
CHARTDIR_DLLAPI void __cdecl CMark_setDrawOnTop(MarkInternal *ptr, bool b);
CHARTDIR_DLLAPI LineObjInternal * __cdecl CMark_getLine(MarkInternal *ptr);

class AxisInternal;
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CAxis_setLabelStyle(AxisInternal *ptr, const char *font, double fontSize, int fontColor, double fontAngle);
CHARTDIR_DLLAPI void __cdecl CAxis_setLabelFormat(AxisInternal *ptr, const char *formatString);

CHARTDIR_DLLAPI void __cdecl CAxis_setMultiFormat(AxisInternal *ptr, int filter1, const char *format1, int filter2, const char *format2, int labelSpan, bool promoteFirst);
CHARTDIR_DLLAPI void __cdecl CAxis_setMultiFormat2(AxisInternal *ptr, int filterId, const char *formatString, int labelSpan, bool promoteFirst);
CHARTDIR_DLLAPI void __cdecl CAxis_setFormatCondition(AxisInternal *ptr, const char *condition, double operand);

CHARTDIR_DLLAPI void __cdecl CAxis_setLabelGap(AxisInternal *ptr, int d);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CAxis_setTitle(AxisInternal *ptr, const char *text, const char *font, double fontSize, int fontColor);
CHARTDIR_DLLAPI void __cdecl CAxis_setTitlePos(AxisInternal *ptr, int alignment, int titleGap);

CHARTDIR_DLLAPI void __cdecl CAxis_setColors(AxisInternal *ptr, int axisColor, int labelColor, int titleColor, int tickColor);

CHARTDIR_DLLAPI void __cdecl CAxis_setTickLength(AxisInternal *ptr, int majorTickLen);
CHARTDIR_DLLAPI void __cdecl CAxis_setTickLength2(AxisInternal *ptr, int majorTickLen, int minorTickLen);
CHARTDIR_DLLAPI void __cdecl CAxis_setTickWidth(AxisInternal *ptr, int majorTickWidth, int minorTickWidth);
CHARTDIR_DLLAPI void __cdecl CAxis_setTickColor(AxisInternal *ptr, int majorTickColor, int minorTickColor);

CHARTDIR_DLLAPI void __cdecl CAxis_setWidth(AxisInternal *ptr, int width);
CHARTDIR_DLLAPI void __cdecl CAxis_setLength(AxisInternal *ptr, int length);
CHARTDIR_DLLAPI void __cdecl CAxis_setOffset(AxisInternal *ptr, int x, int y);
CHARTDIR_DLLAPI void __cdecl CAxis_setAngle(AxisInternal *ptr, double startAngle);
CHARTDIR_DLLAPI void __cdecl CAxis_setMargin(AxisInternal *ptr, int topMargin, int bottomMargin);
CHARTDIR_DLLAPI void __cdecl CAxis_setIndent(AxisInternal *ptr, bool indent);
CHARTDIR_DLLAPI void __cdecl CAxis_setTickOffset(AxisInternal *ptr, double offset);
CHARTDIR_DLLAPI void __cdecl CAxis_setLabelOffset(AxisInternal *ptr, double offset);
CHARTDIR_DLLAPI void __cdecl CAxis_setLabelAlignment(AxisInternal *ptr, int alignment, int minLabelSpace);

CHARTDIR_DLLAPI void __cdecl CAxis_setAutoScale(AxisInternal *ptr, double topExtension, double bottomExtension, double zeroAffinity);
CHARTDIR_DLLAPI void __cdecl CAxis_setRounding(AxisInternal *ptr, bool roundMin, bool roundMax);
CHARTDIR_DLLAPI void __cdecl CAxis_setTickDensity(AxisInternal *ptr, int majorTickSpacing, int minorTickSpacing);
CHARTDIR_DLLAPI void __cdecl CAxis_setReverse(AxisInternal *ptr, bool b);
CHARTDIR_DLLAPI void __cdecl CAxis_setMinTickInc(AxisInternal *ptr, double inc);

CHARTDIR_DLLAPI TextBoxInternal * __cdecl CAxis_setLabels(AxisInternal *ptr, const char * const * labelsData, int labelsLen);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CAxis_setLabels2(AxisInternal *ptr, const double *labelsData, int labelsLen, const char *formatString);

CHARTDIR_DLLAPI CDMLTableInternal * __cdecl CAxis_makeLabelTable(AxisInternal *ptr);
CHARTDIR_DLLAPI CDMLTableInternal * __cdecl CAxis_getLabelTable(AxisInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CAxis_setLabelStep(AxisInternal *ptr, int majorTickStep, int minorTickStep, int majorTickOffset, int minorTickOffset);

CHARTDIR_DLLAPI void __cdecl CAxis_setLinearScale3(AxisInternal *ptr, const char *formatString);
CHARTDIR_DLLAPI void __cdecl CAxis_setLinearScale2(AxisInternal *ptr, double lowerLimit, double upperLimit, const char * const * labelsData, int labelsLen);
CHARTDIR_DLLAPI void __cdecl CAxis_setLinearScale(AxisInternal *ptr, double lowerLimit, double upperLimit, double majorTickInc, double minorTickInc);

CHARTDIR_DLLAPI void __cdecl CAxis_setLogScale3(AxisInternal *ptr, const char *formatString);
CHARTDIR_DLLAPI void __cdecl CAxis_setLogScale2(AxisInternal *ptr, double lowerLimit, double upperLimit, const char * const * labelsData, int labelsLen);
CHARTDIR_DLLAPI void __cdecl CAxis_setLogScale(AxisInternal *ptr, double lowerLimit, double upperLimit, double majorTickInc, double minorTickInc);

CHARTDIR_DLLAPI void __cdecl CAxis_setDateScale3(AxisInternal *ptr, const char *formatString);
CHARTDIR_DLLAPI void __cdecl CAxis_setDateScale2(AxisInternal *ptr, double lowerLimit, double upperLimit, const char * const * labelsData, int labelsLen);
CHARTDIR_DLLAPI void __cdecl CAxis_setDateScale(AxisInternal *ptr, double lowerLimit, double upperLimit, double majorTickInc, double minorTickInc);

CHARTDIR_DLLAPI void __cdecl CAxis_syncAxis(AxisInternal *ptr, const AxisInternal *axis, double slope, double intercept);
CHARTDIR_DLLAPI void __cdecl CAxis_copyAxis(AxisInternal *ptr, const AxisInternal *axis);

CHARTDIR_DLLAPI void __cdecl CAxis_addLabel(AxisInternal *ptr, double pos, const char *label);

CHARTDIR_DLLAPI MarkInternal * __cdecl CAxis_addMark(AxisInternal *ptr, double value, int lineColor, const char *text, const char *font, double fontSize);
CHARTDIR_DLLAPI void __cdecl CAxis_addZone(AxisInternal *ptr, double startValue, double endValue, int color);

CHARTDIR_DLLAPI int __cdecl CAxis_getCoor(AxisInternal *ptr, double v);
CHARTDIR_DLLAPI double __cdecl CAxis_getMinValue(AxisInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CAxis_getMaxValue(AxisInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CAxis_getThickness(AxisInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CAxis_getTicks(AxisInternal *ptr, const double **d, int *len);
CHARTDIR_DLLAPI const char * __cdecl CAxis_getLabel(AxisInternal *ptr, double i);

CHARTDIR_DLLAPI const char * __cdecl CAxis_getAxisImageMap(AxisInternal *ptr, int noOfSegments, int mapWidth, const char *url, const char *queryFormat, const char *extraAttr, int offsetX, int offsetY);
CHARTDIR_DLLAPI const char * __cdecl CAxis_getHTMLImageMap(AxisInternal *ptr, const char *url, const char *queryFormat, const char *extraAttr, int offsetX, int offsetY); 

CHARTDIR_DLLAPI const char * __cdecl CAxis_getFormattedLabel(AxisInternal *ptr, double v, const char *formatString);
CHARTDIR_DLLAPI int __cdecl CAxis_getX(AxisInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CAxis_getY(AxisInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CAxis_getAlignment(AxisInternal *ptr);

typedef AxisInternal RadialAxisInternal;

class AngularAxisInternal;
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CAngularAxis_setLabelStyle(AngularAxisInternal *ptr, const char *font, double fontSize, int fontColor, double fontAngle);
CHARTDIR_DLLAPI void __cdecl CAngularAxis_setLabelGap(AngularAxisInternal *ptr, int d);

CHARTDIR_DLLAPI TextBoxInternal * __cdecl CAngularAxis_setLabels(AngularAxisInternal *ptr, const char * const * labelsData, int labelsLen);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CAngularAxis_setLabels2(AngularAxisInternal *ptr, const double * labelsData, int labelsLen, const char *formatString);
CHARTDIR_DLLAPI void __cdecl CAngularAxis_addLabel(AngularAxisInternal *ptr, double pos, const char *label);

CHARTDIR_DLLAPI void __cdecl CAngularAxis_setLinearScale2(AngularAxisInternal *ptr, double lowerLimit, double upperLimit, const char * const * labelsData, int labelsLen);
CHARTDIR_DLLAPI void __cdecl CAngularAxis_setLinearScale(AngularAxisInternal *ptr, double lowerLimit, double upperLimit, double majorTickInc, double minorTickInc);

CHARTDIR_DLLAPI void __cdecl CAngularAxis_addZone(AngularAxisInternal *ptr, double startValue, double endValue, double startRadius, double endRadius, int fillColor, int edgeColor);
CHARTDIR_DLLAPI void __cdecl CAngularAxis_addZone2(AngularAxisInternal *ptr, double startValue, double endValue, int fillColor, int edgeColor);

CHARTDIR_DLLAPI const char * __cdecl CAngularAxis_getAxisImageMap(AngularAxisInternal *ptr, int noOfSegments, int mapWidth, const char *url, const char *queryFormat, const char *extraAttr, int offsetX, int offsetY);
CHARTDIR_DLLAPI const char * __cdecl CAngularAxis_getHTMLImageMap(AngularAxisInternal *ptr, const char *url, const char *queryFormat, const char *extraAttr, int offsetX, int offsetY); 

class ColorAxisInternal;
CHARTDIR_DLLAPI AxisInternal * __cdecl ColorAxis2Axis(ColorAxisInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CColorAxis_setColorGradient(ColorAxisInternal *ptr, bool isContinuous, const int *colorsData, int colorsLen, int underflowColor, int overflowColor);
CHARTDIR_DLLAPI void __cdecl CColorAxis_setColorScale(ColorAxisInternal *ptr, const double *colorsData, int colorsLen, int underflowColor, int overflowColor);
CHARTDIR_DLLAPI void __cdecl CColorAxis_setAxisPos(ColorAxisInternal *ptr, int x, int y, int alignment);
CHARTDIR_DLLAPI void __cdecl CColorAxis_setLevels(ColorAxisInternal *ptr, int maxLevels);
CHARTDIR_DLLAPI void __cdecl CColorAxis_setCompactAxis(ColorAxisInternal *ptr, bool b);
CHARTDIR_DLLAPI void __cdecl CColorAxis_setAxisBorder(ColorAxisInternal *ptr, int edgeColor, int raisedEffect);
CHARTDIR_DLLAPI void __cdecl CColorAxis_setBoundingBox(ColorAxisInternal *ptr, int fillColor, int edgeColor, int raisedEffect);
CHARTDIR_DLLAPI void __cdecl CColorAxis_setBoxMargin(ColorAxisInternal *ptr, int m);
CHARTDIR_DLLAPI void __cdecl CColorAxis_setBoxMargin2(ColorAxisInternal *ptr, int leftMargin, int rightMargin, int topMargin, int bottomMargin);
CHARTDIR_DLLAPI void __cdecl CColorAxis_setRoundedCorners(ColorAxisInternal *ptr, int r1, int r2, int r3, int r4);
CHARTDIR_DLLAPI int __cdecl CColorAxis_getBoxWidth(ColorAxisInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CColorAxis_getBoxHeight(ColorAxisInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CColorAxis_getColor(ColorAxisInternal *ptr, double z);


////////////////////////////////////////////////////////////////////////////////////////
//	layer.h
////////////////////////////////////////////////////////////////////////////////////////
class DataSetInternal;
CHARTDIR_DLLAPI void __cdecl CDataSet_setData(DataSetInternal *ptr, const double *dataData, int dataLen);
CHARTDIR_DLLAPI void __cdecl CDataSet_setDataName(DataSetInternal *ptr, const char *name);
CHARTDIR_DLLAPI void __cdecl CDataSet_setDataColor(DataSetInternal *ptr, int dataColor, int edgeColor, int shadowColor, int shadowEdgeColor);

CHARTDIR_DLLAPI void __cdecl CDataSet_setDataSymbol2(DataSetInternal *ptr, const char *image);
CHARTDIR_DLLAPI void __cdecl CDataSet_setDataSymbol3(DataSetInternal *ptr, const DrawAreaInternal *image);
CHARTDIR_DLLAPI void __cdecl CDataSet_setDataSymbol(DataSetInternal *ptr, int symbol, int size, int fillColor, int edgeColor, int lineWidth);
CHARTDIR_DLLAPI void __cdecl CDataSet_setDataSymbol4(DataSetInternal *ptr, const int *polygonData, int polygonLen, int size, int fillColor, int edgeColor);
CHARTDIR_DLLAPI void __cdecl CDataSet_setSymbolOffset(DataSetInternal *ptr, int offsetX, int offsetY);

CHARTDIR_DLLAPI void __cdecl CDataSet_setLineWidth(DataSetInternal *ptr, int w);

CHARTDIR_DLLAPI void __cdecl CDataSet_setDataLabelFormat(DataSetInternal *ptr, const char *formatString);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CDataSet_setDataLabelStyle(DataSetInternal *ptr, const char *font, double fontSize, int fontColor, double fontAngle);

CHARTDIR_DLLAPI void __cdecl CDataSet_setUseYAxis2(DataSetInternal *ptr, bool b);
CHARTDIR_DLLAPI void __cdecl CDataSet_setUseYAxis(DataSetInternal *ptr, const AxisInternal* a);

CHARTDIR_DLLAPI double __cdecl CDataSet_getValue(DataSetInternal *ptr, int i);
CHARTDIR_DLLAPI double __cdecl CDataSet_getPosition(DataSetInternal *ptr, int i);
CHARTDIR_DLLAPI const char * __cdecl CDataSet_getDataName(DataSetInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CDataSet_getDataColor(DataSetInternal *ptr);
CHARTDIR_DLLAPI const char * __cdecl CDataSet_getLegendIcon(DataSetInternal *ptr);
CHARTDIR_DLLAPI AxisInternal * __cdecl CDataSet_getUseYAxis(DataSetInternal *ptr);

class LayerInternal;
CHARTDIR_DLLAPI void __cdecl CLayer_moveFront(LayerInternal *ptr, LayerInternal *layer);
CHARTDIR_DLLAPI void __cdecl CLayer_moveBack(LayerInternal *ptr, LayerInternal *layer);

CHARTDIR_DLLAPI void __cdecl CLayer_set3D(LayerInternal *ptr, int d, int zGap);
CHARTDIR_DLLAPI void __cdecl CLayer_setLineWidth(LayerInternal *ptr, int w);
CHARTDIR_DLLAPI void __cdecl CLayer_setBorderColor(LayerInternal *ptr, int color, int raisedEffect);
CHARTDIR_DLLAPI void __cdecl CLayer_setLegend(LayerInternal *ptr, int m);
CHARTDIR_DLLAPI void __cdecl CLayer_setLegendOrder(LayerInternal *ptr, int dataSetOrder, int layerOrder);
CHARTDIR_DLLAPI const char * __cdecl CLayer_getLegendIcon(LayerInternal *ptr, int dataSetNo);

CHARTDIR_DLLAPI void __cdecl CLayer_setDataCombineMethod(LayerInternal *ptr, int m);
CHARTDIR_DLLAPI void __cdecl CLayer_setBaseLine(LayerInternal *ptr, double baseLine);

CHARTDIR_DLLAPI DataSetInternal * __cdecl CLayer_addDataSet(LayerInternal *ptr, const double *dataData, int dataLen, int color, const char *name);
CHARTDIR_DLLAPI void __cdecl CLayer_addDataGroup(LayerInternal *ptr, const char *name);
CHARTDIR_DLLAPI void __cdecl CLayer_addExtraField(LayerInternal *ptr, const char * const * textsData, int textsLen);
CHARTDIR_DLLAPI void __cdecl CLayer_addExtraField2(LayerInternal *ptr, const double *numbersData, int numbersLen);
CHARTDIR_DLLAPI DataSetInternal * __cdecl CLayer_getDataSet(LayerInternal *ptr, int dataSet);
CHARTDIR_DLLAPI void __cdecl CLayer_setUseYAxis2(LayerInternal *ptr, bool b);
CHARTDIR_DLLAPI void __cdecl CLayer_setUseYAxis(LayerInternal *ptr, const AxisInternal* a);

CHARTDIR_DLLAPI void __cdecl CLayer_setXData(LayerInternal *ptr, const double *xDataData, int xDataLen);
CHARTDIR_DLLAPI void __cdecl CLayer_setXData2(LayerInternal *ptr, double minValue, double maxValue);
CHARTDIR_DLLAPI void __cdecl CLayer_alignLayer(LayerInternal *ptr, const LayerInternal *layer, int dataSet);

CHARTDIR_DLLAPI int __cdecl CLayer_getXCoor(LayerInternal *ptr, double v);
CHARTDIR_DLLAPI int __cdecl CLayer_getYCoor(LayerInternal *ptr, double v, bool yAxis);
CHARTDIR_DLLAPI int __cdecl CLayer_getYCoor2(LayerInternal *ptr, double v, const AxisInternal *yAxis);
CHARTDIR_DLLAPI int __cdecl CLayer_xZoneColor(LayerInternal *ptr, double threshold, int belowColor, int aboveColor);
CHARTDIR_DLLAPI int __cdecl CLayer_yZoneColor(LayerInternal *ptr, double threshold, int belowColor, int aboveColor, bool mainAxis);
CHARTDIR_DLLAPI int __cdecl CLayer_yZoneColor2(LayerInternal *ptr, double threshold, int belowColor, int aboveColor, const AxisInternal *yAxis);

CHARTDIR_DLLAPI const char * __cdecl CLayer_getImageCoor(LayerInternal *ptr, int dataSet, int dataItem, int offsetX, int offsetY);
CHARTDIR_DLLAPI const char * __cdecl CLayer_getImageCoor2(LayerInternal *ptr, int dataItem, int offsetX, int offsetY);
CHARTDIR_DLLAPI const char * __cdecl CLayer_getHTMLImageMap(LayerInternal *ptr, const char *url, const char *queryFormat, const char *extraAttr, int offsetX, int offsetY);
CHARTDIR_DLLAPI void __cdecl CLayer_setHTMLImageMap(LayerInternal *ptr, const char *url, const char *queryFormat, const char *extraAttr);

CHARTDIR_DLLAPI void __cdecl CLayer_setDataLabelFormat(LayerInternal *ptr, const char *formatString);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CLayer_setDataLabelStyle(LayerInternal *ptr, const char *font, double fontSize, int fontColor, double fontAngle);

CHARTDIR_DLLAPI void __cdecl CLayer_setAggregateLabelFormat(LayerInternal *ptr, const char *formatString);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CLayer_setAggregateLabelStyle(LayerInternal *ptr, const char *font, double fontSize, int fontColor, double fontAngle);

CHARTDIR_DLLAPI TextBoxInternal * __cdecl CLayer_addCustomDataLabel(LayerInternal *ptr, int dataSet, int dataItem, const char *label, const char *font, double fontSize, int fontColor, double fontAngle);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CLayer_addCustomAggregateLabel(LayerInternal *ptr, int dataItem, const char *label, const char *font, double fontSize, int fontColor, double fontAngle);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CLayer_addCustomGroupLabel(LayerInternal *ptr, int dataGroup, int dataItem, const char *label, const char *font, double fontSize, int fontColor, double fontAngle);

CHARTDIR_DLLAPI DataSetInternal * __cdecl CLayer_getDataSetByZ(LayerInternal *ptr, int i);
CHARTDIR_DLLAPI int __cdecl CLayer_getDataSetCount(LayerInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CLayer_getXPosition(LayerInternal *ptr, int i);
CHARTDIR_DLLAPI double __cdecl CLayer_getNearestXValue(LayerInternal *ptr, double target);
CHARTDIR_DLLAPI int __cdecl CLayer_getXIndexOf(LayerInternal *ptr, double xValue, double tolerance);

////////////////////////////////////////////////////////////////////////////////////////
//	barlayer.h
////////////////////////////////////////////////////////////////////////////////////////
class BarLayerInternal;
CHARTDIR_DLLAPI LayerInternal * __cdecl BarLayer2Layer(BarLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CBarLayer_setBarGap(BarLayerInternal *ptr, double barGap, double subBarGap);
CHARTDIR_DLLAPI void __cdecl CBarLayer_setBarWidth(BarLayerInternal *ptr, int barWidth, int subBarWidth);
CHARTDIR_DLLAPI void __cdecl CBarLayer_setMinLabelSize(BarLayerInternal *ptr, int s);
CHARTDIR_DLLAPI void __cdecl CBarLayer_setMinImageMapSize(BarLayerInternal *ptr, int s);
CHARTDIR_DLLAPI void __cdecl CBarLayer_setBarShape(BarLayerInternal *ptr, int shape, int dataGroup, int dataItem);
CHARTDIR_DLLAPI void __cdecl CBarLayer_setBarShape2(BarLayerInternal *ptr, const int *shapeData, int shapeLen, int dataGroup, int dataItem);
CHARTDIR_DLLAPI void __cdecl CBarLayer_setRoundedCorners(BarLayerInternal *ptr, int r1, int r2, int r3, int r4);
CHARTDIR_DLLAPI void __cdecl CBarLayer_setIconSize(BarLayerInternal *ptr, int height, int width);
CHARTDIR_DLLAPI void __cdecl CBarLayer_setOverlapRatio(BarLayerInternal *ptr, double overlapRatio, bool firstOnTop);


////////////////////////////////////////////////////////////////////////////////////////
//	linelayer.h
////////////////////////////////////////////////////////////////////////////////////////
class InterLineLayerInternal;
CHARTDIR_DLLAPI LayerInternal * __cdecl InterLineLayer2Layer(InterLineLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CInterLineLayer_setGapColor(InterLineLayerInternal *ptr, int gapColor12, int gapColor21);

class LineLayerInternal;
CHARTDIR_DLLAPI LayerInternal * __cdecl LineLayer2Layer(LineLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CLineLayer_setSymbolScale(LineLayerInternal *ptr, const double *zDataXData, int zDataXLen, int scaleTypeX, const double *zDataYData, int zDataYLen, int scaleTypeY);
CHARTDIR_DLLAPI void __cdecl CLineLayer_setGapColor(LineLayerInternal *ptr, int lineColor, int lineWidth);
CHARTDIR_DLLAPI void __cdecl CLineLayer_setImageMapWidth(LineLayerInternal *ptr, int width);
CHARTDIR_DLLAPI LineObjInternal * __cdecl CLineLayer_getLine(LineLayerInternal *ptr, int dataSet);
CHARTDIR_DLLAPI void __cdecl CLineLayer_setFastLineMode(LineLayerInternal *ptr, bool b);

class SplineLayerInternal;
CHARTDIR_DLLAPI LineLayerInternal * __cdecl SplineLayer2LineLayer(SplineLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CSplineLayer_setTension(SplineLayerInternal *ptr, double tension);
CHARTDIR_DLLAPI void __cdecl CSplineLayer_setMonotonicity(SplineLayerInternal *ptr, int m);

class StepLineLayerInternal;
CHARTDIR_DLLAPI LineLayerInternal * __cdecl StepLineLayer2LineLayer(StepLineLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CStepLineLayer_setAlignment(StepLineLayerInternal *ptr, int a);

typedef LineLayerInternal ScatterLayerInternal;


////////////////////////////////////////////////////////////////////////////////////////
//	arealayer.h
////////////////////////////////////////////////////////////////////////////////////////
class AreaLayerInternal;
CHARTDIR_DLLAPI LayerInternal * __cdecl AreaLayer2Layer(AreaLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CAreaLayer_setMinLabelSize(AreaLayerInternal *ptr, int s);
CHARTDIR_DLLAPI void __cdecl CAreaLayer_setGapColor(AreaLayerInternal *ptr, int fillColor);


////////////////////////////////////////////////////////////////////////////////////////
//	hloclayer.h
////////////////////////////////////////////////////////////////////////////////////////
class BaseBoxLayerInternal;
CHARTDIR_DLLAPI LayerInternal * __cdecl BaseBoxLayer2Layer(BaseBoxLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CBaseBoxLayer_setDataGap(BaseBoxLayerInternal *ptr, double gap);
CHARTDIR_DLLAPI void __cdecl CBaseBoxLayer_setDataWidth(BaseBoxLayerInternal *ptr, int width);
CHARTDIR_DLLAPI void __cdecl CBaseBoxLayer_setMinImageMapSize(BaseBoxLayerInternal *ptr, int size);
CHARTDIR_DLLAPI void __cdecl CBaseBoxLayer_setRoundedCorners(BaseBoxLayerInternal *ptr, int r1, int r2, int r3, int r4);

class HLOCLayerInternal;
CHARTDIR_DLLAPI BaseBoxLayerInternal * __cdecl HLOCLayer2BaseBoxLayer(HLOCLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CHLOCLayer_setColorMethod(HLOCLayerInternal *ptr, int colorMethod, int riseColor, int fallColor, double leadValue);

class CandleStickLayerInternal;
CHARTDIR_DLLAPI BaseBoxLayerInternal * __cdecl CandleStickLayer2BaseBoxLayer(CandleStickLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CCandleStickLayer_setColors(CandleStickLayerInternal *ptr, int upFillColor, int upLineColor, int downFillColor, int downLineColor);
CHARTDIR_DLLAPI void __cdecl CCandleStickLayer_setExtraColors(CandleStickLayerInternal *ptr, int upDownFillColor, int upDownLineColor, int downDownFillColor, int downDownLineColor, double leadValue);

class BoxWhiskerLayerInternal;
CHARTDIR_DLLAPI BaseBoxLayerInternal * __cdecl BoxWhiskerLayer2BaseBoxLayer(BoxWhiskerLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CBoxWhiskerLayer_setBoxColors(BoxWhiskerLayerInternal *ptr, const int *colorsData, int colorsLen, const char * const * namesData, int namesLen);
CHARTDIR_DLLAPI void __cdecl CBoxWhiskerLayer_setBoxColor(BoxWhiskerLayerInternal *ptr, int item, int boxColor);
CHARTDIR_DLLAPI void __cdecl CBoxWhiskerLayer_setWhiskerBrightness(BoxWhiskerLayerInternal *ptr, double whiskerBrightness);


////////////////////////////////////////////////////////////////////////////////////////
//	trendlayer.h
////////////////////////////////////////////////////////////////////////////////////////
class TrendLayerInternal;
CHARTDIR_DLLAPI LayerInternal * __cdecl TrendLayer2Layer(TrendLayerInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CTrendLayer_setRegressionType(TrendLayerInternal *ptr, int regressionType);

CHARTDIR_DLLAPI void __cdecl CTrendLayer_addConfidenceBand(TrendLayerInternal *ptr, double confidence, int upperFillColor, int upperEdgeColor, int upperLineWidth, int lowerFillColor, int lowerEdgeColor, int lowerLineWidth);
CHARTDIR_DLLAPI void __cdecl CTrendLayer_addPredictionBand(TrendLayerInternal *ptr, double confidence, int upperFillColor, int upperEdgeColor, int upperLineWidth, int lowerFillColor, int lowerEdgeColor, int lowerLineWidth);

CHARTDIR_DLLAPI double __cdecl CTrendLayer_getSlope(TrendLayerInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CTrendLayer_getIntercept(TrendLayerInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CTrendLayer_getCorrelation(TrendLayerInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CTrendLayer_getStdError(TrendLayerInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CTrendLayer_getCoefficient(TrendLayerInternal *ptr, int i);

CHARTDIR_DLLAPI void __cdecl CTrendLayer_setImageMapWidth(TrendLayerInternal *ptr, int width);
CHARTDIR_DLLAPI LineObjInternal * __cdecl CTrendLayer_getLine(TrendLayerInternal *ptr);


////////////////////////////////////////////////////////////////////////////////////////
//	vectorlayer.h
////////////////////////////////////////////////////////////////////////////////////////
class VectorLayerInternal;
CHARTDIR_DLLAPI LayerInternal * __cdecl VectorLayer2Layer(VectorLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CVectorLayer_setVector(VectorLayerInternal *ptr, const double * lengthsData, int lengthsLen, const double *directionsData, int directionsLen, int lengthScale);
CHARTDIR_DLLAPI void __cdecl CVectorLayer_setArrowHead2(VectorLayerInternal *ptr, const int *polygonData, int polygonLen);
CHARTDIR_DLLAPI void __cdecl CVectorLayer_setArrowHead(VectorLayerInternal *ptr, int width, int height);
CHARTDIR_DLLAPI void __cdecl CVectorLayer_setArrowStem(VectorLayerInternal *ptr, const int *polygonData, int polygonLen);
CHARTDIR_DLLAPI void __cdecl CVectorLayer_setArrowAlignment(VectorLayerInternal *ptr, int alignment);
CHARTDIR_DLLAPI void __cdecl CVectorLayer_setIconSize(VectorLayerInternal *ptr, int height, int width);
CHARTDIR_DLLAPI void __cdecl CVectorLayer_setVectorMargin(VectorLayerInternal *ptr, double startMargin, double endMargin);


////////////////////////////////////////////////////////////////////////////////////////
//	contourlayer.h
////////////////////////////////////////////////////////////////////////////////////////
class ContourLayerInternal;
CHARTDIR_DLLAPI LayerInternal * __cdecl ContourLayer2Layer(ContourLayerInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CContourLayer_setZData(ContourLayerInternal *ptr, const double *zDataData, int zDataLen);
CHARTDIR_DLLAPI void __cdecl CContourLayer_setSmoothInterpolation(ContourLayerInternal *ptr, bool b);
CHARTDIR_DLLAPI void __cdecl CContourLayer_setContourColor(ContourLayerInternal *ptr, int contourColor, int minorContourColor);
CHARTDIR_DLLAPI void __cdecl CContourLayer_setContourWidth(ContourLayerInternal *ptr, int contourWidth, int minorContourWidth);
CHARTDIR_DLLAPI ColorAxisInternal * __cdecl CContourLayer_setColorAxis(ContourLayerInternal *ptr, int x, int y, int alignment, int length, int orientation);
CHARTDIR_DLLAPI ColorAxisInternal * __cdecl CContourLayer_colorAxis(ContourLayerInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CContourLayer_setZBounds(ContourLayerInternal *ptr, double minZ, double maxZ);
CHARTDIR_DLLAPI void __cdecl CContourLayer_setExactContour(ContourLayerInternal *ptr, bool contour, bool markContour);


////////////////////////////////////////////////////////////////////////////////////////
//	xychart.h
////////////////////////////////////////////////////////////////////////////////////////
class PlotAreaInternal;
CHARTDIR_DLLAPI void __cdecl CPlotArea_setBackground(PlotAreaInternal *ptr, int color, int altBgColor, int edgeColor);
CHARTDIR_DLLAPI void __cdecl CPlotArea_setBackground2(PlotAreaInternal *ptr, const char *img, int align);
CHARTDIR_DLLAPI void __cdecl CPlotArea_set4QBgColor(PlotAreaInternal *ptr, int Q1Color, int Q2Color, int Q3Color, int Q4Color, int edgeColor);
CHARTDIR_DLLAPI void __cdecl CPlotArea_setAltBgColor(PlotAreaInternal *ptr, bool horizontal, int color1, int color2, int edgeColor);
CHARTDIR_DLLAPI void __cdecl CPlotArea_setGridColor(PlotAreaInternal *ptr, int hGridColor, int vGridColor, int minorHGridColor, int minorVGridColor);
CHARTDIR_DLLAPI void __cdecl CPlotArea_setGridWidth(PlotAreaInternal *ptr, int hGridWidth, int vGridWidth, int minorHGridWidth, int minorVGridWidth);
CHARTDIR_DLLAPI void __cdecl CPlotArea_setGridAxis(PlotAreaInternal *ptr, const AxisInternal *xGridAxis, const AxisInternal *yGridAxis);
CHARTDIR_DLLAPI void __cdecl CPlotArea_moveGridBefore(PlotAreaInternal *ptr, LayerInternal *layer);

CHARTDIR_DLLAPI int __cdecl CPlotArea_getLeftX(PlotAreaInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CPlotArea_getTopY(PlotAreaInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CPlotArea_getWidth(PlotAreaInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CPlotArea_getHeight(PlotAreaInternal *ptr);

CHARTDIR_DLLAPI int __cdecl CPlotArea_getRightX(PlotAreaInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CPlotArea_getBottomY(PlotAreaInternal *ptr);

class XYChartInternal;
CHARTDIR_DLLAPI BaseChartInternal * __cdecl XYChart2BaseChart(XYChartInternal *ptr);
CHARTDIR_DLLAPI XYChartInternal * __cdecl CXYChart_create(int width, int height, int bgColor,	int edgeColor, int raisedEffect);

CHARTDIR_DLLAPI AxisInternal * __cdecl CXYChart_addAxis(XYChartInternal *ptr, int align, int offset);
CHARTDIR_DLLAPI AxisInternal * __cdecl CXYChart_yAxis(XYChartInternal *ptr);
CHARTDIR_DLLAPI AxisInternal * __cdecl CXYChart_yAxis2(XYChartInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CXYChart_syncYAxis(XYChartInternal *ptr, double slope, double intercept);
CHARTDIR_DLLAPI void __cdecl CXYChart_setYAxisOnRight(XYChartInternal *ptr, bool b);
CHARTDIR_DLLAPI AxisInternal * __cdecl CXYChart_xAxis(XYChartInternal *ptr);
CHARTDIR_DLLAPI AxisInternal * __cdecl CXYChart_xAxis2(XYChartInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CXYChart_setXAxisOnTop(XYChartInternal *ptr, bool b);
CHARTDIR_DLLAPI void __cdecl CXYChart_swapXY(XYChartInternal *ptr, bool b);
CHARTDIR_DLLAPI void __cdecl CXYChart_setAxisAtOrigin(XYChartInternal *ptr, int originMode, int symmetryMode);

CHARTDIR_DLLAPI int __cdecl CXYChart_getXCoor(XYChartInternal *ptr, double v);
CHARTDIR_DLLAPI int __cdecl CXYChart_getYCoor(XYChartInternal *ptr, double v, const AxisInternal *yAxis);
CHARTDIR_DLLAPI int __cdecl CXYChart_xZoneColor(XYChartInternal *ptr, double threshold, int belowColor, int aboveColor);
CHARTDIR_DLLAPI int __cdecl CXYChart_yZoneColor(XYChartInternal *ptr, double threshold, int belowColor, int aboveColor, const AxisInternal *axis);

CHARTDIR_DLLAPI PlotAreaInternal * __cdecl CXYChart_setPlotArea(XYChartInternal *ptr, int x, int y, int width, int height, int bgColor, int altBgColor, int edgeColor, int hGridColor, int vGridColor);
CHARTDIR_DLLAPI PlotAreaInternal * __cdecl CXYChart_getPlotArea(XYChartInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CXYChart_setClipping(XYChartInternal *ptr, int margin);
CHARTDIR_DLLAPI void __cdecl CXYChart_setTrimData(XYChartInternal *ptr, int startPos, int len);

CHARTDIR_DLLAPI BarLayerInternal * __cdecl CXYChart_addBarLayer(XYChartInternal *ptr, const double *dataData, int dataLen, int color, const char *name, int depth);
CHARTDIR_DLLAPI BarLayerInternal * __cdecl CXYChart_addBarLayer3(XYChartInternal *ptr, const double *dataData, int dataLen, const int *colorsData, int colorsLen, const char * const *namesData, int namesLen, int depth);
CHARTDIR_DLLAPI BarLayerInternal * __cdecl CXYChart_addBarLayer2(XYChartInternal *ptr, int dataCombineMethod, int depth);

CHARTDIR_DLLAPI LineLayerInternal * __cdecl CXYChart_addLineLayer(XYChartInternal *ptr, const double *dataData, int dataLen, int color, const char *name, int depth);
CHARTDIR_DLLAPI LineLayerInternal * __cdecl CXYChart_addLineLayer2(XYChartInternal *ptr, int dataCombineMethod, int depth);

CHARTDIR_DLLAPI AreaLayerInternal * __cdecl CXYChart_addAreaLayer(XYChartInternal *ptr, const double *dataData, int dataLen, int color, const char *name, int depth);
CHARTDIR_DLLAPI AreaLayerInternal * __cdecl CXYChart_addAreaLayer2(XYChartInternal *ptr, int dataCombineMethod, int depth);

CHARTDIR_DLLAPI HLOCLayerInternal * __cdecl CXYChart_addHLOCLayer2(XYChartInternal *ptr);
CHARTDIR_DLLAPI HLOCLayerInternal * __cdecl CXYChart_addHLOCLayer(XYChartInternal *ptr, const double *highDataData, int highDataLen,
	const double *lowDataData, int lowDataLen, const double *openDataData, int openDataLen,
	const double *closeDataData, int closeDataLen, int color);
CHARTDIR_DLLAPI HLOCLayerInternal * __cdecl CXYChart_addHLOCLayer3(XYChartInternal *ptr, const double *highDataData, int highDataLen,
	const double *lowDataData, int lowDataLen, const double *openDataData, int openDataLen,
	const double *closeDataData, int closeDataLen, int upColor, int downColor, int colorMode, double leadValue);

CHARTDIR_DLLAPI CandleStickLayerInternal * __cdecl CXYChart_addCandleStickLayer(XYChartInternal *ptr, const double *highDataData, int highDataLen,
	const double *lowDataData, int lowDataLen, const double *openDataData, int openDataLen,
	const double *closeDataData, int closeDataLen, int riseColor, int fallColor, int edgeColor);

CHARTDIR_DLLAPI BoxWhiskerLayerInternal * __cdecl CXYChart_addBoxWhiskerLayer(XYChartInternal *ptr, const double *boxTopData, int boxTopLen,
	const double *boxBottomData, int boxBottomLen, const double *maxDataData, int maxDataLen,
	const double *minDataData, int minDataLen, const double *midDataData, int midDataLen,
	int fillColor, int whiskerColor, int edgeColor);
CHARTDIR_DLLAPI BoxWhiskerLayerInternal * __cdecl CXYChart_addBoxWhiskerLayer2(XYChartInternal *ptr, const double *boxTopData, int boxTopLen,
	const double *boxBottomData, int boxBottomLen, const double *maxDataData, int maxDataLen,
	const double *minDataData, int minDataLen, const double *midDataData, int midDataLen,
	const int *fillColorsData, int fillColorsLen, double whiskerBrightness, const char * const * namesData, int namesLen);
CHARTDIR_DLLAPI BoxWhiskerLayerInternal * __cdecl CXYChart_addBoxLayer(XYChartInternal *ptr, const double *boxTopData, int boxTopLen,
	const double *boxBottomData, int boxBottomLen, int color, const char *name);

CHARTDIR_DLLAPI ScatterLayerInternal * __cdecl CXYChart_addScatterLayer(XYChartInternal *ptr, const double *xDataData, int xDataLen, const double *yDataData, int yDataLen, const char *name,
	int symbol, int symbolSize, int fillColor, int edgeColor);

CHARTDIR_DLLAPI TrendLayerInternal * __cdecl CXYChart_addTrendLayer(XYChartInternal *ptr, const double *dataData, int dataLen, int color, const char *name, int depth);
CHARTDIR_DLLAPI TrendLayerInternal * __cdecl CXYChart_addTrendLayer2(XYChartInternal *ptr, const double *xDataData, int xDataLen, const double *yDataData, int yDataLen, int color, const char *name, int depth);

CHARTDIR_DLLAPI SplineLayerInternal * __cdecl CXYChart_addSplineLayer(XYChartInternal *ptr, const double *dataData, int dataLen, int color, const char *name);
CHARTDIR_DLLAPI StepLineLayerInternal * __cdecl CXYChart_addStepLineLayer(XYChartInternal *ptr, const double *dataData, int dataLen, int color, const char *name);

CHARTDIR_DLLAPI InterLineLayerInternal * __cdecl CXYChart_addInterLineLayer(XYChartInternal *ptr, LineObjInternal *line1, LineObjInternal *line2, int color12, int color21);

CHARTDIR_DLLAPI VectorLayerInternal * __cdecl CXYChart_addVectorLayer(XYChartInternal *ptr, const double *xDataData, int xDataLen,
	const double *yDataData, int yDataLen, const double *lengthsData, int lengthsLen, const double *directionsData, int directionsLen, int lengthScale, int color, const char *name);

CHARTDIR_DLLAPI ContourLayerInternal * __cdecl CXYChart_addContourLayer(XYChartInternal *ptr, const double *xDataData, int xDataLen, 
	const double *yDataData, int yDataLen, const double *zDataData, int zDataLen);

CHARTDIR_DLLAPI void __cdecl CXYChart_layoutAxes(XYChartInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CXYChart_packPlotArea(XYChartInternal *ptr, int leftX, int topY, int rightX, int bottomY, int minWidth, int minHeight);

CHARTDIR_DLLAPI double  __cdecl CXYChart_getXValue(XYChartInternal *ptr, int xCoor);
CHARTDIR_DLLAPI double  __cdecl CXYChart_getNearestXValue(XYChartInternal *ptr, double xCoor);
CHARTDIR_DLLAPI double  __cdecl CXYChart_getYValue(XYChartInternal *ptr, int yCoor, const AxisInternal *yAxis); 
CHARTDIR_DLLAPI LayerInternal * __cdecl CXYChart_getLayer(XYChartInternal *ptr, int i);
CHARTDIR_DLLAPI LayerInternal * __cdecl CXYChart_getLayerByZ(XYChartInternal *ptr, int i);
CHARTDIR_DLLAPI int __cdecl CXYChart_getLayerCount(XYChartInternal *ptr);


////////////////////////////////////////////////////////////////////////////////////////
//	surfacechart.h
////////////////////////////////////////////////////////////////////////////////////////
class ThreeDChartInternal;
CHARTDIR_DLLAPI BaseChartInternal * __cdecl ThreeDChart2BaseChart(ThreeDChartInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CThreeDChart_setPlotRegion(ThreeDChartInternal *ptr, int cx, int cy, int xWidth, int yDepth, int zHeight);
CHARTDIR_DLLAPI void __cdecl CThreeDChart_setViewAngle(ThreeDChartInternal *ptr, double elevation, double rotation, double twist);
CHARTDIR_DLLAPI void __cdecl CThreeDChart_setPerspective(ThreeDChartInternal *ptr, double perspective);

CHARTDIR_DLLAPI AxisInternal * __cdecl CThreeDChart_xAxis(ThreeDChartInternal *ptr);
CHARTDIR_DLLAPI AxisInternal * __cdecl CThreeDChart_yAxis(ThreeDChartInternal *ptr);
CHARTDIR_DLLAPI AxisInternal * __cdecl CThreeDChart_zAxis(ThreeDChartInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CThreeDChart_setZAxisPos(ThreeDChartInternal *ptr, int pos);

CHARTDIR_DLLAPI int __cdecl CThreeDChart_getXCoor(ThreeDChartInternal *ptr, double xValue, double yValue, double zValue);
CHARTDIR_DLLAPI int __cdecl CThreeDChart_getYCoor(ThreeDChartInternal *ptr, double xValue, double yValue, double zValue);

CHARTDIR_DLLAPI ColorAxisInternal * __cdecl CThreeDChart_setColorAxis(ThreeDChartInternal *ptr, int x, int y, int alignment, int length, int orientation);
CHARTDIR_DLLAPI ColorAxisInternal * __cdecl CThreeDChart_colorAxis(ThreeDChartInternal *ptr);
	
CHARTDIR_DLLAPI void __cdecl CThreeDChart_setWallVisibility(ThreeDChartInternal *ptr, bool xyVisible, bool yzVisible, bool zxVisible);
CHARTDIR_DLLAPI void __cdecl CThreeDChart_setWallColor(ThreeDChartInternal *ptr, int xyColor, int yzColor, int zxColor, int borderColor);
CHARTDIR_DLLAPI void __cdecl CThreeDChart_setWallThickness(ThreeDChartInternal *ptr, int xyThickness, int yzThickness, int zxThickness);
CHARTDIR_DLLAPI void __cdecl CThreeDChart_setWallGrid(ThreeDChartInternal *ptr, int majorXGridColor, int majorYGridColor, int majorZGridColor,
	int minorXGridColor, int minorYGridColor, int minorZGridColor);

class SurfaceChartInternal;
CHARTDIR_DLLAPI ThreeDChartInternal * __cdecl SurfaceChart2ThreeDChart(SurfaceChartInternal *ptr);
CHARTDIR_DLLAPI SurfaceChartInternal * __cdecl CSurfaceChart_create(int width, int height, int bgColor, int edgeColor, int raisedEffect);
	
CHARTDIR_DLLAPI void __cdecl CSurfaceChart_setData(SurfaceChartInternal *ptr, const double *xDataData, int xDataLen, 
	const double *yDataData, int yDataLen, const double *zDataData, int zDataLen);
CHARTDIR_DLLAPI void __cdecl CSurfaceChart_setInterpolation(SurfaceChartInternal *ptr, int xSamples, int ySamples, bool isSmooth);
	
CHARTDIR_DLLAPI void __cdecl CSurfaceChart_setLighting(SurfaceChartInternal *ptr, double ambientIntensity, double diffuseIntensity, double specularIntensity, double shininess);
CHARTDIR_DLLAPI void __cdecl CSurfaceChart_setShadingMode(SurfaceChartInternal *ptr, int shadingMode, int wireWidth);

CHARTDIR_DLLAPI void __cdecl CSurfaceChart_setSurfaceAxisGrid(SurfaceChartInternal *ptr, int majorXGridColor, int majorYGridColor, int minorXGridColor, int minorYGridColor);
CHARTDIR_DLLAPI void __cdecl CSurfaceChart_setSurfaceDataGrid(SurfaceChartInternal *ptr, int xGridColor, int yGridColor);
CHARTDIR_DLLAPI void __cdecl CSurfaceChart_setContourColor(SurfaceChartInternal *ptr, int contourColor, int minorContourColor);

CHARTDIR_DLLAPI void __cdecl CSurfaceChart_setBackSideBrightness(SurfaceChartInternal *ptr, double brightness);
CHARTDIR_DLLAPI void __cdecl CSurfaceChart_setBackSideColor(SurfaceChartInternal *ptr, int color);
CHARTDIR_DLLAPI void __cdecl CSurfaceChart_setBackSideLighting(SurfaceChartInternal *ptr, double ambientLight, double diffuseLight, double specularLight, double shininess);

class ThreeDScatterGroupInternal;
CHARTDIR_DLLAPI void __cdecl CThreeDScatterGroup_setDataSymbol(ThreeDScatterGroupInternal *ptr, int symbol, int size, int fillColor, int edgeColor, int lineWidth);
CHARTDIR_DLLAPI void __cdecl CThreeDScatterGroup_setDataSymbol2(ThreeDScatterGroupInternal *ptr, const char *image);
CHARTDIR_DLLAPI void __cdecl CThreeDScatterGroup_setDataSymbol3(ThreeDScatterGroupInternal *ptr, const DrawAreaInternal *image);
CHARTDIR_DLLAPI void __cdecl CThreeDScatterGroup_setDataSymbol4(ThreeDScatterGroupInternal *ptr, const int *polygonData, int polygonLen, int size, int fillColor, int edgeColor);
CHARTDIR_DLLAPI void __cdecl CThreeDScatterGroup_setSymbolOffset(ThreeDScatterGroupInternal *ptr, int offsetX, int offsetY);
CHARTDIR_DLLAPI void __cdecl CThreeDScatterGroup_setDropLine(ThreeDScatterGroupInternal *ptr, int dropLineColor, int dropLineWidth);
CHARTDIR_DLLAPI void __cdecl CThreeDScatterGroup_setLegendIcon(ThreeDScatterGroupInternal *ptr, int width, int height, int color);

class ThreeDScatterChartInternal;
CHARTDIR_DLLAPI ThreeDChartInternal * __cdecl ThreeDScatterChart2ThreeDChart(ThreeDScatterChartInternal *ptr);
CHARTDIR_DLLAPI ThreeDScatterChartInternal * __cdecl CThreeDScatterChart_create(int width, int height, int bgColor, int edgeColor, int raisedEffect);
CHARTDIR_DLLAPI ThreeDScatterGroupInternal * __cdecl CThreeDScatterChart_addScatterGroup(ThreeDScatterChartInternal *ptr, const double *xData, int xLen, 
	const double *yData, int yLen, const double *zData, int zLen, const char *name, int symbol, int symbolSize, int fillColor, int edgeColor);


////////////////////////////////////////////////////////////////////////////////////////
//	polarchart.h
////////////////////////////////////////////////////////////////////////////////////////
class PolarLayerInternal;
CHARTDIR_DLLAPI void __cdecl CPolarLayer_setData(PolarLayerInternal *ptr, const double *dataData, int dataLen, int color, const char *name);
CHARTDIR_DLLAPI void __cdecl CPolarLayer_setAngles(PolarLayerInternal *ptr, const double *anglesData, int anglesLen);
CHARTDIR_DLLAPI void __cdecl CPolarLayer_setBorderColor(PolarLayerInternal *ptr, int edgeColor);
CHARTDIR_DLLAPI void __cdecl CPolarLayer_setLineWidth(PolarLayerInternal *ptr, int w);
CHARTDIR_DLLAPI void __cdecl CPolarLayer_setDataSymbol2(PolarLayerInternal *ptr, const char *image);
CHARTDIR_DLLAPI void __cdecl CPolarLayer_setDataSymbol3(PolarLayerInternal *ptr, const DrawAreaInternal *image);
CHARTDIR_DLLAPI void __cdecl CPolarLayer_setDataSymbol(PolarLayerInternal *ptr, int symbol, int size, int fillColor, int edgeColor, int lineWidth);
CHARTDIR_DLLAPI void __cdecl CPolarLayer_setDataSymbol4(PolarLayerInternal *ptr, const int *polygonData, int polygonLen, int size, int fillColor, int edgeColor);
CHARTDIR_DLLAPI void __cdecl CPolarLayer_setSymbolScale(PolarLayerInternal *ptr, const double *zDataData, int zDataLen, int scaleType);
CHARTDIR_DLLAPI void __cdecl CPolarLayer_setSymbolOffset(PolarLayerInternal *ptr, int offsetX, int offsetY);
CHARTDIR_DLLAPI void __cdecl CPolarLayer_setImageMapWidth(PolarLayerInternal *ptr, int width);
CHARTDIR_DLLAPI const char * __cdecl CPolarLayer_getImageCoor(PolarLayerInternal *ptr, int dataItem, int offsetX, int offsetY);
CHARTDIR_DLLAPI const char * __cdecl CPolarLayer_getHTMLImageMap(PolarLayerInternal *ptr, const char *url, const char *queryFormat, const char *extraAttr, int offsetX, int offsetY);
CHARTDIR_DLLAPI void __cdecl CPolarLayer_setHTMLImageMap(PolarLayerInternal *ptr, const char *url, const char *queryFormat, const char *extraAttr);
CHARTDIR_DLLAPI void __cdecl CPolarLayer_setDataLabelFormat(PolarLayerInternal *ptr, const char *formatString);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CPolarLayer_setDataLabelStyle(PolarLayerInternal *ptr, const char *font, double fontSize, int fontColor, double fontAngle);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CPolarLayer_addCustomDataLabel(PolarLayerInternal *ptr, int i, const char *label, const char *font, double fontSize, int fontColor, double fontAngle);

class PolarAreaLayerInternal;
CHARTDIR_DLLAPI PolarLayerInternal * __cdecl PolarAreaLayer2PolarLayer(PolarAreaLayerInternal *ptr);

class PolarLineLayerInternal;
CHARTDIR_DLLAPI PolarLayerInternal * __cdecl PolarLineLayer2PolarLayer(PolarLineLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CPolarLineLayer_setCloseLoop(PolarLineLayerInternal *ptr, bool b);
CHARTDIR_DLLAPI void __cdecl CPolarLineLayer_setGapColor(PolarLineLayerInternal *ptr, int lineColor, int lineWidth);

class PolarSplineLineLayerInternal;
CHARTDIR_DLLAPI PolarLineLayerInternal * __cdecl PolarSplineLineLayer2PolarLineLayer(PolarSplineLineLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CPolarSplineLineLayer_setTension(PolarSplineLineLayerInternal *ptr, double tension);

class PolarSplineAreaLayerInternal;
CHARTDIR_DLLAPI PolarAreaLayerInternal * __cdecl PolarSplineAreaLayer2PolarAreaLayer(PolarSplineAreaLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CPolarSplineAreaLayer_setTension(PolarSplineAreaLayerInternal *ptr, double tension);

class PolarVectorLayerInternal;
CHARTDIR_DLLAPI PolarLayerInternal * __cdecl PolarVectorLayer2PolarLayer(PolarVectorLayerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CPolarVectorLayer_setVector(PolarVectorLayerInternal *ptr, const double * lengthsData, int lengthsLen, const double *directionsData, int directionsLen, int lengthScale);
CHARTDIR_DLLAPI void __cdecl CPolarVectorLayer_setArrowHead2(PolarVectorLayerInternal *ptr, const int *polygonData, int polygonLen);
CHARTDIR_DLLAPI void __cdecl CPolarVectorLayer_setArrowHead(PolarVectorLayerInternal *ptr, int width, int height);
CHARTDIR_DLLAPI void __cdecl CPolarVectorLayer_setArrowStem(PolarVectorLayerInternal *ptr, const int *polygonData, int polygonLen);
CHARTDIR_DLLAPI void __cdecl CPolarVectorLayer_setArrowAlignment(PolarVectorLayerInternal *ptr, int alignment);
CHARTDIR_DLLAPI void __cdecl CPolarVectorLayer_setIconSize(PolarVectorLayerInternal *ptr, int height, int width);
CHARTDIR_DLLAPI void __cdecl CPolarVectorLayer_setVectorMargin(PolarVectorLayerInternal *ptr, double startMargin, double endMargin);

class PolarChartInternal;
CHARTDIR_DLLAPI BaseChartInternal * __cdecl PolarChart2BaseChart(PolarChartInternal *ptr);
CHARTDIR_DLLAPI PolarChartInternal * __cdecl CPolarChart_create(int width, int height, int bgColor, int edgeColor, int raisedEffect);

CHARTDIR_DLLAPI void __cdecl CPolarChart_setPlotArea(PolarChartInternal *ptr, int x, int y, int r, int bgColor, int edgeColor, int edgeWidth);
CHARTDIR_DLLAPI void __cdecl CPolarChart_setPlotAreaBg(PolarChartInternal *ptr, int bgColor1, int bgColor2, bool altRings);
CHARTDIR_DLLAPI void __cdecl CPolarChart_setGridColor(PolarChartInternal *ptr, int rGridColor, int rGridWidth, int aGridColor, int aGridWidth);
CHARTDIR_DLLAPI void __cdecl CPolarChart_setGridStyle(PolarChartInternal *ptr, bool polygonGrid, bool gridOnTop);
CHARTDIR_DLLAPI void __cdecl CPolarChart_setStartAngle(PolarChartInternal *ptr, double startAngle, bool clockwise);

CHARTDIR_DLLAPI AngularAxisInternal * __cdecl CPolarChart_angularAxis(PolarChartInternal *ptr);
CHARTDIR_DLLAPI RadialAxisInternal * __cdecl CPolarChart_radialAxis(PolarChartInternal *ptr);

CHARTDIR_DLLAPI int __cdecl CPolarChart_getXCoor(PolarChartInternal *ptr, double r, double a);
CHARTDIR_DLLAPI int __cdecl CPolarChart_getYCoor(PolarChartInternal *ptr, double r, double a);

CHARTDIR_DLLAPI PolarAreaLayerInternal * __cdecl CPolarChart_addAreaLayer(PolarChartInternal *ptr, const double *dataData, int dataLen, int color, const char *name);
CHARTDIR_DLLAPI PolarLineLayerInternal * __cdecl CPolarChart_addLineLayer(PolarChartInternal *ptr, const double *dataData, int dataLen, int color, const char *name);
CHARTDIR_DLLAPI PolarSplineLineLayerInternal * __cdecl CPolarChart_addSplineLineLayer(PolarChartInternal *ptr, const double *dataData, int dataLen, int color, const char *name);
CHARTDIR_DLLAPI PolarSplineAreaLayerInternal * __cdecl CPolarChart_addSplineAreaLayer(PolarChartInternal *ptr, const double *dataData, int dataLen, int color, const char *name);
CHARTDIR_DLLAPI PolarVectorLayerInternal * __cdecl CPolarChart_addVectorLayer(PolarChartInternal *ptr, const double *rDataData, int rDataLen,
	const double *aDataData, int aDataLen, const double *lengthsData, int lengthsLen, const double *directionsData, int directionsLen, int lengthScale, int color, const char *name);


////////////////////////////////////////////////////////////////////////////////////////
//	pyramidchart.h
////////////////////////////////////////////////////////////////////////////////////////
class PyramidLayerInternal;

CHARTDIR_DLLAPI TextBoxInternal * __cdecl CPyramidLayer_setCenterLabel(PyramidLayerInternal *ptr, const char *labelTemplate, const char *font, double fontSize, int fontColor);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CPyramidLayer_setRightLabel(PyramidLayerInternal *ptr, const char *labelTemplate, const char *font, double fontSize, int fontColor);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CPyramidLayer_setLeftLabel(PyramidLayerInternal *ptr, const char *labelTemplate, const char *font, double fontSize, int fontColor);

CHARTDIR_DLLAPI void __cdecl CPyramidLayer_setColor(PyramidLayerInternal *ptr, int color);
CHARTDIR_DLLAPI void __cdecl CPyramidLayer_setJoinLine(PyramidLayerInternal *ptr, int color, int width);
CHARTDIR_DLLAPI void __cdecl CPyramidLayer_setJoinLineGap(PyramidLayerInternal *ptr, int pyramidGap, int pyramidMargin, int textGap);
CHARTDIR_DLLAPI void __cdecl CPyramidLayer_setLayerBorder(PyramidLayerInternal *ptr, int color, int width);
CHARTDIR_DLLAPI void __cdecl CPyramidLayer_setLayerGap(PyramidLayerInternal *ptr, double layerGap);

class PyramidChartInternal;
CHARTDIR_DLLAPI BaseChartInternal * __cdecl PyramidChart2BaseChart(PyramidChartInternal *ptr);
CHARTDIR_DLLAPI PyramidChartInternal * __cdecl CPyramidChart_create(int width, int height, int bgColor, int edgeColor, int raisedEffect);

CHARTDIR_DLLAPI void __cdecl CPyramidChart_setPyramidSize(PyramidChartInternal *ptr, int cx, int cy, int radius, int height);
CHARTDIR_DLLAPI void __cdecl CPyramidChart_setConeSize(PyramidChartInternal *ptr, int cx, int cy, int radius, int height);
CHARTDIR_DLLAPI void __cdecl CPyramidChart_setFunnelSize(PyramidChartInternal *ptr, int cx, int cy, int radius, int height, double tubeRadius, double tubeHeight);
CHARTDIR_DLLAPI void __cdecl CPyramidChart_setData(PyramidChartInternal *ptr, const double *dataData, int dataLen, const char * const * labelsData, int labelsLen);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CPyramidChart_setCenterLabel(PyramidChartInternal *ptr, const char *labelTemplate, const char *font, double fontSize, int fontColor);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CPyramidChart_setRightLabel(PyramidChartInternal *ptr, const char *labelTemplate, const char *font, double fontSize, int fontColor);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CPyramidChart_setLeftLabel(PyramidChartInternal *ptr, const char *labelTemplate, const char *font, double fontSize, int fontColor);

CHARTDIR_DLLAPI void __cdecl CPyramidChart_setPyramidSides(PyramidChartInternal *ptr, int noOfSides);
CHARTDIR_DLLAPI void __cdecl CPyramidChart_setViewAngle(PyramidChartInternal *ptr, double elevation, double rotation, double twist);

CHARTDIR_DLLAPI void __cdecl CPyramidChart_setGradientShading(PyramidChartInternal *ptr, double startBrightness, double endBrightness);
CHARTDIR_DLLAPI void __cdecl CPyramidChart_setLighting(PyramidChartInternal *ptr, double ambientIntensity, double diffuseIntensity, double specularIntensity, double shininess);

CHARTDIR_DLLAPI void __cdecl CPyramidChart_setJoinLine(PyramidChartInternal *ptr, int color, int width);
CHARTDIR_DLLAPI void __cdecl CPyramidChart_setJoinLineGap(PyramidChartInternal *ptr, int pyramidGap, int pyramidMargin, int textGap);
CHARTDIR_DLLAPI void __cdecl CPyramidChart_setLayerBorder(PyramidChartInternal *ptr, int color, int width);
CHARTDIR_DLLAPI void __cdecl CPyramidChart_setLayerGap(PyramidChartInternal *ptr, double layerGap);

CHARTDIR_DLLAPI PyramidLayerInternal * __cdecl CPyramidChart_getLayer(PyramidChartInternal *ptr, int layerNo);


////////////////////////////////////////////////////////////////////////////////////////
//	meterpointer.h
////////////////////////////////////////////////////////////////////////////////////////
class MeterPointerInternal;
CHARTDIR_DLLAPI DrawObjInternal * __cdecl MeterPointer2DrawObj(MeterPointerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CMeterPointer_setColor(MeterPointerInternal *ptr, int fillColor, int edgeColor);
CHARTDIR_DLLAPI void __cdecl CMeterPointer_setPos(MeterPointerInternal *ptr, double value);
CHARTDIR_DLLAPI void __cdecl CMeterPointer_setShape(MeterPointerInternal *ptr, int pointerType, double lengthRatio, double widthRatio);
CHARTDIR_DLLAPI void __cdecl CMeterPointer_setShape2(MeterPointerInternal *ptr, const int *pointerCoor, int len, double lengthRatio, double widthRatio);
CHARTDIR_DLLAPI void __cdecl CMeterPointer_setShapeAndOffset(MeterPointerInternal *ptr, int pointerType, double startOffset, double endOffset, double widthRatio);
CHARTDIR_DLLAPI void __cdecl CMeterPointer_setShapeAndOffset2(MeterPointerInternal *ptr, const int *pointerCoor, int len, double startOffset, double endOffset, double widthRatio);


////////////////////////////////////////////////////////////////////////////////////////
//	basemeter.h
////////////////////////////////////////////////////////////////////////////////////////
class BaseMeterInternal;
CHARTDIR_DLLAPI BaseChartInternal * __cdecl BaseMeter2BaseChart(BaseMeterInternal *ptr);
CHARTDIR_DLLAPI MeterPointerInternal * __cdecl CBaseMeter_addPointer(BaseMeterInternal *ptr, double value, int fillColor, int edgeColor);

CHARTDIR_DLLAPI void __cdecl CBaseMeter_setScale(BaseMeterInternal *ptr, double lowerLimit, double upperLimit, double majorTickInc, double minorTickInc, double microTickInc);
CHARTDIR_DLLAPI void __cdecl CBaseMeter_setScale2(BaseMeterInternal *ptr, double lowerLimit, double upperLimit, const char * const * labels, int len);
CHARTDIR_DLLAPI void __cdecl CBaseMeter_setScale3(BaseMeterInternal *ptr, double lowerLimit, double upperLimit, const double *labels, int len, const char *formatString);

CHARTDIR_DLLAPI void __cdecl CBaseMeter_addColorScale(BaseMeterInternal *ptr, const double *data, int len, int startPos, int startWidth, int endPos, int endWidth, int edgeColor);

CHARTDIR_DLLAPI void __cdecl CBaseMeter_addLabel(BaseMeterInternal *ptr, double pos, const char *label);
CHARTDIR_DLLAPI const char * __cdecl CBaseMeter_getLabel(BaseMeterInternal *ptr, double i);
CHARTDIR_DLLAPI void __cdecl CBaseMeter_getTicks(BaseMeterInternal *ptr, const double **d, int *len);

CHARTDIR_DLLAPI TextBoxInternal * __cdecl CBaseMeter_setLabelStyle(BaseMeterInternal *ptr, const char *font, double fontSize, int fontColor, double fontAngle);
CHARTDIR_DLLAPI void __cdecl CBaseMeter_setLabelPos(BaseMeterInternal *ptr, bool labelInside, int labelOffset);
CHARTDIR_DLLAPI void __cdecl CBaseMeter_setLabelFormat(BaseMeterInternal *ptr, const char *mainLabelFormat);
CHARTDIR_DLLAPI void __cdecl CBaseMeter_setTickLength(BaseMeterInternal *ptr, int majorLen, int minorLen, int microLen);
CHARTDIR_DLLAPI void __cdecl CBaseMeter_setLineWidth(BaseMeterInternal *ptr, int axisWidth, int majorTickWidth, int minorTickWidth, int microTickWidth);
CHARTDIR_DLLAPI void __cdecl CBaseMeter_setMeterColors(BaseMeterInternal *ptr, int axisColor, int labelColor, int tickColor);

CHARTDIR_DLLAPI int __cdecl CBaseMeter_getCoor(BaseMeterInternal *ptr, double v);


////////////////////////////////////////////////////////////////////////////////////////
//	angularmeter.h
////////////////////////////////////////////////////////////////////////////////////////
class AngularMeterInternal;
CHARTDIR_DLLAPI BaseMeterInternal * __cdecl AngularMeter2BaseMeter(AngularMeterInternal *ptr);
CHARTDIR_DLLAPI AngularMeterInternal * __cdecl CAngularMeter_create(int width, int height, int bgColor,	int edgeColor, int raisedEffect);

CHARTDIR_DLLAPI void __cdecl CAngularMeter_addRing(AngularMeterInternal *ptr, int startRadius, int endRadius, int fillColor, int edgeColor);
CHARTDIR_DLLAPI void __cdecl CAngularMeter_addRingSector(AngularMeterInternal *ptr, int startRadius, int endRadius, double a1, double a2, int fillColor, int edgeColor);

CHARTDIR_DLLAPI void __cdecl CAngularMeter_setMeter(AngularMeterInternal *ptr, int cx, int cy, int radius, double startAngle, double endAngle);
CHARTDIR_DLLAPI void __cdecl CAngularMeter_addScaleBackground(AngularMeterInternal *ptr, int bgRadius, int fillColor, int edgeWidth, int edgeColor, int scaleRadius, double startAngle, double endAngle);
CHARTDIR_DLLAPI void __cdecl CAngularMeter_addGlare(AngularMeterInternal *ptr, double radius, double span, double rotate, double glareRadius, double intensity);

CHARTDIR_DLLAPI void __cdecl CAngularMeter_setCap(AngularMeterInternal *ptr, int radius, int fillColor, int edgeColor);
CHARTDIR_DLLAPI void __cdecl CAngularMeter_setCap2(AngularMeterInternal *ptr, int backColor, int frontColor, int frontEdgeColor, double lightingRatio, double backRadiusRatio, double frontRadiusRatio, double frontEdgeWidthRatio);
CHARTDIR_DLLAPI MeterPointerInternal * __cdecl CAngularMeter_addPointer2(AngularMeterInternal *ptr, double value, int fillColor, int edgeColor, int pointerType, double startOffset, double endOffset, double widthRatio);

CHARTDIR_DLLAPI void __cdecl CAngularMeter_addZone(AngularMeterInternal *ptr, double startValue, double endValue, int startRadius, int endRadius, int fillColor, int edgeColor);
CHARTDIR_DLLAPI void __cdecl CAngularMeter_addZone2(AngularMeterInternal *ptr, double startValue, double endValue, int fillColor, int edgeColor);

CHARTDIR_DLLAPI int __cdecl CAngularMeter_relativeRadialGradient(AngularMeterInternal *ptr, const double *data, int len, double radius);
CHARTDIR_DLLAPI int __cdecl CAngularMeter_relativeLinearGradient(AngularMeterInternal *ptr, const double *data, int len, double angle, double radius);


////////////////////////////////////////////////////////////////////////////////////////
//	linearmeter.h
////////////////////////////////////////////////////////////////////////////////////////
class LinearMeterInternal;
CHARTDIR_DLLAPI BaseMeterInternal * __cdecl LinearMeter2BaseMeter(LinearMeterInternal *ptr);
CHARTDIR_DLLAPI LinearMeterInternal * __cdecl CLinearMeter_create(int width, int height, int bgColor, int edgeColor, int raisedEffect);

CHARTDIR_DLLAPI void __cdecl CLinearMeter_setMeter(LinearMeterInternal *ptr, int leftX, int topY, int width, int height, int axisPos, bool isReversed);
CHARTDIR_DLLAPI void __cdecl CLinearMeter_setRail(LinearMeterInternal *ptr, int railColor, int railWidth, int railOffset);

CHARTDIR_DLLAPI int __cdecl CLinearMeter_getMeterLeftX(LinearMeterInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CLinearMeter_getMeterTopY(LinearMeterInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CLinearMeter_getMeterWidth(LinearMeterInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CLinearMeter_getMeterHeight(LinearMeterInternal *ptr); 

CHARTDIR_DLLAPI TextBoxInternal * __cdecl CLinearMeter_addZone(LinearMeterInternal *ptr, double startValue, double endValue, int color, const char *label);
CHARTDIR_DLLAPI TextBoxInternal * __cdecl CLinearMeter_addBar(LinearMeterInternal *ptr, double startValue, double endValue, int color, int effect, int roundedCorner);


////////////////////////////////////////////////////////////////////////////////////////
//	datafilter.h
////////////////////////////////////////////////////////////////////////////////////////
class ArrayMathInternal;
CHARTDIR_DLLAPI ArrayMathInternal * __cdecl CArrayMath_create(const double *aData, int aLen);
CHARTDIR_DLLAPI void __cdecl CArrayMath_destroy(ArrayMathInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CArrayMath_add(ArrayMathInternal *ptr, const double *bData, int bLen);
CHARTDIR_DLLAPI void __cdecl CArrayMath_add2(ArrayMathInternal *ptr, double b);
CHARTDIR_DLLAPI void __cdecl CArrayMath_sub(ArrayMathInternal *ptr, const double *bData, int bLen);
CHARTDIR_DLLAPI void __cdecl CArrayMath_sub2(ArrayMathInternal *ptr, double b);
CHARTDIR_DLLAPI void __cdecl CArrayMath_mul(ArrayMathInternal *ptr, const double *bData, int bLen);
CHARTDIR_DLLAPI void __cdecl CArrayMath_mul2(ArrayMathInternal *ptr, double b);
CHARTDIR_DLLAPI void __cdecl CArrayMath_div(ArrayMathInternal *ptr, const double *bData, int bLen);
CHARTDIR_DLLAPI void __cdecl CArrayMath_div2(ArrayMathInternal *ptr, double b);
CHARTDIR_DLLAPI void __cdecl CArrayMath_financeDiv(ArrayMathInternal *ptr, const double *bData, int bLen, double zeroByZeroValue);
CHARTDIR_DLLAPI void __cdecl CArrayMath_shift(ArrayMathInternal *ptr, int offset, double fillValue);
CHARTDIR_DLLAPI void __cdecl CArrayMath_delta(ArrayMathInternal *ptr, int offset);
CHARTDIR_DLLAPI void __cdecl CArrayMath_rate(ArrayMathInternal *ptr, int offset);
CHARTDIR_DLLAPI void __cdecl CArrayMath_abs(ArrayMathInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CArrayMath_acc(ArrayMathInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CArrayMath_selectGTZ(ArrayMathInternal *ptr, const double *bData, int bLen, double fillValue);
CHARTDIR_DLLAPI void __cdecl CArrayMath_selectGEZ(ArrayMathInternal *ptr, const double *bData, int bLen, double fillValue);
CHARTDIR_DLLAPI void __cdecl CArrayMath_selectLTZ(ArrayMathInternal *ptr, const double *bData, int bLen, double fillValue);
CHARTDIR_DLLAPI void __cdecl CArrayMath_selectLEZ(ArrayMathInternal *ptr, const double *bData, int bLen, double fillValue);
CHARTDIR_DLLAPI void __cdecl CArrayMath_selectEQZ(ArrayMathInternal *ptr, const double *bData, int bLen, double fillValue);
CHARTDIR_DLLAPI void __cdecl CArrayMath_selectNEZ(ArrayMathInternal *ptr, const double *bData, int bLen, double fillValue);

CHARTDIR_DLLAPI void __cdecl CArrayMath_selectStartOfSecond(ArrayMathInternal *ptr, int majorTickStep, double initialMargin);
CHARTDIR_DLLAPI void __cdecl CArrayMath_selectStartOfMinute(ArrayMathInternal *ptr, int majorTickStep, double initialMargin);
CHARTDIR_DLLAPI void __cdecl CArrayMath_selectStartOfHour(ArrayMathInternal *ptr, int majorTickStep, double initialMargin);
CHARTDIR_DLLAPI void __cdecl CArrayMath_selectStartOfDay(ArrayMathInternal *ptr, int majorTickStep, double initialMargin);
CHARTDIR_DLLAPI void __cdecl CArrayMath_selectStartOfWeek(ArrayMathInternal *ptr, int majorTickStep, double initialMargin);
CHARTDIR_DLLAPI void __cdecl CArrayMath_selectStartOfMonth(ArrayMathInternal *ptr, int majorTickStep, double initialMargin);
CHARTDIR_DLLAPI void __cdecl CArrayMath_selectStartOfYear(ArrayMathInternal *ptr, int majorTickStep, double initialMargin);
CHARTDIR_DLLAPI void __cdecl CArrayMath_selectRegularSpacing(ArrayMathInternal *ptr, int majorTickStep, int minorTickStep, int initialMargin);

CHARTDIR_DLLAPI void __cdecl CArrayMath_trim(ArrayMathInternal *ptr, int startIndex, int len);
CHARTDIR_DLLAPI void __cdecl CArrayMath_insert(ArrayMathInternal *ptr, const double *aData, int aLen, int insertPoint);
CHARTDIR_DLLAPI void __cdecl CArrayMath_insert2(ArrayMathInternal *ptr, double c, int len, int insertPoint);
CHARTDIR_DLLAPI void __cdecl CArrayMath_replace(ArrayMathInternal *ptr, double a, double b);

CHARTDIR_DLLAPI void __cdecl CArrayMath_movAvg(ArrayMathInternal *ptr, int interval);
CHARTDIR_DLLAPI void __cdecl CArrayMath_expAvg(ArrayMathInternal *ptr, double smoothingFactor);
CHARTDIR_DLLAPI void __cdecl CArrayMath_movMed(ArrayMathInternal *ptr, int interval);
CHARTDIR_DLLAPI void __cdecl CArrayMath_movPercentile(ArrayMathInternal *ptr, int interval, double percentile);
CHARTDIR_DLLAPI void __cdecl CArrayMath_movMax(ArrayMathInternal *ptr, int interval);
CHARTDIR_DLLAPI void __cdecl CArrayMath_movMin(ArrayMathInternal *ptr, int interval);
CHARTDIR_DLLAPI void __cdecl CArrayMath_movStdDev(ArrayMathInternal *ptr, int interval);
CHARTDIR_DLLAPI void __cdecl CArrayMath_movCorr(ArrayMathInternal *ptr, int interval, const double *bData, int bLen);
CHARTDIR_DLLAPI void __cdecl CArrayMath_lowess(ArrayMathInternal *ptr, double smoothness, int iteration);
CHARTDIR_DLLAPI void __cdecl CArrayMath_lowess2(ArrayMathInternal *ptr, const double *bData, int bLen, double smoothness, int iteration);

CHARTDIR_DLLAPI void __cdecl CArrayMath_result(ArrayMathInternal *ptr, const double **ret, int *retLen);
CHARTDIR_DLLAPI double __cdecl CArrayMath_max(ArrayMathInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CArrayMath_min(ArrayMathInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CArrayMath_avg(ArrayMathInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CArrayMath_sum(ArrayMathInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CArrayMath_stdDev(ArrayMathInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CArrayMath_med(ArrayMathInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CArrayMath_percentile(ArrayMathInternal *ptr, double p);
CHARTDIR_DLLAPI int __cdecl CArrayMath_maxIndex(ArrayMathInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CArrayMath_minIndex(ArrayMathInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CArrayMath_aggregate(ArrayMathInternal *ptr, const double *srcData, int srcLen, int aggregateMethod, 
	double param, const double **ret, int *retLen);


////////////////////////////////////////////////////////////////////////////////////////
//	rantable.h
////////////////////////////////////////////////////////////////////////////////////////
class RanTableInternal;
CHARTDIR_DLLAPI RanTableInternal * __cdecl CRanTable_create(int seed, int noOfCols, int noOfRows);
CHARTDIR_DLLAPI void __cdecl CRanTable_destroy(RanTableInternal *ptr);

CHARTDIR_DLLAPI void __cdecl CRanTable_setCol(RanTableInternal *ptr, int colNo, double minValue, double maxValue);
CHARTDIR_DLLAPI void __cdecl CRanTable_setCol2(RanTableInternal *ptr, int colNo, double startValue, double minDelta, double maxDelta,
	double lowerLimit, double upperLimit);
CHARTDIR_DLLAPI void __cdecl CRanTable_setDateCol(RanTableInternal *ptr, int i, double startTime, double tickInc, bool weekDayOnly);
CHARTDIR_DLLAPI void __cdecl CRanTable_setHLOCCols(RanTableInternal *ptr, int i, double startValue, double minDelta, double maxDelta,
	double lowerLimit, double upperLimit);
CHARTDIR_DLLAPI int __cdecl CRanTable_selectDate(RanTableInternal *ptr, int ColNo, double minDate, double maxData);
CHARTDIR_DLLAPI void __cdecl CRanTable_getCol(RanTableInternal *ptr, int i, const double **ret, int *len);

class RanSeriesInternal;
CHARTDIR_DLLAPI RanSeriesInternal *__cdecl CRanSeries_create(int seed);
CHARTDIR_DLLAPI void __cdecl CRanSeries_destroy(RanSeriesInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CRanSeries_getSeries(RanSeriesInternal *ptr, int len, double minValue, double maxValue, const double **ret, int *retLen);
CHARTDIR_DLLAPI void __cdecl CRanSeries_getSeries2(RanSeriesInternal *ptr, int len, double startValue, double minDelta, double maxDelta, double lowerLimit, double upperLimit, const double **ret, int *retLen);
CHARTDIR_DLLAPI void __cdecl CRanSeries_getGaussianSeries(RanSeriesInternal *ptr, int len, double mean, double stdDev, const double **ret, int *retLen);
CHARTDIR_DLLAPI void __cdecl CRanSeries_get2DSeries(RanSeriesInternal *ptr, int xLen, int yLen, double minValue, double maxValue, const double **ret, int *retLen);
CHARTDIR_DLLAPI void __cdecl CRanSeries_getDateSeries(RanSeriesInternal *ptr, int len, double startTime, double tickInc, bool weekDayOnly, const double **ret, int *retLen);

class FinanceSimulatorInternal;
CHARTDIR_DLLAPI FinanceSimulatorInternal * __cdecl CFinanceSimulator_create(int seed, double startTime, double endTime, int resolution);
CHARTDIR_DLLAPI FinanceSimulatorInternal * __cdecl CFinanceSimulator_create2(const char *seed, double startTime, double endTime, int resolution);
CHARTDIR_DLLAPI void __cdecl CFinanceSimulator_destroy(FinanceSimulatorInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CFinanceSimulator_getTimeStamps(FinanceSimulatorInternal *ptr, const double **ret, int *len);
CHARTDIR_DLLAPI void __cdecl CFinanceSimulator_getHighData(FinanceSimulatorInternal *ptr, const double **ret, int *len);
CHARTDIR_DLLAPI void __cdecl CFinanceSimulator_getLowData(FinanceSimulatorInternal *ptr, const double **ret, int *len);
CHARTDIR_DLLAPI void __cdecl CFinanceSimulator_getOpenData(FinanceSimulatorInternal *ptr, const double **ret, int *len);
CHARTDIR_DLLAPI void __cdecl CFinanceSimulator_getCloseData(FinanceSimulatorInternal *ptr, const double **ret, int *len);
CHARTDIR_DLLAPI void __cdecl CFinanceSimulator_getVolData(FinanceSimulatorInternal *ptr, const double **ret, int *len);


////////////////////////////////////////////////////////////////////////////////////////
//	imagemaphandler.h
////////////////////////////////////////////////////////////////////////////////////////
class ImageMapHandlerInternal;
CHARTDIR_DLLAPI ImageMapHandlerInternal * __cdecl CImageMapHandler_create(const char *imageMap);
CHARTDIR_DLLAPI void __cdecl CImageMapHandler_destroy(ImageMapHandlerInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CImageMapHandler_getHotSpot(ImageMapHandlerInternal *ptr, int x, int y);
CHARTDIR_DLLAPI const char * __cdecl CImageMapHandler_getValue(ImageMapHandlerInternal *ptr, const char *key);
CHARTDIR_DLLAPI const char * __cdecl CImageMapHandler_getKey(ImageMapHandlerInternal *ptr, int i);
CHARTDIR_DLLAPI const char * __cdecl CImageMapHandler_getValue2(ImageMapHandlerInternal *ptr, int i);


////////////////////////////////////////////////////////////////////////////////////////
//	viewportmanager.h
////////////////////////////////////////////////////////////////////////////////////////
class ViewPortManagerInternal;
CHARTDIR_DLLAPI ViewPortManagerInternal * __cdecl CViewPortManager_create();
CHARTDIR_DLLAPI void __cdecl CViewPortManager_destroy(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_setChartMetrics(ViewPortManagerInternal *ptr, const char *metrics);
CHARTDIR_DLLAPI int __cdecl CViewPortManager_getPlotAreaLeft(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CViewPortManager_getPlotAreaTop(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CViewPortManager_getPlotAreaWidth(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI int __cdecl CViewPortManager_getPlotAreaHeight(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CViewPortManager_getViewPortLeft(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CViewPortManager_getViewPortTop(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CViewPortManager_getViewPortWidth(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CViewPortManager_getViewPortHeight(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_setViewPortLeft(ViewPortManagerInternal *ptr, double left);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_setViewPortTop(ViewPortManagerInternal *ptr, double top);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_setViewPortWidth(ViewPortManagerInternal *ptr, double width);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_setViewPortHeight(ViewPortManagerInternal *ptr, double height);
CHARTDIR_DLLAPI double __cdecl CViewPortManager_getZoomInWidthLimit(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CViewPortManager_getZoomOutWidthLimit(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_setZoomInWidthLimit(ViewPortManagerInternal *ptr, double viewPortWidth);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_setZoomOutWidthLimit(ViewPortManagerInternal *ptr, double viewPortWidth);
CHARTDIR_DLLAPI double __cdecl CViewPortManager_getZoomInHeightLimit(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CViewPortManager_getZoomOutHeightLimit(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_setZoomInHeightLimit(ViewPortManagerInternal *ptr, double viewPortHeight);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_setZoomOutHeightLimit(ViewPortManagerInternal *ptr, double viewPortHeight);

CHARTDIR_DLLAPI void __cdecl CViewPortManager_validateViewPort(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI bool __cdecl CViewPortManager_inPlotArea(ViewPortManagerInternal *ptr, int x, int y);
CHARTDIR_DLLAPI bool __cdecl CViewPortManager_canZoomIn(ViewPortManagerInternal *ptr, int zoomDirection);
CHARTDIR_DLLAPI bool __cdecl CViewPortManager_canZoomOut(ViewPortManagerInternal *ptr, int zoomDirection);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_startDrag(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI bool __cdecl CViewPortManager_dragTo(ViewPortManagerInternal *ptr, int scrollDirection, int deltaX, int deltaY);
CHARTDIR_DLLAPI bool __cdecl CViewPortManager_zoomAt(ViewPortManagerInternal *ptr, int zoomDirection, int x, int y, double zoomRatio);
CHARTDIR_DLLAPI bool __cdecl CViewPortManager_zoomTo(ViewPortManagerInternal *ptr, int zoomDirection, int x1, int y1, int x2, int y2);
CHARTDIR_DLLAPI bool __cdecl CViewPortManager_zoomAround(ViewPortManagerInternal *ptr, int x, int y, double xZoomRatio, double yZoomRatio);

CHARTDIR_DLLAPI void __cdecl CViewPortManager_setFullRange(ViewPortManagerInternal *ptr, const char *id, double minValue, double maxValue);
CHARTDIR_DLLAPI bool __cdecl CViewPortManager_updateFullRangeHV(ViewPortManagerInternal *ptr, const char *id, double minValue, double maxValue, int updateType, bool isHorizontal);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_clearAllRanges(ViewPortManagerInternal *ptr);
CHARTDIR_DLLAPI double __cdecl CViewPortManager_getValueAtViewPort(ViewPortManagerInternal *ptr, const char *id, double ratio, bool isLogScale);
CHARTDIR_DLLAPI double __cdecl CViewPortManager_getViewPortAtValue(ViewPortManagerInternal *ptr, const char *id, double ratio, bool isLogScale);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_syncLinearAxisWithViewPort(ViewPortManagerInternal *ptr, const char *id, AxisInternal *axis);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_syncLogAxisWithViewPort(ViewPortManagerInternal *ptr, const char *id, AxisInternal *axis);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_syncDateAxisWithViewPort(ViewPortManagerInternal *ptr, const char *id, AxisInternal *axis);
CHARTDIR_DLLAPI void __cdecl CViewPortManager_commitPendingSyncAxis(ViewPortManagerInternal *ptr, BaseChartInternal *c);

CHARTDIR_DLLAPI void __cdecl CViewPortManager_setPlotAreaMouseMargin(ViewPortManagerInternal *ptr, int leftMargin, int rightMargin, int topMargin, int bottomMargin);
CHARTDIR_DLLAPI bool __cdecl CViewPortManager_inExtendedPlotArea(ViewPortManagerInternal *ptr, int x, int y);


////////////////////////////////////////////////////////////////////////////////////////
//	viewportcontrollerbase.h
////////////////////////////////////////////////////////////////////////////////////////
class ViewPortControlBaseInternal;
CHARTDIR_DLLAPI ViewPortControlBaseInternal * __cdecl CViewPortControlBase_create();
CHARTDIR_DLLAPI void __cdecl CViewPortControlBase_destroy(ViewPortControlBaseInternal *ptr);
CHARTDIR_DLLAPI void __cdecl CViewPortControlBase_setChart(ViewPortControlBaseInternal *ptr, BaseChartInternal *c);
CHARTDIR_DLLAPI	void __cdecl CViewPortControlBase_setViewPortManager(ViewPortControlBaseInternal *ptr, ViewPortManagerInternal *m);
CHARTDIR_DLLAPI	void __cdecl CViewPortControlBase_handleMouseDown(ViewPortControlBaseInternal *ptr, double x, double y);
CHARTDIR_DLLAPI	void __cdecl CViewPortControlBase_handleMouseUp(ViewPortControlBaseInternal *ptr, double x, double y);
CHARTDIR_DLLAPI	void __cdecl CViewPortControlBase_handleMouseMove(ViewPortControlBaseInternal *ptr, double x, double y, bool isDragging);
CHARTDIR_DLLAPI	void __cdecl CViewPortControlBase_paintViewPort(ViewPortControlBaseInternal *ptr);
CHARTDIR_DLLAPI	bool __cdecl CViewPortControlBase_isOnPlotArea(ViewPortControlBaseInternal *ptr, double x, double y);
CHARTDIR_DLLAPI	int __cdecl CViewPortControlBase_getProperty(ViewPortControlBaseInternal *ptr, int attr); 
CHARTDIR_DLLAPI	void __cdecl CViewPortControlBase_setProperty(ViewPortControlBaseInternal *ptr, int attr, int value); 

}

#endif
