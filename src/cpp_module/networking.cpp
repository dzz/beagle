#include "networking.h"

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#include <stdlib.h>
#include <string>
#include <map>
#include <queue>
#include <cstdio>
#include <cstdint>

using namespace network;


//file wide networking variables
SOCKET listen_socket = INVALID_SOCKET, accept_socket = INVALID_SOCKET;

static std::map<int, std::queue<Data *>> outbound_data{};
static std::map<int, std::queue<Data *>> inbound_data{};

static int bytes_transfer = 0;

WSAOVERLAPPED overlapped_send = {0};
WSAOVERLAPPED overlapped_recv = {0};
//accept/connect, send, recv
#define EVENT_CON 0
#define EVENT_SEND 1
#define EVENT_RECV 2
WSAEVENT wsa_events[3] = {0};

static char network_buffer[NET_BUFF_SIZE] = {0};


//public functions
void init() {
    WSADATA wsa_data;
    int rc;
    rc = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (rc != 0) {
        printf("Unable to load Winsock: %d\n", rc);
    }
}

void close() {
    WSACleanup();
}

DWORD WINAPI accept_connection(LPVOID param) {
    wsa_events[EVENT_CON] = WSACreateEvent();

    if (WSAEventSelect(listen_socket, wsa_events[EVENT_CON], FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR) {
        printf("WSAEventSelect failed with error: %d\n", WSAGetLastError());
        return 1;
    }
    //accept an incoming connection request off the bat just incase
    accept_socket = WSAAccept(listen_socket, 0, 0, 0, 0);
    if (accept_socket == INVALID_SOCKET && WSAGetLastError() != WSAEWOULDBLOCK) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        return 1;
    }

    if (WSAWaitForMultipleEvents(1, wsa_events + EVENT_CON, 1, WSA_INFINITE, 1) == WSA_WAIT_FAILED) {
        printf("WSAWaitForMultipleEvents failed with error: %d\n", WSAGetLastError());
        return 1;
    }
    accept_socket = accept(listen_socket, 0, 0);
    if (accept_socket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        return 1;
    }
    printf("Client connected...\n");
    return 0;
}

int start_server(int port) {
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(struct sockaddr_in);

    DWORD SendBytes;
    DWORD Flags;

    int rc, i;

    //make sure the hints struct is zeroed out
    SecureZeroMemory((PVOID) &hints, sizeof(struct addrinfo));

    //initialize the hints to obtain the 
    //wildcard bind address for IPv4
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //default to localhost bind
    rc = getaddrinfo(0, PORT, &hints, &result);
    if (rc != 0) {
        printf("getaddrinfo failed with error: %d\n", rc);
        return 1;
    }

    listen_socket = socket(result->ai_family,
                          result->ai_socktype, result->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        return 1;
    }

    rc = bind(listen_socket, result->ai_addr, (int) result->ai_addrlen);
    if (rc == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listen_socket);
        return 1;
    }
    freeaddrinfo(result);

    //might want to later increase the number of connections to accept at once
    rc = listen(listen_socket, 1);
    if (rc == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        return 1;
    }

    //create thread for handling overlapped connection requests
    int thread_id;
    if (!CreateThread(0, 0, accept_connection, 0, 0, &thread_id)) {
        printf("CreateThread failed\n");
        closesocket(listen_socket);
        return 1;
    }

    return 0;
}

DWORD WINAPI start_connection(LPVOID param) {
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int err = 0;
    int rc;
    // Make sure the hints struct is zeroed out
    SecureZeroMemory((PVOID) &hints, sizeof (struct addrinfo));

    // Initialize the hints to retrieve the server address for IPv4
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    rc = getaddrinfo(address, PORT, &hints, &result);
    if (rc != 0) {
        printf("getaddrinfo failed with error: %d\n", rc);
        return 1;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        accept_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (accept_socket == INVALID_SOCKET) {
            printf("socket failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(result);
            return 1;
        }
        rc = connect(accept_socket, ptr->ai_addr, (int) ptr->ai_addrlen);
        if (rc == SOCKET_ERROR) {
            if (WSAECONNREFUSED == (err = WSAGetLastError())) {
                closesocket(accept_socket);
                accept_socket = INVALID_SOCKET;
                continue;
            }
            printf("connect failed with error: %d\n", err);
            freeaddrinfo(result);
            closesocket(accept_socket);
            return 1;
        }
        break;
    }
    freeaddrinfo(result);
    if (accept_socket == INVALID_SOCKET) {
        printf("Unable to establish connection with the server!\n");
        return 1;
    }

    printf("Client connected...\n");

    // Make sure the RecvOverlapped struct is zeroed out
    SecureZeroMemory((PVOID) & overlapped_recv, sizeof (WSAOVERLAPPED));

    // Create an event handle and setup an overlapped structure.
    overlapped_recv.hEvent = WSACreateEvent();
    if (overlapped_recv.hEvent == NULL) {
        printf("WSACreateEvent failed: %d\n", WSAGetLastError());
        closesocket(accept_socket);
        return 1;
    }
    return 0;
}

int start_client(const char * address) {
    if (!CreateThread(0, 0, start_connection, 0, 0, &thread_id)) {
        printf("CreateThread failed\n");
        return 1;
    }
}

void build_send_buffer() {
    bytes_transfer = 0;
    int key, size;
    for (auto& entry : outbound_data) {
        size = entry.second.size();
        if (!size) continue;
        key = entry.first;
        *(network_buffer + bytes_transfer) = key;
        ++bytes_transfer;
        *(network_buffer + bytes_transfer) = size;
        ++bytes_transfer;
        while (!entry.second.empty()) {
            Data * temp = entry.second.front();
            entry.second.pop();
            size = temp->size + sizeof(int);
            std::memcpy(network_buffer + bytes_transfer, temp, size);
            bytes_transfer += size;
        }
    }
}

void build_recv_buffer() {
    int key, size, temp_size, entry_bytes;
    Data * temp;
    for (int i = 0; i < bytes_transfer; ++i) {
        key = *(network_buffer + i);
        ++i;
        size = *(network_buffer + i);
        entry_bytes = 0;
        for (int j = 0; j < size; ++j) {
            temp_size = *(network_buffer + i + entry_bytes);
            temp = reinterpret_cast<Data *>(malloc(sizeof(int) + temp_size));
            std::memcpy(temp, network_buffer + i + entry_bytes, sizeof(int) + temp_size);
            entry_bytes += sizeof(int) + temp_size;
            inbound_data[key].push(temp);
        }
        i += entry_bytes;
    }
}

void send_pending() {
    //build and send if one is not already waiting
    send(socketfd, network_buffer, bytes_transfer, 0);
}

void recv_pending() {
    bytes_transfer = 0;
    int trans;
    while ((trans = recv(socketfd, network_buffer + bytes_transfer, NET_BUFF_SIZE-bytes_transfer, 0))) {
        bytes_transfer += trans;
    }
    //build the recv buffer if enough was read in
}

bool has_data(int channel) {
    return inbound_data[channel].size() > 0;
}

void add_data(int channel, Data * data) {
    Data * temp = reinterpret_cast<Data *>(malloc(sizeof(int) + data->size));
    temp->size = data->size;
    std::memcpy(temp+sizeof(int), data->buffer, data->size);
    outbound_data[channel].push(temp);
}

void get_data(int channel, Data * data) {
    Data * temp = inbound_data[channel].front();
    inbound_data[channel].pop();
    data->size = temp->size;
    std::memcpy(data+sizeof(int), temp->buffer, temp->size);
}
