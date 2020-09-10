#include "WinSocket.h"

WinSocket::WinSocket(PCSTR _pNodeName, PCSTR _pServiceName, int& _iResult) : pNodeName(_pNodeName), pServiceName(_pServiceName)
{
    assert(pNodeName != NULL);
    assert(pServiceName != NULL);

    WSADATA wsaData;

    // Initialize Winsock...
    this->InitializeWinSocket(_iResult, wsaData);

    struct addrinfo* result = NULL, *ptr = NULL, hints;

    // Resolve the server address and port
    this->ResolveServerAddressAndPort(_iResult, hints, result);



    this->Socket = INVALID_SOCKET;

    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr = result;

    // Create a SOCKET for connecting to server
    this->CreateSocket(this->Socket, ptr, result);

    // Connect to the host/server...
    this->ConnectToServer(_iResult, this->Socket, ptr);

    // Should really try the next address returned by getaddrinfo if the connect call failed...
    // But for simplicity we just free the resources returned by getaddrinfo and print an error message...
    freeaddrinfo(result);

    if (this->Socket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        this->RESULT = WinSocket::STATUS_CODE::FAILURE;
    }
}

WinSocket::~WinSocket()
{
}

void WinSocket::InitializeWinSocket(int& _iResult, WSADATA& _wsaData)
{
    _iResult = WSAStartup(MAKEWORD(2, 2), &_wsaData);

    if (_iResult != 0)
    {
        printf("WSAStartup failed: %d\n", _iResult);
        this->RESULT = WinSocket::STATUS_CODE::FAILURE;
    }
}

void WinSocket::ResolveServerAddressAndPort(int& _iResult, addrinfo& _pHints, addrinfo*& _pResult)
{
    ZeroMemory(&_pHints, sizeof(_pHints));
    _pHints.ai_family = AF_UNSPEC;
    _pHints.ai_socktype = SOCK_STREAM;
    _pHints.ai_protocol = IPPROTO_TCP;

    _iResult = getaddrinfo(this->pNodeName, this->pServiceName, &_pHints, &_pResult);
    if (_iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", _iResult);
        WSACleanup();
        this->RESULT = WinSocket::STATUS_CODE::FAILURE;
    }
}

void WinSocket::CreateSocket(SOCKET& _socket, addrinfo*& _ptr, addrinfo*& _result)
{
    _socket = socket(_ptr->ai_family, _ptr->ai_socktype, _ptr->ai_protocol);
    if (_socket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(_result);
        WSACleanup();
        this->RESULT = WinSocket::STATUS_CODE::FAILURE;
    }
}

void WinSocket::ConnectToServer(int& _iResult, SOCKET& _Socket, addrinfo*& _ptr)
{
    _iResult = connect(_Socket, _ptr->ai_addr, (int)_ptr->ai_addrlen);
    if (_iResult == SOCKET_ERROR)
    {
        closesocket(_Socket);
        _Socket = INVALID_SOCKET;
        this->RESULT = WinSocket::STATUS_CODE::FAILURE;
    }
}

void WinSocket::ReceiveFromServer(int& _iResult, char* _buf, const int& _len, int& _bytesReceived)
{
    // Receive data...
    _iResult = recv(this->Socket, _buf, _len, 0);
    if (_iResult == SOCKET_ERROR)
    {
        printf("Receive failed: %d\n", WSAGetLastError());
        closesocket(this->Socket);
        WSACleanup();
        this->RESULT = WinSocket::STATUS_CODE::FAILURE;
    }

    else if (_iResult > 0)
    {
        printf("Bytes received: %d\n", _iResult);
        _bytesReceived = _iResult;
    }

    else if (_iResult == 0)
    {
        printf("Connection closed\n");
    }

    else
    {
        printf("Receive failed: %d\n", WSAGetLastError());
        this->RESULT = WinSocket::STATUS_CODE::FAILURE;
    }
}

void WinSocket::SendToServer(int& _iResult, const char* _buf, const int& _len)
{
    // Send an initial buffer...
    _iResult = send(this->Socket, _buf, _len, 0);
    if (_iResult == SOCKET_ERROR)
    {
        printf("Send failed: %d\n", WSAGetLastError());
        closesocket(this->Socket);
        WSACleanup();
        this->RESULT = WinSocket::STATUS_CODE::FAILURE;
    }

    printf("Bytes Sent: %ld\n", _iResult);
}