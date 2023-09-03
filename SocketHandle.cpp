#include "SocketHandle.h"

SOCKET SocketHandle::createSocket(LPCSTR host, LPCSTR scheme)
{
    
    addrinfo* result = nullptr;
    addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    
    int iResult = getaddrinfo(host, scheme, &hints, &result);
    if (iResult != 0)
    {
        std::cerr << "getaddrinfo failed: " << iResult << '\n';
        WSACleanup();
        return INVALID_SOCKET;
    }

    SOCKET sck = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sck == INVALID_SOCKET)
    {
        std::cerr << "Error at socket(): " << WSAGetLastError() << '\n';
        freeaddrinfo(result);
        WSACleanup();
        return INVALID_SOCKET;
    }

    iResult = connect(sck, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) 
    {
        closesocket(sck);
        freeaddrinfo(result);
        sck = INVALID_SOCKET;
    }

    
    return sck;
}

SSL* SocketHandle::createSSL(SOCKET& sck)
{
    SSL_library_init();
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());

    if (!ctx)
    {
        std::cout << "SSL_CTX_new failed.\n";
        return nullptr;
    }

    SSL* ssl = nullptr;
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sck);

    int iResult = SSL_connect(ssl);

    if (iResult <= 0)
    {
        std::cout << "SSL connect failed.\n";
        closesocket(sck);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        WSACleanup();
        return nullptr;
    }

    return ssl;

}

bool SocketHandle::sendRequest(SSL* ssl, LPCSTR request)
{
    int iResult = SSL_write(ssl, request, strlen(request));

    if (iResult <= 0)
    {
        std::cerr << "Send failed: " << WSAGetLastError() << '\n';
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        WSACleanup();
        return false;
    }
    return true;
}

bool SocketHandle::dowloadFile(SSL* ssl, LPCSTR filename)
{
    HANDLE hout = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (hout == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error opening file!\n";
        return false;
    }
    //bytes read
    int iResult = -1;
    BYTE recvbuf[1024];
    while (true)
    {
        iResult = SSL_read(ssl, recvbuf, sizeof(recvbuf));

        if (iResult > 0) 
        {
            //pointer to the end of the response header
            BYTE* endOfHeader = nullptr;
            endOfHeader = (BYTE*)strstr((char*)recvbuf, "\r\n\r\n"); //response header finish with \r\n\r\n

            DWORD dwBytesWritten;
            if (endOfHeader != nullptr)
            {
                WriteFile(hout, endOfHeader + 4, iResult - (endOfHeader - recvbuf + 4), &dwBytesWritten, NULL);
            }
            else
            {
                WriteFile(hout, recvbuf, iResult, &dwBytesWritten, NULL);
            }
        }
        else if (iResult == 0)
        {
            break;
        }
        else
        {
            std::cout << "SSL read failed.\n";
            ERR_print_errors_fp(stderr);
            return false;
        }
    }
    
    return true;
}
