#include "stdafx.h"

#include "EClientSocket.h"

#include "EWrapper.h"
#include "MySocket.h"
#include "TwsSocketClientErrors.h"

#include "Contract.h"
#include "Order.h"
#include "OrderState.h"
#include "Execution.h"
#include "ScannerSubscription.h"

#include <sstream>
#include <iomanip>

/////////////////////////////////////////////////////////////////////////////////
// SOCKET CLIENT VERSION CHANGE LOG : Incremented when the format of incomming 
//                                    server responses change 
/////////////////////////////////////////////////////////////////////////////////
// constants
// 6 = Added parentId to orderStatus
// 7 = The new execDetails event returned for an order filled status and reqExecDetails
//     Also added market depth support.
// 8 = Added 'lastFillPrice' to orderStatus and 'permId' to execDetails
// 9 = Added 'avgCost', 'unrealizedPNL', and 'unrealizedPNL' to updatePortfolio event
// 10 = Added 'serverId' to the 'open order' & 'order status' events. 
//      We send back all the API open orders upon connection.
//      Added new methods reqAllOpenOrders, reqAutoOpenOrders()
//      Added FA support - reqExecution has filter.
//                       - reqAccountUpdates takes acct code.
// 11 = Added permId to openOrder event.
// 12 = Added IgnoreRth, hidden, and discretionaryAmt
// 13 = Added GoodAfterTime
// 14 = always send size on bid/ask/last tick
// 15 = send allocation string with open order
// 16 = can receive account name in account and portfolio updates, and fa params in openOrder
// 17 = can receive liquidation field in exec reports, and notAutoAvailable field in mkt data
// 18 = can receive good till date field in open order messages, and send backfill requests
// 19 = can receive new extended order attributes in OPEN_ORDER
// 20 = expects TWS time string on connection after server version >= 20, and parentId in open order
// 21 = can receive bond contract details.
// 22 = can receive price magnifier in contract details
// 23 = support for scanner
// 24 = can receive volatility order parameters in open order messages
// 25 = can receive HMDS query start and end times
// 26 = can receive option vols in option market data messages
// 27 = can receive delta neutral order type and delta neutral aux price
// 28 = can receive option model computation ticks
// 29 = can receive trail stop limit price in open order and can place them: API 8.91
// 30 = can receive extended bond contract def, new ticks, and trade count in bars
// 31 = can receive EFP extensions to scanner and market data, and combo legs on open orders 
//    ; can receive RT bars 
// 32 = can receive TickType.LAST_TIMESTAMP
// 33 = can receive ScaleNumComponents and ScaleComponentSize is open order messages
// 34 = can receive whatIf orders / order state
// 35 = can receive contId field for Contract objects
// 36 = can receive outsideRth field for Order objects
// 37 = can receive clearingAccount and clearingIntent for Order objects

const int CLIENT_VERSION    = 37;
const int SERVER_VERSION    = 38;

// outgoing msg id's
const int REQ_MKT_DATA      = 1;
const int CANCEL_MKT_DATA	= 2;
const int PLACE_ORDER		= 3;
const int CANCEL_ORDER      = 4;
const int REQ_OPEN_ORDERS   = 5;
const int REQ_ACCT_DATA     = 6;
const int REQ_EXECUTIONS    = 7;
const int REQ_IDS           = 8;
const int REQ_CONTRACT_DATA = 9;
const int REQ_MKT_DEPTH     = 10;
const int CANCEL_MKT_DEPTH  = 11;
const int REQ_NEWS_BULLETINS = 12;
const int CANCEL_NEWS_BULLETINS = 13;
const int SET_SERVER_LOGLEVEL   = 14;
const int REQ_AUTO_OPEN_ORDERS  = 15;
const int REQ_ALL_OPEN_ORDERS   = 16;
const int REQ_MANAGED_ACCTS     = 17;
const int REQ_FA = 18 ;
const int REPLACE_FA = 19 ;
const int REQ_HISTORICAL_DATA = 20;
const int EXERCISE_OPTIONS = 21;
const int REQ_SCANNER_SUBSCRIPTION = 22;
const int CANCEL_SCANNER_SUBSCRIPTION = 23;
const int REQ_SCANNER_PARAMETERS = 24;
const int CANCEL_HISTORICAL_DATA = 25;
const int REQ_CURRENT_TIME = 49;
const int REQ_REAL_TIME_BARS = 50;
const int CANCEL_REAL_TIME_BARS = 51;

//const int MIN_SERVER_VER_REAL_TIME_BARS = 34;
//const int MIN_SERVER_VER_SCALE_ORDERS = 35;
//const int MIN_SERVER_VER_SNAPSHOT_MKT_DATA = 35;
//const int MIN_SERVER_VER_SSHORT_COMBO_LEGS = 35;
//const int MIN_SERVER_VER_WHAT_IF_ORDERS = 36;
//const int MIN_SERVER_VER_CONTRACT_CONID = 37;
const int MIN_SERVER_VER_PTA_ORDERS = 39;

// incoming msg id's
const int TICK_PRICE		= 1;
const int TICK_SIZE			= 2;
const int ORDER_STATUS		= 3;
const int ERR_MSG			= 4;
const int OPEN_ORDER		= 5;
const int ACCT_VALUE        = 6;
const int PORTFOLIO_VALUE   = 7;
const int ACCT_UPDATE_TIME  = 8;
const int NEXT_VALID_ID     = 9;
const int CONTRACT_DATA     = 10;
const int EXECUTION_DATA    = 11;
const int MARKET_DEPTH      = 12; 
const int MARKET_DEPTH_L2   = 13;
const int NEWS_BULLETINS    = 14;
const int MANAGED_ACCTS     = 15;
const int RECEIVE_FA        = 16 ;
const int HISTORICAL_DATA   = 17 ;
const int BOND_CONTRACT_DATA = 18;
const int SCANNER_PARAMETERS = 19;
const int SCANNER_DATA       = 20;
const int TICK_OPTION_COMPUTATION = 21;
const int TICK_GENERIC = 45;
const int TICK_STRING = 46;
const int TICK_EFP = 47;
const int CURRENT_TIME = 49;
const int REAL_TIME_BARS = 50;

// TWS New Bulletins constants
const int NEWS_MSG            = 1;    // standard IB news bulleting message
const int EXCHANGE_AVAIL_MSG  = 2;    // control message specifing that an exchange is available for trading
const int EXCHANGE_UNAVAIL_MSG= 3;    // control message specifing that an exchange is unavailable for trading

///////////////////////////////////////////////////////////
// utility funcs
static inline bool is(const char *str)
{
	// return true if the string is not empty
	return str && *str;
}

static inline bool isNull(const char *str) {
	// return true if the string is null or empty
	return !is( str);
}

static CString errMsg(CException *e) {
	// return the error associated with this exception
	char buf[1024];
	e->GetErrorMessage( (LPTSTR)(LPCTSTR) buf, sizeof buf);
	return CString( buf);
}

///////////////////////////////////////////////////////////
// helper macroses
#define DECODE_FIELD(x) if (!DecodeField(x, ptr, endPtr)) return 0;
#define DECODE_FIELD_MAX(x) if (!DecodeFieldMax(x, ptr, endPtr)) return 0;

#define ENCODE_FIELD(x) EncodeField(msg, x);
#define ENCODE_FIELD_MAX(x) EncodeFieldMax(msg, x);

///////////////////////////////////////////////////////////
// helper structures
namespace {

struct BarData {
	CString date;
	double open;
	double high;
	double low;
	double close;
	int volume;
	double average;
	CString hasGaps;
	int barCount;
};

struct ScanData {
	ContractDetails contract;
	int rank;
	CString distance;
	CString benchmark;
	CString projection;
	CString legsStr;
};

} // end of anonymous namespace

///////////////////////////////////////////////////////////
// encoders
template<class T>
void EClientSocket::EncodeField(std::ostream& os, T value)
{
	os << value << '\0';
}

template<>
void EClientSocket::EncodeField<bool>(std::ostream& os, bool boolValue)
{
	EncodeField<int>(os, boolValue ? 1 : 0);
}

///////////////////////////////////////////////////////////
// "max" encoders
void EClientSocket::EncodeFieldMax(std::ostream& os, int intValue)
{
	if (intValue == INT_MAX) {
		EncodeField(os, "");
		return;
	}
	EncodeField(os, intValue);
}

void EClientSocket::EncodeFieldMax(std::ostream& os, double doubleValue)
{
	if (doubleValue == DBL_MAX) {
		EncodeField(os, "");
		return;
	}
	EncodeField(os, doubleValue);
}

///////////////////////////////////////////////////////////
// decoders
bool EClientSocket::CheckOffset(const char* ptr, const char* endPtr)
{
	ASSERT (ptr && ptr <= endPtr);
	return (ptr && ptr < endPtr);
}

const char* EClientSocket::FindFieldEnd(const char* ptr, const char* endPtr)
{
	return (const char*)memchr(ptr, 0, endPtr - ptr);
}

bool EClientSocket::DecodeField(bool& boolValue, const char*& ptr, const char* endPtr)
{
	int intValue;
	if (!DecodeField(intValue, ptr, endPtr))
		return false;
	boolValue = (intValue > 0);
	return true;
}

bool EClientSocket::DecodeField(int& intValue, const char*& ptr, const char* endPtr)
{
	if (!CheckOffset(ptr, endPtr))
		return false;
	const char* fieldBeg = ptr;
	const char* fieldEnd = FindFieldEnd(fieldBeg, endPtr);
	if (!fieldEnd)
		return false;
	intValue = atoi(fieldBeg);
	ptr = ++fieldEnd;
	return true;
}

bool EClientSocket::DecodeField(long& longValue, const char*& ptr, const char* endPtr)
{
	int intValue;
	if (!DecodeField(intValue, ptr, endPtr))
		return false;
	longValue = intValue;
	return true;
}

bool EClientSocket::DecodeField(double& doubleValue, const char*& ptr, const char* endPtr)
{
	if (!CheckOffset(ptr, endPtr))
		return false;
	const char* fieldBeg = ptr;
	const char* fieldEnd = FindFieldEnd(fieldBeg, endPtr);
	if (!fieldEnd)
		return false;
	doubleValue = atof(fieldBeg);
	ptr = ++fieldEnd;
	return true;
}

bool EClientSocket::DecodeField(CString& stringValue,
								const char*& ptr, const char* endPtr)
{
	if (!CheckOffset(ptr, endPtr))
		return false;
	const char* fieldBeg = ptr;
	const char* fieldEnd = FindFieldEnd(ptr, endPtr);
	if (!fieldEnd)
		return false;
	stringValue = fieldBeg; // better way?
	ptr = ++fieldEnd;
	return true;
}

bool EClientSocket::DecodeFieldMax(int& intValue, const char*& ptr, const char* endPtr)
{
	CString stringValue;
	if (!DecodeField(stringValue, ptr, endPtr))
		return false;
	intValue = (stringValue.IsEmpty() ? UNSET_INTEGER : atoi((const char * )(LPCTSTR)stringValue));
	return true;
}

bool EClientSocket::DecodeFieldMax(long& longValue, const char*& ptr, const char* endPtr)
{
	int intValue;
	if (!DecodeFieldMax(intValue, ptr, endPtr))
		return false;
	longValue = intValue;
	return true;
}

bool EClientSocket::DecodeFieldMax(double& doubleValue, const char*& ptr, const char* endPtr)
{
	CString stringValue;
	if (!DecodeField(stringValue, ptr, endPtr))
		return false;
	doubleValue = (stringValue.IsEmpty() ? UNSET_DOUBLE : atof((const char * )(LPCTSTR) stringValue));
	return true;
}

///////////////////////////////////////////////////////////
// static helpers

static const size_t BufferSizeHighMark = 1 * 1024 * 1024; // 1Mb

void EClientSocket::CleanupBuffer(BytesVec& buffer, int processed)
{
	ASSERT( buffer.empty() || processed <= (int)buffer.size());

	if( buffer.empty())
		return;

	if( processed <= 0)
		return;

	if( processed == buffer.size()) {
		if (buffer.capacity() >= BufferSizeHighMark) {
			BytesVec().swap(buffer);
		}
		else {
			buffer.clear();
		}
	}
	else {
		buffer.erase( buffer.begin(), buffer.begin() + processed);
	}
};

///////////////////////////////////////////////////////////
// member funcs
EClientSocket::EClientSocket( EWrapper *ptr)
	: m_pEWrapper(ptr)
	, m_clientId(-1)
	, m_connected(false)
	, m_serverVersion(0)
{
}

EClientSocket::~EClientSocket()
{
	eDisconnect();
}

int EClientSocket::serverVersion()
{
	return m_serverVersion;
}

CString EClientSocket::TwsConnectionTime()
{
	return m_TwsTime;
}

bool EClientSocket::eConnect( const char *host, UINT port, int clientId)
{
	// already connected?
	if( m_connected) {
		m_pEWrapper->error( NO_VALID_ID, ALREADY_CONNECTED.code(), ALREADY_CONNECTED.msg());
		return false;
	}

	// init sockets
	AfxSocketInit();

	// close open connection if there was one
	eDisconnect();

	// create socket
	m_pSocket.reset(new MySocket(this));
	if( !m_pSocket->Create()) {
		eDisconnect();
		m_pEWrapper->winError( (LPCTSTR) "Failed to create socket", GetLastError() );
		m_pEWrapper->error( NO_VALID_ID, FAIL_CREATE_SOCK.code(), FAIL_CREATE_SOCK.msg());
		return false;
	}

	// use local machine if no host passed in
	if( isNull(host)) {
		host = "127.0.0.1";
	}

	// connect to server
	if( !m_pSocket->Connect((LPCTSTR) host, port)) {
		int lastError = GetLastError();
		if( lastError != WSAEWOULDBLOCK && !handleSocketError(GetLastError())) {
			return false;
		}
	}

	m_clientId = clientId;

	{
		// Wait till we are fully connected (or for an error)
		CWinThread* pThread = AfxGetThread();
		while (m_pSocket.get() && !m_connected) {
			if (!pThread->PumpMessage())
				return false;
		}
	}
	return true;
}

void EClientSocket::eDisconnect()
{
	m_TwsTime.Empty();
	m_serverVersion = 0;
	m_connected = false;
	m_clientId = -1;
	m_outBuffer.clear();
	m_inBuffer.clear();
	m_pSocket.reset();
}

void EClientSocket::reqMktData(TickerId tickerId, const Contract& contract,
							   const CString& genericTicks, bool snapshot)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// not needed anymore validation
	//if( m_serverVersion < MIN_SERVER_VER_SNAPSHOT_MKT_DATA && snapshot) {
	//	m_pEWrapper->error( tickerId, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support snapshot market data requests.");
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 7;
	
	// send req mkt data msg
	ENCODE_FIELD( REQ_MKT_DATA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	// send contract fields
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.expiry);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier); // srv v15 and above

	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.primaryExchange); // srv v14 and above
	ENCODE_FIELD( contract.currency);

	ENCODE_FIELD( contract.localSymbol); // srv v2 and above

    // Send combo legs for BAG requests (srv v8 and above)
	if( contract.secType.CompareNoCase((LPCTSTR) "BAG") == 0) {
		if( !contract.comboLegs || contract.comboLegs->empty()) {
			ENCODE_FIELD( 0);
		}
		else {
			typedef Contract::ComboLegList ComboLegList;
			const ComboLegList& comboLegs = *contract.comboLegs;
			ENCODE_FIELD( (int)comboLegs.size() );
			ComboLegList::const_iterator iter = comboLegs.begin();
			const ComboLegList::const_iterator iterEnd = comboLegs.end();
			for( ; iter != iterEnd; ++iter) {
				const ComboLeg* comboLeg = *iter;
				ASSERT( comboLeg);
				ENCODE_FIELD( comboLeg->conId);
				ENCODE_FIELD( comboLeg->ratio);
				ENCODE_FIELD( comboLeg->action);
				ENCODE_FIELD( comboLeg->exchange);
			}
		}
    }

	ENCODE_FIELD( genericTicks); // srv v31 and above
	ENCODE_FIELD( snapshot); // srv v35 and above

	bufferedSend( msg.str());
}

void EClientSocket::cancelMktData(TickerId tickerId)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::ostringstream msg;

	const int VERSION = 2;
	
	// send cancel mkt data msg
	ENCODE_FIELD( CANCEL_MKT_DATA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	bufferedSend( msg.str());
}

void EClientSocket::reqMktDepth( TickerId tickerId, const Contract &contract, int numRows)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	// This feature is only available for versions of TWS >=6
	//if( m_serverVersion < 6) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 3;
	
	// send req mkt data msg
	ENCODE_FIELD( REQ_MKT_DEPTH);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	// send contract fields
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.expiry);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier); // srv v15 and above
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol);
	ENCODE_FIELD( numRows); // srv v19 and above

	bufferedSend( msg.str());
}


void EClientSocket::cancelMktDepth( TickerId tickerId)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	// This feature is only available for versions of TWS >=6
	//if( m_serverVersion < 6) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 1;
	
	// send cancel mkt data msg
	ENCODE_FIELD( CANCEL_MKT_DEPTH);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	bufferedSend( msg.str());	
}

void EClientSocket::reqHistoricalData( TickerId tickerId, const Contract &contract, 
									   const CString &endDateTime, const CString &durationStr,
									   const CString & barSizeSetting, const CString &whatToShow,
									   int useRTH, int formatDate)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 16) {
	//	m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 4;
    
	ENCODE_FIELD( REQ_HISTORICAL_DATA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	// send contract fields
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.expiry);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier);
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.primaryExchange);
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol);
	ENCODE_FIELD( contract.includeExpired); // srv v31 and above

	ENCODE_FIELD( endDateTime); // srv v20 and above
	ENCODE_FIELD( barSizeSetting); // srv v20 and above

	ENCODE_FIELD( durationStr);
	ENCODE_FIELD( useRTH);
	ENCODE_FIELD( whatToShow);
	ENCODE_FIELD( formatDate); // srv v16 and above

	if( !contract.comboLegs || contract.comboLegs->empty()) {
		ENCODE_FIELD( 0);
	}
	else {
		typedef Contract::ComboLegList ComboLegList;
		const ComboLegList& comboLegs = *contract.comboLegs;
		ENCODE_FIELD( (int)comboLegs.size());
		ComboLegList::const_iterator iter = comboLegs.begin();
		const ComboLegList::const_iterator iterEnd = comboLegs.end();
		for( ; iter != iterEnd; ++iter) {
			const ComboLeg* comboLeg = *iter;
			ASSERT( comboLeg);
			ENCODE_FIELD( comboLeg->conId);
			ENCODE_FIELD( comboLeg->ratio);
			ENCODE_FIELD( comboLeg->action);
			ENCODE_FIELD( comboLeg->exchange);
		}
	}

	bufferedSend( msg.str());
}

void EClientSocket::cancelHistoricalData(TickerId tickerId)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 24) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support historical data query cancellation.");
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 1;

	ENCODE_FIELD( CANCEL_HISTORICAL_DATA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	bufferedSend( msg.str());
}

void EClientSocket::reqRealTimeBars(TickerId tickerId, const Contract &contract,
									int barSize, const CString &whatToShow, bool useRTH)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < MIN_SERVER_VER_REAL_TIME_BARS) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support real time bars.");
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 1;

	ENCODE_FIELD( REQ_REAL_TIME_BARS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	// send contract fields
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.expiry);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier);
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.primaryExchange);
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol);
	ENCODE_FIELD( barSize);
	ENCODE_FIELD( whatToShow);
	ENCODE_FIELD( useRTH);

	bufferedSend( msg.str());
}


void EClientSocket::cancelRealTimeBars(TickerId tickerId)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < MIN_SERVER_VER_REAL_TIME_BARS) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support realtime bar data query cancellation.");
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 1;

	ENCODE_FIELD( CANCEL_REAL_TIME_BARS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	bufferedSend( msg.str());
}


void EClientSocket::reqScannerParameters()
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 24) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support API scanner subscription.");
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 1;

	ENCODE_FIELD( REQ_SCANNER_PARAMETERS);
	ENCODE_FIELD( VERSION);

	bufferedSend( msg.str());
}


void EClientSocket::reqScannerSubscription(int tickerId,
	const ScannerSubscription& subscription)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 24) {
	//	m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support API scanner subscription.");
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 3;

	ENCODE_FIELD( REQ_SCANNER_SUBSCRIPTION);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);
	ENCODE_FIELD_MAX( subscription.numberOfRows);
	ENCODE_FIELD( subscription.instrument);
	ENCODE_FIELD( subscription.locationCode);
	ENCODE_FIELD( subscription.scanCode);
	ENCODE_FIELD_MAX( subscription.abovePrice);
	ENCODE_FIELD_MAX( subscription.belowPrice);
	ENCODE_FIELD_MAX( subscription.aboveVolume);
	ENCODE_FIELD_MAX( subscription.marketCapAbove);
	ENCODE_FIELD_MAX( subscription.marketCapBelow);
	ENCODE_FIELD( subscription.moodyRatingAbove);
	ENCODE_FIELD( subscription.moodyRatingBelow);
	ENCODE_FIELD( subscription.spRatingAbove);
	ENCODE_FIELD( subscription.spRatingBelow);
	ENCODE_FIELD( subscription.maturityDateAbove);
	ENCODE_FIELD( subscription.maturityDateBelow);
	ENCODE_FIELD_MAX( subscription.couponRateAbove);
	ENCODE_FIELD_MAX( subscription.couponRateBelow);
	ENCODE_FIELD_MAX( subscription.excludeConvertible);
	ENCODE_FIELD_MAX( subscription.averageOptionVolumeAbove); // srv v25 and above
	ENCODE_FIELD( subscription.scannerSettingPairs); // srv v25 and above
	ENCODE_FIELD( subscription.stockTypeFilter); // srv v27 and above

	bufferedSend( msg.str());
}

void EClientSocket::cancelScannerSubscription(int tickerId)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( tickerId, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 24) {
	//	m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support API scanner subscription.");
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 1;

	ENCODE_FIELD( CANCEL_SCANNER_SUBSCRIPTION);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	bufferedSend( msg.str());
}

void EClientSocket::reqContractDetails(const Contract& contract)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	// This feature is only available for versions of TWS >=4
	//if( m_serverVersion < 4) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 4;
	
	// send req mkt data msg
	ENCODE_FIELD( REQ_CONTRACT_DATA);
	ENCODE_FIELD( VERSION);

	// send contract fields
	ENCODE_FIELD( contract.conId); // srv v37 and above
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.expiry);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier); // srv v15 and above
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol);
	ENCODE_FIELD( contract.includeExpired); // srv v31 and above

	bufferedSend( msg.str());
}

void EClientSocket::reqCurrentTime()
{
    // not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	// This feature is only available for versions of TWS >= 33
	//if( m_serverVersion < 33) {
	//	m_pEWrapper->error(NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//		"  It does not support current time requests.");
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 1;

	// send current time req
	ENCODE_FIELD( REQ_CURRENT_TIME);
	ENCODE_FIELD( VERSION);

	bufferedSend( msg.str());
}

void EClientSocket::placeOrder( OrderId id, const Contract &contract, const Order &order)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( id, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < MIN_SERVER_VER_SCALE_ORDERS) {
	//	if( order.scaleNumComponents != UNSET_INTEGER ||
	//		order.scaleComponentSize != UNSET_INTEGER ||
	//		order.scalePriceIncrement != UNSET_DOUBLE) {
	//		m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//			"  It does not support Scale orders.");
	//		return;
	//	}
	//}
	//
	//if( m_serverVersion < MIN_SERVER_VER_SSHORT_COMBO_LEGS) {
	//	if( contract.comboLegs && !contract.comboLegs->empty()) {
	//		typedef Contract::ComboLegList ComboLegList;
	//		const ComboLegList& comboLegs = *contract.comboLegs;
	//		ComboLegList::const_iterator iter = comboLegs.begin();
	//		const ComboLegList::const_iterator iterEnd = comboLegs.end();
	//		for( ; iter != iterEnd; ++iter) {
	//			const ComboLeg* comboLeg = *iter;
	//			ASSERT( comboLeg);
	//			if( comboLeg->shortSaleSlot != 0 ||
	//				!comboLeg->designatedLocation.IsEmpty()) {
	//				m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//					"  It does not support SSHORT flag for combo legs.");
	//				return;
	//			}
	//		}
	//	}
	//}
	//
	//if( m_serverVersion < MIN_SERVER_VER_WHAT_IF_ORDERS) {
	//	if( order.whatIf) {
	//		m_pEWrapper->error( id, UPDATE_TWS.code(), UPDATE_TWS.msg() +
	//			"  It does not support what-if orders.");
	//		return;
	//	}
	//}

	std::ostringstream msg;

	const int VERSION = 25;

	// send place order msg
	ENCODE_FIELD( PLACE_ORDER);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( id);

	// send contract fields
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.expiry);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier); // srv v15 and above
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.primaryExchange); // srv v14 and above
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol); // srv v2 and above

	// send main order fields
	ENCODE_FIELD( order.action);
	ENCODE_FIELD( order.totalQuantity);
	ENCODE_FIELD( order.orderType);
	ENCODE_FIELD( order.lmtPrice);
	ENCODE_FIELD( order.auxPrice);

	// send extended order fields
	ENCODE_FIELD( order.tif);
	ENCODE_FIELD( order.ocaGroup);
	ENCODE_FIELD( order.account);
	ENCODE_FIELD( order.openClose);
	ENCODE_FIELD( order.origin);
	ENCODE_FIELD( order.orderRef);
	ENCODE_FIELD( order.transmit);
	ENCODE_FIELD( order.parentId); // srv v4 and above

	ENCODE_FIELD( order.blockOrder); // srv v5 and above
	ENCODE_FIELD( order.sweepToFill); // srv v5 and above
	ENCODE_FIELD( order.displaySize); // srv v5 and above
	ENCODE_FIELD( order.triggerMethod); // srv v5 and above

	//if (m_serverVersion < 38) {
	// will never happen
	//	ENCODE_FIELD(/* order.ignoreRth */ false);
	//}
	//else {
		ENCODE_FIELD( order.outsideRth); // srv v5 and above
	//}

	ENCODE_FIELD( order.hidden); // srv v7 and above

	// Send combo legs for BAG requests (srv v8 and above)
	if( /* m_serverVersion >= 8 && */ contract.secType.CompareNoCase("BAG") == 0) {
		if( !contract.comboLegs || contract.comboLegs->empty()) {
			ENCODE_FIELD( 0);
		}
		else {
			typedef Contract::ComboLegList ComboLegList;
			const ComboLegList& comboLegs = *contract.comboLegs;
			ENCODE_FIELD( (int)comboLegs.size());
			ComboLegList::const_iterator iter = comboLegs.begin();
			const ComboLegList::const_iterator iterEnd = comboLegs.end();
			for( ; iter != iterEnd; ++iter) {
				const ComboLeg* comboLeg = *iter;
				ASSERT( comboLeg);
				ENCODE_FIELD( comboLeg->conId);
				ENCODE_FIELD( comboLeg->ratio);
				ENCODE_FIELD( comboLeg->action);
				ENCODE_FIELD( comboLeg->exchange);
				ENCODE_FIELD( comboLeg->openClose);

				ENCODE_FIELD( comboLeg->shortSaleSlot); // srv v35 and above
				ENCODE_FIELD( comboLeg->designatedLocation); // srv v35 and above
            }
        }
    }

	/////////////////////////////////////////////////////////////////////////////
	// Send the shares allocation.
	//
	// This specifies the number of order shares allocated to each Financial
	// Advisor managed account. The format of the allocation string is as
	// follows:              
	//			<account_code1>/<number_shares1>,<account_code2>/<number_shares2>,...N
	// E.g.
	//		To allocate 20 shares of a 100 share order to account 'U101' and the 
	//      residual 80 to account 'U203' enter the following share allocation string:
	//          U101/20,U203/80
	/////////////////////////////////////////////////////////////////////////////
	{
		// send deprecated sharesAllocation field
		ENCODE_FIELD( ""); // srv v9 and above
	}

	ENCODE_FIELD( order.discretionaryAmt); // srv v10 and above
	ENCODE_FIELD( order.goodAfterTime); // srv v11 and above
	ENCODE_FIELD( order.goodTillDate); // srv v12 and above

	ENCODE_FIELD( order.faGroup); // srv v13 and above
	ENCODE_FIELD( order.faMethod); // srv v13 and above
	ENCODE_FIELD( order.faPercentage); // srv v13 and above
	ENCODE_FIELD( order.faProfile); // srv v13 and above

	// institutional short saleslot data (srv v18 and above)
	ENCODE_FIELD( order.shortSaleSlot);      // 0 for retail, 1 or 2 for institutions
	ENCODE_FIELD( order.designatedLocation); // populate only when shortSaleSlot = 2.

	// not needed anymore
	//bool isVolOrder = (order.orderType.CompareNoCase("VOL") == 0);

	// srv v19 and above fields
	ENCODE_FIELD( order.ocaType); 
	//if (m_serverVersion < 38) {
	// will never happen
	//	send( /* order.rthOnly */ false); 
	//}
	ENCODE_FIELD( order.rule80A);
	ENCODE_FIELD( order.settlingFirm);
	ENCODE_FIELD( order.allOrNone);
	ENCODE_FIELD_MAX( order.minQty);
	ENCODE_FIELD_MAX( order.percentOffset);
	ENCODE_FIELD( order.eTradeOnly);
	ENCODE_FIELD( order.firmQuoteOnly);
	ENCODE_FIELD_MAX( order.nbboPriceCap);
	ENCODE_FIELD( order.auctionStrategy); // AUCTION_MATCH, AUCTION_IMPROVEMENT, AUCTION_TRANSPARENT
	ENCODE_FIELD_MAX( order.startingPrice);
	ENCODE_FIELD_MAX( order.stockRefPrice);
	ENCODE_FIELD_MAX( order.delta);
	// Volatility orders had specific watermark price attribs in server version 26
	//double lower = (m_serverVersion == 26 && isVolOrder) ? DBL_MAX : order.stockRangeLower;
	//double upper = (m_serverVersion == 26 && isVolOrder) ? DBL_MAX : order.stockRangeUpper;
	ENCODE_FIELD_MAX( order.stockRangeLower);
	ENCODE_FIELD_MAX( order.stockRangeLower);

	ENCODE_FIELD( order.overridePercentageConstraints); // srv v22 and above

	// Volatility orders (srv v26 and above)
	ENCODE_FIELD_MAX( order.volatility);
	ENCODE_FIELD_MAX( order.volatilityType);
	// will never happen
	//if (m_serverVersion < 28) {
	//	send( order.deltaNeutralOrderType.CompareNoCase("MKT") == 0);
	//}
	//else {
		ENCODE_FIELD( order.deltaNeutralOrderType); // srv v28 and above
		ENCODE_FIELD_MAX( order.deltaNeutralAuxPrice); // srv v28 and above
	//}
	ENCODE_FIELD( order.continuousUpdate);
	//if (m_serverVersion == 26) {
	//	// Volatility orders had specific watermark price attribs in server version 26
	//	double lower = (isVolOrder ? order.stockRangeLower : DBL_MAX);
	//	double upper = (isVolOrder ? order.stockRangeUpper : DBL_MAX);
	//	ENCODE_FIELD_MAX( lower);
	//	ENCODE_FIELD_MAX( upper);
	//}
	ENCODE_FIELD_MAX( order.referencePriceType);

	ENCODE_FIELD_MAX( order.trailStopPrice); // srv v30 and above

	// SCALE orders (srv v35 and above)
	ENCODE_FIELD_MAX( order.scaleNumComponents);
	ENCODE_FIELD_MAX( order.scaleComponentSize);
	ENCODE_FIELD_MAX( order.scalePriceIncrement);

	if (m_serverVersion >= MIN_SERVER_VER_PTA_ORDERS) {
		ENCODE_FIELD( order.clearingAccount);
		ENCODE_FIELD( order.clearingIntent);
	}

	ENCODE_FIELD( order.whatIf); // srv v36 and above

	bufferedSend( msg.str());
}

void EClientSocket::cancelOrder( OrderId id)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( id, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	const int VERSION = 1;

	// send cancel order msg
	std::ostringstream msg;

	ENCODE_FIELD( CANCEL_ORDER);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( id);
	
	bufferedSend( msg.str());
}

void EClientSocket::reqAccountUpdates(bool subscribe, const CString& acctCode)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::ostringstream msg;

	const int VERSION = 2;
	
	// send req acct msg
	ENCODE_FIELD( REQ_ACCT_DATA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( subscribe);  // TRUE = subscribe, FALSE = unsubscribe.

	// Send the account code. This will only be used for FA clients
	ENCODE_FIELD( acctCode); // srv v9 and above

	bufferedSend( msg.str());
}

void EClientSocket::reqOpenOrders()
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::ostringstream msg;

	const int VERSION = 1;

	// send req open orders msg
	ENCODE_FIELD( REQ_OPEN_ORDERS);
	ENCODE_FIELD( VERSION);

	bufferedSend( msg.str());
}

void EClientSocket::reqAutoOpenOrders(bool bAutoBind)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::ostringstream msg;

	const int VERSION = 1;

	// send req open orders msg
	ENCODE_FIELD( REQ_AUTO_OPEN_ORDERS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( bAutoBind);

	bufferedSend( msg.str());
}

void EClientSocket::reqAllOpenOrders()
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::ostringstream msg;

	const int VERSION = 1;

	// send req open orders msg
	ENCODE_FIELD( REQ_ALL_OPEN_ORDERS);
	ENCODE_FIELD( VERSION);

	bufferedSend( msg.str());
}

void EClientSocket::reqExecutions(const ExecutionFilter& filter) 
{
	//NOTE: Time format must be 'yyyymmdd-hh:mm:ss' E.g. '20030702-14:55'

	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::ostringstream msg;

	const int VERSION = 2;

	// send req open orders msg
	ENCODE_FIELD( REQ_EXECUTIONS);
	ENCODE_FIELD( VERSION);

	// Send the execution rpt filter data (srv v9 and above)
	ENCODE_FIELD( filter.m_clientId);
	ENCODE_FIELD( filter.m_acctCode);
	ENCODE_FIELD( filter.m_time);
	ENCODE_FIELD( filter.m_symbol);
	ENCODE_FIELD( filter.m_secType);
	ENCODE_FIELD( filter.m_exchange);
	ENCODE_FIELD( filter.m_side);

	bufferedSend( msg.str());
}

void EClientSocket::reqIds( int numIds)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( numIds, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::ostringstream msg;

	const int VERSION = 1;

	// send req open orders msg
	ENCODE_FIELD( REQ_IDS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( numIds);

	bufferedSend( msg.str());
}

void EClientSocket::reqNewsBulletins(bool allMsgs)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::ostringstream msg;

	const int VERSION = 1;
	
	// send req news bulletins msg
	ENCODE_FIELD( REQ_NEWS_BULLETINS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( allMsgs);

	bufferedSend( msg.str());
}

void EClientSocket::cancelNewsBulletins()
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::ostringstream msg;

	const int VERSION = 1;
	
	// send req news bulletins msg
	ENCODE_FIELD( CANCEL_NEWS_BULLETINS);
	ENCODE_FIELD( VERSION);

	bufferedSend( msg.str());
}

void EClientSocket::setServerLogLevel(int logLevel)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::ostringstream msg;

	const int VERSION = 1;
	
	// send the set server logging level message
	ENCODE_FIELD( SET_SERVER_LOGLEVEL);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( logLevel);

	bufferedSend( msg.str());
}

void EClientSocket::reqManagedAccts()
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	std::ostringstream msg;

	const int VERSION = 1;
	
	// send req FA managed accounts msg
	ENCODE_FIELD( REQ_MANAGED_ACCTS);
	ENCODE_FIELD( VERSION);

	bufferedSend( msg.str());
}


void EClientSocket::requestFA(faDataType pFaDataType)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 13) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 1;
	
	ENCODE_FIELD( REQ_FA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( (int)pFaDataType);

	bufferedSend( msg.str());
}

void EClientSocket::replaceFA(faDataType pFaDataType, const CString& cxml)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if ( m_serverVersion < 13) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 1;

	ENCODE_FIELD( REPLACE_FA);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( (int)pFaDataType);
	ENCODE_FIELD( cxml);

	bufferedSend( msg.str());
}



void EClientSocket::exerciseOptions( TickerId tickerId, const Contract &contract,
                                     int exerciseAction, int exerciseQuantity,
                                     const CString& account, int override)
{
	// not connected?
	if( !m_connected) {
		m_pEWrapper->error( NO_VALID_ID, NOT_CONNECTED.code(), NOT_CONNECTED.msg());
		return;
	}

	// Not needed anymore validation
	//if( m_serverVersion < 21) {
	//	m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
	//	return;
	//}

	std::ostringstream msg;

	const int VERSION = 1;

	ENCODE_FIELD( EXERCISE_OPTIONS);
	ENCODE_FIELD( VERSION);
	ENCODE_FIELD( tickerId);

	// send contract fields
	ENCODE_FIELD( contract.symbol);
	ENCODE_FIELD( contract.secType);
	ENCODE_FIELD( contract.expiry);
	ENCODE_FIELD( contract.strike);
	ENCODE_FIELD( contract.right);
	ENCODE_FIELD( contract.multiplier);
	ENCODE_FIELD( contract.exchange);
	ENCODE_FIELD( contract.currency);
	ENCODE_FIELD( contract.localSymbol);
	ENCODE_FIELD( exerciseAction);
	ENCODE_FIELD( exerciseQuantity);
	ENCODE_FIELD( account);
	ENCODE_FIELD( override);

	bufferedSend( msg.str());
}

int EClientSocket::send(const char* buf, size_t sz)
{
	if( sz <= 0)
		return 0;

	int nResult = m_pSocket->Send( buf, sz);
	if( nResult == SOCKET_ERROR && !handleSocketError( GetLastError())) {
		return -1;
	}
	if( nResult <= 0) {
		return 0;
	}
	return nResult;
}

int EClientSocket::sendBufferedData()
{
	if( m_outBuffer.empty())
		return 0;

	int nResult = send( &m_outBuffer[0], m_outBuffer.size());
	if( nResult <= 0) {
		return nResult;
	}
	CleanupBuffer( m_outBuffer, nResult);
	return nResult;
}

int EClientSocket::bufferedSend(const char* buf, size_t sz)
{
	if( sz <= 0)
		return 0;

	if( !m_outBuffer.empty()) {
		m_outBuffer.insert( m_outBuffer.end(), buf, buf + sz);
		return sendBufferedData();
	}

	int nResult = send(buf, sz);
	if( nResult < (int)sz) {
		int sent = max( nResult, 0);
		m_outBuffer.insert( m_outBuffer.end(), buf + sent, buf + sz);
	}

	return nResult;
}

int EClientSocket::bufferedSend(const std::string& msg)
{
	return bufferedSend( msg.data(), msg.size());
}

int EClientSocket::bufferedRead()
{
	char buf[8192];
	int nResult = m_pSocket->Receive( buf, sizeof(buf));
	if( nResult == SOCKET_ERROR && !handleSocketError( GetLastError())) {
		return -1;
	}
	if( nResult <= 0) {
		return 0;
	}
	m_inBuffer.insert( m_inBuffer.end(), &buf[0], &buf[0] + nResult);
	return nResult;
}

void EClientSocket::checkMessages()
{
	if( !m_pSocket.get())
		return;

	if( bufferedRead() <= 0) {;
		return;
	}

	const char* beginPtr = &m_inBuffer[0];
	const char*	ptr = beginPtr;
	const char*	endPtr = ptr + m_inBuffer.size();

	try {
		while( (m_connected ? processMsg( ptr, endPtr)
			                : processConnectAck( ptr, endPtr)) > 0) {
			if( (ptr - beginPtr) >= (int)m_inBuffer.size())
				break;
		}
	}
	catch (...) {

		CleanupBuffer( m_inBuffer, (ptr - beginPtr));
		throw;
	}

	CleanupBuffer( m_inBuffer, (ptr - beginPtr));
}

int EClientSocket::processConnectAck(const char*& beginPtr, const char* endPtr)
{
	// process a connect Ack message from the buffer;
	// return number of bytes consumed

	ASSERT( beginPtr && beginPtr < endPtr);

	TRY{

		const char* ptr = beginPtr;

		// check server version
		DECODE_FIELD( m_serverVersion);
		if( m_serverVersion >= 20) {
			DECODE_FIELD( m_TwsTime);
		}

		if( m_serverVersion < SERVER_VERSION) {
			eDisconnect();
			m_pEWrapper->error( NO_VALID_ID, UPDATE_TWS.code(), UPDATE_TWS.msg());
			return -1;
		}

		// send the clientId
		if( m_serverVersion >= 3) {
			std::ostringstream msg;
			ENCODE_FIELD( m_clientId);
			bufferedSend( msg.str());
		}

		m_connected = true;

		// That would be the place to notify client
		// that we are fully connected
		// e.g: m_pEWrapper->connected();

		int processed = ptr - beginPtr;
		beginPtr = ptr;
		return processed;
	}
	CATCH( CException, e) {
		m_pEWrapper->error( NO_VALID_ID, SOCKET_EXCEPTION.code(),
			SOCKET_EXCEPTION.msg() + errMsg( e) );
	}
	END_CATCH
	return 0;
}

int EClientSocket::processMsg(const char*& beginPtr, const char* endPtr)
{
	// process a single message from the buffer;
	// return number of bytes consumed

	ASSERT( beginPtr && beginPtr < endPtr);

    TRY {

		const char* ptr = beginPtr;

	    int msgId;
		DECODE_FIELD( msgId);

		switch( msgId) {
			case TICK_PRICE:
			{
				int version;
				int tickerId;
				int tickTypeInt;
				double price;

				int size;
				int canAutoExecute;

				DECODE_FIELD( version);
				DECODE_FIELD( tickerId);
				DECODE_FIELD( tickTypeInt);
				DECODE_FIELD( price);

				DECODE_FIELD( size); // ver 2 field
				DECODE_FIELD( canAutoExecute); // ver 3 field

				m_pEWrapper->tickPrice( tickerId, (TickType)tickTypeInt, price, canAutoExecute);

				// process ver 2 fields
				{
					TickType sizeTickType = NOT_SET;
					switch( (TickType)tickTypeInt) {
						case BID: 
							sizeTickType = BID_SIZE;
							break;
						case ASK: 
							sizeTickType = ASK_SIZE;
							break;
						case LAST:
							sizeTickType = LAST_SIZE;
						break;
					}
					if( sizeTickType != NOT_SET)
						m_pEWrapper->tickSize( tickerId, sizeTickType, size);
				}

				break;
			}

			case TICK_SIZE:
			{
				int version;
				int tickerId;
				int tickTypeInt;
				int size;

				DECODE_FIELD( version);
				DECODE_FIELD( tickerId);
				DECODE_FIELD( tickTypeInt);
				DECODE_FIELD( size);
			    
				m_pEWrapper->tickSize( tickerId, (TickType)tickTypeInt, size);
				break;
		    }

			case TICK_OPTION_COMPUTATION:
			{
				int version;
				int tickerId;
				int tickTypeInt;
				double impliedVol;
				double delta;

				double modelPrice = DBL_MAX;
				double pvDividend = DBL_MAX;

				DECODE_FIELD( version);
				DECODE_FIELD( tickerId);
				DECODE_FIELD( tickTypeInt);

				DECODE_FIELD( impliedVol);
				DECODE_FIELD( delta);

				if( impliedVol < 0) { // -1 is the "not computed" indicator
					impliedVol = DBL_MAX;
				}
				if( delta > 1 || delta < -1) { // -2 is the "not computed" indicator
					delta = DBL_MAX;
				}

				if( tickTypeInt == MODEL_OPTION) { // introduced in version == 5

					DECODE_FIELD( modelPrice);
					DECODE_FIELD( pvDividend);

					if( modelPrice < 0) { // -1 is the "not computed" indicator
                    	modelPrice = DBL_MAX;
					}
					if( pvDividend < 0) { // -1 is the "not computed" indicator
						pvDividend = DBL_MAX;
					}
				}

				m_pEWrapper->tickOptionComputation( tickerId, (TickType)tickTypeInt,
					impliedVol, delta, modelPrice, pvDividend);

				break;
            }

            case TICK_GENERIC:
			{
				int version;
				int tickerId;
				int tickTypeInt;
				double value;

				DECODE_FIELD( version);
				DECODE_FIELD( tickerId);
				DECODE_FIELD( tickTypeInt);
				DECODE_FIELD( value);

				m_pEWrapper->tickGeneric( tickerId, (TickType)tickTypeInt, value);
				break;
			}

			case TICK_STRING:
			{
				int version;
				int tickerId;
				int tickTypeInt;
				CString value;

				DECODE_FIELD( version);
				DECODE_FIELD( tickerId);
				DECODE_FIELD( tickTypeInt);
				DECODE_FIELD( value);

				m_pEWrapper->tickString( tickerId, (TickType)tickTypeInt, value);
				break;
			}

			case TICK_EFP:
			{
				int version;
                int tickerId;
                int tickTypeInt;
                double basisPoints;
                CString formattedBasisPoints;
                double impliedFuturesPrice;
                int holdDays;
                CString futureExpiry;
                double dividendImpact;
                double dividendsToExpiry;

				DECODE_FIELD( version);
				DECODE_FIELD( tickerId);
				DECODE_FIELD( tickTypeInt);
				DECODE_FIELD( basisPoints);
				DECODE_FIELD( formattedBasisPoints);
				DECODE_FIELD( impliedFuturesPrice);
				DECODE_FIELD( holdDays);
				DECODE_FIELD( futureExpiry);
				DECODE_FIELD( dividendImpact);
				DECODE_FIELD( dividendsToExpiry);

				m_pEWrapper->tickEFP( tickerId, (TickType)tickTypeInt, basisPoints, formattedBasisPoints,
					impliedFuturesPrice, holdDays, futureExpiry, dividendImpact, dividendsToExpiry);
				break;
			}

			case ORDER_STATUS:
			{
				int version;
				int orderId;
				CString status;
				int filled;
				int remaining;
				double avgFillPrice;
				int permId;
				int parentId;
				double lastFillPrice;
				int clientId;
				CString whyHeld;

				DECODE_FIELD( version);
				DECODE_FIELD( orderId);
				DECODE_FIELD( status);
				DECODE_FIELD( filled);
				DECODE_FIELD( remaining);
				DECODE_FIELD( avgFillPrice);

				DECODE_FIELD( permId); // ver 2 field
				DECODE_FIELD( parentId); // ver 3 field
				DECODE_FIELD( lastFillPrice); // ver 4 field
				DECODE_FIELD( clientId); // ver 5 field
				DECODE_FIELD( whyHeld); // ver 6 field

				m_pEWrapper->orderStatus( orderId, status, filled, remaining,
					avgFillPrice, permId, parentId, lastFillPrice, clientId, whyHeld);

				break;
			}

			case ERR_MSG:
			{
				int version;
				int id; // ver 2 field
				int errorCode; // ver 2 field
				CString errorMsg;

				DECODE_FIELD( version);
				DECODE_FIELD( id);
				DECODE_FIELD( errorCode);
				DECODE_FIELD( errorMsg);
			
				m_pEWrapper->error( id, errorCode, errorMsg);
				break;
			}

			case OPEN_ORDER:
			{
				// read version
				int version;
				DECODE_FIELD( version);

				// read order id
				Order order;
				DECODE_FIELD( order.orderId);

				// read contract fields
				Contract contract;
				DECODE_FIELD( contract.conId); // ver 17 field
				DECODE_FIELD( contract.symbol);
				DECODE_FIELD( contract.secType);
				DECODE_FIELD( contract.expiry);
				DECODE_FIELD( contract.strike);
				DECODE_FIELD( contract.right);
				DECODE_FIELD( contract.exchange);
				DECODE_FIELD( contract.currency);
				DECODE_FIELD( contract.localSymbol); // ver 2 field

				// read order fields
				DECODE_FIELD( order.action);
				DECODE_FIELD( order.totalQuantity);
				DECODE_FIELD( order.orderType);
				DECODE_FIELD( order.lmtPrice);
				DECODE_FIELD( order.auxPrice);
				DECODE_FIELD( order.tif);
				DECODE_FIELD( order.ocaGroup);
				DECODE_FIELD( order.account);
				DECODE_FIELD( order.openClose);

				int orderOriginInt;
				DECODE_FIELD( orderOriginInt);
				order.origin = (Origin)orderOriginInt;
				
				DECODE_FIELD( order.orderRef);
				DECODE_FIELD( order.clientId); // ver 3 field
				DECODE_FIELD( order.permId); // ver 4 field
			    
				//if (version < 18) {
				//	// will never happen
				//	/* order.ignoreRth = */ readBoolFromInt();
				//}

				DECODE_FIELD( order.outsideRth); // ver 18 field
				DECODE_FIELD( order.hidden); // ver 4 field
				DECODE_FIELD( order.discretionaryAmt); // ver 4 field
				DECODE_FIELD( order.goodAfterTime); // ver 5 field

				{
					CString sharesAllocation;
					DECODE_FIELD( sharesAllocation); // deprecated ver 6 field
				}

				DECODE_FIELD( order.faGroup); // ver 7 field
				DECODE_FIELD( order.faMethod); // ver 7 field
				DECODE_FIELD( order.faPercentage); // ver 7 field
				DECODE_FIELD( order.faProfile); // ver 7 field

				DECODE_FIELD( order.goodTillDate); // ver 8 field

				DECODE_FIELD( order.rule80A); // ver 9 field
				DECODE_FIELD( order.percentOffset); // ver 9 field
				DECODE_FIELD( order.settlingFirm); // ver 9 field
				DECODE_FIELD( order.shortSaleSlot); // ver 9 field
				DECODE_FIELD( order.designatedLocation); // ver 9 field
				DECODE_FIELD( order.auctionStrategy); // ver 9 field
				DECODE_FIELD( order.startingPrice); // ver 9 field
				DECODE_FIELD( order.stockRefPrice); // ver 9 field
				DECODE_FIELD( order.delta); // ver 9 field
				DECODE_FIELD( order.stockRangeLower); // ver 9 field
				DECODE_FIELD( order.stockRangeUpper); // ver 9 field
				DECODE_FIELD( order.displaySize); // ver 9 field

				//if (version < 18) {
				//		// will never happen
				//		/* order.rthOnly = */ readBoolFromInt();
				//}

				DECODE_FIELD( order.blockOrder); // ver 9 field
				DECODE_FIELD( order.sweepToFill); // ver 9 field
				DECODE_FIELD( order.allOrNone); // ver 9 field
				DECODE_FIELD( order.minQty); // ver 9 field
				DECODE_FIELD( order.ocaType); // ver 9 field
				DECODE_FIELD( order.eTradeOnly); // ver 9 field
				DECODE_FIELD( order.firmQuoteOnly); // ver 9 field
				DECODE_FIELD( order.nbboPriceCap); // ver 9 field

				DECODE_FIELD( order.parentId); // ver 10 field
				DECODE_FIELD( order.triggerMethod); // ver 10 field

				DECODE_FIELD( order.volatility); // ver 11 field
				DECODE_FIELD( order.volatilityType); // ver 11 field
				DECODE_FIELD( order.deltaNeutralOrderType); // ver 11 field (had a hack for ver 11)
				DECODE_FIELD( order.deltaNeutralAuxPrice); // ver 12 field
				DECODE_FIELD( order.continuousUpdate); // ver 11 field

				// will never happen
				//if (m_serverVersion == 26) {
				//	order.stockRangeLower = readDouble();
				//	order.stockRangeUpper = readDouble();
				//}

				DECODE_FIELD( order.referencePriceType); // ver 11 field

				DECODE_FIELD( order.trailStopPrice); // ver 13 field

				DECODE_FIELD( order.basisPoints); // ver 14 field
				DECODE_FIELD( order.basisPointsType); // ver 14 field
				DECODE_FIELD( contract.comboLegsDescrip); // ver 14 field

				DECODE_FIELD_MAX( order.scaleNumComponents); // ver 15 field
				DECODE_FIELD_MAX( order.scaleComponentSize); // ver 15 field
				DECODE_FIELD_MAX( order.scalePriceIncrement); // ver 15 field

				DECODE_FIELD( order.clearingAccount); // ver 19 field
				DECODE_FIELD( order.clearingIntent); // ver 19 field

				OrderState orderState;

				DECODE_FIELD( order.whatIf); // ver 16 field

				DECODE_FIELD( orderState.status); // ver 16 field
				DECODE_FIELD( orderState.initMargin); // ver 16 field
				DECODE_FIELD( orderState.maintMargin); // ver 16 field
				DECODE_FIELD( orderState.equityWithLoan); // ver 16 field
				DECODE_FIELD_MAX( orderState.commission); // ver 16 field
				DECODE_FIELD_MAX( orderState.minCommission); // ver 16 field
				DECODE_FIELD_MAX( orderState.maxCommission); // ver 16 field
				DECODE_FIELD( orderState.commissionCurrency); // ver 16 field
				DECODE_FIELD( orderState.warningText); // ver 16 field

				m_pEWrapper->openOrder( (OrderId)order.orderId, contract, order, orderState);
				break;
			}

			case ACCT_VALUE:
			{
				int version;
				CString key;
				CString val;
				CString cur;
				CString accountName;

				DECODE_FIELD( version);
				DECODE_FIELD( key);
				DECODE_FIELD( val);
				DECODE_FIELD( cur);
				DECODE_FIELD( accountName); // ver 2 field
				
				m_pEWrapper->updateAccountValue( key, val, cur, accountName);			
				break;
			}

			case PORTFOLIO_VALUE:
			{
				// decode version
				int version;
				DECODE_FIELD( version);

				// read contract fields
				Contract contract;
				DECODE_FIELD( contract.conId); // ver 6 field
				DECODE_FIELD( contract.symbol);
				DECODE_FIELD( contract.secType);
				DECODE_FIELD( contract.expiry);
				DECODE_FIELD( contract.strike);
				DECODE_FIELD( contract.right);
				DECODE_FIELD( contract.currency);
				DECODE_FIELD( contract.localSymbol); // ver 2 field

				int     position;
				double  marketPrice;  
				double  marketValue;
				double  averageCost;
				double  unrealizedPNL;
				double  realizedPNL;

				DECODE_FIELD( position);
				DECODE_FIELD( marketPrice);
				DECODE_FIELD( marketValue);
				DECODE_FIELD( averageCost); // ver 3 field
				DECODE_FIELD( unrealizedPNL); // ver 3 field
				DECODE_FIELD( realizedPNL); // ver 3 field

				CString accountName;
				DECODE_FIELD( accountName); // ver 4 field
			             
				m_pEWrapper->updatePortfolio( contract,
					position, marketPrice, marketValue, averageCost,
					unrealizedPNL, realizedPNL, accountName);

				break;
			}

			case ACCT_UPDATE_TIME:
			{
				int version;
				CString accountTime;

				DECODE_FIELD( version);
				DECODE_FIELD( accountTime);

				m_pEWrapper->updateAccountTime( accountTime);
				break;
			}

			case NEXT_VALID_ID:
			{
				int version;
				int orderId;

				DECODE_FIELD( version);
				DECODE_FIELD( orderId);

				m_pEWrapper->nextValidId(orderId);
				break;
			}

			case CONTRACT_DATA:
			{
				int version;
				DECODE_FIELD( version);

				ContractDetails contract;
				DECODE_FIELD( contract.summary.symbol);
				DECODE_FIELD( contract.summary.secType);
				DECODE_FIELD( contract.summary.expiry);
				DECODE_FIELD( contract.summary.strike);
				DECODE_FIELD( contract.summary.right);
				DECODE_FIELD( contract.summary.exchange);
				DECODE_FIELD( contract.summary.currency);
				DECODE_FIELD( contract.summary.localSymbol);
				DECODE_FIELD( contract.marketName);
				DECODE_FIELD( contract.tradingClass);
				DECODE_FIELD( contract.summary.conId);
				DECODE_FIELD( contract.minTick);
				DECODE_FIELD( contract.summary.multiplier);
				DECODE_FIELD( contract.orderTypes);
				DECODE_FIELD( contract.validExchanges);
				DECODE_FIELD( contract.priceMagnifier); // ver 2 field

				m_pEWrapper->contractDetails( contract);
				break;
			}

			case BOND_CONTRACT_DATA:
			{
				int version;
				DECODE_FIELD( version);

				ContractDetails contract;
				DECODE_FIELD( contract.summary.symbol);
				DECODE_FIELD( contract.summary.secType);
				DECODE_FIELD( contract.cusip);
				DECODE_FIELD( contract.coupon);
				DECODE_FIELD( contract.maturity);
				DECODE_FIELD( contract.issueDate);
				DECODE_FIELD( contract.ratings);
				DECODE_FIELD( contract.bondType);
				DECODE_FIELD( contract.couponType);
				DECODE_FIELD( contract.convertible);
				DECODE_FIELD( contract.callable);
				DECODE_FIELD( contract.putable);
				DECODE_FIELD( contract.descAppend);
				DECODE_FIELD( contract.summary.exchange);
				DECODE_FIELD( contract.summary.currency);
				DECODE_FIELD( contract.marketName);
				DECODE_FIELD( contract.tradingClass);
				DECODE_FIELD( contract.summary.conId);
				DECODE_FIELD( contract.minTick);
				DECODE_FIELD( contract.orderTypes);
				DECODE_FIELD( contract.validExchanges);
				DECODE_FIELD( contract.nextOptionDate); // ver 2 field
				DECODE_FIELD( contract.nextOptionType); // ver 2 field
				DECODE_FIELD( contract.nextOptionPartial); // ver 2 field
				DECODE_FIELD( contract.notes); // ver 2 field

				m_pEWrapper->bondContractDetails(contract);
				break;
            }

			case EXECUTION_DATA:
			{
				int version;
				DECODE_FIELD( version);

				int id;
				DECODE_FIELD( id);

				// decode contract fields
				Contract contract;
				DECODE_FIELD( contract.conId); // ver 5 field
				DECODE_FIELD( contract.symbol);
				DECODE_FIELD( contract.secType);
				DECODE_FIELD( contract.expiry);
				DECODE_FIELD( contract.strike);
				DECODE_FIELD( contract.right);
				DECODE_FIELD( contract.exchange);
				DECODE_FIELD( contract.currency);
				DECODE_FIELD( contract.localSymbol);

				// decode execution fields
				Execution exec;
				exec.orderId = id;
				DECODE_FIELD( exec.execId);
				DECODE_FIELD( exec.time);
				DECODE_FIELD( exec.acctNumber);
				DECODE_FIELD( exec.exchange);
				DECODE_FIELD( exec.side);
				DECODE_FIELD( exec.shares);
				DECODE_FIELD( exec.price);
				DECODE_FIELD( exec.permId); // ver 2 field
				DECODE_FIELD( exec.clientId); // ver 3 field
				DECODE_FIELD( exec.liquidation); // ver 4 field

				m_pEWrapper->execDetails( id, contract, exec);
				break;
			}

			case MARKET_DEPTH:
			{
				int version;
				int id;
				int position;
				int operation;
				int side;
				double price;
				int size;

				DECODE_FIELD( version);
				DECODE_FIELD( id);
				DECODE_FIELD( position);
				DECODE_FIELD( operation);
				DECODE_FIELD( side);
				DECODE_FIELD( price);
				DECODE_FIELD( size);

				m_pEWrapper->updateMktDepth( id, position, operation, side, price, size);
				break;
			}

			case MARKET_DEPTH_L2:
			{
				int version;
				int id;
				int position;
				CString marketMaker;
				int operation;
				int side;
				double price;
				int size;

				DECODE_FIELD( version);
				DECODE_FIELD( id);
				DECODE_FIELD( position);
				DECODE_FIELD( marketMaker);
				DECODE_FIELD( operation);
				DECODE_FIELD( side);
				DECODE_FIELD( price);
				DECODE_FIELD( size);

				m_pEWrapper->updateMktDepthL2( id, position, marketMaker, operation, side,
					price, size);

				break;
			}

			case NEWS_BULLETINS:
			{
				int version;
				int msgId;
				int msgType;
				CString newsMessage;
				CString originatingExch;

				DECODE_FIELD( version);
				DECODE_FIELD( msgId);
				DECODE_FIELD( msgType);
				DECODE_FIELD( newsMessage);
				DECODE_FIELD( originatingExch);

				m_pEWrapper->updateNewsBulletin( msgId, msgType, newsMessage, originatingExch);
				break;
			}

			case MANAGED_ACCTS:
			{
				int version;
				CString accountsList;

				DECODE_FIELD( version);
				DECODE_FIELD( accountsList);

				m_pEWrapper->managedAccounts( accountsList);
				break;
			}

			case RECEIVE_FA:
			{
				int version;
				int faDataTypeInt;
				CString cxml;

				DECODE_FIELD( version);
				DECODE_FIELD( faDataTypeInt);
				DECODE_FIELD( cxml);

				m_pEWrapper->receiveFA( (faDataType)faDataTypeInt, cxml);
				break;
			}

			case HISTORICAL_DATA:
			{
				int version;
				int reqId;
				CString startDateStr;
				CString endDateStr;

				DECODE_FIELD( version);
				DECODE_FIELD( reqId);
				DECODE_FIELD( startDateStr); // ver 2 field
				DECODE_FIELD( endDateStr); // ver 2 field

                int itemCount;
				DECODE_FIELD( itemCount);

				typedef std::vector<BarData> BarDataList;
				BarDataList bars;

				bars.reserve( itemCount);

				for( int ctr = 0; ctr < itemCount; ++ctr) {

					BarData bar;
					DECODE_FIELD( bar.date);
					DECODE_FIELD( bar.open);
					DECODE_FIELD( bar.high);
					DECODE_FIELD( bar.low);
					DECODE_FIELD( bar.close);
					DECODE_FIELD( bar.volume);
					DECODE_FIELD( bar.average);
					DECODE_FIELD( bar.hasGaps);
					DECODE_FIELD( bar.barCount); // ver 3 field

					bars.push_back(bar);
				}

				ASSERT( bars.size() == itemCount);

				for( int ctr = 0; ctr < itemCount; ++ctr) {

					const BarData& bar = bars[ctr];
					m_pEWrapper->historicalData( reqId, bar.date, bar.open, bar.high, bar.low,
						bar.close, bar.volume, bar.barCount, bar.average,
						bar.hasGaps.CompareNoCase("true") == 0);
				}

				// send end of dataset marker
				CString finishedStr = CString("finished-") + startDateStr + "-" + endDateStr;
				m_pEWrapper->historicalData( reqId, finishedStr, -1, -1, -1, -1, -1, -1, -1, 0);
				break;
			}

			case SCANNER_DATA:
			{
				int version;
				int tickerId;

				DECODE_FIELD( version);
				DECODE_FIELD( tickerId);

				int numberOfElements;
				DECODE_FIELD( numberOfElements);

				typedef std::vector<ScanData> ScanDataList;
				ScanDataList scannerDataList;

				scannerDataList.reserve( numberOfElements);

				for( int ctr=0; ctr < numberOfElements; ++ctr) {

					ScanData data;

					DECODE_FIELD( data.rank);
					DECODE_FIELD( data.contract.summary.conId); // ver 3 field
					DECODE_FIELD( data.contract.summary.symbol);
					DECODE_FIELD( data.contract.summary.secType);
					DECODE_FIELD( data.contract.summary.expiry);
					DECODE_FIELD( data.contract.summary.strike);
					DECODE_FIELD( data.contract.summary.right);
					DECODE_FIELD( data.contract.summary.exchange);
					DECODE_FIELD( data.contract.summary.currency);
					DECODE_FIELD( data.contract.summary.localSymbol);
					DECODE_FIELD( data.contract.marketName);
					DECODE_FIELD( data.contract.tradingClass);
					DECODE_FIELD( data.distance);
					DECODE_FIELD( data.benchmark);
					DECODE_FIELD( data.projection);
					DECODE_FIELD( data.legsStr);

					scannerDataList.push_back( data);
				}

				ASSERT( scannerDataList.size() == numberOfElements);

				for( int ctr=0; ctr < numberOfElements; ++ctr) {

					const ScanData& data = scannerDataList[ctr];
					m_pEWrapper->scannerData( tickerId, data.rank, data.contract,
						data.distance, data.benchmark, data.projection, data.legsStr);
				}

				m_pEWrapper->scannerDataEnd( tickerId);
                break;
            }

			case SCANNER_PARAMETERS:
			{
				int version;
				CString xml;

				DECODE_FIELD( version);
				DECODE_FIELD( xml);

				m_pEWrapper->scannerParameters( xml);
				break;
			}

			case CURRENT_TIME:
			{
				int version;
				int time;

				DECODE_FIELD(version);
				DECODE_FIELD(time);

				m_pEWrapper->currentTime( time);
				break;
			}

			case REAL_TIME_BARS:
			{
				int version;
				int reqId;
				int time;
				double open;
				double high;
				double low;
				double close;
				int volume;
				double average;
				int count;

				DECODE_FIELD( version);
				DECODE_FIELD( reqId);
				DECODE_FIELD( time);
				DECODE_FIELD( open);
				DECODE_FIELD( high);
				DECODE_FIELD( low);
				DECODE_FIELD( close);
				DECODE_FIELD( volume);
				DECODE_FIELD( average);
				DECODE_FIELD( count);

				m_pEWrapper->realtimeBar( reqId, time, open, high, low, close,
					volume, average, count);

				break;
			}

			default:
			{
				m_pEWrapper->error( msgId, UNKNOWN_ID.code(), UNKNOWN_ID.msg());
				break;
			}
		}

		int processed = ptr - beginPtr;
		beginPtr = ptr;
		return processed;
	}
	CATCH( CException, e) {
		m_pEWrapper->error( NO_VALID_ID, SOCKET_EXCEPTION.code(),
			SOCKET_EXCEPTION.msg() + errMsg(e));
	}
	END_CATCH
	return 0;
}

///////////////////////////////////////////////////////////
// callbacks from socket

void EClientSocket::onConnect( int i)
{
	if( !handleSocketError( i))
		return;

	// send client version
	std::ostringstream msg;
	ENCODE_FIELD( CLIENT_VERSION);
	bufferedSend( msg.str());
}

void EClientSocket::onReceive( int i)
{
	if( !handleSocketError( i))
		return;

	checkMessages();
}

void EClientSocket::onSend( int i)
{
	if( !handleSocketError( i))
		return;

	sendBufferedData();
}

void EClientSocket::onClose( int i)
{
	// this function is called when the TWS terminates the connection

	eDisconnect();
	m_pEWrapper->connectionClosed();
}

///////////////////////////////////////////////////////////
// helper
bool EClientSocket::handleSocketError( int lastError)
{
	if( lastError == ERROR_SUCCESS)
		return true;

	if( lastError == WSAEWOULDBLOCK)
		return false;

	if( lastError == WSAECONNREFUSED) {
		m_pEWrapper->error( NO_VALID_ID, CONNECT_FAIL.code(), CONNECT_FAIL.msg());
	}
	else {
		char lastErrorStr[512];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError, 0,
			lastErrorStr, sizeof(lastErrorStr), NULL);
		m_pEWrapper->error( NO_VALID_ID, SOCKET_EXCEPTION.code(),
			SOCKET_EXCEPTION.msg() + lastErrorStr);
	}	
	eDisconnect();
	return false;
}