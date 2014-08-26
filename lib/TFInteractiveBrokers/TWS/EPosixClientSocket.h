/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */
#pragma once
#ifndef eposixclientsocket_def
#define eposixclientsocket_def

#include <windows.h>

#include "EClientSocketBase.h"
#include <map>

//#ifndef DLLEXP
//#define DLLEXP
//#endif

class EWrapper;

//class DLLEXP EPosixClientSocket : public EClientSocketBase
class EPosixClientSocket : public EClientSocketBase
{
	//enum State {
	//	ST_CONNECT,
	//	ST_PLACEORDER,
	//	ST_PLACEORDER_ACK,
	//	ST_CANCELORDER,
	//	ST_CANCELORDER_ACK,
	//	ST_PING,
	//	ST_PING_ACK,
	//	ST_IDLE
	//};

	//State m_state;
	time_t m_sleepDeadline;

	void processMessages();

	//static DWORD WINAPI processMessagesThread(LPVOID lpThreadParameter);
  static DWORD processMessagesThread(LPVOID lpThreadParameter);
	static void CALLBACK socketTimerProc(  _In_  HWND hwnd,  _In_  UINT uMsg,  _In_  UINT_PTR idEvent,  _In_  DWORD dwTime);
	static std::map<UINT_PTR, EPosixClientSocket*> socketMap;
public:

	explicit EPosixClientSocket( EWrapper *ptr);
	~EPosixClientSocket();

	// override virtual funcs from EClient
	bool eConnect( const char *host, unsigned int port, int clientId = 0, bool extraAuth = false);
	void eDisconnect();


private:
	bool isSocketOK() const;
	int fd() const;
	int send( const char* buf, size_t sz);
	int receive( char* buf, size_t sz);

public:
	// callback from socket
	void onReceive();
	void onSend();
	void onError();

private:

	void onConnect();
	void onClose();

public:
	// helper
	bool handleSocketError();

private:

	int m_fd;
};

#endif
