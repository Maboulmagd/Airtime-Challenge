// Airtime.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "WinSocket.h"
#include "SatelliteData.h"

#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "2323"
#define DEFAULT_BUFLEN 512
#define LARGER_BUFLEN 8192

int main()
{
    int iResult;
    WinSocket WinSocket("challenge.airtime.com", DEFAULT_PORT, iResult);
    assert(WinSocket.RESULT != WinSocket::STATUS_CODE::FAILURE);

    if (WinSocket.RESULT == WinSocket::STATUS_CODE::FAILURE) { return 1; }

    // Receive handshake packet upon connection...encoded in UTF-8
    char handshakePacket[DEFAULT_BUFLEN];
    int handshakeBytesRecv = 0;

    // First, receive the handshake packet...
    WinSocket.ReceiveFromServer(iResult, handshakePacket, DEFAULT_BUFLEN, handshakeBytesRecv);
    assert(WinSocket.RESULT != WinSocket::STATUS_CODE::FAILURE);

    if (WinSocket.RESULT == WinSocket::STATUS_CODE::FAILURE) { return 1; }

    // Print/examine the contents of handshakePacket...
    // NOTE: This is only for debugging purposes...

    //for (int i = 0; i < handshakeBytesRecv; ++i)
    //{
    //    std::cout << handshakePacket[i];
    //}



    // Now build my user data/credentials...
    const char* constStringPortion = ":mohamedrefat007@gmail.com:at\n";

    std::string sendbufString = "IAM:";

    int i = 6;
    for (; i < handshakeBytesRecv - 1; ++i)
    {
        sendbufString += handshakePacket[i];
    }

    int constStringPortionLen = static_cast<int>(strlen(constStringPortion));

    for (int x = 0; x < constStringPortionLen; ++x, ++i)
    {
        sendbufString += constStringPortion[x];
    }

    // Print/examine the contents of what we are sending...
    // NOTE: This is only for debugging purposes...

    //for (int x = 0; x < i - 1; ++x)
    //{
    //    std::cout << sendbufString[x];
    //}

    // Data buffer to send...
    const char* sendbuf = const_cast<char*>(sendbufString.c_str());

    // Sending the credentials buffer...
    WinSocket.SendToServer( iResult, sendbuf, static_cast<int>(strlen(sendbuf)) );
    assert(WinSocket.RESULT != WinSocket::STATUS_CODE::FAILURE);

    if (WinSocket.RESULT == WinSocket::STATUS_CODE::FAILURE) { return 1; }



    // Now I need to receive the SUCCESS PACKET, containing my/user identification code...

    // This is where I will be receiving the identification code from the server...
    char successVerificationBuffer[15];

    // Receive the identification code...
    // NOTE: A slight optimization is just reusing the "handshakeBytesRecv" integer variable...
    WinSocket.ReceiveFromServer(iResult, successVerificationBuffer, 15, handshakeBytesRecv);
    assert(WinSocket.RESULT != WinSocket::STATUS_CODE::FAILURE);

    if (WinSocket.RESULT == WinSocket::STATUS_CODE::FAILURE) { return 1; }



    // Finally, receive the entire satellite data dump (packets)...

    // We are going to need the following...
    char recvbuf[LARGER_BUFLEN];

    int totalBytesReceived = 0;

    std::vector<byte> data(4000000, 0);
    //std::cout << data.max_size() << "\n";

    int dataIndex = 0;

    // Now, receive data from the host/server UNTIL the server CLOSES the connection...
    do
    {
        // NOTE: A slight optimization is just reusing the "handshakeBytesRecv" integer variable...
        WinSocket.ReceiveFromServer(iResult, recvbuf, LARGER_BUFLEN, handshakeBytesRecv);
        assert(WinSocket.RESULT != WinSocket::STATUS_CODE::FAILURE);

        if (WinSocket.RESULT == WinSocket::STATUS_CODE::FAILURE) { return 1; }

        totalBytesReceived += iResult;

        for (int i = 0; i < iResult; ++i) { data[dataIndex++] = static_cast<byte>(recvbuf[i]); }

    } while (iResult > 0);

    std::cout << "totalBytesReceived: " << totalBytesReceived << "\n";

    SatelliteData satelliteData(data, totalBytesReceived);
    std::vector<SatelliteData::Packet> vectorOfPackets = satelliteData.ReorderPackets();

    // Write raw byte data to a file...

    // We need a directory/file path for our output...
    // NOTE: CHANGE IT TO YOUR DESIRED PATH!
    satelliteData.WriteByteDataToFile(vectorOfPackets, "C:/Users/ENTER_USER_NAME/Desktop/message.raw");

    return 0;
}