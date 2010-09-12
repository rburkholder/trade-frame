/*
 * Copyright (C) 2009 Advanced Software Engineering Limited.
 *
 * This file is part of the ChartDirector software. Usage of this file is
 * subjected to the ChartDirector license agreement. See the LICENSE.TXT
 * file that comes with the ChartDirector software release, or visit
 * Advanced Software Engineering's web site at www.advsofteng.com or
 * email to support@advsofteng.com for information.
 *
 */

#ifndef CCHARTDIR_HDR
#define CCHARTDIR_HDR

#ifndef CHARTDIR_WIDECHAR
#define CHARTDIR_WIDECHAR void
#endif

#include "bchartdir.h"
#include "memblock.h"

class AutoDestroy
{
public :
	virtual ~AutoDestroy() {}
};

class GarbagePtr
{
private :
	AutoDestroy *ptr;
public :
	GarbagePtr(AutoDestroy *ptr, GarbagePtr *next) : ptr(ptr), next(next) {}
	~GarbagePtr() { delete ptr; }
	GarbagePtr *next;
};

class GarbageContainer
{
private :
	GarbagePtr *root;
public :
	GarbageContainer() : root(0) {}
	virtual ~GarbageContainer() { while (0 != root) { GarbagePtr *temp = root; root = root->next; delete temp; } }
	void reg(AutoDestroy *g) { root = new GarbagePtr(g, root); }
};


///////////////////////////////////////////////////////////////////////////////////////
//	constants
///////////////////////////////////////////////////////////////////////////////////////

namespace Chart 
{
	static const double NoValue = 1.7e308;
	static const double LogTick = 1.6e308;
	static const double LinearTick = 1.5e308;
	static const double TickInc = 1.0e200;
	static const double MinorTickOnly = -1.7e308;
	static const double MicroTickOnly = -1.6e308;
	enum { AutoGrid = -2 };
	static const double TouchBar = -1.7E-100;
	
	enum Alignment
	{
		TopLeft = 7,	TopCenter = 8,		TopRight = 9,
		Left = 4,		Center = 5,			Right = 6,
		BottomLeft = 1,	BottomCenter = 2,	BottomRight = 3,
		Top = TopCenter, Bottom = BottomCenter,
		TopLeft2 = 10, TopRight2 = 11, 
		BottomLeft2 = 12, BottomRight2 = 13
	};

	enum
	{
		DashLine = 0x0505,
		DotLine = 0x0202,
		DotDashLine = 0x05050205,
		AltDashLine = 0x0A050505
	};

	static const int goldGradient[] = {0, 0xFFE743, 0x60, 0xFFFFE0, 0xB0, 0xFFF0B0, 0x100, 0xFFE743};
	static const int silverGradient[] = {0, 0xC8C8C8, 0x60, 0xF8F8F8, 0xB0, 0xE0E0E0, 0x100, 0xC8C8C8};
	static const int redMetalGradient[] = {0, 0xE09898, 0x60, 0xFFF0F0, 0xB0, 0xF0D8D8, 0x100, 0xE09898};
	static const int blueMetalGradient[] = {0, 0x9898E0, 0x60, 0xF0F0FF, 0xB0, 0xD8D8F0, 0x100, 0x9898E0};
	static const int greenMetalGradient[] = {0, 0x98E098, 0x60, 0xF0FFF0, 0xB0, 0xD8F0D8, 0x100, 0x98E098};

	static inline int metalColor(int c, int angle = 90) { return CChart_metalColor(c, angle); }
	static inline int goldColor(int angle = 90) { return metalColor(0xffee44, angle); }
	static inline int silverColor(int angle = 90) { return metalColor(0xdddddd, angle); }

	static inline int brushedMetalColor(int c, int texture = 2, int angle = 90) { return metalColor(c, angle) | ((texture & 0x3) << 18); }
	static inline int brushedSilverColor(int texture = 2, int angle = 90) {return brushedMetalColor(0xdddddd, texture, angle); }
	static inline int brushedGoldColor(int texture = 2, int angle = 90)  {return brushedMetalColor(0xffee44, texture, angle); }

	enum AntiAliasMode { NoAntiAlias, AntiAlias, AutoAntiAlias };
	enum PaletteMode { TryPalette, ForcePalette, NoPalette };
	enum DitherMethod { Quantize, OrderedDither, ErrorDiffusion };

	enum CDFilterType { BoxFilter, LinearFilter, QuadraticFilter, BSplineFilter, HermiteFilter,
		CatromFilter, MitchellFilter, SincFilter, LanczosFilter, GaussianFilter, HanningFilter,
		HammingFilter, BlackmanFilter, BesselFilter };

	enum
	{
		Transparent = 0xff000000,
		Palette = 0xffff0000,
		BackgroundColor = 0xffff0000,
		LineColor = 0xffff0001,
		TextColor = 0xffff0002,
		DataColor = 0xffff0008,
		SameAsMainColor = 0xffff0007
	};

	enum ImgFormat { PNG, GIF, JPG, WMP, BMP, SVG, SVGZ };

	static const int oldDefaultPalette[] =
	{
		0xffffff, 0x000000, 0x000000, 0x808080,
		0x808080, 0x808080, 0x808080, 0x808080,
		0xff0000, 0x00ff00, 0x0000ff, 0xffff00,
		0xff00ff, 0x66ffff,	0xffcc33, 0xcccccc,
		0x9966ff, 0x339966, 0x999900, 0xcc3300,
		0x99cccc, 0x006600, 0x660066, 0xcc9999,
		-1
	};

	static const int defaultPalette[] =
	{
		0xffffff, 0x000000, 0x000000, 0x808080,
		0x808080, 0x808080, 0x808080, 0x808080,
		0xff3333, 0x33ff33, 0x6666ff, 0xffff00,
		0xff66ff, 0x99ffff,	0xffcc33, 0xcccccc,
		0xcc9999, 0x339966, 0x999900, 0xcc3300,
		0x669999, 0x993333, 0x006600, 0x990099,
		0xff9966, 0x99ff99, 0x9999ff, 0xcc6600,
		0x33cc33, 0xcc99ff, 0xff6666, 0x99cc66,
		0x009999, 0xcc3333, 0x9933ff, 0xff0000,
		0x0000ff, 0x00ff00, 0xffcc99, 0x999999,
		-1
	};

	static const int whiteOnBlackPalette[] =
	{
		0x000000, 0xffffff, 0xffffff, 0x808080,
		0x808080, 0x808080, 0x808080, 0x808080,
		0xff0000, 0x00ff00, 0x0000ff, 0xffff00,
		0xff00ff, 0x66ffff,	0xffcc33, 0xcccccc,
		0x9966ff, 0x339966, 0x999900, 0xcc3300,
		0x99cccc, 0x006600, 0x660066, 0xcc9999,
		0xff9966, 0x99ff99, 0x9999ff, 0xcc6600,
		0x33cc33, 0xcc99ff, 0xff6666, 0x99cc66,
		0x009999, 0xcc3333, 0x9933ff, 0xff0000,
		0x0000ff, 0x00ff00, 0xffcc99, 0x999999,
		-1
	};

	static const int transparentPalette[] =
	{
		0xffffff, 0x000000, 0x000000, 0x808080,
		0x808080, 0x808080, 0x808080, 0x808080,
		0x80ff0000, 0x8000ff00, 0x800000ff, 0x80ffff00,
		0x80ff00ff, 0x8066ffff,	0x80ffcc33, 0x80cccccc,
		0x809966ff, 0x80339966, 0x80999900, 0x80cc3300,
		0x8099cccc, 0x80006600, 0x80660066, 0x80cc9999,
		0x80ff9966, 0x8099ff99, 0x809999ff, 0x80cc6600,
		0x8033cc33, 0x80cc99ff, 0x80ff6666, 0x8099cc66,
		0x80009999, 0x80cc3333, 0x809933ff, 0x80ff0000,
		0x800000ff, 0x8000ff00, 0x80ffcc99, 0x80999999,
		-1
	};

	enum { SideLayout, CircleLayout };

	enum
	{	
		DefaultShading = 0,
		FlatShading = 1,
		LocalGradientShading = 2,
		GlobalGradientShading = 3,
		ConcaveShading = 4,
		RoundedEdgeNoGlareShading = 5,
		RoundedEdgeShading = 6,
		RadialShading = 7,
		RingShading = 8
	};

	enum SymbolType
	{
		NoSymbol = 0,
		SquareSymbol = 1,
		DiamondSymbol = 2,
		TriangleSymbol = 3,
		RightTriangleSymbol = 4,
		LeftTriangleSymbol = 5,
		InvertedTriangleSymbol = 6,
		CircleSymbol = 7,
		CrossSymbol = 8,
		Cross2Symbol = 9,
		ChartDir_PolygonSymbol = 11,
		ChartDir_Polygon2Symbol = 12,
		ChartDir_StarSymbol = 13,
		ChartDir_CustomSymbol = 14 
	};

	enum 
	{
		NoShape = 0,
		SquareShape = 1,
		DiamondShape = 2,
		TriangleShape = 3,
		RightTriangleShape = 4,
		LeftTriangleShape = 5,
		InvertedTriangleShape = 6,
		CircleShape = 7,
		CircleShapeNoShading = 10,
		GlassSphereShape = 15,
		GlassSphere2Shape = 16,
		SolidSphereShape = 17
	};

	static inline int CrossShape(double width = 0.5)
	{ return CrossSymbol | (((int)(((width < 0) ? 0 : ((width > 1) ? 1 : width)) * 4095 + 0.5)) << 12); }
	static inline int Cross2Shape(double width = 0.5)
	{ return Cross2Symbol | (((int)(((width < 0) ? 0 : ((width > 1) ? 1 : width)) * 4095 + 0.5)) << 12); }
	static inline int PolygonShape(int side)
	{ return ChartDir_PolygonSymbol | (((side < 0) ? 0 : ((side > 100) ? 100 : side)) << 12); }
	static inline int Polygon2Shape(int side)
	{ return ChartDir_Polygon2Symbol | (((side < 0) ? 0 : ((side > 100) ? 100 : side)) << 12); }
	static inline int StarShape(int side)
	{ return ChartDir_StarSymbol | (((side < 0) ? 0 : ((side > 100) ? 100 : side)) << 12); }
	
	enum DataCombineMethod { Overlay, Stack, Depth, Side, Percentage };

	enum LegendMode { NormalLegend, ReverseLegend, NoLegend };

	enum ScaleType { PixelScale, XAxisScale, YAxisScale, EndPoints,
		AngularAxisScale = XAxisScale, RadialAxisScale = YAxisScale };

	enum 
	{ 
		MonotonicNone = 0, 
		MonotonicX = 1, 
		MonotonicY = 2,
		MonotonicXY = 3,
		MonotonicAuto = 4
	};

	enum 
	{
		ConstrainedLinearRegression = 0,
		LinearRegression = 1,
		ExponentialRegression = -1,
		LogarithmicRegression = -2
	};

	static inline int PolynomialRegression(int n) { return n; }

	enum { HLOCDefault, HLOCOpenClose, HLOCUpDown };

	enum { DiamondPointer, TriangularPointer, ArrowPointer, ArrowPointer2, LinePointer, PencilPointer };

	enum 
	{
		SmoothShading = 0,
		TriangularShading = 1,
		RectangularShading = 2,
		TriangularFrame = 3,
		RectangularFrame = 4
	};
	
	enum 
	{
		ChartBackZ = 0x100,
		ChartFrontZ = 0xffff,
		PlotAreaZ = 0x1000,
		GridLinesZ = 0x2000
	};

	enum 
	{
		XAxisSymmetric = 1,
		XAxisSymmetricIfNeeded = 2,
		YAxisSymmetric = 4,
		YAxisSymmetricIfNeeded = 8,
		XYAxisSymmetric = 16,
		XYAxisSymmetricIfNeeded = 32
	};
	
	enum 
	{
		XAxisAtOrigin = 1,
		YAxisAtOrigin = 2,
		XYAxisAtOrigin = 3
	};

	enum 
	{
		ChartDir_StartOfHourFilterTag = 1,
		ChartDir_StartOfDayFilterTag = 2,
		ChartDir_StartOfWeekFilterTag = 3,
		ChartDir_StartOfMonthFilterTag = 4,
		ChartDir_StartOfYearFilterTag = 5,
		ChartDir_RegularSpacingFilterTag = 6,
		ChartDir_AllPassFilterTag = 7,
		ChartDir_NonePassFilterTag = 8,
		ChartDir_SelectItemFilterTag = 9
	};

	static inline int StartOfHourFilter(int labelStep = 1, double initialMargin = 0.05)
	{ return CChart_encodeFilter(ChartDir_StartOfHourFilterTag, labelStep, initialMargin); }
	static inline int StartOfDayFilter(int labelStep = 1, double initialMargin = 0.05)
	{ return CChart_encodeFilter(ChartDir_StartOfDayFilterTag, labelStep, initialMargin); }
	static inline int StartOfWeekFilter(int labelStep = 1, double initialMargin = 0.05)
	{ return CChart_encodeFilter(ChartDir_StartOfWeekFilterTag, labelStep, initialMargin); }
	static inline int StartOfMonthFilter(int labelStep = 1, double initialMargin = 0.05)
	{ return CChart_encodeFilter(ChartDir_StartOfMonthFilterTag, labelStep, initialMargin); }
	static inline int StartOfYearFilter(int labelStep = 1, double initialMargin = 0.05)
	{ return CChart_encodeFilter(ChartDir_StartOfYearFilterTag, labelStep, initialMargin); }
	static inline int RegularSpacingFilter(int labelStep = 1, int initialMargin = 0)
	{ return CChart_encodeFilter(ChartDir_RegularSpacingFilterTag, labelStep, initialMargin / 4095.0); }
	static inline int AllPassFilter()
	{ return CChart_encodeFilter(ChartDir_AllPassFilterTag, 0, 0); }			
	static inline int NonePassFilter()
	{ return CChart_encodeFilter(ChartDir_NonePassFilterTag, 0, 0); }			
	static inline int SelectItemFilter(int item)
	{ return CChart_encodeFilter(ChartDir_SelectItemFilterTag, item, 0); }

	static inline int getVersion() { return CChart_getVersion(); }
	static inline const char *getDescription() { return CChart_getDescription(); }
	static inline const char *getCopyright() { return CChart_getCopyright(); }
	static inline void getBootLog(char *buffer) { CChart_getBootLog(buffer); }

	static inline bool testFont(const char *font, int fontIndex, double fontHeight, 
						  double fontWidth, double angle, char *buffer)
	{ return CChart_testFont(font, fontIndex, fontHeight, fontWidth, angle, buffer); }

	static inline bool isLicensed() { return CChart_isLicensed(); }
	static inline bool getLicenseAttr(const char *key, char *buffer) 
	{ return CChart_getLicenseAttr(key, buffer); }
	static inline bool setLicenseFile(const char *filename = 0, char *buffer = 0)
	{ return CChart_setLicenseFile(filename, buffer); }
	static inline bool setLicenseCode(const char *licCode, char *buffer = 0)
	{ return CChart_setLicenseCode(licCode, buffer); }

	static inline double chartTime(int y, int m, int d, int h = 0, int n = 0, int s = 0)
	{ return CChart_chartTime(y, m, d, h, n, s); }
	static inline double chartTime2(int t) { return CChart_chartTime2(t); }
	static inline int getChartYMD(double t) { return CChart_getChartYMD(t); }
	static inline int getChartWeekDay(double t) { return ((int)(t / 86400 + 1)) % 7; }

	enum
	{
		NormalGlare = 3,
		ReducedGlare = 2,
		NoGlare = 1
	};

	static inline int glassEffect(int glareSize = Chart::NormalGlare, int glareDirection = Chart::Top,
		int raisedEffect = 5)
	{ return CChart_glassEffect(glareSize, glareDirection, raisedEffect); }
	static inline int softLighting(int direction = Chart::Top, int raisedEffect = 4)
	{ return CChart_softLighting(direction, raisedEffect); }
	static inline int barLighting(double startBrightness = 0.75, double endBrightness = 1.5)
	{ return CChart_barLighting(startBrightness, endBrightness); }
	static inline int cylinderEffect(int orientation = Chart::Center, double ambientIntensity = 0.5, 
			double diffuseIntensity = 0.5, double specularIntensity = 0.75, int shininess = 8)
	{ return CChart_cylinderEffect(orientation, ambientIntensity, diffuseIntensity, specularIntensity, shininess); }

	enum
	{
		AggregateSum = 0,
		AggregateAvg = 1,
		AggregateStdDev = 2,
		AggregateMin = 3,
		AggregateMed = 4,
		AggregateMax = 5,
		AggregatePercentile = 6,
		AggregateFirst = 7,
		AggregateLast = 8,
		AggregateCount = 9
	};

	enum 
	{
		DirectionHorizontal = 0, 
		DirectionVertical = 1, 
		DirectionHorizontalVertical = 2
	};

}  //namespace Chart


///////////////////////////////////////////////////////////////////////////////////////
//	Class wrappers
///////////////////////////////////////////////////////////////////////////////////////

class TTFText
{
private :
	//disable copying
	TTFText(const TTFText &rhs);
	TTFText &operator=(const TTFText &rhs);

	TTFTextInternal *ptr;

public :
	//obsoleted - for compatibility only
	enum AntiAliasMode { NoAntiAlias, AntiAlias, AutoAntiAlias };
	//obsoleted - for compatibility only
	static void destroy(TTFText *t) { t->destroy(); }

	TTFText(TTFTextInternal *ptr) : ptr(ptr) {}
	~TTFText() { CTTFText_destroy(ptr); }
	void destroy() { delete this; }

	int getWidth() const { return CTTFText_getWidth(ptr); }
	int getHeight() const { return CTTFText_getHeight(ptr); }
	int getLineHeight() const { return CTTFText_getLineHeight(ptr); }
	int getLineDistance() const { return CTTFText_getLineDistance(ptr); }

	void draw(int x, int y, int color, int alignment = Chart::TopLeft) const
	{ CTTFText_draw(ptr, x, y, color, alignment); }
};


class DrawArea : public AutoDestroy
{
private :
	//disable copying
	DrawArea(const DrawArea &rhs);
	DrawArea &operator=(const DrawArea &rhs);

	DrawAreaInternal *ptr;
	bool own_this;

public :
	//obsoleted constants - for compatibility only
	enum PaletteMode { TryPalette, ForcePalette, NoPalette };
	//obsoleted constants - for compatibility only
	enum DitherMethod { Quantize, OrderedDither, ErrorDiffusion };

	DrawArea() : ptr(CDrawArea_create()), own_this(true) {}
	static DrawArea* create() { return new DrawArea(); }
	DrawArea(DrawAreaInternal *ptr) : ptr(ptr), own_this(false) {}
	~DrawArea() { if (own_this) CDrawArea_destroy(ptr); }
	void destroy() { delete this; }
	DrawAreaInternal *getInternalPtr() { return ptr; }
	const DrawAreaInternal *getInternalPtr() const { return ptr; }

	void enableVectorOutput() { CDrawArea_enableVectorOutput(ptr); }

	void setSize(int width, int height, int bgColor = 0xffffff)
	{ CDrawArea_setSize(ptr, width, height, bgColor); }
	void resize(int newWidth, int newHeight, int filter = Chart::LinearFilter, double blur = 1)
	{ CDrawArea_resize(ptr, newWidth, newHeight, filter, blur); }
	int getWidth() const { return CDrawArea_getWidth(ptr); }
	int getHeight() const { return CDrawArea_getHeight(ptr); }
	void setClipRect(int left, int top, int right, int bottom)
	{ CDrawArea_setClipRect(ptr, left, top, right, bottom); }
	void setBgColor(int c) { CDrawArea_setBgColor(ptr, c); }

	void move(double xOffset, double yOffset, int bgColor = 0xffffff, int filter = Chart::LinearFilter,
		double blur = 1) { CDrawArea_move(ptr, xOffset, yOffset, bgColor, filter, blur); }
	void rotate(double angle, int bgColor = 0xffffff, double cx = -1, double cy = -1,
		int filter = Chart::LinearFilter, double blur = 1)
		{ CDrawArea_rotate(ptr, angle, bgColor, cx, cy, filter, blur); }
	void hFlip() { CDrawArea_hFlip(ptr); }
	void vFlip() { CDrawArea_vFlip(ptr); }
	void clone(DrawArea *d, int x, int y, int align, int newWidth = -1, int newHeight = -1,
		int filter = Chart::LinearFilter, double blur = 1) const
		{ CDrawArea_clone(ptr, d->ptr, x, y, align, newWidth, newHeight, filter, blur); }

	void pixel(int x, int y, int c) { CDrawArea_pixel(ptr, x, y, c); }
	int getPixel(int x, int y) const { return CDrawArea_getPixel(ptr, x, y); }

	void hline(int x1, int x2, int y, int c) { CDrawArea_hline(ptr, x1, x2, y, c); }
	void vline(int y1, int y2, int x, int c) { CDrawArea_vline(ptr, y1, y2, x, c); }
	void line(double x1, double y1, double x2, double y2, int c, int lineWidth = 1)
	{ CDrawArea_line(ptr, x1, y1, x2, y2, c, lineWidth); }
	void arc(int cx, int cy, int rx, int ry, double a1, double a2, int c)
	{ CDrawArea_arc(ptr, cx, cy, rx, ry, a1, a2, c); }

	void rect(int x1, int y1, int x2, int y2, int edgeColor, int fillColor, int raisedEffect = 0)
	{ CDrawArea_rect(ptr, x1, y1, x2, y2, edgeColor, fillColor, raisedEffect); }
	void polygon(const int *x, const int *y, int noOfPoints, int edgeColor, int fillColor)
	{ polygon(IntArray(x, noOfPoints), IntArray(y, noOfPoints), edgeColor, fillColor); }
	void polygon(IntArray x, IntArray y, int edgeColor, int fillColor)
	{ CDrawArea_polygon2(ptr, x.data, x.len, y.data, y.len, edgeColor, fillColor); }
	void polygon(DoubleArray x, DoubleArray y, int edgeColor, int fillColor)
	{ CDrawArea_polygon(ptr, x.data, x.len, y.data, y.len, edgeColor, fillColor); }
	void surface(double x1, double y1, double x2, double y2, int depthX, int depthY,
		int edgeColor, int fillColor)
	{ CDrawArea_surface(ptr, x1, y1, x2, y2, depthX, depthY, edgeColor, fillColor); }
	void sector(int cx, int cy, int rx, int ry, double a1, double a2, int edgeColor, int fillColor)
	{ CDrawArea_sector(ptr, cx, cy, rx, ry, a1, a2, edgeColor, fillColor); }
	void cylinder(int cx, int cy, int rx, int ry, double a1, double a2,
		int depthX, int depthY, int edgeColor, int fillColor)
	{ CDrawArea_cylinder(ptr, cx, cy, rx, ry, a1, a2, depthX, depthY, edgeColor, fillColor); }
	void circle(int cx, int cy, int rx, int ry, int edgeColor, int fillColor)
	{ CDrawArea_circle(ptr, cx, cy, rx, ry, edgeColor, fillColor); }
	void ringSector(int cx, int cy, int rx, int ry, int rx2, int ry2,
		double a1, double a2, int edgeColor, int fillColor)
	{ CDrawArea_ringSector(ptr, cx, cy, rx, ry, rx2, ry2, a1, a2, edgeColor, fillColor); }
	void ring(int cx, int cy, int rx, int ry, int rx2, int ry2, int edgeColor, int fillColor)
	{ CDrawArea_ring(ptr, cx, cy, rx, ry, rx2, ry2, edgeColor, fillColor); }

	void fill(int x, int y, int color) { CDrawArea_fill(ptr, x, y, color); }
	void fill(int x, int y, int color, int borderColor) { CDrawArea_fill2(ptr, x, y, color, borderColor); }

	void text(const char *str, const char *font, double fontSize,
		int x, int y, int color)
	{ CDrawArea_text(ptr, str, font, fontSize, x, y, color); }
	void text(const CHARTDIR_WIDECHAR *str, const char *font, double fontSize,
		int x, int y, int color)
	{ CDrawArea_textw(ptr, str, font, fontSize, x, y, color); }
	void text(const char *str, const char *font, int fontIndex, double fontHeight, double fontWidth,
		double angle, bool vertical, int x, int y, int color, int alignment = Chart::TopLeft)
	{ CDrawArea_text2(ptr, str, font, fontIndex, fontHeight, fontWidth, angle, vertical, x, y, color, alignment); }
	void text(const CHARTDIR_WIDECHAR *str, const char *font, int fontIndex, double fontHeight, double fontWidth,
		double angle, bool vertical, int x, int y, int color, int alignment = Chart::TopLeft)
	{ CDrawArea_text2w(ptr, str, font, fontIndex, fontHeight, fontWidth, angle, vertical, x, y, color, alignment); }
	TTFText* text(const char *str, const char *font, double fontSize)
	{ return new TTFText(CDrawArea_text3(ptr, str, font, fontSize)); }
	TTFText* text(const void *str, const char *font, double fontSize)
	{ return new TTFText(CDrawArea_text3w(ptr, str, font, fontSize)); }
	TTFText* text(const CHARTDIR_WIDECHAR *text, const char *font, int fontIndex,
		double fontHeight, double fontWidth, double angle, bool vertical)
	{ return new TTFText(CDrawArea_text4w(ptr, text, font, fontIndex, fontHeight, fontWidth, angle, vertical)); }
	TTFText* text(const char *text, const char *font, int fontIndex,
		double fontHeight, double fontWidth, double angle, bool vertical)
	{ return new TTFText(CDrawArea_text4(ptr, text, font, fontIndex, fontHeight, fontWidth, angle, vertical)); }
	void close(TTFText *text) { delete text; }

	void merge(const DrawArea *d, int x, int y, int align, int transparency)
	{ CDrawArea_merge(ptr, d->ptr, x, y, align, transparency); }
	void tile(const DrawArea *d, int transparency)
	{ CDrawArea_tile(ptr, d->ptr, transparency); }

	void setSearchPath(const char *path) { CDrawArea_setSearchPath(ptr, path); }
	bool loadGIF(const char *filename) { return CDrawArea_loadGIF(ptr, filename); }
	bool loadPNG(const char *filename) { return CDrawArea_loadPNG(ptr, filename); }
	bool loadJPG(const char *filename) { return CDrawArea_loadJPG(ptr, filename); }
	bool loadWMP(const char *filename) { return CDrawArea_loadWMP(ptr, filename); }
	bool load(const char *filename)	{ return CDrawArea_load(ptr, filename); }

	void rAffineTransform(double a, double b, double c, double d, double e, double f,
		int bgColor = 0xffffff, int filter = Chart::LinearFilter, double blur = 1)
	{ CDrawArea_rAffineTransform(ptr, a, b, c, d, e, f, bgColor, filter, blur); }
	void affineTransform(double a, double b, double c, double d, double e, double f,
		int bgColor = 0xffffff, int filter = Chart::LinearFilter, double blur = 1)
	{ CDrawArea_affineTransform(ptr, a, b, c, d, e, f, bgColor, filter, blur); }
	void sphereTransform(int xDiameter, int yDiameter, int bgColor = 0xffffff,
		int filter = Chart::LinearFilter, double blur = 1)
	{ CDrawArea_sphereTransform(ptr, xDiameter, yDiameter, bgColor, filter, blur); }
	void hCylinderTransform(int yDiameter, int bgColor = 0xffffff,
		int filter = Chart::LinearFilter, double blur = 1)
	{ CDrawArea_hCylinderTransform(ptr, yDiameter, bgColor, filter, blur); }
	void vCylinderTransform(int xDiameter, int bgColor = 0xffffff,
		int filter = Chart::LinearFilter, double blur = 1)
	{ CDrawArea_vCylinderTransform(ptr, xDiameter, bgColor, filter, blur); }
	void vTriangleTransform(int tHeight = -1, int bgColor = 0xffffff,
		int filter = Chart::LinearFilter, double blur = 1)
	{ CDrawArea_vTriangleTransform(ptr, tHeight, bgColor, filter, blur); }
	void hTriangleTransform(int tWidth = -1, int bgColor = 0xffffff,
		int filter = Chart::LinearFilter, double blur = 1)
	{ CDrawArea_hTriangleTransform(ptr, tWidth, bgColor, filter, blur); }
	void shearTransform(double xShear, double yShear = 0, int bgColor = 0xffffff,
		int filter = Chart::LinearFilter, double blur = 1)
	{ CDrawArea_shearTransform(ptr, xShear, yShear, bgColor, filter, blur); }
	void waveTransform(int period, double amplitude, double direction = 0,
		double startAngle = 0, bool longitudinal = false, int bgColor = 0xffffff,
		int filter = Chart::LinearFilter, double blur = 1)
	{ CDrawArea_waveTransform(ptr, period, amplitude, direction, startAngle, longitudinal, bgColor, filter, blur); }

	bool out(const char *filename) { return CDrawArea_out(ptr, filename); }

	bool outGIF(const char *filename) { return CDrawArea_outGIF(ptr, filename); }
	bool outPNG(const char *filename) { return CDrawArea_outPNG(ptr, filename); }
	bool outJPG(const char *filename, int quality = 80) { return CDrawArea_outJPG(ptr, filename, quality); }
	bool outWMP(const char *filename) { return CDrawArea_outWMP(ptr, filename); }
	bool outBMP(const char *filename) { return CDrawArea_outBMP(ptr, filename); }
	bool outSVG(const char *filename, const char *options = 0) { return CDrawArea_outSVG(ptr, filename, options); }

	bool outGIF(const char **data, int *len) { return CDrawArea_outGIF2(ptr, data, len); }
	bool outPNG(const char **data, int *len) { return CDrawArea_outPNG2(ptr, data, len); }
	bool outJPG(const char **data, int *len, int quality = 80) { return CDrawArea_outJPG2(ptr, data, len, quality); }
	bool outWMP(const char **data, int *len) { return CDrawArea_outWMP2(ptr, data, len); }
	bool outBMP(const char **data, int *len) { return CDrawArea_outBMP2(ptr, data, len); }

	MemBlock outGIF() { const char *data; int len; outGIF(&data, &len); return MemBlock(data, len); }
	MemBlock outPNG() { const char *data; int len; outPNG(&data, &len); return MemBlock(data, len); }
	MemBlock outJPG(int quality = 80) { const char *data; int len; outJPG(&data, &len, quality); return MemBlock(data, len); }
	MemBlock outWMP() { const char *data; int len; outWMP(&data, &len); return MemBlock(data, len); }
	MemBlock outBMP() { const char *data; int len; outBMP(&data, &len); return MemBlock(data, len); }
	MemBlock outSVG2(const char *options = 0) 
	{ const char *data; int len; CDrawArea_outSVG2(ptr, &data, &len, options); return MemBlock(data, len); }
	MemBlock outSVG() { return outSVG2(); }

	void setPaletteMode(int p) { CDrawArea_setPaletteMode(ptr, p); }
	void setDitherMethod(int m) { CDrawArea_setDitherMethod(ptr, m); }
	void setTransparentColor(int c) { CDrawArea_setTransparentColor(ptr, c); }
	void setAntiAliasText(int a) { CDrawArea_setAntiAliasText(ptr, a); }
	void setAntiAlias(bool shapeAntiAlias = true, int textAntiAlias = Chart::AutoAntiAlias)
	{ CDrawArea_setAntiAlias(ptr, shapeAntiAlias, textAntiAlias); }
	void setInterlace(bool i) { CDrawArea_setInterlace(ptr, i); }

	void setColorTable(const int *colors, int noOfColors, int offset)
	{ CDrawArea_setColorTable(ptr, colors, noOfColors, offset); }
	void setColorTable(IntArray colors, int offset)
	{ setColorTable(colors.data, colors.len, offset); }
	int getARGBColor(int c) { return CDrawArea_getARGBColor(ptr, c); }
	int halfColor(int c) { return CDrawArea_halfColor(ptr, c); }
	int adjustBrightness(int c, double brightness) { return CDrawArea_adjustBrightness(ptr, c, brightness); }

	int dashLineColor(int color, int patternCode = Chart::DashLine)
	{ return CDrawArea_dashLineColor(ptr, color, patternCode); }
	int patternColor(const int *c, int w, int h, int startX = 0, int startY = 0)
	{ return patternColor(IntArray(c, w * h), startX, startY); }
	int patternColor(IntArray colorArray, int height, int startX = 0, int startY = 0)
	{ return CDrawArea_patternColor(ptr, colorArray.data, colorArray.len, height, startX, startY); }
	int patternColor(const char *filename, int startX = 0, int startY = 0)
	{ return CDrawArea_patternColor2(ptr, filename, startX, startY); }
	int gradientColor(int startX, int startY, int endX, int endY, int startColor, int endColor)
	{ return CDrawArea_gradientColor(ptr, startX, startY, endX, endY, startColor, endColor); }
	int gradientColor(IntArray colorArray, double angle = 90, double scale = 1.0, int startX = 0, int startY = 0)
	{ return CDrawArea_gradientColor2(ptr, colorArray.data, colorArray.len, angle, scale, startX, startY); }
	int gradientColor(const int *c, double angle = 90, double scale = 1, int startX = 0, int startY = 0)
	{ return CDrawArea_gradientColor2a(ptr, c, angle, scale, startX, startY); }
	int linearGradientColor(int startX, int startY, int endX, int endY, int startColor, int endColor, bool periodic = false)
	{ return CDrawArea_linearGradientColor(ptr, startX, startY, endX, endY, startColor, endColor, periodic); }
	int linearGradientColor(int startX, int startY, int endX, int endY, IntArray c, bool periodic = false)
	{ return CDrawArea_linearGradientColor2(ptr, startX, startY, endX, endY, c.data, c.len, periodic); }
	int radialGradientColor(int cx, int cy, int rx, int ry, int startColor, int endColor, bool periodic = false)
	{ return CDrawArea_radialGradientColor(ptr, cx, cy, rx, ry, startColor, endColor, periodic); }
	int radialGradientColor(int cx, int cy, int rx, int ry, IntArray c, bool periodic = false)
	{ return CDrawArea_radialGradientColor2(ptr, cx, cy, rx, ry, c.data, c.len, periodic); }

	int reduceColors(int colorCount, bool blackAndWhite = false)
	{ return CDrawArea_reduceColors(ptr, colorCount, blackAndWhite); }
	
	void setDefaultFonts(const char *normal, const char *bold = 0, const char *italic = 0,
		const char *boldItalic = 0)
	{ CDrawArea_setDefaultFonts(ptr, normal, bold, italic, boldItalic); }
	void setFontTable(int index, const char *font)
	{ CDrawArea_setFontTable(ptr, index, font); }
};


class DrawObj : public AutoDestroy
{
private :
	//disable copying
	DrawObj(const DrawObj &rhs);
	DrawObj &operator=(const DrawObj &rhs);

	DrawObjInternal *ptr;

public :
	DrawObj(DrawObjInternal *ptr) : ptr(ptr) {}
	void destroy() { delete this; }
	DrawObjInternal *getInternalPtr() { return ptr; }

	void setZOrder(int z) { CDrawObj_setZOrder(ptr, z); }
	void paint(DrawArea *d) { CDrawObj_paint(ptr, d->getInternalPtr()); }
};


class Box : public DrawObj
{
private :
	//disable copying
	Box(const Box &rhs);
	Box &operator=(const Box &rhs);

	BoxInternal *ptr;

public :
	Box(BoxInternal *ptr) : DrawObj(Box2DrawObj(ptr)), ptr(ptr) {}
	~Box() {}

	void setPos(int x, int y) { CBox_setPos(ptr, x, y); }
	void setSize(int w, int h) { CBox_setSize(ptr, w, h); }
	void setBackground(int color, int edgeColor = -1, int raisedEffect = 0)
	{ CBox_setBackground(ptr, color, edgeColor, raisedEffect); }
	void setRoundedCorners(int r1 = 10, int r2 = -1, int r3 = -1, int r4 = -1)
	{ CBox_setRoundedCorners(ptr, r1, r2, r3, r4); }
	int getLeftX() const { return CBox_getLeftX(ptr); }
	int getTopY() const { return CBox_getTopY(ptr); }
	int getWidth() const { return CBox_getWidth(ptr); }
	int getHeight() const { return CBox_getHeight(ptr); }
	const char *getImageCoor(int offsetX = 0, int offsetY = 0)
	{ return CBox_getImageCoor(ptr, offsetX, offsetY); }
};


class TextBox : public Box
{
private :
	//disable copying
	TextBox(const TextBox &rhs);
	TextBox &operator=(const TextBox &rhs);

	TextBoxInternal *ptr;

public :
	TextBox(TextBoxInternal *ptr) : Box(TextBox2Box(ptr)), ptr(ptr) {}
	~TextBox() {}

	void setText(const char *text) { CTextBox_setText(ptr, text); }
	void setAlignment(int a) { CTextBox_setAlignment(ptr, a); }
	void setFontStyle(const char *font, int fontIndex = 0)
	{ CTextBox_setFontStyle(ptr, font, fontIndex); }
	void setFontSize(double fontHeight, double fontWidth = 0)
	{ CTextBox_setFontSize(ptr, fontHeight, fontWidth); }
	void setFontAngle(double angle, bool vertical = false)
	{ CTextBox_setFontAngle(ptr, angle, vertical); }
	void setFontColor(int color) { CTextBox_setFontColor(ptr, color); }
	void setMargin(int m)  { CTextBox_setMargin(ptr, m); }
	void setMargin(int leftMargin, int rightMargin, int topMargin, int bottomMargin)
	{ CTextBox_setMargin2(ptr, leftMargin, rightMargin, topMargin, bottomMargin); }
	void setWidth(int width) { CTextBox_setWidth(ptr, width); }	
	void setHeight(int height) { CTextBox_setHeight(ptr, height); }
	void setMaxWidth(int maxWidth) { CTextBox_setMaxWidth(ptr, maxWidth); }
	void setTruncate(int maxWidth, int maxLines = 1) 
	{ CTextBox_setTruncate(ptr, maxWidth, maxLines); }
};


//for compatibility with ChartDirector Ver 1.5 C++ code
typedef TextBox TextStyle;


class Line : public DrawObj
{
private :
	//disable copying
	Line(const Line &rhs);
	Line &operator=(const Line &rhs);

	LineInternal *ptr;

public :
	Line(LineInternal *ptr) : DrawObj(Line2DrawObj(ptr)), ptr(ptr) {}
	~Line() {}

	void setPos(int x1, int y1, int x2, int y2) { CLine_setPos(ptr, x1, y1, x2, y2); }
	void setColor(int c) { CLine_setColor(ptr, c); }
	void setWidth(int w) { CLine_setWidth(ptr, w); };
};


class CDMLTable : public DrawObj, protected GarbageContainer
{
private :
	//disable copying
	CDMLTable(const CDMLTable &rhs);
	CDMLTable &operator=(const CDMLTable &rhs);

	CDMLTableInternal *ptr;

	TextBox *makeTextBox(TextBoxInternal *p) 
	{ if (!p) return 0; TextBox *ret = new TextBox(p); reg(ret); return ret; }

public :
	CDMLTable(CDMLTableInternal *ptr) : DrawObj(CDMLTable2DrawObj(ptr)), ptr(ptr) {}
	~CDMLTable() {}

	void setPos(int x, int y, int alignment = Chart::TopLeft)
	{ CCDMLTable_setPos(ptr, x, y, alignment); }

	TextBox *insertCol(int col) { return makeTextBox(CCDMLTable_insertCol(ptr, col)); }
	TextBox *appendCol() { return makeTextBox(CCDMLTable_appendCol(ptr)); }
	int getColCount() const { return CCDMLTable_getColCount(ptr); }
	
	TextBox *insertRow(int row) { return makeTextBox(CCDMLTable_insertRow(ptr, row)); }
	TextBox *appendRow() { return makeTextBox(CCDMLTable_appendRow(ptr)); }
	int getRowCount() const { return CCDMLTable_getRowCount(ptr); }

	TextBox *setText(int col, int row, const char *text) 
	{ return makeTextBox(CCDMLTable_setText(ptr, col, row, text)); }
	TextBox *setCell(int col, int row, int width, int height, const char *text)
	{ return makeTextBox(CCDMLTable_setCell(ptr, col, row, width, height, text)); }
	TextBox *getCell(int col, int row)
	{ return makeTextBox(CCDMLTable_getCell(ptr, col, row)); }

	TextBox *getColStyle(int col) { return makeTextBox(CCDMLTable_getColStyle(ptr, col)); }
	TextBox *getRowStyle(int row) { return makeTextBox(CCDMLTable_getRowStyle(ptr, row)); }
	TextBox *getStyle() { return makeTextBox(CCDMLTable_getStyle(ptr)); }

	void layout() { CCDMLTable_layout(ptr); }
	
	int getColWidth(int col) const { return CCDMLTable_getColWidth(ptr, col); }
	int getRowHeight(int row) const { return CCDMLTable_getRowHeight(ptr, row); }
	int getWidth() const { return CCDMLTable_getWidth(ptr); }
	int getHeight() const { return CCDMLTable_getHeight(ptr); }
};


class LegendBox : public TextBox
{
private :
	//disable copying
	LegendBox(const LegendBox &rhs);
	LegendBox &operator=(const LegendBox &rhs);

	LegendBoxInternal *ptr;

public :
	LegendBox(LegendBoxInternal *ptr) : TextBox(LegendBox2TextBox(ptr)), ptr(ptr) {}
	~LegendBox() {}

	void setCols(int noOfCols) { CLegendBox_setCols(ptr, noOfCols); }
	void setReverse(bool b = true) { CLegendBox_setReverse(ptr, b); }
	void setLineStyleKey(bool b = true) { CLegendBox_setLineStyleKey(ptr, b); }

	void addKey(const char *text, int color, int lineWidth = 0, const DrawArea *drawarea = 0)
	{ CLegendBox_addKey(ptr, text, color, lineWidth, drawarea ? drawarea->getInternalPtr() : 0); }
	void addKey(int pos, const char *text, int color, int lineWidth = 0, const DrawArea *drawarea = 0)
	{ CLegendBox_addKey2(ptr, pos, text, color, lineWidth, drawarea ? drawarea->getInternalPtr() : 0); }
	void setKeySize(int width, int height = -1, int gap = -1)
	{ CLegendBox_setKeySize(ptr, width, height, gap); }
	void setKeySpacing(int keySpacing, int lineSpacing = -1)
	{ CLegendBox_setKeySpacing(ptr, keySpacing, lineSpacing); }
	void setKeyBorder(int edgeColor, int raisedEffect = 0)
	{ CLegendBox_setKeyBorder(ptr, edgeColor, raisedEffect); }

	const char *getImageCoor(int dataItem, int offsetX = 0, int offsetY = 0)
	{ return CLegendBox_getImageCoor(ptr, dataItem, offsetX, offsetY); }
	const char *getHTMLImageMap(const char *url, const char *queryFormat = 0, const char *extraAttr = 0,
		int offsetX = 0, int offsetY = 0)
	{ return CLegendBox_getHTMLImageMap(ptr, url, queryFormat, extraAttr, offsetX, offsetY); }
};


class BaseChart : protected GarbageContainer
{
private :
	//disable copying
	BaseChart(const BaseChart &rhs);
	BaseChart &operator=(const BaseChart &rhs);

	BaseChartInternal *ptr;

public :
	//obsoleted constants - for compatibility only
	enum ImgFormat {PNG, GIF, JPG, WMP};

	BaseChart() : ptr(0) {}
	void init(BaseChartInternal *ptr) { this->ptr = ptr; }
	~BaseChart() { CBaseChart_destroy(ptr); }
	void destroy() { delete this; }
	BaseChartInternal *getInternalPtr() { return ptr; }
	const BaseChartInternal *getInternalPtr() const { return ptr; }

	void enableVectorOutput() { CBaseChart_enableVectorOutput(ptr); }

	//////////////////////////////////////////////////////////////////////////////////////
	//	set overall chart
	//////////////////////////////////////////////////////////////////////////////////////
	void setSize(int width, int height) { CBaseChart_setSize(ptr, width, height); }
	int getWidth() const { return CBaseChart_getWidth(ptr); }
	int getHeight() const { return CBaseChart_getHeight(ptr); }
	void setBackground(int color, int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ CBaseChart_setBackground(ptr, color, edgeColor, raisedEffect); }
	void setBorder(int color)	//deprecated - use setBackground
	{ CBaseChart_setBorder(ptr, color); }
	void setRoundedFrame(int extColor = 0xffffff, int r1 = 10, int r2 = -1, int r3 = -1, int r4 = -1)
	{ CBaseChart_setRoundedFrame(ptr, extColor, r1, r2, r3, r4); }
	void setWallpaper(const char *img) { CBaseChart_setWallpaper(ptr, img); }
	void setBgImage(const char *img, int align = Chart::Center)
	{ CBaseChart_setBgImage(ptr, img, align); }
	void setDropShadow(int color = 0xaaaaaa, int offsetX = 5, int offsetY = 0x7fffffff, int blurRadius = 5)
	{ CBaseChart_setDropShadow(ptr, color, offsetX, offsetY, blurRadius) ; }
	void setTransparentColor(int c)
	{ CBaseChart_setTransparentColor(ptr, c); }
	void setAntiAlias(bool shapeAntiAlias = true, int textAntiAlias = Chart::AutoAntiAlias)
	{ CBaseChart_setAntiAlias(ptr, shapeAntiAlias, textAntiAlias); }
	void setSearchPath(const char *path)
	{ CBaseChart_setSearchPath(ptr, path); }

	TextBox *addTitle(const char *text, const char *font = 0, double fontSize = 12,
		int fontColor = Chart::TextColor, int bgColor = Chart::Transparent, int edgeColor = Chart::Transparent)
	{ TextBox *ret = new TextBox(CBaseChart_addTitle(ptr, text, font, fontSize, fontColor, bgColor, edgeColor)); reg(ret); return ret;}
	TextBox *addTitle(int alignment, const char *text, const char *font = 0, double fontSize = 12,
		int fontColor = Chart::TextColor, int bgColor = Chart::Transparent, int edgeColor = Chart::Transparent)
	{ TextBox *ret = new TextBox(CBaseChart_addTitle2(ptr, alignment, text, font, fontSize, fontColor, bgColor, edgeColor)); reg(ret); return ret; }
	LegendBox *addLegend(int x, int y, bool vertical = true, const char *font = 0,
		double fontSize = 10)
	{ LegendBox *ret = new LegendBox(CBaseChart_addLegend(ptr, x, y, vertical, font, fontSize)); reg(ret); return ret; }
	LegendBox *addLegend2(int x, int y, int noOfCols, const char *font = 0, double fontSize = 10)
	{ LegendBox *ret = new LegendBox(CBaseChart_addLegend2(ptr, x, y, noOfCols, font, fontSize)); reg(ret); return ret; }
	LegendBox *getLegend() { LegendBox *ret = new LegendBox(CBaseChart_getLegend(ptr)); reg(ret); return ret; }

	//////////////////////////////////////////////////////////////////////////////////////
	//	drawing primitives
	//////////////////////////////////////////////////////////////////////////////////////
	DrawArea *getDrawArea() { DrawArea *ret = new DrawArea(CBaseChart_getDrawArea(ptr)); reg(ret); return ret; }
	TextBox *addText(int x, int y, const char *text, const char *font = 0, double fontSize = 8,
		int fontColor = Chart::TextColor, int alignment = Chart::TopLeft, double angle = 0, bool vertical = false)
	{ TextBox *ret = new TextBox(CBaseChart_addText(ptr, x, y, text, font, fontSize, fontColor, alignment, angle, vertical)); reg(ret); return ret; }
	Line *addLine(int x1, int y1, int x2, int y2, int color = Chart::LineColor, int lineWidth = 1)
	{ Line *ret = new Line(CBaseChart_addLine(ptr, x1, y1, x2, y2, color, lineWidth)); reg(ret); return ret; }
	CDMLTable *addTable(int x, int y, int alignment, int col, int row)
	{ CDMLTable *ret = new CDMLTable(CBaseChart_addTable(ptr, x, y, alignment, col, row)); reg(ret); return ret; }
	void addExtraField(StringArray texts) { CBaseChart_addExtraField(ptr, texts.data, texts.len); }
	void addExtraField(DoubleArray numbers) { CBaseChart_addExtraField2(ptr, numbers.data, numbers.len); }

	//////////////////////////////////////////////////////////////////////////////////////
	//	color management methods
	//////////////////////////////////////////////////////////////////////////////////////
	void setColor(int paletteEntry, int color)
	{ setColors(paletteEntry, IntArray(&color, 1)); }
	void setColors(int paletteEntry, const int *colors)
	{ for (int i = 0; i < 0xffff; ++i) if (colors[i] == -1) { setColors(paletteEntry, IntArray(colors, i)); return; } }
	void setColors(int paletteEntry, IntArray colors)
	{ CBaseChart_setColors(ptr, paletteEntry, colors.data, colors.len); }
	void setColors(const int *colors) { setColors(0, colors); }
	void setColors(IntArray colors) { setColors(0, colors); }
	int getColor(int paletteEntry) { return CBaseChart_getColor(ptr, paletteEntry); }
	int halfColor(int c) { return CBaseChart_halfColor(ptr, c); }
	int adjustBrightness(int c, double brightness) { return CBaseChart_adjustBrightness(ptr, c, brightness); }

	int dashLineColor(int color, int patternCode = Chart::DashLine)
	{ return CBaseChart_dashLineColor(ptr, color, patternCode); }
	int patternColor(const int *c, int w, int h, int startX = 0, int startY = 0)
	{ return patternColor(IntArray(c, w * h), startX, startY); }
	int patternColor(IntArray colorArray, int height, int startX = 0, int startY = 0)
	{ return CBaseChart_patternColor(ptr, colorArray.data, colorArray.len, height, startX, startY); }
	int patternColor(const char *filename, int startX = 0, int startY = 0)
	{ return CBaseChart_patternColor2(ptr, filename, startX, startY); }
	int gradientColor(int startX, int startY, int endX, int endY, int startColor, int endColor)
	{ return CBaseChart_gradientColor(ptr, startX, startY, endX, endY, startColor, endColor); }
	int gradientColor(IntArray colorArray, double angle = 90, double scale = 1.0, int startX = 0, int startY = 0)
	{ return CBaseChart_gradientColor2(ptr, colorArray.data, colorArray.len, angle, scale, startX, startY); }
	int gradientColor(const int *c, double angle = 90, double scale = 1, int startX = 0, int startY = 0)
	{ return CBaseChart_gradientColor2a(ptr, c, angle, scale, startX, startY); }
	int linearGradientColor(int startX, int startY, int endX, int endY, int startColor, int endColor, bool periodic = false)
	{ return CBaseChart_linearGradientColor(ptr, startX, startY, endX, endY, startColor, endColor, periodic); }
	int linearGradientColor(int startX, int startY, int endX, int endY, IntArray c, bool periodic = false)
	{ return CBaseChart_linearGradientColor2(ptr, startX, startY, endX, endY, c.data, c.len, periodic); }
	int radialGradientColor(int cx, int cy, int rx, int ry, int startColor, int endColor, bool periodic = false)
	{ return CBaseChart_radialGradientColor(ptr, cx, cy, rx, ry, startColor, endColor, periodic); }
	int radialGradientColor(int cx, int cy, int rx, int ry, IntArray c, bool periodic = false)
	{ return CBaseChart_radialGradientColor2(ptr, cx, cy, rx, ry, c.data, c.len, periodic); }

	//////////////////////////////////////////////////////////////////////////////////////
	//	locale support
	//////////////////////////////////////////////////////////////////////////////////////
	void setDefaultFonts(const char *normal, const char *bold = 0, const char *italic = 0,
		const char *boldItalic = 0)
	{ CBaseChart_setDefaultFonts(ptr, normal, bold, italic, boldItalic); }
	void setFontTable(int index, const char *font)
	{ CBaseChart_setFontTable(ptr, index, font); }

	void setNumberFormat(char thousandSeparator = '~', char decimalPointChar = '.', char signChar = '-')
	{ CBaseChart_setNumberFormat(ptr, thousandSeparator, decimalPointChar, signChar); }
	void setMonthNames(StringArray names)
	{ CBaseChart_setMonthNames(ptr, names.data, names.len); }
	void setWeekDayNames(StringArray names)
	{ CBaseChart_setWeekDayNames(ptr, names.data, names.len); }
	void setAMPM(const char *am, const char *pm)
	{ CBaseChart_setAMPM(ptr, am, pm); }
	const char *formatValue(double value, const char *formatString)
	{ return CBaseChart_formatValue(ptr, value, formatString); }

	//////////////////////////////////////////////////////////////////////////////////////
	//	chart creation methods
	//////////////////////////////////////////////////////////////////////////////////////
	LegendBox *layoutLegend() { LegendBox *ret = new LegendBox(CBaseChart_layoutLegend(ptr)); reg(ret); return ret; }
	void layout() { CBaseChart_layout(ptr); }
	bool makeChart(const char *filename)
	{ return CBaseChart_makeChart(ptr, filename); }
	bool makeChart(int format, const char **data, int *len)
	{ return CBaseChart_makeChart2(ptr, format, data, len); }
	MemBlock makeChart(int format)
	{ const char *data; int len; makeChart(format, &data, &len); return MemBlock(data, len); }
	DrawArea *makeChart()
	{ DrawArea *ret = new DrawArea(CBaseChart_makeChart3(ptr)); reg(ret); return ret; }

	//////////////////////////////////////////////////////////////////////////////////////
	//	image map support
	//////////////////////////////////////////////////////////////////////////////////////
	const char *getHTMLImageMap(const char *url, const char *queryFormat = 0, const char *extraAttr = 0,
		int offsetX = 0, int offsetY = 0)
	{ return CBaseChart_getHTMLImageMap(ptr, url, queryFormat, extraAttr, offsetX, offsetY); }
	const char *getChartMetrics() { return CBaseChart_getChartMetrics(ptr); }	
};


class MultiChart : public BaseChart
{
private :
	//disable copying
	MultiChart(const MultiChart &rhs);
	MultiChart &operator=(const MultiChart &rhs);

	MultiChartInternal *ptr;

public :
	MultiChart(int width, int height, int bgColor = Chart::BackgroundColor,
		int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ ptr = CMultiChart_create(width, height, bgColor, edgeColor, raisedEffect);
	  init(MultiChart2BaseChart(ptr)); }
	static MultiChart *create(int width, int height, int bgColor = Chart::BackgroundColor,
		int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ return new MultiChart(width, height, bgColor, edgeColor, raisedEffect); }
	void addChart(int x, int y, BaseChart *c)
	{ CMultiChart_addChart(ptr, x, y, c->getInternalPtr()); }
	void setMainChart(const BaseChart *c) { CMultiChart_setMainChart(ptr, c->getInternalPtr()); }
};


class Sector : public AutoDestroy, protected GarbageContainer
{
private :
	//disable copying
	Sector(const Sector &rhs);
	Sector &operator=(const Sector &rhs);

	SectorInternal *ptr;

public :
	Sector(SectorInternal *ptr) : ptr(ptr) {}
	~Sector() {}

	void setExplode(int distance = -1)
	{ CSector_setExplode(ptr, distance); }
	void setLabelFormat(const char *formatString)
	{ CSector_setLabelFormat(ptr, formatString); }
	TextBox *setLabelStyle(const char *font = 0, double fontSize = 8,
		int fontColor = Chart::TextColor)
	{ TextBox *ret = new TextBox(CSector_setLabelStyle(ptr, font, fontSize, fontColor)); reg(ret); return ret;}
	void setLabelPos(int pos, int joinLineColor = -1)
	{ CSector_setLabelPos(ptr, pos, joinLineColor); }
	void setLabelLayout(int layoutMethod, int pos = -1)
	{ CSector_setLabelLayout(ptr, layoutMethod, pos); }
	void setJoinLine(int joinLineColor, int joinLineWidth = 1)
	{ CSector_setJoinLine(ptr, joinLineColor, joinLineWidth); }
	void setColor(int color, int edgeColor = -1, int joinLineColor = -1)
	{ CSector_setColor(ptr, color, edgeColor, joinLineColor); }
	void setStyle(int shadingMethod, int edgeColor = -1, int edgeWidth = -1)
	{ CSector_setStyle(ptr, shadingMethod, edgeColor, edgeWidth); }
	const char *getImageCoor(int offsetX = 0, int offsetY = 0)
	{ return CSector_getImageCoor(ptr, offsetX, offsetY); }
	const char *getLabelCoor(int offsetX = 0, int offsetY = 0)
	{ return CSector_getLabelCoor(ptr, offsetX, offsetY); }
};


class PieChart : public BaseChart
{
private :
	//disable copying
	PieChart(const PieChart &rhs);
	PieChart &operator=(const PieChart &rhs);

	PieChartInternal *ptr;

public :
	PieChart(int width, int height, int bgColor = Chart::BackgroundColor,
		int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ ptr = CPieChart_create(width, height, bgColor, edgeColor, raisedEffect);
	  init(PieChart2BaseChart(ptr)); }
	static PieChart *create(int width, int height, int bgColor = Chart::BackgroundColor,
		int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ return new PieChart(width, height, bgColor, edgeColor, raisedEffect); }

	void setPieSize(int x, int y, int r)
	{ CPieChart_setPieSize(ptr, x, y, r); }
	void setDonutSize(int x, int y, int r, int r2)
	{ CPieChart_setDonutSize(ptr, x, y, r, r2); }
	void set3D(int depth = -1, double angle = -1, bool shadowMode = false)
	{ CPieChart_set3D(ptr, depth, angle, shadowMode); }
	void set3D(DoubleArray depths, double angle = 45, bool shadowMode = false)
	{ CPieChart_set3D2(ptr, depths.data, depths.len, angle, shadowMode); }
	void setSectorStyle(int shadingMethod, int edgeColor = -1, int edgeWidth = -1)
	{ CPieChart_setSectorStyle(ptr, shadingMethod, edgeColor, edgeWidth); }
	void setStartAngle(double startAngle, bool clockWise = true)
	{ CPieChart_setStartAngle(ptr, startAngle, clockWise); }
	void setExplode(int sectorNo = -1, int distance = -1)
	{ CPieChart_setExplode(ptr, sectorNo, distance); }
	void setExplodeGroup(int startSector, int endSector, int distance = -1)
	{ CPieChart_setExplodeGroup(ptr, startSector, endSector, distance); }

	void setLabelFormat(const char *formatString)
	{ CPieChart_setLabelFormat(ptr, formatString); }
	TextBox *setLabelStyle(const char *font = 0, double fontSize = 8,
		int fontColor = Chart::TextColor)
	{ TextBox *ret = new TextBox(CPieChart_setLabelStyle(ptr, font, fontSize, fontColor)); reg(ret); return ret;}
	void setLabelPos(int pos, int joinLineColor = -1)
	{ CPieChart_setLabelPos(ptr, pos, joinLineColor); }
	void setLabelLayout(int layoutMethod, int pos = -1, int topBound = -1, int bottomBound = -1)
	{ CPieChart_setLabelLayout(ptr, layoutMethod, pos, topBound, bottomBound); }
	void setJoinLine(int joinLineColor, int joinLineWidth = 1)
	{ CPieChart_setJoinLine(ptr, joinLineColor, joinLineWidth); }
	void setLineColor(int edgeColor, int joinLineColor = -1)
	{ CPieChart_setLineColor(ptr, edgeColor, joinLineColor); }

	void setData(int noOfPoints, const double *data, const char* const* labels = 0)
	{ setData(DoubleArray(data, noOfPoints), StringArray(labels, labels ? noOfPoints : 0)); }
	void setData(DoubleArray data, StringArray labels = StringArray())
	{ CPieChart_setData(ptr, data.data, data.len, labels.data, labels.len); }

	Sector *getSector(int sectorNo)
	{ SectorInternal *p = CPieChart_sector(ptr, sectorNo); if (!p) return 0;
	  Sector *ret = new Sector(p); reg(ret); return ret; }
	Sector *sector(int sectorNo) { return getSector(sectorNo); }
};


class LineObj;
class Mark : public TextBox
{
private :
	//disable copying
	Mark(const Mark &rhs);
	Mark &operator=(const Mark &rhs);

	MarkInternal *ptr;

public :
	Mark(MarkInternal *ptr) : TextBox(Mark2TextBox(ptr)), ptr(ptr) {}
	~Mark() {}

	void setValue(double value) { CMark_setValue(ptr, value); }
	void setMarkColor(int lineColor, int textColor = -1, int tickColor = -1)
	{ CMark_setMarkColor(ptr, lineColor, textColor, tickColor); }
	void setLineWidth(int w) { CMark_setLineWidth(ptr, w); }
	void setDrawOnTop(bool b) { CMark_setDrawOnTop(ptr, b); }
	LineObj *getLine() { return (LineObj *)CMark_getLine(ptr); }
};


class Axis : public AutoDestroy, protected GarbageContainer
{
private :
	//disable copying
	Axis(const Axis &rhs);
	Axis &operator=(const Axis &rhs);

	AxisInternal *ptr;

public :
	Axis(AxisInternal *ptr) : ptr(ptr) {}
	~Axis() {}
	AxisInternal *getInternalPtr() { return ptr; }
	const AxisInternal *getInternalPtr() const { return ptr; }

	TextBox *setLabelStyle(const char *font = 0, double fontSize = 8,
		int fontColor = Chart::TextColor, double fontAngle = 0)
	{ TextBox *ret = new TextBox(CAxis_setLabelStyle(ptr, font, fontSize, fontColor, fontAngle)); reg(ret); return ret; }
	void setLabelFormat(const char *formatString)
	{ CAxis_setLabelFormat(ptr, formatString); }
	void setLabelGap(int d)
	{ CAxis_setLabelGap(ptr, d); }

	void setMultiFormat(int filter1, const char *format1, int filter2, const char *format2, int labelSpan = 1, bool promoteFirst = true)
	{ CAxis_setMultiFormat(ptr, filter1, format1, filter2, format2, labelSpan, promoteFirst); }
	void setMultiFormat(int filterId, const char *formatString, int labelSpan = 1, bool promoteFirst = true)
	{ CAxis_setMultiFormat2(ptr, filterId, formatString, labelSpan, promoteFirst); }
	void setFormatCondition(const char *condition, double operand = 0)
	{ CAxis_setFormatCondition(ptr, condition, operand); }

	TextBox *setTitle(const char *text, const char *font = 0,
		double fontSize = 8, int fontColor = Chart::TextColor)
	{ TextBox *ret = new TextBox(CAxis_setTitle(ptr, text, font, fontSize, fontColor)); reg(ret); return ret; }
	void setTitlePos(int alignment, int titleGap = 3)
	{ CAxis_setTitlePos(ptr, alignment, titleGap); }

	void setColors(int axisColor, int labelColor = Chart::TextColor, int titleColor = -1,
		int tickColor = -1)
	{ CAxis_setColors(ptr, axisColor, labelColor, titleColor, tickColor); }

	void setTickLength(int majorTickLen)
	{ CAxis_setTickLength(ptr, majorTickLen); }
	void setTickLength(int majorTickLen, int minorTickLen)
	{ CAxis_setTickLength2(ptr, majorTickLen, minorTickLen); }
	void setTickWidth(int majorTickWidth, int minorTickWidth = -1)
	{ CAxis_setTickWidth(ptr, majorTickWidth, minorTickWidth); }
	void setTickColor(int majorTickColor, int minorTickColor = -1)
	{ CAxis_setTickColor(ptr, majorTickColor, minorTickColor); }

	void setWidth(int width) { CAxis_setWidth(ptr, width); }
	void setLength(int length) { CAxis_setLength(ptr, length); }
	void setOffset(int x, int y) { CAxis_setOffset(ptr, x, y); }

	void setAngle(double startAngle) { CAxis_setAngle(ptr, startAngle); }
	void setTopMargin(int topMargin) //obsoleted - use setMargin(int, int)
	{ setMargin(topMargin); }
	void setMargin(int topMargin, int bottomMargin = 0)
	{ CAxis_setMargin(ptr, topMargin, bottomMargin); }
	void setIndent(bool indent) { CAxis_setIndent(ptr, indent); }
	void setTickOffset(double offset) { CAxis_setTickOffset(ptr, offset); }
	void setLabelOffset(double offset) { CAxis_setLabelOffset(ptr, offset); }

	void setAutoScale(double topExtension = 0.1, double bottomExtension = 0.1, double zeroAffinity = 0.8)
	{ CAxis_setAutoScale(ptr, topExtension, bottomExtension, zeroAffinity); }
	void setRounding(bool roundMin, bool roundMax)
	{ CAxis_setRounding(ptr, roundMin, roundMax); }
	void setTickDensity(int majorTickSpacing, int minorTickSpacing = -1)
	{ CAxis_setTickDensity(ptr, majorTickSpacing, minorTickSpacing); }
	void setReverse(bool b = true)
	{ CAxis_setReverse(ptr, b); }
	void setMinTickInc(double inc) { CAxis_setMinTickInc(ptr, inc); }

	TextBox *setLabels(int noOfLabels, const char* const* text) //obsoleted - use setLabels(StringArray)
	{ return setLabels(StringArray(text, noOfLabels)); }
	TextBox *setLabels(StringArray labels)
	{ TextBox *ret = new TextBox(CAxis_setLabels(ptr, labels.data, labels.len)); reg(ret); return ret; }
	TextBox *setLabels(DoubleArray labels, const char *formatString = 0)
	{ TextBox *ret = new TextBox(CAxis_setLabels2(ptr, labels.data, labels.len, formatString)); reg(ret); return ret; }
	CDMLTable *makeLabelTable() 
	{ CDMLTable *ret = new CDMLTable(CAxis_makeLabelTable(ptr)); reg(ret); return ret; }
	CDMLTable *getLabelTable()
	{ CDMLTableInternal *p = CAxis_getLabelTable(ptr); if (!p) return 0; 
	  CDMLTable *ret = new CDMLTable(p); reg(ret); return ret; }

	void setLabelStep(int majorTickStep, int minorTickStep = 0, int majorTickOffset = 0, int minorTickOffset = -0x7fffffff)
	{ CAxis_setLabelStep(ptr, majorTickStep, minorTickStep, majorTickOffset, minorTickOffset); }

	void setLinearScale(const char *formatString = 0)
	{ CAxis_setLinearScale3(ptr, formatString); }
	void setLinearScale(double lowerLimit, double upperLimit, StringArray labels)
	{ CAxis_setLinearScale2(ptr, lowerLimit, upperLimit, labels.data, labels.len); }
	void setLinearScale(double lowerLimit, double upperLimit, double majorTickInc = 0, double minorTickInc = 0)
	{ CAxis_setLinearScale(ptr, lowerLimit, upperLimit, majorTickInc, minorTickInc); }

	void setLogScale(bool logScale) //obsoleted - use setLogScale(const char *)/setLinearScale(const char *)
	{ if (logScale) setLogScale(); else setLinearScale(); }

	void setLogScale(const char *formatString = 0)
	{ CAxis_setLogScale3(ptr, formatString); }
	void setLogScale(double lowerLimit, double upperLimit, StringArray labels)
	{ CAxis_setLogScale2(ptr, lowerLimit, upperLimit, labels.data, labels.len); }
	void setLogScale(double lowerLimit, double upperLimit, double majorTickInc = 0, double minorTickInc = 0)
	{ CAxis_setLogScale(ptr, lowerLimit, upperLimit, majorTickInc, minorTickInc); }

	void setDateScale(const char *formatString = 0)
	{ CAxis_setDateScale3(ptr, formatString); }
	void setDateScale(double lowerLimit, double upperLimit, StringArray labels)
	{ CAxis_setDateScale2(ptr, lowerLimit, upperLimit, labels.data, labels.len); }
	void setDateScale(double lowerLimit, double upperLimit,	double majorTickInc = 0, double minorTickInc = 0)
	{ CAxis_setDateScale(ptr, lowerLimit, upperLimit, majorTickInc, minorTickInc); }

	void syncAxis(const Axis *axis, double slope = 1, double intercept = 0)
	{ CAxis_syncAxis(ptr, axis->ptr, slope, intercept); }
	void copyAxis(const Axis *axis)
	{ CAxis_copyAxis(ptr, axis->ptr); }

	void addLabel(double pos, const char *label)
	{ CAxis_addLabel(ptr, pos, label); }

	Mark *addMark(double value, int lineColor, const char *text = 0,
		const char *font = 0, double fontSize = 8)
	{ Mark *ret = new Mark(CAxis_addMark(ptr, value, lineColor, text, font, fontSize)); reg(ret); return ret; }
	void addZone(double startValue, double endValue, int color)
	{ CAxis_addZone(ptr, startValue, endValue, color); }

	int getCoor(double v) const { return CAxis_getCoor(ptr, v); }
	double getMinValue() const { return CAxis_getMinValue(ptr); }
	double getMaxValue() const { return CAxis_getMaxValue(ptr); }
	int getThickness() const { return CAxis_getThickness(ptr); }

	DoubleArray getTicks() const
	{ const double *d; int len; CAxis_getTicks(ptr, &d, &len); return DoubleArray(d, len); }
	const char *getLabel(double i) const { return CAxis_getLabel(ptr, i); }

	const char *getAxisImageMap(int noOfSegments, int mapWidth, const char *url, const char *queryFormat = 0, 
		const char *extraAttr = 0, int offsetX = 0, int offsetY = 0) const
	{ return CAxis_getAxisImageMap(ptr, noOfSegments, mapWidth, url, queryFormat, extraAttr, offsetX, offsetY); }
	const char *getHTMLImageMap(const char *url, const char *queryFormat = 0, const char *extraAttr = 0,
		int offsetX = 0, int offsetY = 0) const
	{ return CAxis_getHTMLImageMap(ptr, url, queryFormat, extraAttr, offsetX, offsetY); }
};

//
//	for compatibility
//
typedef Axis BaseAxis;
typedef BaseAxis XAxis;
typedef BaseAxis YAxis;

//
//	In this version, PolarChart RadialAxis is the same as the XYChart Axis
//
typedef Axis RadialAxis;

//
//	AngularAxis for PolarChart
//
class AngularAxis : public AutoDestroy, protected GarbageContainer
{
private :
	//disable copying
	AngularAxis(const AngularAxis &rhs);
	AngularAxis &operator=(const AngularAxis &rhs);

	AngularAxisInternal *ptr;

public :
	AngularAxis(AngularAxisInternal *ptr) : ptr(ptr) {}
	~AngularAxis() {}

	TextBox *setLabelStyle(const char *font = "bold", double fontSize = 10,
		int fontColor = Chart::TextColor, double fontAngle = 0)
	{ TextBox *ret = new TextBox(CAngularAxis_setLabelStyle(ptr, font, fontSize, fontColor, fontAngle)); reg(ret); return ret; }
	void setLabelGap(int d) { CAngularAxis_setLabelGap(ptr, d); }

	TextBox *setLabels(StringArray labels)
	{ TextBox *ret = new TextBox(CAngularAxis_setLabels(ptr, labels.data, labels.len)); reg(ret); return ret; }
	TextBox *setLabels(DoubleArray labels, const char *formatString = 0)
	{ TextBox *ret = new TextBox(CAngularAxis_setLabels2(ptr, labels.data, labels.len, formatString)); reg(ret); return ret; }
	void addLabel(double pos, const char *label)
	{ CAngularAxis_addLabel(ptr, pos, label); }

	void setLinearScale(double lowerLimit, double upperLimit, StringArray labels)
	{ CAngularAxis_setLinearScale2(ptr, lowerLimit, upperLimit, labels.data, labels.len); }
	void setLinearScale(double lowerLimit, double upperLimit, double majorTickInc = 0, double minorTickInc = 0)
	{ CAngularAxis_setLinearScale(ptr, lowerLimit, upperLimit, majorTickInc, minorTickInc); }

	void addZone(double startValue, double endValue, double startRadius, double endRadius, 
		int fillColor, int edgeColor = -1)
	{ CAngularAxis_addZone(ptr, startValue, endValue, startRadius, endRadius, fillColor, edgeColor); }
	void addZone(double startValue, double endValue, int fillColor, int edgeColor = -1)
	{ CAngularAxis_addZone2(ptr, startValue, endValue, fillColor, edgeColor); }

	const char *getAxisImageMap(int noOfSegments, int mapWidth, const char *url, const char *queryFormat = 0, 
		const char *extraAttr = 0, int offsetX = 0, int offsetY = 0) const
	{ return CAngularAxis_getAxisImageMap(ptr, noOfSegments, mapWidth, url, queryFormat, extraAttr, offsetX, offsetY); }
	const char *getHTMLImageMap(const char *url, const char *queryFormat = 0, const char *extraAttr = 0,
		int offsetX = 0, int offsetY = 0) const
	{ return CAngularAxis_getHTMLImageMap(ptr, url, queryFormat, extraAttr, offsetX, offsetY); }
};


class ColorAxis : public Axis
{
private :
	//disable copying
	ColorAxis(const ColorAxis &rhs);
	ColorAxis &operator=(const ColorAxis &rhs);

	ColorAxisInternal *ptr;

public :
	ColorAxis(ColorAxisInternal *ptr) : Axis(ColorAxis2Axis(ptr)), ptr(ptr) {}
	~ColorAxis() {}

	void setColorGradient(bool isContinuous = true, IntArray colors = IntArray(), int overflowColor = -1, int underflowColor = -1)
	{ CColorAxis_setColorGradient(ptr, isContinuous, colors.data, colors.len, overflowColor, underflowColor); }
	void setAxisPos(int x, int y, int alignment) { CColorAxis_setAxisPos(ptr, x, y, alignment); }
	void setLevels(int maxLevels) { CColorAxis_setLevels(ptr, maxLevels); }
	void setCompactAxis(bool b = true) { CColorAxis_setCompactAxis(ptr, b); }
	void setAxisBorder(int edgeColor, int raisedEffect = 0) { CColorAxis_setAxisBorder(ptr, edgeColor, raisedEffect); }
	void setBoundingBox(int fillColor, int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ CColorAxis_setBoundingBox(ptr, fillColor, edgeColor, raisedEffect); }
	void setBoxMargin(int m)  { CColorAxis_setBoxMargin(ptr, m); }
	void setBoxMargin(int leftMargin, int rightMargin, int topMargin, int bottomMargin)
	{ CColorAxis_setBoxMargin2(ptr, leftMargin, rightMargin, topMargin, bottomMargin); }
	void setRoundedCorners(int r1 = 10, int r2 = -1, int r3 = -1, int r4 = -1)
	{ CColorAxis_setRoundedCorners(ptr, r1, r2, r3, r4); }
	int getBoxWidth() const { return CColorAxis_getBoxWidth(ptr); }
	int getBoxHeight() const { return CColorAxis_getBoxHeight(ptr); }
	int getColor(double z) const { return CColorAxis_getColor(ptr, z); }
};


class DataSet : public AutoDestroy, protected GarbageContainer
{
private :
	//disable copying
	DataSet(const DataSet &rhs);
	DataSet &operator=(const DataSet &rhs);

	DataSetInternal *ptr;

public :
	DataSet(DataSetInternal *ptr) : ptr(ptr) {}
	~DataSet() {}

	void setData(int noOfPoints, const double *data)
	{ setData(DoubleArray(data, noOfPoints)); }
	void setData(DoubleArray data)
	{ CDataSet_setData(ptr, data.data, data.len); }

	void setDataName(const char *name)
	{ CDataSet_setDataName(ptr, name); }
	void setDataColor(int dataColor, int edgeColor = -1,
		int shadowColor = -1, int shadowEdgeColor = -1)
	{ CDataSet_setDataColor(ptr, dataColor, edgeColor, shadowColor, shadowEdgeColor); }

	void setDataSymbol(int symbol, int size = 5,
		int fillColor = -1, int edgeColor = -1, int lineWidth = 1)
	{ CDataSet_setDataSymbol(ptr, symbol, size, fillColor, edgeColor, lineWidth); }
	void setDataSymbol(const char *image)
	{ CDataSet_setDataSymbol2(ptr, image); }
	void setDataSymbol(const DrawArea *obj)
	{ CDataSet_setDataSymbol3(ptr, obj->getInternalPtr()); }
	void setDataSymbol(IntArray polygon, int size = 11, int fillColor = -1, 
		int edgeColor = -1)
	{ CDataSet_setDataSymbol4(ptr, polygon.data, polygon.len, size, fillColor, edgeColor); }
	void setLineWidth(int w)
	{ CDataSet_setLineWidth(ptr, w); }

	void setDataLabelFormat(const char *formatString)
	{ CDataSet_setDataLabelFormat(ptr, formatString); }
	TextBox *setDataLabelStyle(const char *font = 0, double fontSize = 8,
		int fontColor = Chart::TextColor, double fontAngle = 0)
	{ TextBox *ret = new TextBox(CDataSet_setDataLabelStyle(ptr, font, fontSize, fontColor, fontAngle)); reg(ret); return ret;}

	void setUseYAxis2(bool b = true) { CDataSet_setUseYAxis2(ptr, b); }
	void setUseYAxis(const Axis* a) { CDataSet_setUseYAxis(ptr, a->getInternalPtr()); }
};


class Layer : public AutoDestroy, protected GarbageContainer
{
private :
	//disable copying
	Layer(const Layer &rhs);
	Layer &operator=(const Layer &rhs);

	LayerInternal *ptr;

public :
	Layer(LayerInternal *ptr) : ptr(ptr) {}
	~Layer() {}
	LayerInternal *getInternalPtr() { return ptr; }
	const LayerInternal *getInternalPtr() const { return ptr; }

	void moveFront(Layer *layer = 0) { CLayer_moveFront(ptr, layer ? layer->ptr : 0); }
	void moveBack(Layer *layer = 0) { CLayer_moveBack(ptr, layer ? layer->ptr : 0); }
	
	void set3D(int d = -1, int zGap = 0) { CLayer_set3D(ptr, d, zGap); }
	void setLineWidth(int w) { CLayer_setLineWidth(ptr, w); }
	void setBorderColor(int color, int raisedEffect = 0)
	{ CLayer_setBorderColor(ptr, color, raisedEffect); }
	void setLegend(int m) { CLayer_setLegend(ptr, m); }
	void setLegendOrder(int dataSetOrder, int layerOrder = -1)
	{ CLayer_setLegendOrder(ptr, dataSetOrder, layerOrder); }
	const char *getLegendIcon(int dataSetNo) const 
	{ return CLayer_getLegendIcon(ptr, dataSetNo); }

	//obsoleted constants - for compatibility only
	enum DataCombineMethod { Overlay, Stack, Depth, Side };

	void setDataCombineMethod(int m) { CLayer_setDataCombineMethod(ptr, m); }
	void setBaseLine(double baseLine) { CLayer_setBaseLine(ptr, baseLine); }
	
	DataSet *addDataSet(int noOfPoints, const double *d, int color = -1, const char *name = 0)
	{ return addDataSet(DoubleArray(d, noOfPoints), color, name); }
	DataSet *addDataSet(DoubleArray data, int color = -1, const char *name = 0)
	{ DataSet *ret = new DataSet(CLayer_addDataSet(ptr, data.data, data.len, color, name)); reg(ret); return ret; }
	void addDataGroup(const char *name = 0) { CLayer_addDataGroup(ptr, name); }
	void addExtraField(StringArray texts) { CLayer_addExtraField(ptr, texts.data, texts.len); }
	void addExtraField(DoubleArray numbers) { CLayer_addExtraField2(ptr, numbers.data, numbers.len); }
	DataSet *getDataSet(int dataSet)
	{ DataSetInternal *p = CLayer_getDataSet(ptr, dataSet); if (!p) return 0;
	  DataSet *ret = new DataSet(p); reg(ret); return ret;}
	void setUseYAxis2(bool b = true) { CLayer_setUseYAxis2(ptr, b); }
	void setUseYAxis(const Axis* a) { CLayer_setUseYAxis(ptr, a->getInternalPtr()); }

	void setXData(DoubleArray xData) { CLayer_setXData(ptr, xData.data, xData.len); }
	void setXData(double minValue, double maxValue) { CLayer_setXData2(ptr, minValue, maxValue); }
	void alignLayer(const Layer *layer, int dataSet) { CLayer_alignLayer(ptr, layer->getInternalPtr(), dataSet); } 

	int getXCoor(double v) const { return CLayer_getXCoor(ptr, v); }
	int getYCoor(double v, bool yAxis = true) const { return CLayer_getYCoor(ptr, v, yAxis); }
	int getYCoor(double v, const Axis *yAxis) const 
	{ if (0 == yAxis) return getYCoor(v); else return CLayer_getYCoor2(ptr, v, yAxis->getInternalPtr()); }
	int xZoneColor(double threshold, int belowColor, int aboveColor)
	{ return CLayer_xZoneColor(ptr, threshold, belowColor, aboveColor); }
	int yZoneColor(double threshold, int belowColor, int aboveColor, bool yAxis = true)
	{ return CLayer_yZoneColor(ptr, threshold, belowColor, aboveColor, yAxis); }
	int yZoneColor(double threshold, int belowColor, int aboveColor, const Axis *yAxis)
	{ if (0 == yAxis) return yZoneColor(threshold, belowColor, aboveColor); 
	  else return CLayer_yZoneColor2(ptr, threshold, belowColor, aboveColor, yAxis->getInternalPtr()); }
	
	//*** Note ***: The default value -0x7fffffff is maintained for compatibility
	const char *getImageCoor(int dataSet, int dataItem = -0x7fffffff, int offsetX = 0, int offsetY = 0)
	{ return CLayer_getImageCoor(ptr, dataSet, dataItem, offsetX, offsetY); }
	const char *getImageCoor2(int dataItem, int offsetX = 0, int offsetY = 0)
	{ return CLayer_getImageCoor2(ptr, dataItem, offsetX, offsetY); }
	const char *getHTMLImageMap(const char *url, const char *queryFormat = 0,
		const char *extraAttr = 0, int offsetX = 0, int offsetY = 0)
	{ return CLayer_getHTMLImageMap(ptr, url, queryFormat, extraAttr, offsetX, offsetY); }
	void setHTMLImageMap(const char *url, const char *queryFormat = 0, const char *extraAttr = 0)
	{ CLayer_setHTMLImageMap(ptr, url, queryFormat, extraAttr); }

	void setDataLabelFormat(const char *formatString)
	{ CLayer_setDataLabelFormat(ptr, formatString); }
	TextBox *setDataLabelStyle(const char *font = 0, double fontSize = 8,
		int fontColor = Chart::TextColor, double fontAngle = 0)
	{ TextBox *ret = new TextBox(CLayer_setDataLabelStyle(ptr, font, fontSize, fontColor, fontAngle)); reg(ret); return ret;}

	void setAggregateLabelFormat(const char *formatString)
	{ CLayer_setAggregateLabelFormat(ptr, formatString); }
	TextBox *setAggregateLabelStyle(const char *font = 0, double fontSize = 8,
		int fontColor = Chart::TextColor, double fontAngle = 0)
	{ TextBox *ret = new TextBox(CLayer_setAggregateLabelStyle(ptr, font, fontSize, fontColor, fontAngle)); reg(ret); return ret;}

	TextBox *addCustomDataLabel(int dataSet, int dataItem, const char *label,
		const char *font = 0, double fontSize = 8, int fontColor = Chart::TextColor, double fontAngle = 0)
	{ TextBox *ret = new TextBox(CLayer_addCustomDataLabel(ptr, dataSet, dataItem, label, font, fontSize, fontColor, fontAngle)); reg(ret); return ret;}
	TextBox *addCustomAggregateLabel(int dataItem, const char *label, const char *font = 0,
		double fontSize = 8, int fontColor = Chart::TextColor, double fontAngle = 0)
	{ TextBox *ret = new TextBox(CLayer_addCustomAggregateLabel(ptr, dataItem, label, font, fontSize, fontColor, fontAngle)); reg(ret); return ret;}
	TextBox *addCustomGroupLabel(int dataGroup, int dataItem, const char *label, 
		const char *font = 0, double fontSize = 8, int fontColor = Chart::TextColor, double fontAngle = 0)
	{ TextBox *ret = new TextBox(CLayer_addCustomGroupLabel(ptr, dataGroup, dataItem, label, font, fontSize, fontColor, fontAngle)); reg(ret); return ret;}
};


class BarLayer : public Layer
{
private :
	//disable copying
	BarLayer(const BarLayer &rhs);
	BarLayer &operator=(const BarLayer &rhs);

	BarLayerInternal *ptr;

public :
	BarLayer(BarLayerInternal *ptr) : Layer(BarLayer2Layer(ptr)), ptr(ptr) {}
	~BarLayer() {}

	void setBarGap(double barGap, double subBarGap = Chart::NoValue)
	{ CBarLayer_setBarGap(ptr, barGap, subBarGap); }
	void setBarWidth(int barWidth, int subBarWidth = -1)
	{ CBarLayer_setBarWidth(ptr, barWidth, subBarWidth); }
	void setMinLabelSize(int s)
	{ CBarLayer_setMinLabelSize(ptr, s); }
	void setMinImageMapSize(int s)
	{ CBarLayer_setMinImageMapSize(ptr, s); }

	void setBarShape(int shape, int dataGroup = -1, int dataItem = -1)
	{ CBarLayer_setBarShape(ptr, shape, dataGroup, dataItem); }
	void setBarShape(IntArray shape, int dataGroup = -1, int dataItem = -1)
	{ CBarLayer_setBarShape2(ptr, shape.data, shape.len, dataGroup, dataItem); }
	void setIconSize(int height, int width = -1)
	{ CBarLayer_setIconSize(ptr, height, width); }
	void setOverlapRatio(double overlapRatio, bool firstOnTop = true)
	{ CBarLayer_setOverlapRatio(ptr, overlapRatio, firstOnTop); }
};


class InterLineLayer : public Layer
{
private :
	//disable copying
	InterLineLayer(const InterLineLayer &rhs);
	InterLineLayer &operator=(const InterLineLayer &rhs);

	InterLineLayerInternal *ptr;

public :
	InterLineLayer(InterLineLayerInternal *ptr) : Layer(InterLineLayer2Layer(ptr)), ptr(ptr) {}
	~InterLineLayer() {}

	void setGapColor(int gapColor12, int gapColor21 = -1)
	{ CInterLineLayer_setGapColor(ptr, gapColor12, gapColor21); }
};


class LineLayer : public Layer
{
private :
	//disable copying
	LineLayer(const LineLayer &rhs);
	LineLayer &operator=(const LineLayer &rhs);

	LineLayerInternal *ptr;

public :
	LineLayer(LineLayerInternal *ptr) : Layer(LineLayer2Layer(ptr)), ptr(ptr) {}
	~LineLayer() {}

	void setSymbolScale(DoubleArray zDataX, int scaleTypeX = Chart::PixelScale,
		DoubleArray zDataY = DoubleArray(), int scaleTypeY = Chart::PixelScale)
	{ CLineLayer_setSymbolScale(ptr, zDataX.data, zDataX.len, scaleTypeX,
		zDataY.data, zDataY.len, scaleTypeY); }

	void setGapColor(int lineColor, int lineWidth = -1)
	{ CLineLayer_setGapColor(ptr, lineColor, lineWidth); }
	void setImageMapWidth(int width)
	{ CLineLayer_setImageMapWidth(ptr, width); }

	LineObj *getLine(int dataSet = 0)
	{ return (LineObj *)CLineLayer_getLine(ptr, dataSet); }
};


//In the current version, a Scatter chart is implemented as a
//special configuration of a Line chart.
typedef LineLayer ScatterLayer;

class SplineLayer : public LineLayer
{
private :
	//disable copying
	SplineLayer(const SplineLayer &rhs);
	SplineLayer &operator=(const SplineLayer &rhs);

	SplineLayerInternal *ptr;

public :
	SplineLayer(SplineLayerInternal *ptr) : LineLayer(SplineLayer2LineLayer(ptr)), ptr(ptr) {}
	~SplineLayer() {}

	void setTension(double tension) { CSplineLayer_setTension(ptr, tension); }
	void setMonotonicity(int m) { CSplineLayer_setMonotonicity(ptr, m); }
};


class StepLineLayer : public LineLayer
{
private :
	//disable copying
	StepLineLayer(const StepLineLayer &rhs);
	StepLineLayer &operator=(const StepLineLayer &rhs);

	StepLineLayerInternal *ptr;

public :
	StepLineLayer(StepLineLayerInternal *ptr) : LineLayer(StepLineLayer2LineLayer(ptr)), ptr(ptr) {}
	~StepLineLayer() {}

	void setAlignment(int a) { CStepLineLayer_setAlignment(ptr, a); }
};


class AreaLayer : public Layer
{
private :
	//disable copying
	AreaLayer(const AreaLayer &rhs);
	AreaLayer &operator=(const AreaLayer &rhs);

	AreaLayerInternal *ptr;

public :
	AreaLayer(AreaLayerInternal *ptr) : Layer(AreaLayer2Layer(ptr)), ptr(ptr) {}
	~AreaLayer() {}

	void setMinLabelSize(int s) { CAreaLayer_setMinLabelSize(ptr, s); }
	void setGapColor(int fillColor) { CAreaLayer_setGapColor(ptr, fillColor); }
};


class BaseBoxLayer : public Layer
{
private :
	//disable copying
	BaseBoxLayer(const BaseBoxLayer &rhs);
	BaseBoxLayer &operator=(const BaseBoxLayer &rhs);

	BaseBoxLayerInternal *ptr;

public :
	BaseBoxLayer(BaseBoxLayerInternal *ptr) : Layer(BaseBoxLayer2Layer(ptr)), ptr(ptr) {}
	~BaseBoxLayer() {}

	void setDataGap(double gap) { CBaseBoxLayer_setDataGap(ptr, gap); }
	void setDataWidth(int width) { CBaseBoxLayer_setDataWidth(ptr, width); }
	void setMinImageMapSize(int size) { CBaseBoxLayer_setMinImageMapSize(ptr, size); }
};


class HLOCLayer : public BaseBoxLayer
{
private :
	//disable copying
	HLOCLayer(const HLOCLayer &rhs);
	HLOCLayer &operator=(const HLOCLayer &rhs);

	HLOCLayerInternal *ptr;

public :
	HLOCLayer(HLOCLayerInternal *ptr) : BaseBoxLayer(HLOCLayer2BaseBoxLayer(ptr)), ptr(ptr) {}
	~HLOCLayer() {}

	void setColorMethod(int colorMethod, int riseColor, int fallColor = -1,
		double leadValue = -1.7E308)
	{ CHLOCLayer_setColorMethod(ptr, colorMethod, riseColor, fallColor, leadValue); }
};


class CandleStickLayer : public BaseBoxLayer
{
private :
	//disable copying
	CandleStickLayer(const CandleStickLayer &rhs);
	CandleStickLayer &operator=(const CandleStickLayer &rhs);

	CandleStickLayerInternal *ptr;

public :
	CandleStickLayer(CandleStickLayerInternal *ptr) : BaseBoxLayer(CandleStickLayer2BaseBoxLayer(ptr)), ptr(ptr) {}
	~CandleStickLayer() {}
};


class BoxWhiskerLayer : public BaseBoxLayer
{
private :
	//disable copying
	BoxWhiskerLayer(const BoxWhiskerLayer &rhs);
	BoxWhiskerLayer &operator=(const BoxWhiskerLayer &rhs);

	BoxWhiskerLayerInternal *ptr;

public :
	BoxWhiskerLayer(BoxWhiskerLayerInternal *ptr) : BaseBoxLayer(BoxWhiskerLayer2BaseBoxLayer(ptr)), ptr(ptr) {}
	~BoxWhiskerLayer() {}

	void setBoxColors(IntArray colors, StringArray names = StringArray())
	{ CBoxWhiskerLayer_setBoxColors(ptr, colors.data, colors.len, names.data, names.len); }
	void setBoxColor(int item, int boxColor)
	{ CBoxWhiskerLayer_setBoxColor(ptr, item, boxColor); }
	void setWhiskerBrightness(double whiskerBrightness)
	{ CBoxWhiskerLayer_setWhiskerBrightness(ptr, whiskerBrightness); }
};


class TrendLayer : public Layer
{
private :
	//disable copying
	TrendLayer(const TrendLayer &rhs);
	TrendLayer &operator=(const TrendLayer &rhs);

	TrendLayerInternal *ptr;

public :
	TrendLayer(TrendLayerInternal *ptr) : Layer(TrendLayer2Layer(ptr)), ptr(ptr) {}
	~TrendLayer() {}

	void setRegressionType(int regressionType) { CTrendLayer_setRegressionType(ptr, regressionType); }

	void addConfidenceBand(double confidence, int upperFillColor, int upperEdgeColor = Chart::Transparent,
		int upperLineWidth = 1, int lowerFillColor = -1, int lowerEdgeColor = -1, int lowerLineWidth = -1)
	{ CTrendLayer_addConfidenceBand(ptr, confidence, upperFillColor, upperEdgeColor,
		upperLineWidth, lowerFillColor, lowerEdgeColor, lowerLineWidth); }
	void addPredictionBand(double confidence, int upperFillColor, int upperEdgeColor = Chart::Transparent,
		int upperLineWidth = 1, int lowerFillColor = -1, int lowerEdgeColor = -1, int lowerLineWidth = -1)
	{ CTrendLayer_addPredictionBand(ptr, confidence, upperFillColor, upperEdgeColor,
		upperLineWidth, lowerFillColor, lowerEdgeColor, lowerLineWidth); }

	double getSlope() { return CTrendLayer_getSlope(ptr); }
	double getIntercept() { return CTrendLayer_getIntercept(ptr); }
	double getCorrelation() { return CTrendLayer_getCorrelation(ptr); }
	double getStdError() { return CTrendLayer_getStdError(ptr); }
	double getCoefficient(int i) { return CTrendLayer_getCoefficient(ptr, i); }

	void setImageMapWidth(int width) { CTrendLayer_setImageMapWidth(ptr, width); }
	LineObj *getLine() { return (LineObj *)CTrendLayer_getLine(ptr); }
};


class VectorLayer : public Layer
{
private :
	//disable copying
	VectorLayer(const VectorLayer &rhs);
	VectorLayer &operator=(const VectorLayer &rhs);

	VectorLayerInternal *ptr;

public :
	VectorLayer(VectorLayerInternal *ptr) : Layer(VectorLayer2Layer(ptr)), ptr(ptr) {}
	~VectorLayer() {}

	void setVector(DoubleArray lengths, DoubleArray directions, int lengthScale = Chart::PixelScale)
	{ CVectorLayer_setVector(ptr, lengths.data, lengths.len, directions.data, directions.len, lengthScale); }
	void setArrowHead(int width, int height = 0) { CVectorLayer_setArrowHead(ptr, width, height); }
	void setArrowHead(IntArray polygon) { CVectorLayer_setArrowHead2(ptr, polygon.data, polygon.len); }
	void setArrowStem(IntArray polygon) { CVectorLayer_setArrowStem(ptr, polygon.data, polygon.len); }
	void setArrowAlignment(int alignment) { CVectorLayer_setArrowAlignment(ptr, alignment); }
	void setIconSize(int height, int width = 0) { CVectorLayer_setIconSize(ptr, height, width); }
	void setVectorMargin(double startMargin, double endMargin = Chart::NoValue)
	{ CVectorLayer_setVectorMargin(ptr, startMargin, endMargin); }
};


class ContourLayer : public Layer
{
private :
	//disable copying
	ContourLayer(const ContourLayer &rhs);
	ContourLayer &operator=(const ContourLayer &rhs);

	ContourLayerInternal *ptr;

public :
	ContourLayer(ContourLayerInternal *ptr) : Layer(ContourLayer2Layer(ptr)), ptr(ptr) {}
	~ContourLayer() {}

	void setZData(DoubleArray zData) { CContourLayer_setZData(ptr, zData.data, zData.len); }
	void setSmoothInterpolation(bool b) { CContourLayer_setSmoothInterpolation(ptr, b); }
	void setContourColor(int contourColor, int minorContourColor = -1) 
	{ CContourLayer_setContourColor(ptr, contourColor, minorContourColor); }
	void setContourWidth(int contourWidth, int minorContourWidth = -1)
	{ CContourLayer_setContourWidth(ptr, contourWidth, minorContourWidth); }
	ColorAxis *setColorAxis(int x, int y, int alignment, int length, int orientation) 
	{ ColorAxis *ret = new ColorAxis(CContourLayer_setColorAxis(ptr, x, y, alignment, length, orientation)); reg(ret); return ret; }
	ColorAxis *colorAxis()
	{ ColorAxis *ret = new ColorAxis(CContourLayer_colorAxis(ptr)); reg(ret); return ret; }
};


class PlotArea : public AutoDestroy
{
private :
	//disable copying
	PlotArea(const PlotArea &rhs);
	PlotArea &operator=(const PlotArea &rhs);

	PlotAreaInternal *ptr;

public :
	PlotArea(PlotAreaInternal *ptr) : ptr(ptr) {}
	~PlotArea() {}

	void setBackground(int color, int altBgColor = -1, int edgeColor = -1)
	{ CPlotArea_setBackground(ptr, color, altBgColor, edgeColor); }
	void setBackground(const char *img, int align = Chart::Center)
	{ CPlotArea_setBackground2(ptr, img, align); }
	void set4QBgColor(int Q1Color, int Q2Color, int Q3Color, int Q4Color, int edgeColor = -1)
	{ CPlotArea_set4QBgColor(ptr, Q1Color, Q2Color, Q3Color, Q4Color, edgeColor); }
	void setAltBgColor(bool horizontal, int color1, int color2, int edgeColor = -1)
	{ CPlotArea_setAltBgColor(ptr, horizontal, color1, color2, edgeColor); }
	void setGridColor(int hGridColor, int vGridColor = Chart::Transparent,
		int minorHGridColor = -1, int minorVGridColor = -1)
	{ CPlotArea_setGridColor(ptr, hGridColor, vGridColor, minorHGridColor, minorVGridColor); }
	void setGridWidth(int hGridWidth, int vGridWidth = -1,
		int minorHGridWidth = -1, int minorVGridWidth = -1)
	{ CPlotArea_setGridWidth(ptr, hGridWidth, vGridWidth, minorHGridWidth, minorVGridWidth); }
	void setGridAxis(const Axis *xGridAxis, const Axis *yGridAxis)
	{ CPlotArea_setGridAxis(ptr, xGridAxis ? xGridAxis->getInternalPtr() : 0, yGridAxis ? yGridAxis->getInternalPtr() : 0); }
	void moveGridBefore(Layer *layer = 0) { CPlotArea_moveGridBefore(ptr, layer ? layer->getInternalPtr() : 0); }

	int getLeftX() const { return CPlotArea_getLeftX(ptr); }
	int getTopY() const { return CPlotArea_getTopY(ptr); }
	int getWidth() const { return CPlotArea_getWidth(ptr); }
	int getHeight() const { return CPlotArea_getHeight(ptr); }
};


class XYChart : public BaseChart
{
private :
	//disable copying
	XYChart(const XYChart &rhs);
	XYChart &operator=(const XYChart &rhs);

	XYChartInternal *ptr;

public :
	XYChart(int width, int height, int bgColor = Chart::BackgroundColor,
		int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ ptr = CXYChart_create(width, height, bgColor, edgeColor, raisedEffect);
	  init(XYChart2BaseChart(ptr)); }
	static XYChart *create(int width, int height, int bgColor = Chart::BackgroundColor,
		int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ return new XYChart(width, height, bgColor, edgeColor, raisedEffect); }

	Axis *addAxis(int align, int offset) { Axis *ret = new Axis(CXYChart_addAxis(ptr, align, offset)); reg(ret); return ret; }
	Axis *yAxis() { Axis *ret = new Axis(CXYChart_yAxis(ptr)); reg(ret); return ret; }
	Axis *yAxis2() { Axis *ret = new Axis(CXYChart_yAxis2(ptr)); reg(ret); return ret; }
	void syncYAxis(double slope = 1, double intercept = 0)
	{ CXYChart_syncYAxis(ptr, slope, intercept); }
	void setYAxisOnRight(bool b = true) { CXYChart_setYAxisOnRight(ptr, b); }
	Axis *xAxis() { Axis *ret = new Axis(CXYChart_xAxis(ptr)); reg(ret); return ret; }
	Axis *xAxis2() { Axis *ret = new Axis(CXYChart_xAxis2(ptr)); reg(ret); return ret; }
	void setXAxisOnTop(bool b = true) { CXYChart_setXAxisOnTop(ptr, b); }
	void swapXY(bool b = true) { CXYChart_swapXY(ptr, b); }
	void setAxisAtOrigin(int originMode = Chart::XYAxisAtOrigin, int symmetryMode = 0)
	{ CXYChart_setAxisAtOrigin(ptr, originMode, symmetryMode); }

	int getXCoor(double v) const { return CXYChart_getXCoor(ptr, v); }
	int getYCoor(double v, const Axis *yAxis = 0) 
	{ return CXYChart_getYCoor(ptr, v, (0 == yAxis) ? 0 : yAxis->getInternalPtr()); }

	int xZoneColor(double threshold, int belowColor, int aboveColor)
	{ return CXYChart_xZoneColor(ptr, threshold, belowColor, aboveColor); }
	int yZoneColor(double threshold, int belowColor, int aboveColor, Axis *yAxis = 0)
	{ return CXYChart_yZoneColor(ptr, threshold, belowColor, aboveColor, (0 == yAxis) ? 0 : yAxis->getInternalPtr()); }

	PlotArea *setPlotArea(int x, int y, int width, int height,
		int bgColor = Chart::Transparent, int altBgColor = -1, int edgeColor = -1,
		int hGridColor = 0xc0c0c0, int vGridColor = Chart::Transparent)
	{ PlotArea *ret = new PlotArea(CXYChart_setPlotArea(ptr, x, y, width, height, bgColor,
		altBgColor, edgeColor, hGridColor, vGridColor)); reg(ret); return ret; }
	PlotArea *getPlotArea() { PlotArea *ret = new PlotArea(CXYChart_getPlotArea(ptr)); reg(ret); return ret; }
	void setClipping(int margin = 0) { CXYChart_setClipping(ptr, margin); }
	void setTrimData(int startPos, int len = 0x7fffffff) { CXYChart_setTrimData(ptr, startPos, len); }

	BarLayer *addBarLayer(int noOfPoints, const double *data, int color = -1, const char *name = 0, int depth = 0)
	{ return addBarLayer(DoubleArray(data, noOfPoints), color, name, depth); }
	BarLayer *addBarLayer(DoubleArray data, int color = -1, const char *name = 0, int depth = 0)
	{ BarLayer *ret = new BarLayer(CXYChart_addBarLayer(ptr, data.data, data.len, color, name, depth)); reg(ret); return ret; }
	BarLayer *addBarLayer(int noOfPoints, const double *data, const int *colors, const char* const* names = 0, int depth = 0)
	{ return addBarLayer(DoubleArray(data, noOfPoints), IntArray(colors, colors ? noOfPoints : 0), StringArray(names, names ? noOfPoints : 0), depth); }
	BarLayer *addBarLayer(DoubleArray data, IntArray colors, StringArray names = StringArray(), int depth = 0)
	{ BarLayer *ret = new BarLayer(CXYChart_addBarLayer3(ptr, data.data, data.len, colors.data, colors.len, names.data, names.len, depth)); reg(ret); return ret; }
	BarLayer *addBarLayer(int dataCombineMethod = Chart::Side, int depth = 0)
	{ BarLayer *ret = new BarLayer(CXYChart_addBarLayer2(ptr, dataCombineMethod, depth)); reg(ret); return ret; }

	LineLayer *addLineLayer(int noOfPoints, const double *data, int color = -1, const char *name = 0, int depth = 0)
	{ return addLineLayer(DoubleArray(data, noOfPoints), color, name, depth); }
	LineLayer *addLineLayer(DoubleArray data, int color = -1, const char *name = 0, int depth = 0)
	{ LineLayer *ret = new LineLayer(CXYChart_addLineLayer(ptr, data.data, data.len, color, name, depth)); reg(ret); return ret; }
	LineLayer *addLineLayer(int dataCombineMethod = Chart::Overlay, int depth = 0)
	{ LineLayer *ret = new LineLayer(CXYChart_addLineLayer2(ptr, dataCombineMethod, depth)); reg(ret); return ret; }

	AreaLayer *addAreaLayer(int noOfPoints, const double *data, int color = -1, const char *name = 0, int depth = 0)
	{ return addAreaLayer(DoubleArray(data, noOfPoints), color, name, depth); }
	AreaLayer *addAreaLayer(DoubleArray data, int color = -1, const char *name = 0, int depth = 0)
	{ AreaLayer *ret = new AreaLayer(CXYChart_addAreaLayer(ptr, data.data, data.len, color, name, depth)); reg(ret); return ret; }
	AreaLayer *addAreaLayer(int dataCombineMethod = Chart::Stack, int depth = 0)
	{ AreaLayer *ret = new AreaLayer(CXYChart_addAreaLayer2(ptr, dataCombineMethod, depth)); reg(ret); return ret; }

	HLOCLayer *addHLOCLayer()
	{ HLOCLayer *ret = new HLOCLayer(CXYChart_addHLOCLayer2(ptr)); reg(ret); return ret; }
	HLOCLayer *addHLOCLayer(int noOfPoints, const double *highData, const double *lowData, const double *openData = 0,
		const double *closeData = 0, int color = -1)
	{ return addHLOCLayer(DoubleArray(highData, highData ? noOfPoints : 0), DoubleArray(lowData, lowData ? noOfPoints : 0),
		DoubleArray(openData, openData ? noOfPoints : 0), DoubleArray(closeData, closeData ? noOfPoints : 0), color); }
	HLOCLayer *addHLOCLayer(DoubleArray highData, DoubleArray lowData, DoubleArray openData = DoubleArray(),
		DoubleArray closeData = DoubleArray(), int color = -1)
	{ HLOCLayer *ret = new HLOCLayer(CXYChart_addHLOCLayer(ptr, highData.data, highData.len,
		lowData.data, lowData.len, openData.data, openData.len, closeData.data, closeData.len, color)); reg(ret); return ret; }
	HLOCLayer *addHLOCLayer(DoubleArray highData, DoubleArray lowData, DoubleArray openData,
		DoubleArray closeData, int upColor, int downColor, int colorMode = -1, double leadValue = -1.7E308)
	{ HLOCLayer *ret = new HLOCLayer(CXYChart_addHLOCLayer3(ptr, highData.data, highData.len,
		lowData.data, lowData.len, openData.data, openData.len, closeData.data, closeData.len, 
		upColor, downColor, colorMode, leadValue)); reg(ret); return ret; }

	CandleStickLayer *addCandleStickLayer(
		DoubleArray highData, DoubleArray lowData, DoubleArray openData, DoubleArray closeData,
		int riseColor = 0xffffff, int fallColor = 0x0, int edgeColor = Chart::LineColor)
	{ CandleStickLayer *ret = new CandleStickLayer(CXYChart_addCandleStickLayer(ptr, highData.data, highData.len,
		lowData.data, lowData.len, openData.data, openData.len, closeData.data, closeData.len,
		riseColor, fallColor, edgeColor)); reg(ret); return ret; }

	BoxWhiskerLayer *addBoxWhiskerLayer(
		DoubleArray boxTop, DoubleArray boxBottom, DoubleArray maxData = DoubleArray(), DoubleArray minData = DoubleArray(),
		DoubleArray midData = DoubleArray(), int fillColor = -1, int whiskerColor = Chart::LineColor, int edgeColor = -1)
	{ BoxWhiskerLayer *ret = new BoxWhiskerLayer(CXYChart_addBoxWhiskerLayer(ptr, boxTop.data, boxTop.len,
		boxBottom.data, boxBottom.len, maxData.data, maxData.len, minData.data, minData.len, midData.data, midData.len,
		fillColor, whiskerColor, edgeColor)); reg(ret); return ret; }
	BoxWhiskerLayer *addBoxWhiskerLayer2(DoubleArray boxTop, DoubleArray boxBottom, DoubleArray maxData = DoubleArray(),
		DoubleArray minData = DoubleArray(), DoubleArray midData = DoubleArray(), IntArray fillColors = IntArray(), 
		double whiskerBrightness = 0.5, StringArray names = StringArray())
	{ BoxWhiskerLayer *ret = new BoxWhiskerLayer(CXYChart_addBoxWhiskerLayer2(ptr, boxTop.data, boxTop.len,
		boxBottom.data, boxBottom.len, maxData.data, maxData.len, minData.data, minData.len, midData.data, midData.len,
		fillColors.data, fillColors.len, whiskerBrightness, names.data, names.len)); reg(ret); return ret; }
	BoxWhiskerLayer *addBoxLayer(DoubleArray boxTop, DoubleArray boxBottom, int color = -1, const char *name = 0)
	{ BoxWhiskerLayer *ret = new BoxWhiskerLayer(CXYChart_addBoxLayer(ptr, boxTop.data, boxTop.len,
		boxBottom.data, boxBottom.len, color, name)); reg(ret); return ret; }

	ScatterLayer *addScatterLayer(DoubleArray xData, DoubleArray yData, const char *name = 0,
		int symbol = Chart::SquareSymbol, int symbolSize = 5, int fillColor = -1, int edgeColor = -1)
	{ ScatterLayer *ret = new ScatterLayer(CXYChart_addScatterLayer(ptr, xData.data, xData.len,
		yData.data, yData.len, name, symbol, symbolSize, fillColor, edgeColor)); reg(ret); return ret; }

	TrendLayer *addTrendLayer(DoubleArray data, int color = -1, const char *name = 0, int depth = 0)
	{ TrendLayer *ret = new TrendLayer(CXYChart_addTrendLayer(ptr, data.data, data.len, color, name, depth)); reg(ret); return ret; }
	TrendLayer *addTrendLayer(DoubleArray xData, DoubleArray yData, int color = -1, const char *name = 0, int depth = 0)
	{ TrendLayer *ret = new TrendLayer(CXYChart_addTrendLayer2(ptr, xData.data, xData.len,
		yData.data, yData.len, color, name, depth)); reg(ret); return ret; }

	SplineLayer *addSplineLayer(DoubleArray data = DoubleArray(), int color = -1, const char *name = 0)
	{ SplineLayer *ret = new SplineLayer(CXYChart_addSplineLayer(ptr, data.data, data.len, color, name)); reg(ret); return ret; }
	StepLineLayer *addStepLineLayer(DoubleArray data = DoubleArray(), int color = -1, const char *name = 0)
	{ StepLineLayer *ret = new StepLineLayer(CXYChart_addStepLineLayer(ptr, data.data, data.len, color, name)); reg(ret); return ret; }

	InterLineLayer *addInterLineLayer(LineObj *line1, LineObj *line2, int color12, int color21 = -1)
	{ InterLineLayer *ret = new InterLineLayer(CXYChart_addInterLineLayer(ptr, (LineObjInternal *)line1, (LineObjInternal *)line2, color12, color21));
	  reg(ret); return ret; }

	VectorLayer *addVectorLayer(DoubleArray xData, DoubleArray yData, DoubleArray lengths, DoubleArray directions, 
		int lengthScale = Chart::PixelScale, int color = -1, const char *name = 0)
	{ VectorLayer *ret = new VectorLayer(CXYChart_addVectorLayer(ptr, xData.data, xData.len, 
	  yData.data, yData.len, lengths.data, lengths.len, directions.data, directions.len, lengthScale, color, name)); 
	  reg(ret); return ret; }

	ContourLayer *addContourLayer(DoubleArray xData, DoubleArray yData, DoubleArray zData)
	{ ContourLayer *ret = new ContourLayer(CXYChart_addContourLayer(ptr, xData.data, xData.len, yData.data, yData.len, zData.data, zData.len));
	  reg(ret); return ret; }

	void layoutAxes() { CXYChart_layoutAxes(ptr); }
	void packPlotArea(int leftX, int topY, int rightX, int bottomY, int minWidth = 0, int minHeight = 0) 
	{ CXYChart_packPlotArea(ptr, leftX, topY, rightX, bottomY, minWidth, minHeight); }
};


class SurfaceChart : public BaseChart
{
private :
	//disable copying
	SurfaceChart(const SurfaceChart &rhs);
	SurfaceChart &operator=(const SurfaceChart &rhs);

	SurfaceChartInternal *ptr;

public :
	SurfaceChart(int width, int height, int bgColor = Chart::BackgroundColor,
		int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ ptr = CSurfaceChart_create(width, height, bgColor, edgeColor, raisedEffect);
	  init(SurfaceChart2BaseChart(ptr)); }

	void setPlotRegion(int cx, int cy, int xWidth, int yDepth, int zHeight)
	{ CSurfaceChart_setPlotRegion(ptr, cx, cy, xWidth, yDepth, zHeight); }
	void setViewAngle(double elevation, double rotation = 0, double twist = 0)
	{ CSurfaceChart_setViewAngle(ptr, elevation, rotation, twist); }
	void setPerspective(double perspective)
	{ CSurfaceChart_setPerspective(ptr, perspective); }
	
	void setData(DoubleArray xData, DoubleArray yData, DoubleArray zData)
	{ CSurfaceChart_setData(ptr, xData.data, xData.len, yData.data, yData.len, zData.data, zData.len); }
	void setInterpolation(int xSamples, int ySamples = -1, bool isSmooth = true)
	{ CSurfaceChart_setInterpolation(ptr, xSamples, ySamples, isSmooth); }
	
	void setLighting(double ambientIntensity, double diffuseIntensity, double specularIntensity, double shininess)
	{ CSurfaceChart_setLighting(ptr, ambientIntensity, diffuseIntensity, specularIntensity, shininess); }
	void setShadingMode(int shadingMode, int wireWidth = 1)
	{ CSurfaceChart_setShadingMode(ptr, shadingMode, wireWidth); }

	void setSurfaceAxisGrid(int majorXGridColor, int majorYGridColor = -1, int minorXGridColor = -1, int minorYGridColor = -1)
	{ CSurfaceChart_setSurfaceAxisGrid(ptr, majorXGridColor, majorYGridColor, minorXGridColor, minorYGridColor); }
	void setSurfaceDataGrid(int xGridColor, int yGridColor = -1)
	{ CSurfaceChart_setSurfaceDataGrid(ptr, xGridColor, yGridColor); }
	void setContourColor(int contourColor, int minorContourColor = -1)
	{ CSurfaceChart_setContourColor(ptr, contourColor, minorContourColor); }

	void setBackSideBrightness(double brightness) { CSurfaceChart_setBackSideBrightness(ptr, brightness); }
	void setBackSideColor(int color) { CSurfaceChart_setBackSideColor(ptr, color); }
	void setBackSideLighting(double ambientLight, double diffuseLight, double specularLight, double shininess)
	{ CSurfaceChart_setBackSideLighting(ptr, ambientLight, diffuseLight, specularLight, shininess); }

	Axis *xAxis() { Axis *ret = new Axis(CSurfaceChart_xAxis(ptr)); reg(ret); return ret; }
	Axis *yAxis() { Axis *ret = new Axis(CSurfaceChart_yAxis(ptr)); reg(ret); return ret; }
	Axis *zAxis() { Axis *ret = new Axis(CSurfaceChart_zAxis(ptr)); reg(ret); return ret; }
	void setZAxisPos(int pos) { CSurfaceChart_setZAxisPos(ptr, pos); }

	ColorAxis *setColorAxis(int x, int y, int alignment, int length, int orientation)
	{ ColorAxis *ret = new ColorAxis(CSurfaceChart_setColorAxis(ptr, x, y, alignment, length, orientation)); reg(ret); return ret; }
	ColorAxis *colorAxis()
	{ ColorAxis *ret = new ColorAxis(CSurfaceChart_colorAxis(ptr)); reg(ret); return ret; }
	
	void setWallVisibility(bool xyVisible, bool yzVisible, bool zxVisible)
	{ CSurfaceChart_setWallVisibility(ptr, xyVisible, yzVisible, zxVisible); }
	void setWallColor(int xyColor, int yzColor = -1, int zxColor = -1, int borderColor = -1)
	{ CSurfaceChart_setWallColor(ptr, xyColor, yzColor, zxColor, borderColor); }
	void setWallThickness(int xyThickness, int yzThickness = -1, int zxThickness = -1)
	{ CSurfaceChart_setWallThickness(ptr, xyThickness, yzThickness, zxThickness); }
	void setWallGrid(int majorXGridColor, int majorYGridColor = -1, int majorZGridColor = -1, 
		int minorXGridColor = -1, int minorYGridColor = -1, int minorZGridColor = -1)
	{ CSurfaceChart_setWallGrid(ptr, majorXGridColor, majorYGridColor, majorZGridColor, minorXGridColor, minorYGridColor, minorZGridColor); }
};


class PolarLayer : public AutoDestroy, protected GarbageContainer
{
private :
	//disable copying
	PolarLayer(const PolarLayer &rhs);
	PolarLayer &operator=(const PolarLayer &rhs);

	PolarLayerInternal *ptr;

public :
	PolarLayer(PolarLayerInternal *ptr) : ptr(ptr) {}
	~PolarLayer() {}

	void setData(DoubleArray data, int color = -1, const char *name = 0)
	{ CPolarLayer_setData(ptr, data.data, data.len, color, name); }
	void setAngles(DoubleArray angles)
	{ CPolarLayer_setAngles(ptr, angles.data, angles.len); }

	void setBorderColor(int edgeColor) { CPolarLayer_setBorderColor(ptr, edgeColor); }
	void setLineWidth(int w) { CPolarLayer_setLineWidth(ptr, w); }

	void setDataSymbol(const char *image) { CPolarLayer_setDataSymbol2(ptr, image); };
	void setDataSymbol(const DrawArea *obj) { CPolarLayer_setDataSymbol3(ptr, obj->getInternalPtr()); };
	void setDataSymbol(int symbol, int size = 7,
		int fillColor = -1, int edgeColor = -1, int lineWidth = 1)
	{ CPolarLayer_setDataSymbol(ptr, symbol, size, fillColor, edgeColor, lineWidth); }
	void setDataSymbol(IntArray polygon, int size = 11, int fillColor = -1, 
		int edgeColor = -1)
	{ CPolarLayer_setDataSymbol4(ptr, polygon.data, polygon.len, size, fillColor, edgeColor); }
	void setSymbolScale(DoubleArray zData, int scaleType = Chart::PixelScale)
	{ CPolarLayer_setSymbolScale(ptr, zData.data, zData.len, scaleType); }

	void setImageMapWidth(int width) { CPolarLayer_setImageMapWidth(ptr, width); }
	const char *getImageCoor(int dataItem, int offsetX = 0, int offsetY = 0)
	{ return CPolarLayer_getImageCoor(ptr, dataItem, offsetX, offsetY); }
	const char *getHTMLImageMap(const char *url, const char *queryFormat = 0, const char *extraAttr = 0,
		int offsetX = 0, int offsetY = 0)
	{ return CPolarLayer_getHTMLImageMap(ptr, url, queryFormat, extraAttr, offsetX, offsetY); }
	void setHTMLImageMap(const char *url, const char *queryFormat = 0, const char *extraAttr = 0)
	{ CPolarLayer_setHTMLImageMap(ptr, url, queryFormat, extraAttr); }

	void setDataLabelFormat(const char *formatString)
	{ CPolarLayer_setDataLabelFormat(ptr, formatString); }
	TextBox *setDataLabelStyle(const char *font = 0, double fontSize = 8,
		int fontColor = Chart::TextColor, double fontAngle = 0)
	{ TextBox *ret = new TextBox(CPolarLayer_setDataLabelStyle(ptr, font, fontSize, fontColor, fontAngle)); reg(ret); return ret;}
	TextBox *addCustomDataLabel(int i, const char *label, const char *font = 0,
		double fontSize = 8, int fontColor = Chart::TextColor, double fontAngle = 0)
	{ TextBox *ret = new TextBox(CPolarLayer_addCustomDataLabel(ptr, i, label, font, fontSize, fontColor, fontAngle)); reg(ret); return ret;}
};


class PolarAreaLayer : public PolarLayer
{
private :
	//disable copying
	PolarAreaLayer(const PolarAreaLayer &rhs);
	PolarAreaLayer &operator=(const PolarAreaLayer &rhs);

public :
	PolarAreaLayer(PolarAreaLayerInternal *ptr) : PolarLayer(PolarAreaLayer2PolarLayer(ptr)) {}
	~PolarAreaLayer() {}
};


class PolarLineLayer : public PolarLayer
{
private :
	//disable copying
	PolarLineLayer(const PolarLineLayer &rhs);
	PolarLineLayer &operator=(const PolarLineLayer &rhs);

	PolarLineLayerInternal *ptr;

public :
	PolarLineLayer(PolarLineLayerInternal *ptr) : PolarLayer(PolarLineLayer2PolarLayer(ptr)), ptr(ptr) {}
	~PolarLineLayer() {}

	void setCloseLoop(bool b)
	{ CPolarLineLayer_setCloseLoop(ptr, b); }
	void setGapColor(int lineColor, int lineWidth = -1)
	{ CPolarLineLayer_setGapColor(ptr, lineColor, lineWidth); }
};


class PolarSplineLineLayer : public PolarLineLayer
{
private :
	//disable copying
	PolarSplineLineLayer(const PolarSplineLineLayer &rhs);
	PolarSplineLineLayer &operator=(const PolarSplineLineLayer &rhs);

	PolarSplineLineLayerInternal *ptr;

public :
	PolarSplineLineLayer(PolarSplineLineLayerInternal *ptr) : PolarLineLayer(PolarSplineLineLayer2PolarLineLayer(ptr)), ptr(ptr) {}
	~PolarSplineLineLayer() {}

	void setTension(double tension) { CPolarSplineLineLayer_setTension(ptr, tension); }
};


class PolarSplineAreaLayer : public PolarAreaLayer
{
private :
	//disable copying
	PolarSplineAreaLayer(const PolarSplineAreaLayer &rhs);
	PolarSplineAreaLayer &operator=(const PolarSplineAreaLayer &rhs);

	PolarSplineAreaLayerInternal *ptr;

public :
	PolarSplineAreaLayer(PolarSplineAreaLayerInternal *ptr) : PolarAreaLayer(PolarSplineAreaLayer2PolarAreaLayer(ptr)), ptr(ptr) {}
	~PolarSplineAreaLayer() {}

	void setTension(double tension) { CPolarSplineAreaLayer_setTension(ptr, tension); }
};


class PolarVectorLayer : public PolarLayer
{
private :
	//disable copying
	PolarVectorLayer(const PolarVectorLayer &rhs);
	PolarVectorLayer &operator=(const PolarVectorLayer &rhs);

	PolarVectorLayerInternal *ptr;

public :
	PolarVectorLayer(PolarVectorLayerInternal *ptr) : PolarLayer(PolarVectorLayer2PolarLayer(ptr)), ptr(ptr) {}
	~PolarVectorLayer() {}

	void setVector(DoubleArray lengths, DoubleArray directions, int lengthScale = Chart::PixelScale)
	{ CPolarVectorLayer_setVector(ptr, lengths.data, lengths.len, directions.data, directions.len, lengthScale); }
	void setArrowHead(int width, int height = 0) { CPolarVectorLayer_setArrowHead(ptr, width, height); }
	void setArrowHead(IntArray polygon) { CPolarVectorLayer_setArrowHead2(ptr, polygon.data, polygon.len); }
	void setArrowStem(IntArray polygon) { CPolarVectorLayer_setArrowStem(ptr, polygon.data, polygon.len); }
	void setArrowAlignment(int alignment) { CPolarVectorLayer_setArrowAlignment(ptr, alignment); }
	void setIconSize(int height, int width = 0) { CPolarVectorLayer_setIconSize(ptr, height, width); }
	void setVectorMargin(double startMargin, double endMargin = Chart::NoValue)
	{ CPolarVectorLayer_setVectorMargin(ptr, startMargin, endMargin); }
};


class PolarChart : public BaseChart
{
private :
	//disable copying
	PolarChart(const PolarChart &rhs);
	PolarChart &operator=(const PolarChart &rhs);

	PolarChartInternal *ptr;

public :
	PolarChart(int width, int height, int bgColor = Chart::BackgroundColor,
		int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ ptr = CPolarChart_create(width, height, bgColor, edgeColor, raisedEffect);
	  init(PolarChart2BaseChart(ptr)); }
	static PolarChart *create(int width, int height, int bgColor = Chart::BackgroundColor,
		int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ return new PolarChart(width, height, bgColor, edgeColor, raisedEffect); }

	void setPlotArea(int x, int y, int r, int bgColor = Chart::Transparent,
		int edgeColor = Chart::Transparent, int edgeWidth = 1)
	{ CPolarChart_setPlotArea(ptr, x, y, r, bgColor, edgeColor, edgeWidth); }
	void setPlotAreaBg(int bgColor1, int bgColor2 = -1, bool altRings = true)
	{ CPolarChart_setPlotAreaBg(ptr, bgColor1, bgColor2, altRings); }
	void setGridColor(int rGridColor = 0x80000000, int rGridWidth = 1, int aGridColor = 0x80000000,
		int aGridWidth = 1)
	{ CPolarChart_setGridColor(ptr, rGridColor, rGridWidth, aGridColor, aGridWidth); }
	void setGridStyle(bool polygonGrid, bool gridOnTop = true)
	{ CPolarChart_setGridStyle(ptr, polygonGrid, gridOnTop); }
	void setStartAngle(double startAngle, bool clockwise = true)
	{ CPolarChart_setStartAngle(ptr, startAngle, clockwise); }

	AngularAxis *angularAxis() { AngularAxis *ret = new AngularAxis(CPolarChart_angularAxis(ptr)); reg(ret); return ret; }
	RadialAxis *radialAxis() { RadialAxis *ret = new RadialAxis(CPolarChart_radialAxis(ptr)); reg(ret); return ret; }

	int getXCoor(double r, double a) const { return CPolarChart_getXCoor(ptr, r, a); }
	int getYCoor(double r, double a) const { return CPolarChart_getYCoor(ptr, r, a); }

	PolarAreaLayer *addAreaLayer(DoubleArray data, int color = -1, const char *name = 0)
	{ PolarAreaLayer *ret = new PolarAreaLayer(CPolarChart_addAreaLayer(ptr, data.data, data.len, color, name)); reg(ret); return ret; }
	PolarLineLayer *addLineLayer(DoubleArray data, int color = -1, const char *name = 0)
	{ PolarLineLayer *ret = new PolarLineLayer(CPolarChart_addLineLayer(ptr, data.data, data.len, color, name)); reg(ret); return ret; }
	PolarSplineLineLayer *addSplineLineLayer(DoubleArray data, int color = -1, const char *name = 0)
	{ PolarSplineLineLayer *ret = new PolarSplineLineLayer(CPolarChart_addSplineLineLayer(ptr, data.data, data.len, color, name)); reg(ret); return ret; }
	PolarSplineAreaLayer *addSplineAreaLayer(DoubleArray data, int color = -1, const char *name = 0)
	{ PolarSplineAreaLayer *ret = new PolarSplineAreaLayer(CPolarChart_addSplineAreaLayer(ptr, data.data, data.len, color, name)); reg(ret); return ret; }
	PolarVectorLayer *addVectorLayer(DoubleArray rData, DoubleArray aData, DoubleArray lengths,
		DoubleArray directions, int lengthScale = Chart::PixelScale, int color = -1, const char *name = 0)
	{ PolarVectorLayer *ret = new PolarVectorLayer(CPolarChart_addVectorLayer(ptr, rData.data, rData.len, 
	  aData.data, aData.len, lengths.data, lengths.len, directions.data, directions.len, lengthScale, color, name)); 
	  reg(ret); return ret; }
};


class PyramidLayer : public AutoDestroy, protected GarbageContainer
{
private :
	//disable copying
	PyramidLayer(const PyramidLayer &rhs);
	PyramidLayer &operator=(const PyramidLayer &rhs);

	PyramidLayerInternal *ptr;

public :
	PyramidLayer(PyramidLayerInternal *ptr) : ptr(ptr) {}
	~PyramidLayer() {}

	TextBox *setCenterLabel(const char *labelTemplate = "{skip}", const char *font = "{skip}", double fontSize = -1, int fontColor = -1)
	{ TextBox *ret = new TextBox(CPyramidLayer_setCenterLabel(ptr, labelTemplate, font, fontSize, fontColor)); reg(ret); return ret; }
	TextBox *setRightLabel(const char *labelTemplate = "{skip}", const char *font = "{skip}", double fontSize = -1, int fontColor = -1)
	{ TextBox *ret = new TextBox(CPyramidLayer_setRightLabel(ptr, labelTemplate, font, fontSize, fontColor)); reg(ret); return ret; }
	TextBox *setLeftLabel(const char *labelTemplate = "{skip}", const char *font = "{skip}", double fontSize = -1, int fontColor = -1)
	{ TextBox *ret = new TextBox(CPyramidLayer_setLeftLabel(ptr, labelTemplate, font, fontSize, fontColor)); reg(ret); return ret; }

	void setColor(int color)
	{ CPyramidLayer_setColor(ptr, color); }
	void setJoinLine(int color , int width = -1)
	{ CPyramidLayer_setJoinLine(ptr, color, width); }
	void setJoinLineGap(int pyramidGap, int pyramidMargin = -0x7fffffff, int textGap = -0x7fffffff)
	{ CPyramidLayer_setJoinLineGap(ptr, pyramidGap, pyramidMargin, textGap); }
	void setLayerBorder(int color, int width = -1)
	{ CPyramidLayer_setLayerBorder(ptr, color, width); }
	void setLayerGap(double layerGap)
	{ CPyramidLayer_setLayerGap(ptr, layerGap); }
};


class PyramidChart : public BaseChart
{
private :
	//disable copying
	PyramidChart(const PyramidChart &rhs);
	PyramidChart &operator=(const PyramidChart &rhs);

	PyramidChartInternal *ptr;

public :
	PyramidChart(int width, int height, int bgColor = Chart::BackgroundColor,
		int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ ptr = CPyramidChart_create(width, height, bgColor, edgeColor, raisedEffect);
	  init(PyramidChart2BaseChart(ptr)); }

	void setPyramidSize(int cx, int cy, int radius, int height)
	{ CPyramidChart_setPyramidSize(ptr, cx, cy, radius, height); }
	void setConeSize(int cx, int cy, int radius, int height)
	{ CPyramidChart_setConeSize(ptr, cx, cy, radius, height); }
	void setFunnelSize(int cx, int cy, int radius, int height, double tubeRadius = 0.2, double tubeHeight = 0.3)
	{ CPyramidChart_setFunnelSize(ptr, cx, cy, radius, height, tubeRadius, tubeHeight); }
	void setData(DoubleArray data, StringArray labels = StringArray())
	{ CPyramidChart_setData(ptr, data.data, data.len, labels.data, labels.len); }
	TextBox *setCenterLabel(const char *labelTemplate = "{skip}", const char *font = "{skip}", double fontSize = -1, int fontColor = -1)
	{ TextBox *ret = new TextBox(CPyramidChart_setCenterLabel(ptr, labelTemplate, font, fontSize, fontColor)); reg(ret); return ret; }
	TextBox *setRightLabel(const char *labelTemplate = "{skip}", const char *font = "{skip}", double fontSize = -1, int fontColor = -1)
	{ TextBox *ret = new TextBox(CPyramidChart_setRightLabel(ptr, labelTemplate, font, fontSize, fontColor)); reg(ret); return ret; }
	TextBox *setLeftLabel(const char *labelTemplate = "{skip}", const char *font = "{skip}", double fontSize = -1, int fontColor = -1)
	{ TextBox *ret = new TextBox(CPyramidChart_setLeftLabel(ptr, labelTemplate, font, fontSize, fontColor)); reg(ret); return ret; }

	void setPyramidSides(int noOfSides)
	{ CPyramidChart_setPyramidSides(ptr, noOfSides); }
	void setViewAngle(double elevation, double rotation = 0, double twist = 0)
	{ CPyramidChart_setViewAngle(ptr, elevation, rotation, twist); }

	void setGradientShading(double startBrightness, double endBrightness)
	{ CPyramidChart_setGradientShading(ptr, startBrightness, endBrightness); }
	void setLighting(double ambientIntensity = 0.5, double diffuseIntensity = 0.5, double specularIntensity = 1, double shininess = 8)
	{ CPyramidChart_setLighting(ptr, ambientIntensity, diffuseIntensity, specularIntensity, shininess); }

	void setJoinLine(int color, int width = -1)
	{ CPyramidChart_setJoinLine(ptr, color, width); }
	void setJoinLineGap(int pyramidGap, int pyramidMargin = -0x7fffffff, int textGap = -0x7fffffff)
	{ CPyramidChart_setJoinLineGap(ptr, pyramidGap, pyramidMargin, textGap); }
	void setLayerBorder(int color, int width = -1)
	{ CPyramidChart_setLayerBorder(ptr, color, width); }
	void setLayerGap(double layerGap)
	{ CPyramidChart_setLayerGap(ptr, layerGap); }

	PyramidLayer *getLayer(int layerNo)
	{ PyramidLayerInternal *p = CPyramidChart_getLayer(ptr, layerNo); if (!p) return 0; 
	  PyramidLayer *ret = new PyramidLayer(p); reg(ret); return ret; }
};


class MeterPointer : public DrawObj
{
	//disable copying
	MeterPointer(const MeterPointer &rhs);
	MeterPointer &operator=(const MeterPointer &rhs);

	MeterPointerInternal *ptr;

public :
	MeterPointer(MeterPointerInternal *ptr) : DrawObj(MeterPointer2DrawObj(ptr)), ptr(ptr) {}
	void setColor(int fillColor, int edgeColor = -1)
	{ CMeterPointer_setColor(ptr, fillColor, edgeColor); }
	void setPos(double value) { CMeterPointer_setPos(ptr, value); }
	void setShape(int pointerType, double lengthRatio = Chart::NoValue, double widthRatio = Chart::NoValue)
	{ CMeterPointer_setShape(ptr, pointerType, lengthRatio, widthRatio); }
	void setShape(IntArray pointerCoor, double lengthRatio = Chart::NoValue, double widthRatio = Chart::NoValue)
	{ CMeterPointer_setShape2(ptr, pointerCoor.data, pointerCoor.len, lengthRatio, widthRatio); }
};


class BaseMeter : public BaseChart
{
private :
	//disable copying
	BaseMeter(const BaseMeter &rhs);
	BaseMeter &operator=(const BaseMeter &rhs);

	BaseMeterInternal *ptr;

public :
	BaseMeter() : ptr(0) {}
	void init(BaseMeterInternal *ptr) { this->ptr = ptr; BaseChart::init(BaseMeter2BaseChart(ptr)); }

	MeterPointer *addPointer(double value, int fillColor = Chart::LineColor, int edgeColor = -1)
	{ MeterPointer *ret = new MeterPointer(CBaseMeter_addPointer(ptr, value, fillColor, edgeColor)); reg(ret); return ret; }

	void setScale(double lowerLimit, double upperLimit, double majorTickInc = 0, double minorTickInc = 0, double microTickInc = 0) 
	{ CBaseMeter_setScale(ptr, lowerLimit, upperLimit, majorTickInc, minorTickInc, microTickInc); }
	void setScale(double lowerLimit, double upperLimit, StringArray labels)
	{ CBaseMeter_setScale2(ptr, lowerLimit, upperLimit, labels.data, labels.len); }
	void setScale(double lowerLimit, double upperLimit, DoubleArray labels, const char *formatString = 0)
	{ CBaseMeter_setScale3(ptr, lowerLimit, upperLimit, labels.data, labels.len, formatString); }

	void addLabel(double pos, const char *label) { CBaseMeter_addLabel(ptr, pos, label); }
	const char *getLabel(double i) const { return CBaseMeter_getLabel(ptr, i); }
	DoubleArray getTicks() const
	{ const double *d; int len; CBaseMeter_getTicks(ptr, &d, &len); return DoubleArray(d, len); }

	TextBox *setLabelStyle(const char *font = "bold", double fontSize = -1, int fontColor = Chart::TextColor, double fontAngle = 0)
	{ TextBox *ret = new TextBox(CBaseMeter_setLabelStyle(ptr, font, fontSize, fontColor, fontAngle)); reg(ret); return ret; }
	void setLabelPos(bool labelInside, int labelOffset = 0) { CBaseMeter_setLabelPos(ptr, labelInside, labelOffset); }
	void setLabelFormat(const char *mainLabelFormat) { CBaseMeter_setLabelFormat(ptr, mainLabelFormat); }
	void setTickLength(int majorLen, int minorLen = -0x7fffffff, int microLen = -0x7fffffff)
	{ CBaseMeter_setTickLength(ptr, majorLen, minorLen, microLen); }
	void setLineWidth(int axisWidth, int majorTickWidth = 1, int minorTickWidth = 1, int microTickWidth = 1)
	{ CBaseMeter_setLineWidth(ptr, axisWidth, majorTickWidth, minorTickWidth, microTickWidth); }
	void setMeterColors(int axisColor, int labelColor = -1, int tickColor = -1)
	{ CBaseMeter_setMeterColors(ptr, axisColor, labelColor, tickColor); }

	int getCoor(double v) const { return CBaseMeter_getCoor(ptr, v); }
};


class AngularMeter : public BaseMeter
{
private :
	//disable copying
	AngularMeter(const AngularMeter &rhs);
	AngularMeter &operator=(const AngularMeter &rhs);

	AngularMeterInternal *ptr;

public :
	AngularMeter(int width, int height, int bgColor = Chart::BackgroundColor,
		int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ ptr = CAngularMeter_create(width, height, bgColor, edgeColor, raisedEffect);
	  init(AngularMeter2BaseMeter(ptr)); }

	void addRing(int startRadius, int endRadius, int fillColor, int edgeColor = -1)
	{ CAngularMeter_addRing(ptr, startRadius, endRadius, fillColor, edgeColor); }
	void addRingSector(int startRadius, int endRadius, double a1, double a2, int fillColor, int edgeColor = -1)
	{ CAngularMeter_addRingSector(ptr, startRadius, endRadius, a1, a2, fillColor, edgeColor); }
	void setCap(int radius, int fillColor, int edgeColor = Chart::LineColor)
	{ CAngularMeter_setCap(ptr, radius, fillColor, edgeColor); }

	void setMeter(int cx, int cy, int radius, double startAngle, double endAngle)
	{ CAngularMeter_setMeter(ptr, cx, cy, radius, startAngle, endAngle); }

	void addZone(double startValue, double endValue, int startRadius, int endRadius, int fillColor, int edgeColor = -1)
	{ CAngularMeter_addZone(ptr, startValue, endValue, startRadius, endRadius, fillColor, edgeColor); }
	void addZone(double startValue, double endValue, int fillColor, int edgeColor = -1)
	{ CAngularMeter_addZone2(ptr, startValue, endValue, fillColor, edgeColor); }
};


class LinearMeter : public BaseMeter
{
private :
	//disable copying
	LinearMeter(const LinearMeter &rhs);
	LinearMeter &operator=(const LinearMeter &rhs);

	LinearMeterInternal *ptr;

public :
	LinearMeter(int width, int height, int bgColor = Chart::BackgroundColor,
		int edgeColor = Chart::Transparent, int raisedEffect = 0)
	{ ptr = CLinearMeter_create(width, height, bgColor, edgeColor, raisedEffect);
	  init(LinearMeter2BaseMeter(ptr)); }

	void setMeter(int leftX, int topY, int width, int height, int axisPos = Chart::Left, bool isReversed = false)
	{ CLinearMeter_setMeter(ptr, leftX, topY, width, height, axisPos, isReversed); }
	void setRail(int railColor, int railWidth = 2, int railOffset = 6)
	{ CLinearMeter_setRail(ptr, railColor, railWidth, railOffset); }
		
	TextBox *addZone(double startValue, double endValue, int color, const char *label = 0)
	{ TextBox *ret = new TextBox(CLinearMeter_addZone(ptr, startValue, endValue, color, label)); reg(ret); return ret; }
};


class ArrayMath
{
private :
	ArrayMathInternal *ptr;

public :
	ArrayMath(DoubleArray a) : ptr(CArrayMath_create(a.data, a.len)) {}
	~ArrayMath() { CArrayMath_destroy(ptr); }
	static ArrayMath *create(DoubleArray a) { return new ArrayMath(a); }
	void destroy() { delete this; }

	ArrayMath(const ArrayMath &rhs) { DoubleArray r  = rhs; ptr = CArrayMath_create(r.data, r.len); }
	ArrayMath &operator=(const ArrayMath &rhs) { CArrayMath_destroy(ptr); DoubleArray r  = rhs; ptr = CArrayMath_create(r.data, r.len); return *this;}
	operator DoubleArray() const { return result(); }

	ArrayMath& add(DoubleArray b) { CArrayMath_add(ptr, b.data, b.len); return *this; }
	ArrayMath& add(double b) { CArrayMath_add2(ptr, b); return *this; }
	ArrayMath& sub(DoubleArray b) { CArrayMath_sub(ptr, b.data, b.len); return *this; }
	ArrayMath& sub(double b) { CArrayMath_sub2(ptr, b); return *this; }
	ArrayMath& mul(DoubleArray b) { CArrayMath_mul(ptr, b.data, b.len); return *this; }
	ArrayMath& mul(double b) { CArrayMath_mul2(ptr, b); return *this; }
	ArrayMath& div(DoubleArray b) { CArrayMath_div(ptr, b.data, b.len); return *this; }
	ArrayMath& div(double b) { CArrayMath_div2(ptr, b); return *this; }
	ArrayMath& financeDiv(DoubleArray b, double zeroByZeroValue) { CArrayMath_financeDiv(ptr, b.data, b.len, zeroByZeroValue); return *this; }
	ArrayMath& shift(int offset = 1, double fillValue = Chart::NoValue) { CArrayMath_shift(ptr, offset, fillValue); return *this; }
	ArrayMath& delta(int offset = 1) { CArrayMath_delta(ptr, offset); return *this; }
	ArrayMath& rate(int offset = 1) { CArrayMath_rate(ptr, offset); return *this; }
	ArrayMath& abs() { CArrayMath_abs(ptr); return *this; }
	ArrayMath& acc() { CArrayMath_acc(ptr); return *this; }

	ArrayMath& selectGTZ(DoubleArray decisionArray = DoubleArray(), double fillValue = 0)
	{ CArrayMath_selectGTZ(ptr, decisionArray.data, decisionArray.len, fillValue); return *this; }
	ArrayMath& selectGEZ(DoubleArray decisionArray = DoubleArray(), double fillValue = 0)
	{ CArrayMath_selectGEZ(ptr, decisionArray.data, decisionArray.len, fillValue); return *this; }
	ArrayMath& selectLTZ(DoubleArray decisionArray = DoubleArray(), double fillValue = 0)
	{ CArrayMath_selectLTZ(ptr, decisionArray.data, decisionArray.len, fillValue); return *this; }
	ArrayMath& selectLEZ(DoubleArray decisionArray = DoubleArray(), double fillValue = 0)
	{ CArrayMath_selectLEZ(ptr, decisionArray.data, decisionArray.len, fillValue); return *this; }
	ArrayMath& selectEQZ(DoubleArray decisionArray = DoubleArray(), double fillValue = 0)
	{ CArrayMath_selectEQZ(ptr, decisionArray.data, decisionArray.len, fillValue); return *this; }
	ArrayMath& selectNEZ(DoubleArray decisionArray = DoubleArray(), double fillValue = 0)
	{ CArrayMath_selectNEZ(ptr, decisionArray.data, decisionArray.len, fillValue); return *this; }

	ArrayMath& selectStartOfHour(int majorTickStep = 1, double initialMargin = 300)
	{ CArrayMath_selectStartOfHour(ptr, majorTickStep, initialMargin); return *this; }
	ArrayMath& selectStartOfDay(int majorTickStep = 1, double initialMargin = 3 * 3600)
	{ CArrayMath_selectStartOfDay(ptr, majorTickStep, initialMargin); return *this; }
	ArrayMath& selectStartOfWeek(int majorTickStep = 1, double initialMargin = 2 * 86400)
	{ CArrayMath_selectStartOfWeek(ptr, majorTickStep, initialMargin); return *this; }
	ArrayMath& selectStartOfMonth(int majorTickStep = 1, double initialMargin = 5 * 86400)
	{ CArrayMath_selectStartOfMonth(ptr, majorTickStep, initialMargin); return *this; }
	ArrayMath& selectStartOfYear(int majorTickStep = 1, double initialMargin = 60 * 86400)
	{ CArrayMath_selectStartOfYear(ptr, majorTickStep, initialMargin); return *this; }
	ArrayMath& selectRegularSpacing(int majorTickStep, int minorTickStep = 0, int initialMargin = 0)
	{ CArrayMath_selectRegularSpacing(ptr, majorTickStep, minorTickStep, initialMargin); return *this; }

	ArrayMath& trim(int startIndex = 0, int len = -1)
	{ CArrayMath_trim(ptr, startIndex, len); return *this; }
	ArrayMath& insert(DoubleArray a, int insertPoint = -1)
	{ CArrayMath_insert(ptr, a.data, a.len, insertPoint); return *this; }
	ArrayMath& insert(double c, int len, int insertPoint = -1)
	{ CArrayMath_insert2(ptr, c, len, insertPoint); return *this; }
	ArrayMath& replace(double a, double b)
	{ CArrayMath_replace(ptr, a, b); return *this; }

	ArrayMath& movAvg(int interval)
	{ CArrayMath_movAvg(ptr, interval); return *this; }
	ArrayMath& expAvg(double smoothingFactor)
	{ CArrayMath_expAvg(ptr, smoothingFactor); return *this; }
	ArrayMath& movMed(int interval)
	{ CArrayMath_movMed(ptr, interval); return *this; }
	ArrayMath& movPercentile(int interval, double percentile)
	{ CArrayMath_movPercentile(ptr, interval, percentile); return *this; }
	ArrayMath& movMax(int interval)
	{ CArrayMath_movMax(ptr, interval); return *this; }
	ArrayMath& movMin(int interval)
	{ CArrayMath_movMin(ptr, interval); return *this; }
	ArrayMath& movStdDev(int interval)
	{ CArrayMath_movStdDev(ptr, interval); return *this; }
	ArrayMath& movCorr(int interval, DoubleArray b = DoubleArray())
	{ CArrayMath_movCorr(ptr, interval, b.data, b.len); return *this; }
	ArrayMath& lowess(double smoothness = 0.25, int iteration = 0)
	{ CArrayMath_lowess(ptr, smoothness, iteration); return *this; }
	ArrayMath& lowess(DoubleArray b, double smoothness = 0.25, int iteration = 0)
	{ CArrayMath_lowess2(ptr, b.data, b.len, smoothness, iteration); return *this; }

	DoubleArray result() const { const double *d; int len; CArrayMath_result(ptr, &d, &len); return DoubleArray(d, len); }
#ifndef max
	double max() const { return CArrayMath_max(ptr); }
#endif
	double maxValue() const { return CArrayMath_max(ptr); }
#ifndef min
	double min() const { return CArrayMath_min(ptr); }
#endif
	double minValue() const { return CArrayMath_min(ptr); }
	double avg() const { return CArrayMath_avg(ptr); }
	double sum() const { return CArrayMath_sum(ptr); }
	double stdDev() const { return CArrayMath_stdDev(ptr); }
	double med() const { return CArrayMath_med(ptr); }
	double percentile(double p) const { return CArrayMath_percentile(ptr, p); }
	int maxIndex() const { return CArrayMath_maxIndex(ptr); }
	int minIndex() const { return CArrayMath_minIndex(ptr); }

	DoubleArray aggregate(DoubleArray srcArray, int aggregateMethod, double param = 50) const
	{ const double *d; int len; CArrayMath_aggregate(ptr, srcArray.data, srcArray.len, aggregateMethod, param,
	  &d, &len); return DoubleArray(d, len); }
};


class RanTable
{
private :
	//disable copying
	RanTable(const RanTable &rhs);
	RanTable &operator=(const RanTable &rhs);

	RanTableInternal *ptr;

public :
	RanTable(int seed, int noOfCols, int noOfRows) : ptr(CRanTable_create(seed, noOfCols, noOfRows)) {}
	~RanTable() { CRanTable_destroy(ptr); }
	static RanTable *create(int seed, int noOfCols, int noOfRows) { return new RanTable(seed, noOfCols, noOfRows); }
	void destroy() { delete this; }

	void setCol(int colNo, double minValue, double maxValue)
	{ CRanTable_setCol(ptr, colNo, minValue, maxValue); }
	void setCol(int colNo, double startValue, double minDelta, double maxDelta,
		double lowerLimit = -1E+308, double upperLimit = 1E+308)
	{ CRanTable_setCol2(ptr, colNo, startValue, minDelta, maxDelta, lowerLimit, upperLimit); }
	void setDateCol(int i, double startTime, double tickInc, bool weekDayOnly = false)
	{ CRanTable_setDateCol(ptr, i, startTime, tickInc, weekDayOnly); }
	void setHLOCCols(int i, double startValue, double minDelta, double maxDelta,
		double lowerLimit = 0, double upperLimit = 1E+308)
	{ CRanTable_setHLOCCols(ptr, i, startValue, minDelta, maxDelta, lowerLimit, upperLimit); }
	int selectDate(int colNo, double minDate, double maxDate)
	{ return CRanTable_selectDate(ptr, colNo, minDate, maxDate); } 
	DoubleArray getCol(int i)
	{ const double *d; int len; CRanTable_getCol(ptr, i, &d, &len); return DoubleArray(d, len); }
};

class FinanceSimulator
{
private :
	//disable copying
	FinanceSimulator(const FinanceSimulator &rhs);
	FinanceSimulator &operator=(const FinanceSimulator &rhs);

	FinanceSimulatorInternal *ptr;

public :
	FinanceSimulator(int seed, double startTime, double endTime, int resolution) :
	   ptr(CFinanceSimulator_create(seed, startTime, endTime, resolution))
	{}
	FinanceSimulator(const char *seed, double startTime, double endTime, int resolution) :
	   ptr(CFinanceSimulator_create2(seed, startTime, endTime, resolution))
	{}
	~FinanceSimulator() { CFinanceSimulator_destroy(ptr); }

	DoubleArray getTimeStamps() 
	{ const double *d; int len; CFinanceSimulator_getTimeStamps(ptr, &d, &len); return DoubleArray(d, len); }
	DoubleArray getHighData()
	{ const double *d; int len; CFinanceSimulator_getHighData(ptr, &d, &len); return DoubleArray(d, len); }
	DoubleArray getLowData()
	{ const double *d; int len; CFinanceSimulator_getLowData(ptr, &d, &len); return DoubleArray(d, len); }
	DoubleArray getOpenData()
	{ const double *d; int len; CFinanceSimulator_getOpenData(ptr, &d, &len); return DoubleArray(d, len); }
	DoubleArray getCloseData()
	{ const double *d; int len; CFinanceSimulator_getCloseData(ptr, &d, &len); return DoubleArray(d, len); }
	DoubleArray getVolData()
	{ const double *d; int len; CFinanceSimulator_getVolData(ptr, &d, &len); return DoubleArray(d, len); }
};

class ImageMapHandler
{
private :
	//disable copying
	ImageMapHandler(const ImageMapHandler &rhs);
	ImageMapHandler &operator=(const ImageMapHandler &rhs);

	ImageMapHandlerInternal *ptr;

public :
	ImageMapHandler(const char *imageMap) : ptr(CImageMapHandler_create(imageMap)) {}
	~ImageMapHandler() { CImageMapHandler_destroy(ptr); }

	int getHotSpot(int x, int y)
	{ return CImageMapHandler_getHotSpot(ptr, x, y); }
	const char *getValue(const char *key)
	{ return CImageMapHandler_getValue(ptr, key); }
	const char *getKey(int i)
	{ return CImageMapHandler_getKey(ptr, i); }
	const char *getValue(int i)
	{ return CImageMapHandler_getValue2(ptr, i); }
};

class ViewPortManager
{
private :
	//disable copying
	ViewPortManager(const ViewPortManager &rhs);
	ViewPortManager &operator=(const ViewPortManager &rhs);

	ViewPortManagerInternal *ptr;

public :

	ViewPortManager() : ptr(CViewPortManager_create()) {}
	~ViewPortManager() { CViewPortManager_destroy(ptr); }

	void setChartMetrics(const char *metrics) { CViewPortManager_setChartMetrics(ptr, metrics); }
	int getPlotAreaLeft() { return CViewPortManager_getPlotAreaLeft(ptr); }
	int getPlotAreaTop() { return CViewPortManager_getPlotAreaTop(ptr); }
	int getPlotAreaWidth() { return CViewPortManager_getPlotAreaWidth(ptr); }
	int getPlotAreaHeight() { return CViewPortManager_getPlotAreaHeight(ptr); }
	bool inPlotArea(int x, int y) { return CViewPortManager_inPlotArea(ptr, x, y); }

	double getViewPortLeft() { return CViewPortManager_getViewPortLeft(ptr); }
	void setViewPortLeft(double left) { CViewPortManager_setViewPortLeft(ptr, left); }
	double getViewPortTop() { return CViewPortManager_getViewPortTop(ptr); }
	void setViewPortTop(double top) { CViewPortManager_setViewPortTop(ptr, top); }
	double getViewPortWidth() { return CViewPortManager_getViewPortWidth(ptr); }
	void setViewPortWidth(double width) { CViewPortManager_setViewPortWidth(ptr, width); }
	double getViewPortHeight() { return CViewPortManager_getViewPortHeight(ptr); }
	void setViewPortHeight(double height) { CViewPortManager_setViewPortHeight(ptr, height); }
	void validateViewPort() { CViewPortManager_validateViewPort(ptr); }

	double getZoomInWidthLimit() { return CViewPortManager_getZoomInWidthLimit(ptr); }
	void setZoomInWidthLimit(double viewPortWidth) { CViewPortManager_setZoomInWidthLimit(ptr, viewPortWidth); }
	double getZoomOutWidthLimit() { return CViewPortManager_getZoomOutWidthLimit(ptr); }
	void setZoomOutWidthLimit(double viewPortWidth){  CViewPortManager_setZoomOutWidthLimit(ptr, viewPortWidth); }
	double getZoomInHeightLimit() { return CViewPortManager_getZoomInHeightLimit(ptr); }
	void setZoomInHeightLimit(double viewPortHeight) { CViewPortManager_setZoomInHeightLimit(ptr, viewPortHeight); }
	double getZoomOutHeightLimit() { return CViewPortManager_getZoomOutHeightLimit(ptr); }
	void setZoomOutHeightLimit(double viewPortHeight){  CViewPortManager_setZoomOutHeightLimit(ptr, viewPortHeight); }
	bool canZoomIn(int zoomDirection) { return CViewPortManager_canZoomIn(ptr, zoomDirection); }
	bool canZoomOut(int zoomDirection) { return CViewPortManager_canZoomOut(ptr, zoomDirection); }
	bool zoomAt(int zoomDirection, int x, int y, double zoomRatio) 
	{ return CViewPortManager_zoomAt(ptr, zoomDirection, x, y, zoomRatio); }
	bool zoomTo(int zoomDirection, int x1, int y1, int x2, int y2) 
	{ return CViewPortManager_zoomTo(ptr, zoomDirection, x1, y1, x2, y2); }

	void startDrag() { CViewPortManager_startDrag(ptr); }
	bool dragTo(int scrollDirection, int deltaX, int deltaY) 
	{ return CViewPortManager_dragTo(ptr, scrollDirection, deltaX, deltaY); }
};


#ifndef CHARTDIR_HIDE_OBSOLETE

//////////////////////////////////////////////////////////////////////////////////////
//	This section is obsoleted - retained for compatibility only.
//	Global identifiers are now moved into the Chart namespace
//////////////////////////////////////////////////////////////////////////////////////

static const double LogTick = +1.6e308;
static const double NoValue = +1.7e308;
static const double TouchBar = -1.7E-100;

enum Alignment
{
	TopLeft = 7,	TopCenter = 8,		TopRight = 9,
	Left = 4,		Center = 5,			Right = 6,
	BottomLeft = 1,	BottomCenter = 2,	BottomRight = 3,
	Top = TopCenter,
	Bottom = BottomCenter
};

enum
{
	DashLine = 0x0505,
	DotLine = 0x0202,
	DotDashLine = 0x05050205,
	AltDashLine = 0x0A050505
};

static const int *goldGradient = Chart::goldGradient;
static const int *silverGradient = Chart::silverGradient;
static const int *redMetalGradient = Chart::redMetalGradient;
static const int *blueMetalGradient = Chart::blueMetalGradient;
static const int *greenMetalGradient = Chart::greenMetalGradient;

enum AntiAliasMode { NoAntiAlias, AntiAlias, AutoAntiAlias };
enum PaletteMode { TryPalette, ForcePalette, NoPalette };
enum DitherMethod { Quantize, OrderedDither, ErrorDiffusion };

enum CDFilterType { BoxFilter, LinearFilter, QuadraticFilter, BSplineFilter, HermiteFilter,
	CatromFilter, MitchellFilter, SincFilter, LanczosFilter, GaussianFilter, HanningFilter,
	HammingFilter, BlackmanFilter, BesselFilter };

enum
{
	Transparent = 0xff000000,
	Palette = 0xffff0000,
	BackgroundColor = 0xffff0000,
	LineColor = 0xffff0001,
	TextColor = 0xffff0002,
	DataColor = 0xffff0008,
	SameAsMainColor = 0xffff0007
};

enum ImgFormat {PNG, GIF, JPG, WMP, BMP};

static const int *oldDefaultPalette = Chart::oldDefaultPalette;
static const int *defaultPalette = Chart::defaultPalette;
static const int *whiteOnBlackPalette = Chart::whiteOnBlackPalette;
static const int *transparentPalette = Chart::transparentPalette;

enum {SideLayout, CircleLayout};

enum SymbolType
{
	NoSymbol = 0,
	SquareSymbol = 1,
	DiamondSymbol = 2,
	TriangleSymbol = 3,
	RightTriangleSymbol = 4,
	LeftTriangleSymbol = 5,
	InvertedTriangleSymbol = 6,
	CircleSymbol = 7,
	CrossSymbol = 8,
	Cross2Symbol = 9
};

enum DataCombineMethod { Overlay, Stack, Depth, Side, Percentage };

enum LegendMode { NormalLegend, ReverseLegend, NoLegend };

enum ScaleType { PixelScale, XAxisScale, YAxisScale,
	AngularAxisScale = XAxisScale, RadialAxisScale = YAxisScale };

static inline int getVersion() { return Chart::getVersion(); }
static inline const char * getDescription() { return Chart::getDescription(); }
static inline const char * getCopyright() { return Chart::getCopyright(); }

static inline bool testFont(const char *font, int fontIndex, double fontHeight,
		double fontWidth, double angle, char *buffer)
{ return Chart::testFont(font, fontIndex, fontHeight, fontWidth, angle, buffer); }

static inline bool isLicensed() { return Chart::isLicensed(); }
static inline bool getLicenseAttr(const char *key, char *buffer) 
{ return Chart::getLicenseAttr(key, buffer); }
static inline bool setLicenseFile(const char *filename = 0, char *buffer = 0)
{ return Chart::setLicenseFile(filename, buffer); }
static inline bool setLicenseCode(const char *licCode, char *buffer = 0)
{ return Chart::setLicenseCode(licCode, buffer); }

static inline double chartTime(int y, int m, int d, int h = 0, int n = 0, int s = 0)
{ return Chart::chartTime(y, m, d, h, n, s); }
static inline double chartTime2(int t)
{ return Chart::chartTime2(t); }

//#ifndef CHARTDIR_HIDE_OBSOLETE
#endif

//#ifndef CCHARTDIR_HDR
#endif
