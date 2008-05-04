#ifndef tswsocketclienterrors_def
#define tswsocketclienterrors_def

static const int NO_VALID_ID = -1;
static const int NO_VALID_ERROR_CODE = 0;
static const int SYSTEM_ERROR = 600;

class CodeMsgPair {
public:
	CodeMsgPair(int code, CString msg) : m_errorCode(code), m_errorMsg(msg) {
	}
private:
	int m_errorCode;
	CString m_errorMsg;
public:
	int	  code() const			        { return m_errorCode; }
	const CString& msg() const			{ return m_errorMsg;  }
};

static const CodeMsgPair ALREADY_CONNECTED(501,	_T( "Already connected.") );
static const CodeMsgPair CONNECT_FAIL(502, _T( "Couldn't connect to TWS.  Confirm that \"Enable ActiveX and Socket Clients\" is enabled on the TWS \"Configure->API\" menu.") );
static const CodeMsgPair UPDATE_TWS(503, _T( "The TWS is out of date and must be upgraded.") );
static const CodeMsgPair NOT_CONNECTED(504, _T( "Not connected") );
static const CodeMsgPair UNKNOWN_ID(505, _T( "Fatal Error: Unknown message id.") );
static const CodeMsgPair ZERO_BYTE_READ(506, _T( "Unexplained zero bytes read.") );
static const CodeMsgPair NULL_STRING_READ(507, _T( "Null string read when expecting integer") );
static const CodeMsgPair NO_BYTES_READ(508, _T( "Error: no bytes read or no null terminator found") );
static const CodeMsgPair SOCKET_EXCEPTION(509, _T( "Exception caught while reading socket - ") );
static const CodeMsgPair FAIL_CREATE_SOCK(520, _T( "Failed to create socket") );
static const CodeMsgPair FAIL_CONNECT_TWS(521, _T( "Couldn't connect to TWS.") );
static const CodeMsgPair FAIL_SEND_FA_REQUEST(522, _T( "FA Information Request Sending Error - ") );
static const CodeMsgPair FAIL_SEND_FA_REPLACE(523, _T( "FA Information Replace Sending Error - ") );
static const CodeMsgPair FAIL_SEND_REQSCANNER(524, _T( "Request Scanner Subscription Sending Error - ") );
static const CodeMsgPair FAIL_SEND_CANSCANNER(525, _T( "Cancel Scanner Subscription Sending Error - ") );
static const CodeMsgPair FAIL_SEND_REQSCANNERPARAMETERS(526, _T( "Request Scanner Parameter Sending Error - ") );
static const CodeMsgPair FAIL_SEND_REQHISTDATA(527, _T( "Request Historical Data Sending Error - ") );
static const CodeMsgPair FAIL_SEND_CANHISTDATA(528, _T( "Cancel Historical Data Sending Error - ") );
static const CodeMsgPair FAIL_SEND_REQRTBARS(529, _T( "Request Real-time Bar Data Sending Error - ") );
static const CodeMsgPair FAIL_SEND_CANRTBARS(530, _T( "Cancel Real-time Bar Data Sending Error - ") );
static const CodeMsgPair FAIL_SEND_REQCURRTIME(531, _T( "Request Current Time Sending Error - ") );

#endif