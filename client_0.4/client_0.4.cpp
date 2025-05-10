#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#ifndef UNICODE
//#define UNICODE
//#endif

//#define WIN32_LEAN_AND_MEAN

#include<iostream>
#include<string>

#include<stdlib.h>
#include<WinSock2.h>
#include<iphlpapi.h>

#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"ws2_32.lib")

#define DEFAULT_PORT 20792
#define DEFAULT_BUFLEN 256

#define FLAG 0x00
#define FLAG_ASK 0xAA    //告诉服务器应答
#define FLAG_NAK 0xBB    //告诉服务器不应答

#define CMD 0x00
#define CONTACT_ADD CMD+1
#define CONTACT_DEL CMD+2
#define CONTACT_GIT CMD+3

int main()
{
    //-----------------------------------------
   // Declare and initialize variables
    WSADATA wsaData;
    int iResult;

    DWORD dwError;
    int i = 0;

    struct hostent* remoteHost;
    const char* servercontacthost = "1vg5062570.51mypc.cn";
    struct in_addr addr;

    char** pAlias;

    SOCKET ConnectSocket = INVALID_SOCKET;
    struct sockaddr_in clientService;

    int recvbuflen = DEFAULT_BUFLEN;
    char msgbuf[DEFAULT_BUFLEN] = "";//(char*)"Client: sending data test";

    addr.S_un.S_addr = INADDR_ANY;
    /*struct ServerInfo pack;*/
    char flag = FLAG_ASK;
    char cmd = CONTACT_ADD;
    USHORT port = DEFAULT_PORT;

    char hostname[20];
    struct hostent* localhost;
    struct in_addr localaddr;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    printf("Calling gethostbyname with %s\n", servercontacthost);
    remoteHost = gethostbyname(servercontacthost);

    if (remoteHost == NULL) {
        dwError = WSAGetLastError();
        if (dwError != 0) {
            if (dwError == WSAHOST_NOT_FOUND) {
                printf("Host not found\n");
                return 1;
            }
            else if (dwError == WSANO_DATA) {
                printf("No data record found\n");
                return 1;
            }
            else {
                printf("Function failed with error: %ld\n", dwError);
                return 1;
            }
        }
    }
    else {
        printf("Function returned:\n");
        printf("\tOfficial name: %s\n", remoteHost->h_name);
        for (pAlias = remoteHost->h_aliases; *pAlias != 0; pAlias++) {
            printf("\tAlternate name #%d: %s\n", ++i, *pAlias);
        }
        printf("\tAddress type: ");
        switch (remoteHost->h_addrtype) {
        case AF_INET:
            printf("AF_INET\n");
            break;
        case AF_NETBIOS:
            printf("AF_NETBIOS\n");
            break;
        default:
            printf(" %d\n", remoteHost->h_addrtype);
            break;
        }
        printf("\tAddress length: %d\n", remoteHost->h_length);

        i = 0;
        if (remoteHost->h_addrtype == AF_INET)
        {
            while (remoteHost->h_addr_list[i] != 0) {
                addr.s_addr = *(u_long*)remoteHost->h_addr_list[i++];
                printf("\tIP Address #%d: %s\n", i, inet_ntoa(addr));
            }
        }
        else if (remoteHost->h_addrtype == AF_NETBIOS)
        {
            printf("NETBIOS address was returned\n");
        }
    }
    //----------------------
    // Get local host ip
    gethostname(hostname, sizeof(hostname));
    printf("Local Host Name:%s\n", hostname);
    localhost = gethostbyname(hostname);
    i = 0;
    if (localhost->h_addrtype == AF_INET)
    {
        while (localhost->h_addr_list[i] != 0) {
            localaddr.s_addr = *(u_long*)localhost->h_addr_list[i++];
            printf("\tIP Address #%d: %s\n", i, inet_ntoa(localaddr));
        }
    }

    /*IP_ADAPTER_INFO*/
    //----------------------
    // Create a SOCKET for connecting to server
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port of the server to be connected to.
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = addr.S_un.S_addr;//inet_addr("127.0.0.1");
    clientService.sin_port = htons(DEFAULT_PORT);

    //----------------------
    // Connect to server.
    iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
    if (iResult == SOCKET_ERROR) {
        wprintf(L"connect failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    //----------------------
    // initial buffer
    memset(msgbuf, 0x00, sizeof(msgbuf));
    msgbuf[0] = (char)flag;
    msgbuf[1] = (char)cmd;
    msgbuf[2] = (char)addr.S_un.S_un_b.s_b1;
    msgbuf[3] = (char)addr.S_un.S_un_b.s_b2;
    msgbuf[4] = (char)addr.S_un.S_un_b.s_b3;
    msgbuf[5] = (char)addr.S_un.S_un_b.s_b4;
    msgbuf[6] = (char)((port & 0xff00) >> 8);
    msgbuf[7] = (char)(port & 0xff);

SEND_DAT:
    //----------------------
    // Send an initial buffer
    iResult = send(ConnectSocket, msgbuf, (int)strlen(msgbuf), 0);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf("Bytes Sent: %d\n", iResult);


    char tmpcmd[2] = { 0x00,0x00 };
    auto ret = recv(ConnectSocket, tmpcmd, sizeof(tmpcmd), 0);
    if (ret == 2 && tmpcmd[0] == 'O' && tmpcmd[1] == 'K')
        goto SHUTDOWN;
    else
        goto SEND_DAT;

SHUTDOWN:
    //shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // close the socket
    iResult = closesocket(ConnectSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"close failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    WSACleanup();
    return 0;
}