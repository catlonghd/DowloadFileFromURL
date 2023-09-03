#include "SocketHandle.h"
#include "Utility.h"

int main(int argc, char* argv[])
{
   if (argc != 2)
   {
       std::cerr << "Invalid parameters!\n";
       return -1;
   }

   //Declare classes
   SocketHandle sckhandle;
   Utility ult;

   //Parse URL to scheme, host and path
   LPCSTR url = argv[1];
   std::string scheme_str;
   LPCSTR scheme;
   std::string host_str;
   LPCSTR host;
   std::string path_str;
   LPCSTR path;

   if (((scheme_str = ult.getScheme(url)) != "") && ((host_str = ult.getHostName(url)) != "") && ((path_str = ult.getPath(url)) != ""))
   {
       scheme = scheme_str.c_str();
       host = host_str.c_str();
       path = path_str.c_str();
   }
   else
   {
       std::cerr << "Invalid URL!\n";
       return -1;
   }

   //initialize socket
   WSADATA wsaData;
   int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if (iResult != 0)
   {
       std::cerr << "WSAStartup failed: " << iResult << '\n';
       return 1;
   }

   //Create socket
   SOCKET sck = sckhandle.createSocket(host, scheme);
   if (sck == INVALID_SOCKET)
   {
       WSACleanup();
       return 1;
   }
   
   //Create SSL
   SSL* ssl = sckhandle.createSSL(sck);
   if (ssl == nullptr)
   {
       closesocket(sck);
       WSACleanup();
       return -1;
   }

   //Create request header
   std::string request_str = "GET " + std::string(path) + " HTTP/1.1\r\n"
                             "Host: " + std::string(host) + "\r\n"
                             "Connection: close\r\n\r\n";

   //Send request to server
   LPCSTR request = request_str.c_str();
   if (!sckhandle.sendRequest(ssl, request))
   {
       closesocket(sck);
       WSACleanup();
       return -1;
   }


   //Dowload file to the same directory of the program
   LPCSTR filename = ult.getFileName(path);

   if (sckhandle.dowloadFile(ssl, filename) == true)
   {
       std::cout << "Download file successfully!\n";
   }
   else
   {
       std::cerr << "Error dowloading file!\n";
       closesocket(sck);
       SSL_free(ssl);
       WSACleanup();
       return -1;
   }

   closesocket(sck);
   SSL_free(ssl);
   WSACleanup();
   return 0;
}