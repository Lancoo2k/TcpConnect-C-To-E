#include "TcpConnect.h"
#include <iphlpapi.h>
#include <conio.h>
#include <atlconv.h>¡£

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib,"ws2_32.lib")

//////////////////////////////////////////////////////////////////////////

CSocketApp::CSocketApp(void)
{
	m_socket = INVALID_SOCKET;
	m_sockFile = INVALID_SOCKET;
	Init();
}

CSocketApp::~CSocketApp(void)
{
	Close();
	WSACleanup();
}

CSocketApp::CSocketApp(int nPort, int nType)
{
	m_socket = INVALID_SOCKET;
	Init();
	Set(nPort, SOCKET_ANY_IP, nType);
}

CSocketApp::CSocketApp(int nPort, string sIP, int nType)
{
	m_socket = INVALID_SOCKET;
	Init();
	Set(nPort, sIP, nType);
}

DWORD CSocketApp::Init(void)
{
	//add by guoyong Use to windows
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		printf(("\n\n-----TCP WSAStartup failed------\n\n"));
		return SOCKET_ERR_INIT;
	}
	return SOCKET_OK;

	/*m_nLastError = 0;
	m_fSendPerson = 0;
	m_fRecvPerson = 0;

	m_bSendFile = false;
	m_bRecvFile = false;

	if (LOBYTE(m_wsaData.wVersion) != 2 ||
		HIBYTE(m_wsaData.wVersion) != 2)
	{
		if (0 != WSAStartup(MAKEWORD(2, 2), &m_wsaData))
		{
			m_nLastError = WSAGetLastError();
			return SOCKET_ERR_INIT;
		}
	}
	return SOCKET_OK;*/
}

DWORD CSocketApp::Init(int nPort, string sIP, int nType)
{
	if (Init())
		return SOCKET_ERR_INIT;
	return Set(nPort, sIP, nType);
}

DWORD CSocketApp::Set(int nPort, string sIP, int nType)
{
	

	m_nLastError = 0;
	USES_CONVERSION;
	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_port = htons(nPort);
	m_sockaddr.sin_addr.s_addr = inet_addr(sIP.c_str());
	if (SOCKET_TYPE_MEG == nType)
	{
		m_socket = socket(AF_INET, SOCK_DGRAM, 0);
		if (SOCKET_ANY_IP == sIP)
		{

			if (SOCKET_ERROR == bind(m_socket, (LPSOCKADDR)&m_sockaddr, sizeof(m_sockaddr)))
			{
				m_nLastError = WSAGetLastError();
				return SOCKET_ERR_BIND;
			}

		}
	}
	else if (SOCKET_TYPE_FILE == nType)
	{
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (SOCKET_ANY_IP != sIP)
		{
			if (SOCKET_ERROR == connect(m_socket, (SOCKADDR*)&m_sockaddr, sizeof(m_sockaddr)))
			{
				m_nLastError = WSAGetLastError();
				return SOCKET_ERR_CONNECT;
			}
		}
		else
		{
			if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&m_sockaddr, sizeof(m_sockaddr)))
			{
				m_nLastError = WSAGetLastError();
				return SOCKET_ERR_CONNECT;
			}
			if (SOCKET_ERROR == listen(m_socket, 5))
			{
				m_nLastError = WSAGetLastError();
				return SOCKET_ERR_LISTEN;
			}
		}
	}
	else
	{
		return SOCKET_ERR_UNKNOWNTYPE;
	}

	if (m_socket == INVALID_SOCKET) {
		return SOCKET_ERR_SET;
	}
	return SOCKET_OK;
}

DWORD CSocketApp::Send(string sMeg)
{
	m_nLastError = 0;
	USES_CONVERSION;
	CHAR szBuff[SOCKET_MEG_SIZE];

	strcpy(szBuff, sMeg.c_str());

	bool op = true;
	if (SOCKET_ERROR == setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char FAR*) & op, sizeof(op)))
	{
		m_nLastError = WSAGetLastError();
		return SOCKET_ERR_INITSEND;
	}
	if (SOCKET_ERROR == sendto(m_socket, (char*)szBuff, sizeof(szBuff), 0, (LPSOCKADDR)&m_sockaddr, sizeof(m_sockaddr)) == SOCKET_ERROR)
	{
		m_nLastError = WSAGetLastError();
		return SOCKET_ERR_SEND;
	}

	return SOCKET_OK;
}

void CSocketApp::Close(void)
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}
