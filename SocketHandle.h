#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

class SocketHandle
{
public:
	SOCKET createSocket(LPCSTR host, LPCSTR scheme);
	SSL* createSSL(SOCKET& sck);
	bool sendRequest(SSL* ssl, LPCSTR request);
	bool dowloadFile(SSL* ssl, LPCSTR filename);
};

