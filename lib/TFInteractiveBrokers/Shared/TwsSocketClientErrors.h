/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#pragma once
#ifndef tswsocketclienterrors_def
#define tswsocketclienterrors_def

static const int NO_VALID_ID = -1;
static const int NO_VALID_ERROR_CODE = 0;
static const int SYSTEM_ERROR = 600;

class CodeMsgPair {
public:
	CodeMsgPair(int code, std::string msg) : m_errorCode(code), m_errorMsg(msg) {
	}
private:
	int m_errorCode;
	std::string m_errorMsg;
public:
	int	  code() const			        { return m_errorCode; }
	const std::string& msg() const			{ return m_errorMsg;  }
};

static const CodeMsgPair ALREADY_CONNECTED(501,	"Already connected.");
static const CodeMsgPair CONNECT_FAIL(502, "Couldn't connect to TWS.  Confirm that \"Enable ActiveX and Socket Clients\" is enabled on the TWS \"Configure->API\" menu.");
static const CodeMsgPair UPDATE_TWS(503, "The TWS is out of date and must be upgraded.");
static const CodeMsgPair NOT_CONNECTED(504, "Not connected");
static const CodeMsgPair UNKNOWN_ID(505, "Fatal Error: Unknown message id.");
static const CodeMsgPair UNSUPPORTED_VERSION(506, "Unsupported version");
static const CodeMsgPair BAD_LENGTH(507, "Bad message length");
static const CodeMsgPair BAD_MESSAGE(508, "Bad message");
static const CodeMsgPair SOCKET_EXCEPTION(509, "Exception caught while reading socket - ");
static const CodeMsgPair FAIL_CREATE_SOCK(520, "Failed to create socket");

#endif
