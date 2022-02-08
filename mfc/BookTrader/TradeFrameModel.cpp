#include "StdAfx.h"
#include "TradeFrameModel.h"

// registers to recieve trade, quote, and marketdepth data for symbol
// registers with portfolio manager to recieve profit/loss information on outstanding orders
// registers with order system to know about order placement and execution
// emits change notifications to view:
//  trade
//  quote 
//  market depth
//  order existence
// but model does not know about view, it only emits change events on requested values

CTradeFrameModel::CTradeFrameModel(void) {
}

CTradeFrameModel::~CTradeFrameModel(void) {
}
