/* Copyright (C) 2013 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#pragma once
#ifndef eposixclientsocket_def
#define eposixclientsocket_def

#include "../Shared/EClientSocketBase.h"

class EWrapper;

class TWSAPIDLLEXP EPosixClientSocket : public EClientSocketBase
{
public:

	explicit EPosixClientSocket( EWrapper *ptr);
	~EPosixClientSocket();

	// override virtual funcs from EClient
	bool eConnect( const char *host, unsigned int port, int clientId = 0, bool extraAuth = false);
	void eDisconnect();

	bool isSocketOK() const;
	int fd() const;

private:

	bool eConnectImpl(int clientId, bool extraAuth, ConnState* stateOutPt);

private:

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
