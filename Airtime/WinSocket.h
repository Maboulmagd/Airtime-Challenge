#ifndef WINSOCKET_H
#define WINSOCKET_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <assert.h>

class WinSocket
{
    public:
        enum class STATUS_CODE
        {
            SUCCESS = 0x0,
            FAILURE = 0x1
        };

        PCSTR pNodeName;
        PCSTR pServiceName;
        SOCKET Socket;
        STATUS_CODE RESULT;

        WinSocket(PCSTR _pNodeName, PCSTR _pServiceName, int& _iResult);

        WinSocket(const WinSocket& _WinSocket) = delete;

        WinSocket& operator=(const WinSocket& _WinSocket) = delete;

        ~WinSocket();

        void InitializeWinSocket(int& _iResult, WSADATA& _wsaData);
        
        void ResolveServerAddressAndPort(int& _iResult, addrinfo& _pHints, addrinfo*& _pResult);
        
        void CreateSocket(SOCKET& _socket, addrinfo*& _ptr, addrinfo*& _result);
        
        void ConnectToServer(int& _iResult, SOCKET& _Socket, addrinfo*& _ptr);

        void ReceiveFromServer(int& _iResult, char* _buf, const int& _len, int& _bytesReceived);

        void SendToServer(int& _iResult, const char* _buf, const int& _len);
};

#endif