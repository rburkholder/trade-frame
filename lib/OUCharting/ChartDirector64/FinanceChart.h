#ifndef FINANCECHART_HDR
#define FINANCECHART_HDR

#include "chartdir.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2012 Advanced Software Engineering Limited
//
// ChartDirector FinanceChart class library
//     - Requires ChartDirector Ver 5.1 or above
//
// You may use and modify the code in this file in your application, provided the code and
// its modifications are used only in conjunction with ChartDirector. Usage of this software
// is subjected to the terms and condition of the ChartDirector license.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

/// <summary>
/// Represents a Financial Chart
/// </summary>
class FinanceChart : public MultiChart
{
    int m_totalWidth;
    int m_totalHeight;
    bool m_antiAlias;
    bool m_logScale;
    bool m_axisOnRight;

    int m_leftMargin;
    int m_rightMargin;
    int m_topMargin;
    int m_bottomMargin;

    int m_plotAreaBgColor;
    int m_plotAreaBorder;
    int m_plotAreaGap;

    int m_majorHGridColor;
    int m_minorHGridColor;
    int m_majorVGridColor;
    int m_minorVGridColor;

    std::string m_legendFont;
    double m_legendFontSize;
    int m_legendFontColor;
    int m_legendBgColor;

    std::string m_yAxisFont;
    double m_yAxisFontSize;
    int m_yAxisFontColor;
    int m_yAxisMargin;
    
    std::string m_xAxisFont;
    double m_xAxisFontSize;
    int m_xAxisFontColor;
    double m_xAxisFontAngle;
    
    DoubleArray m_timeStamps;
    DoubleArray m_highData;
    DoubleArray m_lowData;
    DoubleArray m_openData;
    DoubleArray m_closeData;
    DoubleArray m_volData;
    std::string m_volUnit;
    int m_extraPoints;
    
    std::string m_yearFormat;
    std::string m_firstMonthFormat;
    std::string m_otherMonthFormat;
    std::string m_firstDayFormat;
    std::string m_otherDayFormat;
    std::string m_firstHourFormat;
    std::string m_otherHourFormat;
    int m_timeLabelSpacing;

    std::string m_generalFormat;
    
    std::string m_toolTipMonthFormat;
    std::string m_toolTipDayFormat;
    std::string m_toolTipHourFormat;
    
    XYChart *m_mainChart;
    XYChart *m_currentChart;
    std::vector<XYChart *> garbage;

    private: void init()
    {
        m_totalWidth = 0;
        m_totalHeight = 0;
        m_antiAlias = true;
        m_logScale = false;
        m_axisOnRight = true;

        m_leftMargin = 40;
        m_rightMargin = 40;
        m_topMargin = 30;
        m_bottomMargin = 35;

        m_plotAreaBgColor = 0xffffff;
        m_plotAreaBorder = 0x888888;
        m_plotAreaGap = 2;

        m_majorHGridColor = 0xdddddd;
        m_minorHGridColor = 0xdddddd;
        m_majorVGridColor = 0xdddddd;
        m_minorVGridColor = 0xdddddd;

        m_legendFont = "normal";
        m_legendFontSize = 8;
        m_legendFontColor = Chart::TextColor;
        m_legendBgColor = 0x80cccccc;

        m_yAxisFont = "normal";
        m_yAxisFontSize = 8;
        m_yAxisFontColor = Chart::TextColor;
        m_yAxisMargin = 14;

        m_xAxisFont = "normal";
        m_xAxisFontSize = 8;
        m_xAxisFontColor = Chart::TextColor;
        m_xAxisFontAngle = 0;

        m_volUnit = "";
        m_extraPoints = 0;

        m_yearFormat = "{value|yyyy}";
        m_firstMonthFormat = "<*font=bold*>{value|mmm yy}";
        m_otherMonthFormat = "{value|mmm}";
        m_firstDayFormat = "<*font=bold*>{value|d mmm}";
        m_otherDayFormat = "{value|d}";
        m_firstHourFormat = "<*font=bold*>{value|d mmm\nh:nna}";
        m_otherHourFormat = "{value|h:nna}";
        m_timeLabelSpacing = 50;

        m_generalFormat = "P3";

        m_toolTipMonthFormat = "[{xLabel|mmm yyyy}]";
        m_toolTipDayFormat = "[{xLabel|mmm d, yyyy}]";
        m_toolTipHourFormat = "[{xLabel|mmm d, yyyy hh:nn:ss}]";

        m_mainChart = 0;
        m_currentChart = 0;
    }

    /// <summary>
    /// Create a FinanceChart with a given width. The height will be automatically determined
    /// as the chart is built.
    /// </summary>
    /// <param name="width">Width of the chart in pixels</param>
    public: FinanceChart(int width): MultiChart(width, 1)
    {
        init();
        m_totalWidth = width;
        setMainChart(this);
    }

    // free resources
    public: ~FinanceChart()
    {
        delete[] const_cast<double *>(m_timeStamps.data);
        delete[] const_cast<double *>(m_highData.data);
        delete[] const_cast<double *>(m_lowData.data);
        delete[] const_cast<double *>(m_openData.data);
        delete[] const_cast<double *>(m_closeData.data);
        delete[] const_cast<double *>(m_volData.data);
        
        for (int i = 0; i < (int)garbage.size(); ++i)
            delete garbage[i];
    }

    // disable copying
    private: FinanceChart(const FinanceChart &rhs);
    private: FinanceChart &operator=(const FinanceChart &rhs);  

    /// <summary>
    /// Enable/Disable anti-alias. Enabling anti-alias makes the line smoother. Disabling
    /// anti-alias make the chart file size smaller, and so can be downloaded faster
    /// through the Internet. The default is to enable anti-alias.
    /// </summary>
    /// <param name="antiAlias">True to enable anti-alias. False to disable anti-alias.</param>
    public: void enableAntiAlias(bool antiAlias)
    {
        m_antiAlias = antiAlias;
    }

    /// <summary>
    /// Set the margins around the plot area.
    /// </summary>
    /// <param name="m_leftMargin">The distance between the plot area and the chart left edge.</param>
    /// <param name="m_topMargin">The distance between the plot area and the chart top edge.</param>
    /// <param name="m_rightMargin">The distance between the plot area and the chart right edge.</param>
    /// <param name="m_bottomMargin">The distance between the plot area and the chart bottom edge.</param>
    public: void setMargins(int leftMargin, int topMargin, int rightMargin, int bottomMargin)
    {
        m_leftMargin = leftMargin;
        m_rightMargin = rightMargin;
        m_topMargin = topMargin;
        m_bottomMargin = bottomMargin;
    }

    /// <summary>
    /// Add a text title above the plot area. You may add multiple title above the plot area by
    /// calling this method multiple times.
    /// </summary>
    /// <param name="alignment">The alignment with respect to the region that is on top of the
    /// plot area.</param>
    /// <param name="text">The text to add.</param>
    /// <returns>The TextBox object representing the text box above the plot area.</returns>
    public: TextBox* addPlotAreaTitle(int alignment, const char* text)
    {
        TextBox *ret = addText(m_leftMargin, 0, text, "bold", 10, Chart::TextColor, alignment);
        ret->setSize(m_totalWidth - m_leftMargin - m_rightMargin + 1, m_topMargin - 1);
        ret->setMargin(0);
        return ret;
    }

    /// <summary>
    /// Set the plot area style. The default is to use pale yellow 0xfffff0 as the background,
    /// and light grey 0xdddddd as the grid lines.
    /// </summary>
    /// <param name="bgColor">The plot area background color.</param>
    /// <param name="majorHGridColor">Major horizontal grid color.</param>
    /// <param name="majorVGridColor">Major vertical grid color.</param>
    /// <param name="minorHGridColor">Minor horizontal grid color. In current version, minor
    /// horizontal grid is not used.</param>
    /// <param name="minorVGridColor">Minor vertical grid color.</param>
    public: void setPlotAreaStyle(int bgColor, int majorHGridColor, int majorVGridColor,
        int minorHGridColor, int minorVGridColor)
    {
        m_plotAreaBgColor = bgColor;
        m_majorHGridColor = majorHGridColor;
        m_majorVGridColor = majorVGridColor;
        m_minorHGridColor = minorHGridColor;
        m_minorVGridColor = minorVGridColor;
    }

    /// <summary>
    /// Set the plot area border style. The default is grey color (888888), with a gap
    /// of 2 pixels between charts.
    /// </summary>
    /// <param name="borderColor">The color of the border.</param>
    /// <param name="borderGap">The gap between two charts.</param>
    public: void setPlotAreaBorder(int borderColor, int borderGap)
    {
        m_plotAreaBorder = borderColor;
        m_plotAreaGap = borderGap;
    }

    /// <summary>
    /// Set legend style. The default is Arial 8 pt black color, with light grey background.
    /// </summary>
    /// <param name="font">The font of the legend text.</param>
    /// <param name="fontSize">The font size of the legend text in points.</param>
    /// <param name="fontColor">The color of the legend text.</param>
    /// <param name="bgColor">The background color of the legend box.</param>
    public: void setLegendStyle(const char* font, double fontSize, int fontColor, int bgColor)
    {
        m_legendFont = font;
        m_legendFontSize = fontSize;
        m_legendFontColor = fontColor;
        m_legendBgColor = bgColor;
    }

    /// <summary>
    /// Set x-axis label style. The default is Arial 8 pt black color no rotation.
    /// </summary>
    /// <param name="font">The font of the axis labels.</param>
    /// <param name="fontSize">The font size of the axis labels in points.</param>
    /// <param name="fontColor">The color of the axis labels.</param>
    /// <param name="fontAngle">The rotation of the axis labels.</param>
    public: void setXAxisStyle(const char* font, double fontSize, int fontColor, double fontAngle)
    {
        m_xAxisFont = font;
        m_xAxisFontSize = fontSize;
        m_xAxisFontColor = fontColor;
        m_xAxisFontAngle = fontAngle;
    }

    /// <summary>
    /// Set y-axis label style. The default is Arial 8 pt black color, with 13 pixels margin.
    /// </summary>
    /// <param name="font">The font of the axis labels.</param>
    /// <param name="fontSize">The font size of the axis labels in points.</param>
    /// <param name="fontColor">The color of the axis labels.</param>
    /// <param name="axisMargin">The margin at the top of the y-axis in pixels (to leave
    /// space for the legend box).</param>
    public: void setYAxisStyle(const char* font, double fontSize, int fontColor, int axisMargin)
    {
        m_yAxisFont = font;
        m_yAxisFontSize = fontSize;
        m_yAxisFontColor = fontColor;
        m_yAxisMargin = axisMargin;
    }

    /// <summary>
    /// Set whether the main y-axis is on right of left side of the plot area. The default is
    /// on right.
    /// </summary>
    /// <param name="b">True if the y-axis is on right. False if the y-axis is on left.</param>
    public: void setAxisOnRight(bool b)
    {
        m_axisOnRight = b;
    }

    /// <summary>
    /// Determines if log scale should be used for the main chart. The default is linear scale.
    /// </summary>
    /// <param name="b">True for using log scale. False for using linear scale.</param>
    public: void setLogScale(bool b)
    {
        m_logScale = b;
        if (m_mainChart != 0) {
            if (m_logScale) {
                m_mainChart->yAxis()->setLogScale();
            } else {
                m_mainChart->yAxis()->setLinearScale();
            }
        }
    }

    /// <summary>
    /// Set the date/time formats to use for the x-axis labels under various cases.
    /// </summary>
    /// <param name="yearFormat">The format for displaying labels on an axis with yearly ticks. The
    /// default is "yyyy".</param>
    /// <param name="firstMonthFormat">The format for displaying labels on an axis with monthly ticks.
    /// This parameter applies to the first available month of a year (usually January) only, so it can
    /// be formatted differently from the other labels.</param>
    /// <param name="otherMonthFormat">The format for displaying labels on an axis with monthly ticks.
    /// This parameter applies to months other than the first available month of a year.</param>
    /// <param name="firstDayFormat">The format for displaying labels on an axis with daily ticks.
    /// This parameter applies to the first available day of a month only, so it can be formatted
    /// differently from the other labels.</param>
    /// <param name="otherDayFormat">The format for displaying labels on an axis with daily ticks.
    /// This parameter applies to days other than the first available day of a month.</param>
    /// <param name="firstHourFormat">The format for displaying labels on an axis with hourly
    /// resolution. This parameter applies to the first tick of a day only, so it can be formatted
    /// differently from the other labels.</param>
    /// <param name="otherHourFormat">The format for displaying labels on an axis with hourly.
    /// resolution. This parameter applies to ticks at hourly boundaries, except the first tick
    /// of a day.</param>
    public: void setDateLabelFormat(const char* yearFormat, const char* firstMonthFormat,
        const char* otherMonthFormat, const char* firstDayFormat, const char* otherDayFormat,
        const char* firstHourFormat, const char* otherHourFormat)
    {
        if (yearFormat != 0) {
            m_yearFormat = yearFormat;
        }
        if (firstMonthFormat != 0) {
            m_firstMonthFormat = firstMonthFormat;
        }
        if (otherMonthFormat != 0) {
            m_otherMonthFormat = otherMonthFormat;
        }
        if (firstDayFormat != 0) {
            m_firstDayFormat = firstDayFormat;
        }
        if (otherDayFormat != 0) {
            m_otherDayFormat = otherDayFormat;
        }
        if (firstHourFormat != 0) {
            m_firstHourFormat = firstHourFormat;
        }
        if (otherHourFormat != 0) {
            m_otherHourFormat = otherHourFormat;
        }
    }

    /// <summary>
    /// Set the minimum label spacing between two labels on the time axis
    /// </summary>
    /// <param name="labelSpacing">The minimum label spacing in pixels.</param>
    public: void setDateLabelSpacing(int labelSpacing)
    {
        if (labelSpacing > 0) {
            m_timeLabelSpacing = labelSpacing;
        } else {
             m_timeLabelSpacing = 0;
        }
    }

    /// <summary>
    /// Set the tool tip formats for display date/time
    /// </summary>
    /// <param name="monthFormat">The tool tip format to use if the data point spacing is one
    /// or more months (more than 30 days).</param>
    /// <param name="dayFormat">The tool tip format to use if the data point spacing is 1 day
    /// to less than 30 days.</param>
    /// <param name="hourFormat">The tool tip format to use if the data point spacing is less
    /// than 1 day.</param>
    public: void setToolTipDateFormat(const char* monthFormat, const char* dayFormat,
        const char* hourFormat)
    {
        if (monthFormat != 0) {
            m_toolTipMonthFormat = monthFormat;
        }
        if (dayFormat != 0) {
            m_toolTipDayFormat = dayFormat;
        }
        if (hourFormat != 0) {
            m_toolTipHourFormat = hourFormat;
        }
    }

    /// <summary>
    /// Get the tool tip format for display date/time
    /// </summary>
    /// <returns>The tool tip format string.</returns>
    public: const char *getToolTipDateFormat()
    {
        if (m_timeStamps.len == 0) {
            return m_toolTipHourFormat.c_str();
        }
        if (m_timeStamps.len <= m_extraPoints) {
            return m_toolTipHourFormat.c_str();
        }
        double resolution = (m_timeStamps[m_timeStamps.len - 1] -
            m_timeStamps[0]) / m_timeStamps.len;
        if (resolution >= 30 * 86400) {
            return m_toolTipMonthFormat.c_str();
        } else if (resolution >= 86400) {
            return m_toolTipDayFormat.c_str();
        } else {
            return m_toolTipHourFormat.c_str();
        }
    }

    /// <summary>
    /// Set the number format for use in displaying values in legend keys and tool tips.
    /// </summary>
    /// <param name="formatString">The default number format.</param>
    public: void setNumberLabelFormat(const char* formatString)
    {
        if (formatString != 0) {
            m_generalFormat = formatString;
        }
    }

    /// <summary>
    /// A utility function to compute triangular moving averages
    /// </summary>
    /// <param name="data">An array of numbers as input.</param>
    /// <param name="period">The moving average period.</param>
    /// <returns>An array representing the triangular moving average of the input array.</returns>
    private: ArrayMath computeTriMovingAvg(DoubleArray data, int period)
    {
        int p = period / 2 + 1;
        return ArrayMath(data).movAvg(p).movAvg(p);
    }

    /// <summary>
    /// A utility function to compute weighted moving averages
    /// </summary>
    /// <param name="data">An array of numbers as input.</param>
    /// <param name="period">The moving average period.</param>
    /// <returns>An array representing the weighted moving average of the input array.</returns>
    private: ArrayMath computeWeightedMovingAvg(DoubleArray data, int period)
    {
        ArrayMath acc(data);
        int i;
        for(i = 2; i < period + 1; ++i) {
            acc.add(ArrayMath(data).movAvg(i).mul(i));
        }
        return acc.div((1 + period) * period / 2);
    }

    /// <summary>
    /// A utility function to obtain the first visible closing price.
    /// </summary>
    /// <returns>The first closing price.
    /// are cd.NoValue.</returns>
    private: double firstCloseValue()
    {
        for(int i = m_extraPoints; i < m_closeData.len; ++i) {
            if ((m_closeData[i] != Chart::NoValue) && (m_closeData[i] != 0)) {
                return m_closeData[i];
            }
        }
        return Chart::NoValue;
    }

    /// <summary>
    /// A utility function to obtain the last valid position (that is, position not
    /// containing cd.NoValue) of a data series.
    /// </summary>
    /// <param name="data">An array of numbers as input.</param>
    /// <returns>The last valid position in the input array, or -1 if all positions
    /// are cd.NoValue.</returns>
    private: int lastIndex(DoubleArray data)
    {
        int i = data.len - 1;
        while (i >= 0) {
            if (data[i] != Chart::NoValue) {
                break;
            }
            i = i - 1;
        }
        return i;
    }

    //deep copy array
    private: void deepCopy(DoubleArray &dest, DoubleArray src)
    {
        if (src.len > dest.len)
        {
            delete[] const_cast<double *>(dest.data);
            dest.data = new double[src.len];
        }
        memcpy(const_cast<double *>(dest.data), src.data, src.len * sizeof(double));
        dest.len = src.len;
    }

    /// <summary>
    /// Set the data used in the chart. If some of the data are not available, some artifical
    /// values should be used. For example, if the high and low values are not available, you
    /// may use closeData as highData and lowData.
    /// </summary>
    /// <param name="timeStamps">An array of dates/times for the time intervals.</param>
    /// <param name="highData">The high values in the time intervals.</param>
    /// <param name="lowData">The low values in the time intervals.</param>
    /// <param name="openData">The open values in the time intervals.</param>
    /// <param name="closeData">The close values in the time intervals.</param>
    /// <param name="volData">The volume values in the time intervals.</param>
    /// <param name="extraPoints">The number of leading time intervals that are not
    /// displayed in the chart. These intervals are typically used for computing
    /// indicators that require extra leading data, such as moving averages.</param>
    public: void setData(DoubleArray timeStamps, DoubleArray highData, DoubleArray lowData,
        DoubleArray openData, DoubleArray closeData, DoubleArray volData, int extraPoints)
    {
        deepCopy(m_timeStamps, timeStamps);
        deepCopy(m_highData, highData);
        deepCopy(m_lowData, lowData);
        deepCopy(m_openData, openData);
        deepCopy(m_closeData, closeData);
        if (extraPoints > 0) {
            m_extraPoints = extraPoints;
        } else {
            m_extraPoints = 0;
        }

        /////////////////////////////////////////////////////////////////////////
        // Auto-detect volume units
        /////////////////////////////////////////////////////////////////////////
        double maxVol = ArrayMath(volData).maxValue();
        const char *units[] = {"", "K", "M", "B"};
        int unitIndex = sizeof(units) / sizeof(units[0]) - 1;
        while ((unitIndex > 0) && (maxVol < pow(1000.0, unitIndex))) {
            unitIndex = unitIndex - 1;
        }

        deepCopy(m_volData, ArrayMath(volData).div(pow(1000.0, unitIndex)));
        m_volUnit = units[unitIndex];
    }

    //////////////////////////////////////////////////////////////////////////////
    // Format x-axis labels
    //////////////////////////////////////////////////////////////////////////////
    private: void setXLabels(Axis* a)
    {
        a->setLabels(m_timeStamps);
        if (m_extraPoints < m_timeStamps.len) {
            int tickStep = (m_timeStamps.len - m_extraPoints) *
                m_timeLabelSpacing / (m_totalWidth - m_leftMargin - m_rightMargin) + 1;
            double timeRangeInSeconds = m_timeStamps[m_timeStamps.len
                 - 1] - m_timeStamps[m_extraPoints];
            double secondsBetweenTicks = timeRangeInSeconds / (m_totalWidth - m_leftMargin -
                m_rightMargin) * m_timeLabelSpacing;
                
            if (secondsBetweenTicks * (m_timeStamps.len - m_extraPoints) <= timeRangeInSeconds) {
                tickStep = 1;
                if (m_timeStamps.len > 1)
                    secondsBetweenTicks = m_timeStamps[m_timeStamps.len - 1] - m_timeStamps[m_timeStamps.len - 2];
                else
                    secondsBetweenTicks = 86400;
            }

            if ((secondsBetweenTicks > 360 * 86400) || ((secondsBetweenTicks > 90 * 86400) && (timeRangeInSeconds >= 720 * 86400))) {
                //yearly ticks
                a->setMultiFormat(Chart::StartOfYearFilter(), m_yearFormat.c_str(), tickStep);
            } else if ((secondsBetweenTicks >= 30 * 86400) || ((secondsBetweenTicks > 7 * 86400) && (timeRangeInSeconds >= 60 * 86400))) {
                //monthly ticks
                int monthBetweenTicks = (int)(secondsBetweenTicks / 31 / 86400) + 1;
                a->setMultiFormat(Chart::StartOfYearFilter(), m_firstMonthFormat.c_str(),
                    Chart::StartOfMonthFilter(monthBetweenTicks), m_otherMonthFormat.c_str());
                a->setMultiFormat(Chart::StartOfMonthFilter(), "-", 1, false);
            } else if ((secondsBetweenTicks >= 86400) || ((secondsBetweenTicks > 6 * 3600) & (timeRangeInSeconds >= 86400))) {
                //daily ticks
                a->setMultiFormat(Chart::StartOfMonthFilter(), m_firstDayFormat.c_str(),
                    Chart::StartOfDayFilter(1, 0.5), m_otherDayFormat.c_str(), tickStep);
            } else {
                //hourly ticks
                a->setMultiFormat(Chart::StartOfDayFilter(1, 0.5), m_firstHourFormat.c_str(),
                    Chart::StartOfHourFilter(1, 0.5), m_otherHourFormat.c_str(), tickStep);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    // Create tool tip format string for showing OHLC data
    //////////////////////////////////////////////////////////////////////////////
    private: std::string getHLOCToolTipFormat()
    {
        char buffer[1024];
        sprintf(buffer, "title='%s Op:{open|%s}, Hi:{high|%s}, Lo:{low|%s}, Cl:{close|%s}'",
            getToolTipDateFormat(), m_generalFormat.c_str(), m_generalFormat.c_str(), m_generalFormat.c_str(),
            m_generalFormat.c_str());
        return buffer;
    }

    /// <summary>
    /// Add the main chart - the chart that shows the HLOC data.
    /// </summary>
    /// <param name="height">The height of the main chart in pixels.</param>
    /// <returns>An XYChart object representing the main chart created.</returns>
    public: XYChart* addMainChart(int height)
    {
        m_mainChart = addIndicator(height);
        m_mainChart->yAxis()->setMargin(2 * m_yAxisMargin);
        if (m_logScale) {
            m_mainChart->yAxis()->setLogScale();
        } else {
            m_mainChart->yAxis()->setLinearScale();
        }
        return m_mainChart;
    }

    /// <summary>
    /// Add a candlestick layer to the main chart.
    /// </summary>
    /// <param name="upColor">The candle color for an up day.</param>
    /// <param name="downColor">The candle color for a down day.</param>
    /// <returns>The CandleStickLayer created.</returns>
    public: CandleStickLayer* addCandleStick(int upColor, int downColor)
    {
        addOHLCLabel(upColor, downColor, true);
        CandleStickLayer *ret = m_mainChart->addCandleStickLayer(m_highData, m_lowData, m_openData,
            m_closeData, upColor, downColor);
        ret->setHTMLImageMap("", "", getHLOCToolTipFormat().c_str());
        if (m_highData.len - m_extraPoints > 60) {
            ret->setDataGap(0);
        }
            
        if (m_highData.len > m_extraPoints) {
            int expectedWidth = (m_totalWidth - m_leftMargin - m_rightMargin) / (m_highData.len - m_extraPoints);
            if (expectedWidth <= 5)
                ret->setDataWidth(expectedWidth + 1 - expectedWidth % 2);
        }

        return ret;
    }

    /// <summary>
    /// Add a HLOC layer to the main chart.
    /// </summary>
    /// <param name="upColor">The color of the HLOC symbol for an up day.</param>
    /// <param name="downColor">The color of the HLOC symbol for a down day.</param>
    /// <returns>The HLOCLayer created.</returns>
    public: HLOCLayer* addHLOC(int upColor, int downColor)
    {
        addOHLCLabel(upColor, downColor, false);
        HLOCLayer *ret = m_mainChart->addHLOCLayer(m_highData, m_lowData, m_openData, m_closeData);
        ret->setColorMethod(Chart::HLOCUpDown, upColor, downColor);
        ret->setHTMLImageMap("", "", getHLOCToolTipFormat().c_str());
        ret->setDataGap(0);
        return ret;
    }

    private: void addOHLCLabel(int upColor, int downColor, bool candleStickMode)
    {
        int i = lastIndex(m_closeData);
        if (i >= 0) {
            double openValue = Chart::NoValue;
            double closeValue = Chart::NoValue;
            double highValue = Chart::NoValue;
            double lowValue = Chart::NoValue;

            if (i < m_openData.len) {
                openValue = m_openData[i];
            }
            if (i < m_closeData.len) {
                closeValue = m_closeData[i];
            }
            if (i < m_highData.len) {
                highValue = m_highData[i];
            }
            if (i < m_lowData.len) {
                lowValue = m_lowData[i];
            }

            std::string openLabel = "";
            std::string closeLabel = "";
            std::string highLabel = "";
            std::string lowLabel = "";
            const char *delim = "";
            char buffer[1024];

            if (openValue != Chart::NoValue) {
                sprintf(buffer, "Op:%s", formatValue(openValue, m_generalFormat.c_str()));
                openLabel = buffer;
                delim = ", ";
            }
            if (highValue != Chart::NoValue) {
                sprintf(buffer, "%sHi:%s", delim, formatValue(highValue, m_generalFormat.c_str()));
                highLabel = buffer;
                delim = ", ";
            }
            if (lowValue != Chart::NoValue) {
                sprintf(buffer, "%sLo:%s", delim, formatValue(lowValue, m_generalFormat.c_str()));
                lowLabel = buffer;
                delim = ", ";
            }
            if (closeValue != Chart::NoValue) {
                sprintf(buffer, "%sCl:%s", delim, formatValue(closeValue, m_generalFormat.c_str()));
                closeLabel = buffer;
                delim = ", ";
            }
            std::string label = openLabel + highLabel + lowLabel + closeLabel;

            bool useUpColor = (closeValue >= openValue);
            if (candleStickMode != true) {
                ArrayMath closeChanges = ArrayMath(m_closeData).delta();
                int lastChangeIndex = lastIndex(closeChanges);
                useUpColor = (lastChangeIndex < 0);
                if (useUpColor != true) {
                    useUpColor = (closeChanges.result()[lastChangeIndex] >= 0);
                }
            }

            int udcolor = downColor;
            if (useUpColor) {
                udcolor = upColor;
            }
            m_mainChart->getLegend()->addKey(label.c_str(), udcolor);
        }
    }

    /// <summary>
    /// Add a closing price line on the main chart.
    /// </summary>
    /// <param name="color">The color of the line.</param>
    /// <returns>The LineLayer object representing the line created.</returns>
    public: LineLayer* addCloseLine(int color)
    {
        return addLineIndicator2(m_mainChart, m_closeData, color, "Closing Price");
    }

    /// <summary>
    /// Add a weight close line on the main chart.
    /// </summary>
    /// <param name="color">The color of the line.</param>
    /// <returns>The LineLayer object representing the line created.</returns>
    public: LineLayer* addWeightedClose(int color)
    {
        return addLineIndicator2(m_mainChart, ArrayMath(m_highData).add(m_lowData).add(m_closeData
            ).add(m_closeData).div(4), color, "Weighted Close");
    }

    /// <summary>
    /// Add a typical price line on the main chart.
    /// </summary>
    /// <param name="color">The color of the line.</param>
    /// <returns>The LineLayer object representing the line created.</returns>
    public: LineLayer* addTypicalPrice(int color)
    {
        return addLineIndicator2(m_mainChart, ArrayMath(m_highData).add(m_lowData).add(m_closeData
            ).div(3), color, "Typical Price");
    }

    /// <summary>
    /// Add a median price line on the main chart.
    /// </summary>
    /// <param name="color">The color of the line.</param>
    /// <returns>The LineLayer object representing the line created.</returns>
    public: LineLayer* addMedianPrice(int color)
    {
        return addLineIndicator2(m_mainChart, ArrayMath(m_highData).add(m_lowData).div(2), color,
            "Median Price");
    }

    /// <summary>
    /// Add a simple moving average line on the main chart.
    /// </summary>
    /// <param name="period">The moving average period</param>
    /// <param name="color">The color of the line.</param>
    /// <returns>The LineLayer object representing the line created.</returns>
    public: LineLayer* addSimpleMovingAvg(int period, int color)
    {
        char buffer[1024];
        sprintf(buffer, "SMA (%d)", period);
        return addLineIndicator2(m_mainChart, ArrayMath(m_closeData).movAvg(period), color, buffer);
    }

    /// <summary>
    /// Add an exponential moving average line on the main chart.
    /// </summary>
    /// <param name="period">The moving average period</param>
    /// <param name="color">The color of the line.</param>
    /// <returns>The LineLayer object representing the line created.</returns>
    public: LineLayer* addExpMovingAvg(int period, int color)
    {
        char buffer[1024];
        sprintf(buffer, "EMA (%d)", period);
        return addLineIndicator2(m_mainChart, ArrayMath(m_closeData).expAvg(2.0 / (period + 1)),
            color, buffer);
    }

    /// <summary>
    /// Add a triangular moving average line on the main chart.
    /// </summary>
    /// <param name="period">The moving average period</param>
    /// <param name="color">The color of the line.</param>
    /// <returns>The LineLayer object representing the line created.</returns>
    public: LineLayer* addTriMovingAvg(int period, int color)
    {
        char buffer[1024];
        sprintf(buffer, "TMA (%d)", period);
        return addLineIndicator2(m_mainChart, computeTriMovingAvg(m_closeData, period),
            color, buffer);
    }

    /// <summary>
    /// Add a weighted moving average line on the main chart.
    /// </summary>
    /// <param name="period">The moving average period</param>
    /// <param name="color">The color of the line.</param>
    /// <returns>The LineLayer object representing the line created.</returns>
    public: LineLayer* addWeightedMovingAvg(int period, int color)
    {
        char buffer[1024];
        sprintf(buffer, "WMA (%d)", period);
        return addLineIndicator2(m_mainChart, computeWeightedMovingAvg(m_closeData, period
            ), color, buffer);
    }

                /// <summary>
    /// Add a parabolic SAR indicator to the main chart.
    /// </summary>
    /// <param name="accInitial">Initial acceleration factor</param>
    /// <param name="accIncrement">Acceleration factor increment</param>
    /// <param name="accMaximum">Maximum acceleration factor</param>
    /// <param name="symbolType">The symbol used to plot the parabolic SAR</param>
    /// <param name="symbolSize">The symbol size in pixels</param>
    /// <param name="fillColor">The fill color of the symbol</param>
    /// <param name="edgeColor">The edge color of the symbol</param>
    /// <returns>The LineLayer object representing the layer created.</returns>
    public: LineLayer* addParabolicSAR(double accInitial, double accIncrement, double accMaximum,
        int symbolType, int symbolSize, int fillColor, int edgeColor)
    {
        bool isLong = true;
        double acc = accInitial;
        double extremePoint = 0;
        double *psar = new double[m_lowData.len];

        int i_1 = -1;
        int i_2 = -1;

        for(int i = 0; i < m_lowData.len; ++i) {
            psar[i] = Chart::NoValue;
            if ((m_lowData[i] != Chart::NoValue) && (m_highData[i] != Chart::NoValue)) {
                if ((i_1 >= 0) && (i_2 < 0)) {
                    if (m_lowData[i_1] <= m_lowData[i]) {
                        psar[i] = m_lowData[i_1];
                        isLong = true;
                        if (m_highData[i_1] > m_highData[i]) {
                            extremePoint = m_highData[i_1];
                        } else {
                            extremePoint = m_highData[i];
                        }
                    } else {
                        extremePoint = m_lowData[i];
                        isLong = false;
                        if (m_highData[i_1] > m_highData[i]) {
                            psar[i] = m_highData[i_1];
                        } else {
                            psar[i] = m_highData[i];
                        }
                    }
                } else if ((i_1 >= 0) && (i_2 >= 0)) {
                    if (acc > accMaximum) {
                        acc = accMaximum;
                    }

                    psar[i] = psar[i_1] + acc * (extremePoint - psar[i_1]);

                    if (isLong) {
                        if (m_lowData[i] < psar[i]) {
                            isLong = false;
                            psar[i] = extremePoint;
                            extremePoint = m_lowData[i];
                            acc = accInitial;
                        } else {
                            if (m_highData[i] > extremePoint) {
                                extremePoint = m_highData[i];
                                acc = acc + accIncrement;
                            }

                            if (m_lowData[i_1] < psar[i]) {
                                psar[i] = m_lowData[i_1];
                            }
                            if (m_lowData[i_2] < psar[i]) {
                                psar[i] = m_lowData[i_2];
                            }
                        }
                    } else {
                        if (m_highData[i] > psar[i]) {
                            isLong = true;
                            psar[i] = extremePoint;
                            extremePoint = m_highData[i];
                            acc = accInitial;
                        } else {
                            if (m_lowData[i] < extremePoint) {
                                extremePoint = m_lowData[i];
                                acc = acc + accIncrement;
                            }

                            if (m_highData[i_1] > psar[i]) {
                                psar[i] = m_highData[i_1];
                            }
                            if (m_highData[i_2] > psar[i]) {
                                psar[i] = m_highData[i_2];
                            }
                        }
                    }
                }

                i_2 = i_1;
                i_1 = i;
            }
        }

        LineLayer* ret = addLineIndicator2(m_mainChart, DoubleArray(psar, m_lowData.len), 
            fillColor, "Parabolic SAR");
        ret->setLineWidth(0);
        
        ret = addLineIndicator2(m_mainChart, DoubleArray(psar, m_lowData.len), fillColor, "");
        ret->setLineWidth(0);
        ret->getDataSet(0)->setDataSymbol(symbolType, symbolSize, fillColor, edgeColor);
        
        delete[] psar;
        return ret;
    }

    /// <summary>
    /// Add a comparison line to the main price chart.
    /// </summary>
    /// <param name="data">The data series to compare to</param>
    /// <param name="color">The color of the comparison line</param>
    /// <param name="name">The name of the comparison line</param>
    /// <returns>The LineLayer object representing the line layer created.</returns>
    public: LineLayer* addComparison(DoubleArray data, int color, const char *name)
    {
        int firstIndex = m_extraPoints;
        while ((firstIndex < data.len) && (firstIndex < m_closeData.len)) {
            if ((data[firstIndex] != Chart::NoValue) && (m_closeData[firstIndex] != Chart::NoValue
                ) && (data[firstIndex] != 0) && (m_closeData[firstIndex] != 0)) {
                break;
            }
            firstIndex = firstIndex + 1;
        }
        if ((firstIndex >= data.len) || (firstIndex >= m_closeData.len)) {
            return 0;
        }

        double scaleFactor = m_closeData[firstIndex] / data[firstIndex];
        LineLayer* layer = m_mainChart->addLineLayer(ArrayMath(data).mul(scaleFactor).result(
            ), Chart::Transparent);
        layer->setHTMLImageMap("{disable}");

        Axis* a = m_mainChart->addAxis(Chart::Right, 0);
        a->setColors(Chart::Transparent, Chart::Transparent);
        a->syncAxis(m_mainChart->yAxis(), 1 / scaleFactor, 0);

        LineLayer* ret = addLineIndicator2(m_mainChart, data, color, name);
        ret->setUseYAxis(a);
        return ret;
    }

    /// <summary>
    /// Display percentage axis scale
    /// </summary>
    /// <returns>The Axis object representing the percentage axis.</returns>
    public: Axis* setPercentageAxis()
    {
        double firstClose = firstCloseValue();
        if (firstClose == Chart::NoValue) {
            return 0;
        }

        int axisAlign = Chart::Left;
        if (m_axisOnRight) {
            axisAlign = Chart::Right;
        }

        Axis* ret = m_mainChart->addAxis(axisAlign, 0);
        configureYAxis(ret, 300);
        ret->syncAxis(m_mainChart->yAxis(), 100 / firstClose);
        ret->setRounding(false, false);
        ret->setLabelFormat("{={value}-100|@}%");
        m_mainChart->yAxis()->setColors(Chart::Transparent, Chart::Transparent);
        m_mainChart->getPlotArea()->setGridAxis(0, ret);
        return ret;
    }

    /// <summary>
    /// Add a generic band to the main finance chart. This method is used internally by other methods to add
    /// various bands (eg. Bollinger band, Donchian channels, etc).
    /// </summary>
    /// <param name="upperLine">The data series for the upper band line.</param>
    /// <param name="lowerLine">The data series for the lower band line.</param>
    /// <param name="lineColor">The color of the upper and lower band line.</param>
    /// <param name="fillColor">The color to fill the region between the upper and lower band lines.</param>
    /// <param name="name">The name of the band.</param>
    /// <returns>An InterLineLayer object representing the filled region.</returns>
    public: InterLineLayer* addBand(DoubleArray upperLine, DoubleArray lowerLine, int lineColor,
        int fillColor, const char* name)
    {
        int i = upperLine.len - 1;
        if (i >= lowerLine.len) {
            i = lowerLine.len - 1;
        }

        char buffer[1024];
        while (i >= 0) {
            if ((upperLine[i] != Chart::NoValue) && (lowerLine[i] != Chart::NoValue)) {
                sprintf(buffer, "%s: %s - ", name, formatValue(lowerLine[i], m_generalFormat.c_str()));
                name = strcat(buffer, formatValue(upperLine[i], m_generalFormat.c_str()));
                break;
            }
            i = i - 1;
        }

        LineLayer *layer = m_mainChart->addLineLayer();
        layer->addDataSet(upperLine, lineColor, name);
        layer->addDataSet(lowerLine, lineColor);
        return m_mainChart->addInterLineLayer(layer->getLine(0), layer->getLine(1), fillColor);
    }

    /// <summary>
    /// Add a Bollinger band on the main chart.
    /// </summary>
    /// <param name="period">The period to compute the band.</param>
    /// <param name="bandWidth">The half-width of the band in terms multiples of standard deviation. Typically 2 is used.</param>
    /// <param name="lineColor">The color of the lines defining the upper and lower limits.</param>
    /// <param name="fillColor">The color to fill the regional within the band.</param>
    /// <returns>The InterLineLayer object representing the band created.</returns>
    public: InterLineLayer* addBollingerBand(int period, double bandWidth, int lineColor,
        int fillColor)
    {
        //Bollinger Band is moving avg +/- (width * moving std deviation)
        ArrayMath stdDev = ArrayMath(m_closeData).movStdDev(period).mul(bandWidth);
        ArrayMath movAvg = ArrayMath(m_closeData).movAvg(period);
        char buffer[1024];
        sprintf(buffer, "Bollinger (%d, %g)", period, bandWidth);
        return addBand(ArrayMath(movAvg).add(stdDev), ArrayMath(movAvg).sub(stdDev).selectGTZ(), lineColor,
            fillColor, buffer);
    }

    /// <summary>
    /// Add a Donchian channel on the main chart.
    /// </summary>
    /// <param name="period">The period to compute the band.</param>
    /// <param name="lineColor">The color of the lines defining the upper and lower limits.</param>
    /// <param name="fillColor">The color to fill the regional within the band.</param>
    /// <returns>The InterLineLayer object representing the band created.</returns>
    public: InterLineLayer* addDonchianChannel(int period, int lineColor, int fillColor)
    {
        //Donchian Channel is the zone between the moving max and moving min
        char buffer[1024];
        sprintf(buffer, "Donchian (%d)", period);
        return addBand(ArrayMath(m_highData).movMax(period), ArrayMath(m_lowData).movMin(period),
            lineColor, fillColor, buffer);
    }

    /// <summary>
    /// Add a price envelop on the main chart. The price envelop is a defined as a ratio around a
    /// moving average. For example, a ratio of 0.2 means 20% above and below the moving average.
    /// </summary>
    /// <param name="period">The period for the moving average.</param>
    /// <param name="range">The ratio above and below the moving average.</param>
    /// <param name="lineColor">The color of the lines defining the upper and lower limits.</param>
    /// <param name="fillColor">The color to fill the regional within the band.</param>
    /// <returns>The InterLineLayer object representing the band created.</returns>
    public: InterLineLayer* addEnvelop(int period, double range, int lineColor, int fillColor)
    {
        //Envelop is moving avg +/- percentage
        ArrayMath movAvg = ArrayMath(m_closeData).movAvg(period);
        char buffer[1024];
        sprintf(buffer, "Envelop (SMA %d +/- %g%%)", period, range * 100);
        return addBand(ArrayMath(movAvg).mul(1 + range), ArrayMath(movAvg).mul(1 - range),
            lineColor, fillColor, buffer);
    }

    /// <summary>
    /// Add a volume bar chart layer on the main chart.
    /// </summary>
    /// <param name="height">The height of the bar chart layer in pixels.</param>
    /// <param name="upColor">The color to used on an 'up' day. An 'up' day is a day where
    /// the closing price is higher than that of the previous day.</param>
    /// <param name="downColor">The color to used on a 'down' day. A 'down' day is a day
    /// where the closing price is lower than that of the previous day.</param>
    /// <param name="flatColor">The color to used on a 'flat' day. A 'flat' day is a day
    /// where the closing price is the same as that of the previous day.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: BarLayer* addVolBars(int height, int upColor, int downColor, int flatColor)
    {
        return addVolBars2(m_mainChart, height, upColor, downColor, flatColor);
    }

    private: BarLayer* addVolBars2(XYChart* c, int height, int upColor, int downColor, int flatColor)
    {       
        BarLayer *barLayer = c->addBarLayer(Chart::Overlay);
        barLayer->setBorderColor(Chart::Transparent);
  
        if (c == m_mainChart) {
            configureYAxis(c->yAxis2(), height);
            int topMargin = c->getDrawArea()->getHeight() - m_topMargin - m_bottomMargin - height +
                m_yAxisMargin;
            if (topMargin < 0) {
                topMargin = 0;
            }
            c->yAxis2()->setTopMargin(topMargin);
            barLayer->setUseYAxis2();
        }

        Axis *a = c->yAxis2();
        if (c != m_mainChart) {
            a = c->yAxis();
        }
 
        char buffer[1024];
        if (ArrayMath(m_volData).maxValue() < 10)
            sprintf(buffer, "{value|1}%s", m_volUnit.c_str());
        else
            sprintf(buffer, "{value}%s", m_volUnit.c_str());
        a->setLabelFormat(buffer);

        ArrayMath closeChange = ArrayMath(m_closeData).delta().replace(Chart::NoValue, 0);
        DoubleArray closeChangeData = closeChange.result();
        
        int i = lastIndex(m_volData);
        std::string label = "Vol";
        if (i >= 0) {
            sprintf(buffer, "%s: %s%s", label.c_str(), formatValue(m_volData[i], m_generalFormat.c_str()), 
                m_volUnit.c_str());
            label = buffer;
        }

        DataSet* upDS = barLayer->addDataSet(ArrayMath(m_volData).selectGTZ(closeChange).result(), upColor);
        DataSet* dnDS = barLayer->addDataSet(ArrayMath(m_volData).selectLTZ(closeChange).result(), downColor);
        DataSet* flatDS = barLayer->addDataSet(ArrayMath(m_volData).selectEQZ(closeChange).result(), flatColor);

        if ((i < 0) || (closeChangeData[i] == 0) || (closeChangeData[i] == Chart::NoValue)) {
            flatDS->setDataName(label.c_str());
        } else if (closeChangeData[i] > 0) {
            upDS->setDataName(label.c_str());
        } else {
            dnDS->setDataName(label.c_str());
        }

        return barLayer;
    }

    /// <summary>
    /// Add a blank indicator chart to the finance chart. Used internally to add other indicators.
    /// Override to change the default formatting (eg. axis fonts, etc.) of the various indicators.
    /// </summary>
    /// <param name="height">The height of the chart in pixels.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addIndicator(int height)
    {
        //create a new chart object
        XYChart *ret = new XYChart(m_totalWidth, height + m_topMargin + m_bottomMargin,
            Chart::Transparent);
        garbage.push_back(ret);
        
        ret->setTrimData(m_extraPoints);

        if (m_currentChart != 0) {
            //if there is a chart before the newly created chart, disable its x-axis, and copy
            //its x-axis labels to the new chart
            m_currentChart->xAxis()->setColors(Chart::Transparent, Chart::Transparent);
            ret->xAxis()->copyAxis(m_currentChart->xAxis());

            //add chart to MultiChart and update the total height
            addChart(0, m_totalHeight + m_plotAreaGap, ret);
            m_totalHeight += height + 1 + m_plotAreaGap;
        } else {
            //no existing chart - create the x-axis labels from scratch
            setXLabels(ret->xAxis());

            //add chart to MultiChart and update the total height
            addChart(0, m_totalHeight, ret);
            m_totalHeight += height + 1;
        }

        //the newly created chart becomes the current chart
        m_currentChart = ret;

        //update the size
        setSize(m_totalWidth, m_totalHeight + m_topMargin + m_bottomMargin);

        //configure the plot area
        ret->setPlotArea(m_leftMargin, m_topMargin, m_totalWidth - m_leftMargin - m_rightMargin, 
            height, m_plotAreaBgColor, -1, m_plotAreaBorder)->setGridColor(m_majorHGridColor, 
            m_majorVGridColor, m_minorHGridColor, m_minorVGridColor);
        ret->setAntiAlias(m_antiAlias);

        //configure legend box
        if (m_legendFontColor != (int)Chart::Transparent) {
            LegendBox *box = ret->addLegend(m_leftMargin, m_topMargin, false, m_legendFont.c_str(),
                m_legendFontSize);
            box->setFontColor(m_legendFontColor);
            box->setBackground(m_legendBgColor);
            box->setMargin(5, 0, 2, 1);
            box->setSize(m_totalWidth - m_leftMargin - m_rightMargin + 1, 0);
        }

        //configure x-axis
        Axis *a = ret->xAxis();
        a->setIndent(true);
        a->setTickLength(2, 0);
        a->setColors(Chart::Transparent, m_xAxisFontColor, m_xAxisFontColor, m_xAxisFontColor);
        a->setLabelStyle(m_xAxisFont.c_str(), m_xAxisFontSize, m_xAxisFontColor, m_xAxisFontAngle);

        //configure y-axis
        ret->setYAxisOnRight(m_axisOnRight);
        configureYAxis(ret->yAxis(), height);

        return ret;
    }

    private: void configureYAxis(Axis* a, int height)
    {
        a->setAutoScale(0, 0.05, 0);
        if (height < 100) {
            a->setTickDensity(15);
        }
        a->setMargin(m_yAxisMargin);
        a->setLabelStyle(m_yAxisFont.c_str(), m_yAxisFontSize, m_yAxisFontColor, 0);
        a->setTickLength(-4, -2);
        a->setColors(Chart::Transparent, m_yAxisFontColor, m_yAxisFontColor, m_yAxisFontColor);
    }

    /// <summary>
    /// Add a generic line indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="data">The data series of the indicator line.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="name">The name of the indicator.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addLineIndicator(int height, DoubleArray data, int color, const char* name)
    {
        XYChart *c = addIndicator(height);
        addLineIndicator2(c, data, color, name);
        return c;
    }

    /// <summary>
    /// Add a line to an existing indicator chart.
    /// </summary>
    /// <param name="c">The indicator chart to add the line to.</param>
    /// <param name="data">The data series of the indicator line.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="name">The name of the indicator.</param>
    /// <returns>The LineLayer object representing the line created.</returns>
    public: LineLayer* addLineIndicator2(XYChart* c, DoubleArray data, int color, const char* name)
    {
        return c->addLineLayer(data, color, formatIndicatorLabel(name, data).c_str());
    }

    /// <summary>
    /// Add a generic bar indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="data">The data series of the indicator bars.</param>
    /// <param name="color">The color of the indicator bars.</param>
    /// <param name="name">The name of the indicator.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addBarIndicator(int height, DoubleArray data, int color, const char* name)
    {
        XYChart *c = addIndicator(height);
        addBarIndicator2(c, data, color, name);
        return c;
    }

    /// <summary>
    /// Add a bar layer to an existing indicator chart.
    /// </summary>
    /// <param name="c">The indicator chart to add the bar layer to.</param>
    /// <param name="data">The data series of the indicator bars.</param>
    /// <param name="color">The color of the indicator bars.</param>
    /// <param name="name">The name of the indicator.</param>
    /// <returns>The BarLayer object representing the bar layer created.</returns>
    public: BarLayer* addBarIndicator2(XYChart* c, DoubleArray data, int color, const char* name)
    {
        BarLayer *layer = c->addBarLayer(data, color, formatIndicatorLabel(name, data).c_str());
        layer->setBorderColor(Chart::Transparent);
        return layer;
    }

    /// <summary>
    /// Add an upper/lower threshold range to an existing indicator chart.
    /// </summary>
    /// <param name="c">The indicator chart to add the threshold range to.</param>
    /// <param name="layer">The line layer that the threshold range applies to.</param>
    /// <param name="topRange">The upper threshold.</param>
    /// <param name="topColor">The color to fill the region of the line that is above the
    /// upper threshold.</param>
    /// <param name="bottomRange">The lower threshold.</param>
    /// <param name="bottomColor">The color to fill the region of the line that is below
    /// the lower threshold.</param>
    public: void addThreshold(XYChart* c, LineLayer* layer, double topRange, int topColor,
        double bottomRange, int bottomColor)
    {
        Mark *topMark = c->yAxis()->addMark(topRange, topColor, formatValue(topRange,
            m_generalFormat.c_str()));
        Mark *bottomMark = c->yAxis()->addMark(bottomRange, bottomColor, formatValue(bottomRange,
            m_generalFormat.c_str()));

        c->addInterLineLayer(layer->getLine(), topMark->getLine(), topColor, Chart::Transparent);
        c->addInterLineLayer(layer->getLine(), bottomMark->getLine(), Chart::Transparent,
            bottomColor);
    }

    private: std::string formatIndicatorLabel(const char* name, DoubleArray data)
    {
        int i = lastIndex(data);
        if (name == 0) {
            return name;
        }
        char buffer[1024];
        if ((*name == 0) || (i < 0))
            //still need sprintf to escape the "%" character
            sprintf(buffer, "%s", name);
        else
            sprintf(buffer, "%s: %s", name, formatValue(data[i], m_generalFormat.c_str()));
        return buffer;
    }

    /// <summary>
    /// Add an Accumulation/Distribution indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addAccDist(int height, int color)
    {
        //Close Location Value = ((C - L) - (H - C)) / (H - L)
        //Accumulation Distribution Line = Accumulation of CLV * volume
        ArrayMath range = ArrayMath(m_highData).sub(m_lowData);
        return addLineIndicator(height, ArrayMath(m_closeData).mul(2).sub(m_lowData).sub(m_highData
            ).mul(m_volData).financeDiv(range, 0).acc(), color, "Accumulation/Distribution");
    }

    private: ArrayMath computeAroonUp(int period)
    {
        double *aroonUp = new double[m_highData.len];

        for(int i = 0; i < m_highData.len; ++i) {
            double highValue = m_highData[i];
            if (highValue == Chart::NoValue) {
                aroonUp[i] = Chart::NoValue;
            } else {
                int currentIndex = i;
                int highCount = period;
                int count = period;

                while ((count > 0) && (currentIndex >= count)) {
                    currentIndex = currentIndex - 1;
                    double currentValue = m_highData[currentIndex];
                    if (currentValue != Chart::NoValue) {
                        count = count - 1;
                        if (currentValue > highValue) {
                            highValue = currentValue;
                            highCount = count;
                        }
                    }
                }

                if (count > 0) {
                    aroonUp[i] = Chart::NoValue;
                } else {
                    aroonUp[i] = highCount * 100.0 / period;
                }
            }
        }

        ArrayMath ret(DoubleArray(aroonUp, m_highData.len));
        delete[] aroonUp;
        return ret;
    }

    private: ArrayMath computeAroonDn(int period)
    {
        double *aroonDn = new double[m_lowData.len];
        
        for(int i = 0; i < m_lowData.len; ++i) {
            double lowValue = m_lowData[i];
            if (lowValue == Chart::NoValue) {
                aroonDn[i] = Chart::NoValue;
            } else {
                int currentIndex = i;
                int lowCount = period;
                int count = period;

                while ((count > 0) && (currentIndex >= count)) {
                    currentIndex = currentIndex - 1;
                    double currentValue = m_lowData[currentIndex];
                    if (currentValue != Chart::NoValue) {
                        count = count - 1;
                        if (currentValue < lowValue) {
                            lowValue = currentValue;
                            lowCount = count;
                        }
                    }
                }

                if (count > 0) {
                    aroonDn[i] = Chart::NoValue;
                } else {
                    aroonDn[i] = lowCount * 100.0 / period;
                }
            }
        }

        ArrayMath ret(DoubleArray(aroonDn, m_lowData.len));
        delete[] aroonDn;
        return ret;
    }

    /// <summary>
    /// Add an Aroon Up/Down indicators chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicators.</param>
    /// <param name="upColor">The color of the Aroon Up indicator line.</param>
    /// <param name="downColor">The color of the Aroon Down indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addAroon(int height, int period, int upColor, int downColor)
    {
        XYChart *c = addIndicator(height);
        addLineIndicator2(c, computeAroonUp(period), upColor, "Aroon Up");
        addLineIndicator2(c, computeAroonDn(period), downColor, "Aroon Down");
        c->yAxis()->setLinearScale(0, 100);
        return c;
    }

    /// <summary>
    /// Add an Aroon Oscillator indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addAroonOsc(int height, int period, int color)
    {
        char buffer[1024];
        sprintf(buffer, "Aroon Oscillator (%d)", period);
        XYChart *c = addLineIndicator(height, computeAroonUp(period).sub(computeAroonDn(
            period)), color, buffer);
        c->yAxis()->setLinearScale(-100, 100);
        return c;
    }

    private: ArrayMath computeTrueRange()
    {
        ArrayMath previousClose = ArrayMath(m_closeData).shift();
        DoubleArray previousCloseData = previousClose.result();
        ArrayMath range = ArrayMath(m_highData).sub(m_lowData);
        DoubleArray rangeData = range.result();
        double temp = 0;

        double *ret = new double[m_highData.len];
        for(int i = 0; i < m_highData.len; ++i) {
            ret[i] = rangeData[i];
            if ((ret[i] != Chart::NoValue) && (previousCloseData[i] != Chart::NoValue)) {
                temp = fabs(m_highData[i] - previousCloseData[i]);
                if (temp > ret[i]) {
                    ret[i] = temp;
                }
                temp = fabs(previousCloseData[i] - m_lowData[i]);
                if (temp > ret[i]) {
                    ret[i] = temp;
                }
            }
        }

        ArrayMath ret1(DoubleArray(ret, m_highData.len));
        delete[] ret;
        return ret1;
    }

    /// <summary>
    /// Add an Average Directional Index indicators chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="posColor">The color of the Positive Directional Index line.</param>
    /// <param name="negColor">The color of the Negatuve Directional Index line.</param>
    /// <param name="color">The color of the Average Directional Index line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addADX(int height, int period, int posColor, int negColor, int color)
    {
        //pos/neg directional movement
        ArrayMath pos = ArrayMath(m_highData).delta().selectGTZ();
        ArrayMath neg = ArrayMath(m_lowData).delta().mul(-1).selectGTZ();
        ArrayMath delta = ArrayMath(pos).sub(neg);
        pos.selectGTZ(delta);
        neg.selectLTZ(delta);
        
        //initial value
        DoubleArray posData = pos.result();
        DoubleArray negData = neg.result();
        if ((posData.len > 1) && (posData[1] != Chart::NoValue) && (negData[1] !=
            Chart::NoValue)) {
            const_cast<double *>(posData.data)[1] = (posData[1] * 2 + negData[1]) / 3;
            const_cast<double *>(negData.data)[1] = (negData[1] + posData[1]) / 2;
            pos = ArrayMath(posData);
            neg = ArrayMath(negData);
        }

        //pos/neg directional index
        ArrayMath tr = computeTrueRange();
        tr.expAvg(1.0 / period);
        pos.expAvg(1.0 / period).financeDiv(tr, 0).mul(100);
        neg.expAvg(1.0 / period).financeDiv(tr, 0).mul(100);

        //directional movement index ??? what happen if division by zero???
        ArrayMath totalDM = ArrayMath(pos).add(neg);
        ArrayMath dx = ArrayMath(pos).sub(neg).abs().financeDiv(totalDM,
            0).mul(100).expAvg(1.0 / period);

        XYChart *c = addIndicator(height);
        char buffer1[1024];
        sprintf(buffer1, "+DI (%d)", period);
        char buffer2[1024];
        sprintf(buffer2, "-DI (%d)", period);
        char buffer3[1024];
        sprintf(buffer3, "ADX (%d)", period);
        addLineIndicator2(c, pos, posColor, buffer1);
        addLineIndicator2(c, neg, negColor, buffer2);
        addLineIndicator2(c, dx, color, buffer3);
        return c;
    }

    /// <summary>
    /// Add an Average True Range indicators chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color1">The color of the True Range line.</param>
    /// <param name="color2">The color of the Average True Range line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addATR(int height, int period, int color1, int color2)
    {
        ArrayMath trueRange = computeTrueRange();
        XYChart *c = addLineIndicator(height, trueRange, color1, "True Range");
        char buffer[1024];
        sprintf(buffer, "Average True Range (%d)", period);
        addLineIndicator2(c, ArrayMath(trueRange).expAvg(2.0 / (period + 1)), color2, buffer);
        return c;
    }

    /// <summary>
    /// Add a Bollinger Band Width indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="width">The band width to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addBollingerWidth(int height, int period, double width, int color)
    {
        char buffer[1024];
        sprintf(buffer, "Bollinger Width (%d, %g)", period, width);
        return addLineIndicator(height, ArrayMath(m_closeData).movStdDev(period).mul(width * 2), color,
            buffer);
    }

    /// <summary>
    /// Add a Community Channel Index indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="deviation">The distance beween the middle line and the upper and lower threshold lines.</param>
    /// <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    /// <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addCCI(int height, int period, int color, double deviation, int upColor,
        int downColor)
    {
        //typical price
        ArrayMath tp = ArrayMath(m_highData).add(m_lowData).add(m_closeData).div(3);
        DoubleArray tpData = tp.result();

        //simple moving average of typical price
        ArrayMath smvtp = ArrayMath(tp).movAvg(period);
        DoubleArray smvtpData = smvtp.result();

        //compute mean deviation
        double *movMeanDev = new double[smvtpData.len];
        
        for(int i = 0; i < smvtpData.len; ++i) {
            double avg = smvtpData[i];
            if (avg == Chart::NoValue) {
                movMeanDev[i] = Chart::NoValue;
            } else {
                int currentIndex = i;
                int count = period - 1;
                double acc = 0;

                while ((count >= 0) && (currentIndex >= count)) {
                    double currentValue = tpData[currentIndex];
                    currentIndex = currentIndex - 1;
                    if (currentValue != Chart::NoValue) {
                        count = count - 1;
                        acc = acc + fabs(avg - currentValue);
                    }
                }

                if (count > 0) {
                    movMeanDev[i] = Chart::NoValue;
                } else {
                    movMeanDev[i] = acc / period;
                }
            }
        }

        XYChart *c = addIndicator(height);
        char buffer[1024];
        sprintf(buffer, "CCI (%d)", period);
        LineLayer *layer = addLineIndicator2(c, ArrayMath(tp).sub(smvtpData).financeDiv(
            DoubleArray(movMeanDev, smvtpData.len), 0).div(0.015), color, buffer);
        addThreshold(c, layer, deviation, upColor, -deviation, downColor);
       
        delete[] movMeanDev;
        return c;
    }

    /// <summary>
    /// Add a Chaikin Money Flow indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addChaikinMoneyFlow(int height, int period, int color)
    {
        ArrayMath range = ArrayMath(m_highData).sub(m_lowData);
        ArrayMath volAvg = ArrayMath(m_volData).movAvg(period);
        char buffer[1024];
        sprintf(buffer, "Chaikin Money Flow (%d)", period);
        return addBarIndicator(height, ArrayMath(m_closeData).mul(2).sub(m_lowData).sub(m_highData
            ).mul(m_volData).financeDiv(range, 0).movAvg(period).financeDiv(volAvg, 0), color, buffer
            );
    }

    /// <summary>
    /// Add a Chaikin Oscillator indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addChaikinOscillator(int height, int color)
    {
        //first compute acc/dist line
        ArrayMath range = ArrayMath(m_highData).sub(m_lowData);
        ArrayMath accdist = ArrayMath(m_closeData).mul(2).sub(m_lowData).sub(m_highData).mul(
            m_volData).financeDiv(range, 0).acc();

        //chaikin osc = exp3(accdist) - exp10(accdist)
        ArrayMath expAvg10 = ArrayMath(accdist).expAvg(2.0 / (10 + 1));
        return addLineIndicator(height, ArrayMath(accdist).expAvg(2.0 / (3 + 1)).sub(expAvg10), color,
            "Chaikin Oscillator");
    }

    /// <summary>
    /// Add a Chaikin Volatility indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period1">The period to smooth the range.</param>
    /// <param name="period2">The period to compute the rate of change of the smoothed range.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addChaikinVolatility(int height, int period1, int period2, int color)
    {
        char buffer[1024];
        sprintf(buffer, "Chaikin Volatility (%d, %d)", period1, period2);
        return addLineIndicator(height, ArrayMath(m_highData).sub(m_lowData).expAvg(2.0 / (period1 +
            1)).rate(period2).sub(1).mul(100), color, buffer);
    }

    /// <summary>
    /// Add a Close Location Value indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addCLV(int height, int color)
    {
        //Close Location Value = ((C - L) - (H - C)) / (H - L)
        ArrayMath range = ArrayMath(m_highData).sub(m_lowData);
        return addLineIndicator(height, ArrayMath(m_closeData).mul(2).sub(m_lowData).sub(m_highData
            ).financeDiv(range, 0), color, "Close Location Value");
    }

    /// <summary>
    /// Add a Detrended Price Oscillator indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addDPO(int height, int period, int color)
    {
        char buffer[1024];
        sprintf(buffer, "DPO (%d)", period);
        return addLineIndicator(height, ArrayMath(m_closeData).movAvg(period).shift(period / 2 + 1
            ).sub(m_closeData).mul(-1), color, buffer);
    }

    /// <summary>
    /// Add a Donchian Channel Width indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addDonchianWidth(int height, int period, int color)
    {
        char buffer[1024];
        sprintf(buffer, "Donchian Width (%d)", period);
        return addLineIndicator(height, ArrayMath(m_highData).movMax(period).sub(ArrayMath(m_lowData
            ).movMin(period)), color, buffer);
    }

    /// <summary>
    /// Add a Ease of Movement indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to smooth the indicator.</param>
    /// <param name="color1">The color of the indicator line.</param>
    /// <param name="color2">The color of the smoothed indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addEaseOfMovement(int height, int period, int color1, int color2)
    {
        ArrayMath boxRatioInverted = ArrayMath(m_highData).sub(m_lowData).financeDiv(m_volData, 0);
        ArrayMath result = ArrayMath(m_highData).add(m_lowData).div(2).delta().mul(boxRatioInverted)
            ;

        XYChart *c = addLineIndicator(height, result, color1, "EMV");
        char buffer[1024];
        sprintf(buffer, "EMV EMA (%d)", period);
        addLineIndicator2(c, ArrayMath(result).movAvg(period), color2, buffer);
        return c;
    }

    /// <summary>
    /// Add a Fast Stochastic indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period1">The period to compute the %K line.</param>
    /// <param name="period2">The period to compute the %D line.</param>
    /// <param name="color1">The color of the %K line.</param>
    /// <param name="color2">The color of the %D line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addFastStochastic(int height, int period1, int period2, int color1, int color2)
    {
        ArrayMath movLow = ArrayMath(m_lowData).movMin(period1);
        ArrayMath movRange = ArrayMath(m_highData).movMax(period1).sub(movLow);
        ArrayMath stochastic = ArrayMath(m_closeData).sub(movLow).financeDiv(movRange, 0.5).mul(100)
            ;

        char buffer[1024];
        sprintf(buffer, "Fast Stochastic %%K (%d)", period1);
        XYChart *c = addLineIndicator(height, stochastic, color1, buffer);
        sprintf(buffer, "%%D (%d)", period2);
        addLineIndicator2(c, ArrayMath(stochastic).movAvg(period2), color2, buffer);

        c->yAxis()->setLinearScale(0, 100);
        return c;
    }

    /// <summary>
    /// Add a MACD indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period1">The first moving average period to compute the indicator.</param>
    /// <param name="period2">The second moving average period to compute the indicator.</param>
    /// <param name="period3">The moving average period of the signal line.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="signalColor">The color of the signal line.</param>
    /// <param name="divColor">The color of the divergent bars.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addMACD(int height, int period1, int period2, int period3, int color,
        int signalColor, int divColor)
    {
        XYChart *c = addIndicator(height);
        char buffer[1024];

        //MACD is defined as the difference between two exponential averages (typically 12/26 days)
        ArrayMath expAvg1 = ArrayMath(m_closeData).expAvg(2.0 / (period1 + 1));
        ArrayMath macd = ArrayMath(m_closeData).expAvg(2.0 / (period2 + 1)).sub(expAvg1);

        //Add the MACD line
        sprintf(buffer, "MACD (%d, %d)", period1, period2);
        addLineIndicator2(c, macd, color, buffer);

        //MACD signal line
        ArrayMath macdSignal = ArrayMath(macd).expAvg(2.0 / (period3 + 1));
        sprintf(buffer, "EXP (%d)", period3);
        addLineIndicator2(c, macdSignal, signalColor, buffer);

        //Divergence
        addBarIndicator2(c, ArrayMath(macd).sub(macdSignal), divColor, "Divergence");

        return c;
    }

    /// <summary>
    /// Add a Mass Index indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    /// <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addMassIndex(int height, int color, int upColor, int downColor)
    {
        //Mass Index
        double f = 2.0 / (10);
        ArrayMath exp9 = ArrayMath(m_highData).sub(m_lowData).expAvg(f);
        ArrayMath exp99 = ArrayMath(exp9).expAvg(f);

        XYChart *c = addLineIndicator(height, ArrayMath(exp9).financeDiv(exp99, 1).movAvg(25).mul(25
            ), color, "Mass Index");
        c->yAxis()->addMark(27, upColor);
        c->yAxis()->addMark(26.5, downColor);
        return c;
    }

    /// <summary>
    /// Add a Money Flow Index indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="range">The distance beween the middle line and the upper and lower threshold lines.</param>
    /// <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    /// <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addMFI(int height, int period, int color, double range, int upColor,
        int downColor)
    {
        //Money Flow Index
        ArrayMath typicalPrice = ArrayMath(m_highData).add(m_lowData).add(m_closeData).div(3);
        ArrayMath moneyFlow = ArrayMath(typicalPrice).mul(m_volData);

        ArrayMath selector = ArrayMath(typicalPrice).delta();
        ArrayMath posMoneyFlow = ArrayMath(moneyFlow).selectGTZ(selector).movAvg(period);
        ArrayMath posNegMoneyFlow = ArrayMath(moneyFlow).selectLTZ(selector).movAvg(period).add(
            posMoneyFlow);

        XYChart *c = addIndicator(height);
        char buffer[1024];
        sprintf(buffer, "Money Flow Index (%d)", period);
        LineLayer *layer = addLineIndicator2(c, ArrayMath(posMoneyFlow).financeDiv(posNegMoneyFlow,
            0.5).mul(100), color, buffer);
        addThreshold(c, layer, 50 + range, upColor, 50 - range, downColor);

        c->yAxis()->setLinearScale(0, 100);
        return c;
    }

    /// <summary>
    /// Add a Momentum indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addMomentum(int height, int period, int color)
    {
        char buffer[1024];
        sprintf(buffer, "Momentum (%d)", period);
        return addLineIndicator(height, ArrayMath(m_closeData).delta(period), color, buffer);
    }

    /// <summary>
    /// Add a Negative Volume Index indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the signal line.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="signalColor">The color of the signal line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addNVI(int height, int period, int color, int signalColor)
    {
        double *nvi = new double[m_volData.len];

        double previousNVI = 100;
        double previousVol = Chart::NoValue;
        double previousClose = Chart::NoValue;
        int i;
        for(i = 0; i < m_volData.len; ++i) {
            if (m_volData[i] == Chart::NoValue) {
                nvi[i] = Chart::NoValue;
            } else {
                if ((previousVol != Chart::NoValue) && (m_volData[i] < previousVol) && (
                    previousClose != Chart::NoValue) && (m_closeData[i] != Chart::NoValue)) {
                    nvi[i] = previousNVI + previousNVI * (m_closeData[i] - previousClose) /
                        previousClose;
                } else {
                    nvi[i] = previousNVI;
                }

                previousNVI = nvi[i];
                previousVol = m_volData[i];
                previousClose = m_closeData[i];
            }
        }

        XYChart *c = addLineIndicator(height, DoubleArray(nvi, m_volData.len), color, "NVI");
        if (m_volData.len > period) {
            char buffer[1024];
            sprintf(buffer, "NVI SMA (%d)", period);
            addLineIndicator2(c, ArrayMath(DoubleArray(nvi, m_volData.len)).movAvg(period), 
                signalColor, buffer);
        }

        delete[] nvi;
        return c;
    }

    /// <summary>
    /// Add an On Balance Volume indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addOBV(int height, int color)
    {
        ArrayMath closeChange = ArrayMath(m_closeData).delta();
        ArrayMath upVolume = ArrayMath(m_volData).selectGTZ(closeChange);
        ArrayMath downVolume = ArrayMath(m_volData).selectLTZ(closeChange);

        return addLineIndicator(height, ArrayMath(upVolume).sub(downVolume).acc(), color, "OBV");
    }

    /// <summary>
    /// Add a Performance indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addPerformance(int height, int color)
    {
        double closeValue = firstCloseValue();
        if (closeValue != Chart::NoValue) {
            return addLineIndicator(height, ArrayMath(m_closeData).mul(100 / closeValue
                ).sub(100).result(), color, "Performance");
        } else {
            //chart is empty !!!
            return addIndicator(height);
        }
    }

    /// <summary>
    /// Add a Percentage Price Oscillator indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period1">The first moving average period to compute the indicator.</param>
    /// <param name="period2">The second moving average period to compute the indicator.</param>
    /// <param name="period3">The moving average period of the signal line.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="signalColor">The color of the signal line.</param>
    /// <param name="divColor">The color of the divergent bars.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addPPO(int height, int period1, int period2, int period3, int color,
        int signalColor, int divColor)
    {
        ArrayMath expAvg1 = ArrayMath(m_closeData).expAvg(2.0 / (period1 + 1));
        ArrayMath expAvg2 = ArrayMath(m_closeData).expAvg(2.0 / (period2 + 1));
        ArrayMath ppo = ArrayMath(expAvg2).sub(expAvg1).financeDiv(expAvg2, 0).mul(100);
        ArrayMath ppoSignal = ArrayMath(ppo).expAvg(2.0 / (period3 + 1));

        char buffer[1024];
        sprintf(buffer, "PPO (%d, %d)", period1, period2);
        XYChart *c = addLineIndicator(height, ppo, color, buffer);
        sprintf(buffer, "EMA (%d)", period3);
        addLineIndicator2(c, ppoSignal, signalColor, buffer);
        addBarIndicator2(c, ppo.sub(ppoSignal), divColor, "Divergence");
        return c;
    }

    /// <summary>
    /// Add a Positive Volume Index indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the signal line.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="signalColor">The color of the signal line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addPVI(int height, int period, int color, int signalColor)
    {
        //Positive Volume Index
        double *pvi = new double[m_volData.len];

        double previousPVI = 100;
        double previousVol = Chart::NoValue;
        double previousClose = Chart::NoValue;
        int i;
        for(i = 0; i < m_volData.len; ++i) {
            if (m_volData[i] == Chart::NoValue) {
                pvi[i] = Chart::NoValue;
            } else {
                if ((previousVol != Chart::NoValue) && (m_volData[i] > previousVol) && (
                    previousClose != Chart::NoValue) && (m_closeData[i] != Chart::NoValue)) {
                    pvi[i] = previousPVI + previousPVI * (m_closeData[i] - previousClose) /
                        previousClose;
                } else {
                    pvi[i] = previousPVI;
                }

                previousPVI = pvi[i];
                previousVol = m_volData[i];
                previousClose = m_closeData[i];
            }
        }

        XYChart *c = addLineIndicator(height, DoubleArray(pvi, m_volData.len), color, "PVI");
        if (m_volData.len > period) {
            char buffer[1024];
            sprintf(buffer, "PVI SMA (%d)", period);
            addLineIndicator2(c, ArrayMath(DoubleArray(pvi, m_volData.len)).movAvg(period), 
                signalColor, buffer);
        }

        delete[] pvi;
        return c;
    }

    /// <summary>
    /// Add a Percentage Volume Oscillator indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period1">The first moving average period to compute the indicator.</param>
    /// <param name="period2">The second moving average period to compute the indicator.</param>
    /// <param name="period3">The moving average period of the signal line.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="signalColor">The color of the signal line.</param>
    /// <param name="divColor">The color of the divergent bars.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addPVO(int height, int period1, int period2, int period3, int color,
        int signalColor, int divColor)
    {
        ArrayMath expAvg1 = ArrayMath(m_volData).expAvg(2.0 / (period1 + 1));
        ArrayMath expAvg2 = ArrayMath(m_volData).expAvg(2.0 / (period2 + 1));
        ArrayMath pvo = ArrayMath(expAvg2).sub(expAvg1).financeDiv(expAvg2, 0).mul(100);
        ArrayMath pvoSignal = ArrayMath(pvo).expAvg(2.0 / (period3 + 1));

        char buffer[1024];
        sprintf(buffer, "PVO (%d, %d)", period1, period2);
        XYChart *c = addLineIndicator(height, pvo, color, buffer);
        sprintf(buffer, "EMA (%d)", period3);
        addLineIndicator2(c, pvoSignal, signalColor, buffer);
        addBarIndicator2(c, pvo.sub(pvoSignal), divColor, "Divergence");
        return c;
    }

    /// <summary>
    /// Add a Price Volumne Trend indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addPVT(int height, int color)
    {
        return addLineIndicator(height, ArrayMath(m_closeData).rate().sub(1).mul(m_volData).acc(),
            color, "PVT");
    }

    /// <summary>
    /// Add a Rate of Change indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addROC(int height, int period, int color)
    {
        char buffer[1024];
        sprintf(buffer, "ROC (%d)", period);
        return addLineIndicator(height, ArrayMath(m_closeData).rate(period).sub(1).mul(100), color,
            buffer);
    }

    private: ArrayMath& RSIMovAvg(ArrayMath& dataRef, int period) {
        //The "moving average" in classical RSI is based on a formula that mixes simple 
        //and exponential moving averages.

        if (period <= 0)
            period = 1;
                
        int count = 0;
        double acc = 0;
        int dataLen = dataRef.result().len;
        double *data = const_cast<double *>(dataRef.result().data);
        
        for (int i = 0; i < dataLen; ++i) {
            if (fabs(data[i] / Chart::NoValue - 1) > 1e-005) {
                ++count;
                acc += data[i];
                if (count < period) {
                    data[i] = Chart::NoValue;
                } else {
                    data[i] = acc / period;
                    acc = data[i] * (period - 1);
                }
            }
        }

        return dataRef;
    }

    private: ArrayMath computeRSI(int period)
    {
        //RSI is defined as the average up changes for the last 14 days, divided by the
        //average absolute changes for the last 14 days, expressed as a percentage.

        return RSIMovAvg(ArrayMath(m_closeData).delta().selectGTZ(), period).financeDiv(
            RSIMovAvg(ArrayMath(m_closeData).delta().abs(), period), 0.5).mul(100);
    }

    /// <summary>
    /// Add a Relative Strength Index indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="range">The distance beween the middle line and the upper and lower threshold lines.</param>
    /// <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    /// <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addRSI(int height, int period, int color, double range, int upColor,
        int downColor)
    {
        XYChart *c = addIndicator(height);
        char buffer[1024];
        sprintf(buffer, "RSI (%d)", period);
        LineLayer *layer = addLineIndicator2(c, computeRSI(period), color, buffer);

        //Add range if given
        if ((range > 0) && (range < 50)) {
            addThreshold(c, layer, 50 + range, upColor, 50 - range, downColor);
        }
        c->yAxis()->setLinearScale(0, 100);
        return c;
    }

    /// <summary>
    /// Add a Slow Stochastic indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period1">The period to compute the %K line.</param>
    /// <param name="period2">The period to compute the %D line.</param>
    /// <param name="color1">The color of the %K line.</param>
    /// <param name="color2">The color of the %D line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addSlowStochastic(int height, int period1, int period2, int color1, int color2)
    {
        ArrayMath movLow = ArrayMath(m_lowData).movMin(period1);
        ArrayMath movRange = ArrayMath(m_highData).movMax(period1).sub(movLow);
        ArrayMath stochastic = ArrayMath(m_closeData).sub(movLow).financeDiv(movRange, 0.5
            ).mul(100).movAvg(3);

        char buffer[1024];
        sprintf(buffer, "Slow Stochastic %%K (%d)", period1);
        XYChart *c = addLineIndicator(height, stochastic, color1, buffer);
        sprintf(buffer, "%%D (%d)", period2);
        addLineIndicator2(c, stochastic.movAvg(period2), color2, buffer);

        c->yAxis()->setLinearScale(0, 100);
        return c;
    }

    /// <summary>
    /// Add a Moving Standard Deviation indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addStdDev(int height, int period, int color)
    {
        char buffer[1024];
        sprintf(buffer, "Moving StdDev (%d)", period);
        return addLineIndicator(height, ArrayMath(m_closeData).movStdDev(period), color, buffer);
    }

    /// <summary>
    /// Add a Stochastic RSI indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="range">The distance beween the middle line and the upper and lower threshold lines.</param>
    /// <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    /// <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addStochRSI(int height, int period, int color, double range, int upColor,
        int downColor)
    {
        ArrayMath rsi = computeRSI(period);
        ArrayMath movLow = ArrayMath(rsi).movMin(period);
        ArrayMath movRange = ArrayMath(rsi).movMax(period).sub(movLow);

        XYChart *c = addIndicator(height);
        char buffer[1024];
        sprintf(buffer, "StochRSI (%d)", period);
        LineLayer *layer = addLineIndicator2(c, ArrayMath(rsi).sub(movLow).financeDiv(movRange, 0.5
            ).mul(100), color, buffer);

        //Add range if given
        if ((range > 0) && (range < 50)) {
            addThreshold(c, layer, 50 + range, upColor, 50 - range, downColor);
        }
        c->yAxis()->setLinearScale(0, 100);
        return c;
    }

    /// <summary>
    /// Add a TRIX indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addTRIX(int height, int period, int color)
    {
        double f = 2.0 / (period + 1);
        char buffer[1024];
        sprintf(buffer, "TRIX (%d)", period);
        return addLineIndicator(height, ArrayMath(m_closeData).expAvg(f).expAvg(f).expAvg(f)
            .rate().sub(1).mul(100), color, buffer);
    }

    private: ArrayMath computeTrueLow()
    {
        //the lower of today's low or yesterday's close.
        ArrayMath previousClose = ArrayMath(m_closeData).shift();
        DoubleArray previousCloseData = previousClose.result();
        double *ret = new double[m_lowData.len];

        for(int i = 0; i < m_lowData.len; ++i) {
            if ((m_lowData[i] != Chart::NoValue) && (previousCloseData[i] != Chart::NoValue)) {
                if (m_lowData[i] < previousCloseData[i]) {
                    ret[i] = m_lowData[i];
                } else {
                    ret[i] = previousCloseData[i];
                }
            } else {
                ret[i] = Chart::NoValue;
            }
        }

        ArrayMath ret1(DoubleArray(ret, m_lowData.len));
        delete[] ret;
        return ret1;
    }

    /// <summary>
    /// Add an Ultimate Oscillator indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period1">The first moving average period to compute the indicator.</param>
    /// <param name="period2">The second moving average period to compute the indicator.</param>
    /// <param name="period3">The third moving average period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="range">The distance beween the middle line and the upper and lower threshold lines.</param>
    /// <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    /// <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addUltimateOscillator(int height, int period1, int period2, int period3,
        int color, double range, int upColor, int downColor)
    {
        ArrayMath trueLow = computeTrueLow();
        ArrayMath buyingPressure = ArrayMath(m_closeData).sub(trueLow);
        ArrayMath trueRange = computeTrueRange();

        ArrayMath rawUO1 = ArrayMath(buyingPressure).movAvg(period1).financeDiv(ArrayMath(trueRange
            ).movAvg(period1), 0.5).mul(4);
        ArrayMath rawUO2 = ArrayMath(buyingPressure).movAvg(period2).financeDiv(ArrayMath(trueRange
            ).movAvg(period2), 0.5).mul(2);
        ArrayMath rawUO3 = ArrayMath(buyingPressure).movAvg(period3).financeDiv(ArrayMath(trueRange
            ).movAvg(period3), 0.5).mul(1);

        XYChart *c = addIndicator(height);
        char buffer[1024];
        sprintf(buffer, "Ultimate Oscillator (%d, %d, %d)", period1, period2, period3);
        LineLayer *layer = addLineIndicator2(c, ArrayMath(rawUO1).add(rawUO2).add(rawUO3).mul(100.0
             / 7), color, buffer);
        addThreshold(c, layer, 50 + range, upColor, 50 - range, downColor);

        c->yAxis()->setLinearScale(0, 100);
        return c;
    }

    /// <summary>
    /// Add a Volume indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="upColor">The color to used on an 'up' day. An 'up' day is a day where
    /// the closing price is higher than that of the previous day.</param>
    /// <param name="downColor">The color to used on a 'down' day. A 'down' day is a day
    /// where the closing price is lower than that of the previous day.</param>
    /// <param name="flatColor">The color to used on a 'flat' day. A 'flat' day is a day
    /// where the closing price is the same as that of the previous day.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addVolIndicator(int height, int upColor, int downColor, int flatColor)
    {
        XYChart *c = addIndicator(height);
        addVolBars2(c, height, upColor, downColor, flatColor);
        return c;
    }

    /// <summary>
    /// Add a William %R indicator chart.
    /// </summary>
    /// <param name="height">The height of the indicator chart in pixels.</param>
    /// <param name="period">The period to compute the indicator.</param>
    /// <param name="color">The color of the indicator line.</param>
    /// <param name="range">The distance beween the middle line and the upper and lower threshold lines.</param>
    /// <param name="upColor">The fill color when the indicator exceeds the upper threshold line.</param>
    /// <param name="downColor">The fill color when the indicator falls below the lower threshold line.</param>
    /// <returns>The XYChart object representing the chart created.</returns>
    public: XYChart* addWilliamR(int height, int period, int color, double range, int upColor,
        int downColor)
    {
        ArrayMath movLow = ArrayMath(m_lowData).movMin(period);
        ArrayMath movHigh = ArrayMath(m_highData).movMax(period);
        ArrayMath movRange = ArrayMath(movHigh).sub(movLow);

        XYChart *c = addIndicator(height);
        LineLayer *layer = addLineIndicator2(c, ArrayMath(movHigh).sub(m_closeData).financeDiv(
            movRange, 0.5).mul(-100), color, "William %%R");
        addThreshold(c, layer, -50 + range, upColor, -50 - range, downColor);
        c->yAxis()->setLinearScale(-100, 0);
        return c;
    }

};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
