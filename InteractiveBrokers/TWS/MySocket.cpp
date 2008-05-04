#include "stdafx.h"
#include "MySocket.h"
#include "EClientSocket.h"

MySocket::MySocket( EClientSocket *pClient) 
{
	m_pClient = pClient;
}

void MySocket::OnConnect( int i)
{
	m_pClient->onConnect( i);
}

void MySocket::OnReceive( int i) 
{
	m_pClient->onReceive( i);
}

void MySocket::OnSend( int i) 
{
	m_pClient->onSend( i);
}

void MySocket::OnClose( int i)
{
	m_pClient->onClose( i);
}	
