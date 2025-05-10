#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include<string>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <winsqlite/winsqlite3.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"winsqlite3.lib")

#define DEFAULT_PORT 20792
#define MSGSIZE 256
#define FLAG 0x00
#define FLAG_ASK 0xAA    //告诉服务器应答
#define FLAG_NAK 0xBB    //告诉服务器不应答
#define CMD 0x00
#define CONTACT_ADD CMD+1
#define CONTACT_DEL CMD+2
#define CONTACT_GIT CMD+3

int wmain()
{

    //----------------------
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = 0;

    int addrsize = sizeof(sockaddr_in);

    SOCKET ListenSocket = INVALID_SOCKET;
    sockaddr_in service;

    SOCKET RemoteClient1 = INVALID_SOCKET;;
    sockaddr_in client1;


    char szMessage[MSGSIZE];

    sqlite3* db = NULL;
    char* sqtErrMsg = 0;
    int sqtrc = 0;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"WSAStartup() failed with error: %d\n", iResult);
        return 1;
    }
    //----------------------
    // Create a SOCKET for listening for incoming connection requests.
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port for the socket that is being bound.
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(20792);

    iResult = bind(ListenSocket, (SOCKADDR*)&service, sizeof(service));
    if (iResult == SOCKET_ERROR) {
        wprintf(L"bind function failed with error %d\n", WSAGetLastError());
        iResult = closesocket(ListenSocket);
        if (iResult == SOCKET_ERROR)
            wprintf(L"closesocket function failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    //----------------------
    // Listen for incoming connection requests 
    // on the created socket
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
        wprintf(L"listen function failed with error: %d\n", WSAGetLastError());

    wprintf(L"Listening on socket...\n");

    //打开指定的数据库文件,如果不存在将创建一个同名的数据库文件
    sqtrc = sqlite3_open("ServerContact.db", &db);
    if (sqtrc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    while (1)
    {
    AGAIN:
        RemoteClient1 = accept(ListenSocket, (struct sockaddr*)&client1, &addrsize);
        printf("Accepted client:%s:%d\n", inet_ntoa(client1.sin_addr),
            ntohs(client1.sin_port));

        memset(szMessage, 0, sizeof(szMessage));
        auto ret = recv(RemoteClient1, szMessage, MSGSIZE, 0);
        printf("Received [%d bytes]: '%s'\n", ret, szMessage);

        char flag = szMessage[0];
        char cmd = szMessage[1];
        if (flag != FLAG_ASK)
            goto OVER;

        switch (cmd)
        {
            struct in_addr scip;
            scip.S_un.S_un_b.s_b1 = (UCHAR)szMessage[2];
            scip.S_un.S_un_b.s_b2 = (UCHAR)szMessage[3];
            scip.S_un.S_un_b.s_b3 = (UCHAR)szMessage[4];
            scip.S_un.S_un_b.s_b4 = (UCHAR)szMessage[5];
            USHORT port = ((UCHAR)szMessage[6] << 8) + (UCHAR)szMessage[7];
        case CONTACT_ADD:
            break;
        case CONTACT_DEL:
            break;
        case CONTACT_GIT:

            break;
        default:
            //shutdown the connection since no more data will be sent
            iResult = shutdown(RemoteClient1, SD_SEND);
            if (iResult == SOCKET_ERROR) {
                wprintf(L"shutdown failed with error: %d\n", WSAGetLastError());
                closesocket(RemoteClient1);
            }
            // close the socket
            iResult = closesocket(RemoteClient1);
            if (iResult == SOCKET_ERROR) {
                wprintf(L"close failed with error: %d\n", WSAGetLastError());
            }
            goto AGAIN;
            break;
        }
    OVER:
        std::string s = "OK";
        memset(szMessage, 0, sizeof(szMessage));
        memcpy(szMessage, s.c_str(), s.length());
        szMessage[strlen(szMessage)] = '\0';
        send(RemoteClient1, szMessage, strlen(szMessage) + 1, 0);
    }
    sqlite3_close(db); //关闭数据库

    iResult = closesocket(RemoteClient1);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket function failed with error %d\n", WSAGetLastError());
    }

    iResult = closesocket(ListenSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket function failed with error %d\n", WSAGetLastError());
    }

    WSACleanup();
    return 0;
}