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

//file wide networking variables
SOCKET listen_socket = INVALID_SOCKET;
SOCKET accept_socket = INVALID_SOCKET;

static std::map<int, std::queue<network_Data *>> outbound_data{};
static std::map<int, std::queue<network_Data *>> inbound_data{};

static char in_buffer[NET_BUFF_SIZE] = {0};
static char out_buffer[NET_BUFF_SIZE] = {0};

WSABUF in_wsa_buffer;
WSABUF out_wsa_buffer;

uint16_t bytes_transfer = 0;

WSAEVENT wsa_conn_event[1] = {0};
HANDLE send_event;

void network_init() {
    WSADATA wsa_data;
    int rc;
    rc = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (rc != 0) {
        printf("Unable to load Winsock: %d\n", rc);
    }
}

void network_close() {
    WSACleanup();
}

//buffer building helpers
void build_send_buffer() {
    bytes_transfer = 0;
    int key, size;
    for (auto& entry : outbound_data) {
        size = entry.second.size();
        if (!size) continue;
        key = entry.first;
        *(out_buffer + bytes_transfer) = key;
        ++bytes_transfer;
        *(out_buffer + bytes_transfer) = size;
        ++bytes_transfer;
        while (!entry.second.empty()) {
            network_Data * temp = entry.second.front();
            entry.second.pop();
            size = temp->size + sizeof(int);
            std::memcpy(out_buffer + bytes_transfer, temp, size);
            bytes_transfer += size;
        }
    }
}

void build_recv_buffer() {
    int key, size, temp_size, entry_bytes;
    network_Data * temp;
    for (int i = 0; i < bytes_transfer; ++i) {
        key = *(in_buffer + i);
        ++i;
        size = *(in_buffer + i);
        entry_bytes = 0;
        for (int j = 0; j < size; ++j) {
            temp_size = *(in_buffer + i + entry_bytes);
            temp = reinterpret_cast<network_Data *>(malloc(sizeof(int) + temp_size));
            std::memcpy(temp, in_buffer + i + entry_bytes, sizeof(int) + temp_size);
            entry_bytes += sizeof(int) + temp_size;
            inbound_data[key].push(temp);
        }
        i += entry_bytes;
    }
}

//trigger a send event on the send thread
void network_send_pending() {
    SetEvent(&send_event);
}

DWORD WINAPI send_thread(LPVOID) {
    send_event = CreateEvent(0, 1, 0, 0);

    WSAOVERLAPPED overlapped = {0};
    overlapped.hEvent = WSACreateEvent();

    WSABUF wsd;
    wsd.buf = out_buffer;

    DWORD sent_bytes;
    int rc, err;

    while(WaitForSingleObject(send_event, INFINITE) == WAIT_OBJECT_0) {
        wsd.len = bytes_transfer;
        build_send_buffer();
        if ((rc = WSASend(accept_socket, &wsd, 1, &sent_bytes, 0, &overlapped, 0)) == SOCKET_ERROR) {
            if ((err = WSAGetLastError()) == WSA_IO_PENDING) {
                //this may break but i think it makes sense
                WaitForSingleObject(overlapped.hEvent, INFINITE);
            } else {
                printf("WSASend failed with error: %d\n", err);
                return 1;
            }
        }//else it completed without delay
    }
    CloseHandle(send_event);
    return 0;
}
void CALLBACK recv_callback(DWORD err, DWORD bytes_read, LPWSAOVERLAPPED, DWORD) {
    if (!err) {
        bytes_transfer = bytes_read;
        build_recv_buffer();
    } else {
        printf("WSARecv failed with error: %d\n", err);
    }
}
DWORD WINAPI recv_thread(LPVOID) {
    WSAEVENT recv_event = WSACreateEvent();

    WSAOVERLAPPED overlapped = {0};
    overlapped.hEvent = WSACreateEvent();

    WSABUF wsd;
    wsd.buf = in_buffer;

    DWORD recv_bytes;
    int rc, err;
    if (WSAEventSelect(accept_socket, recv_event, FD_READ | FD_CLOSE) == SOCKET_ERROR) {
        printf("WSAEventSelect failed with error: %d\n", WSAGetLastError());
        WSACloseEvent(recv_event);
        return 0;
    }

    while(WSAWaitForMultipleEvents(1, &recv_event, 1, WSA_INFINITE, 1) != WSA_WAIT_FAILED) {
        wsd.len = NET_BUFF_SIZE;
        if ((rc = WSARecv(accept_socket, &wsd, 1, &recv_bytes, 0, &overlapped, 0)) == SOCKET_ERROR) {
            if ((err = WSAGetLastError()) == WSA_IO_PENDING) {
                //this may break but i think it makes sense
                //WaitForSingleObject(overlapped.hEvent, INFINITE);
                if (SleepEx(INFINITE, 1) != WAIT_IO_COMPLETION) {
                    closesocket(accept_socket);
                    accept_socket = INVALID_SOCKET;
                    return 1;//something not a callback woke us, quit
                }
            } else {
                printf("WSASend failed with error: %d\n", err);
                return 1;
            }
        } else {
            //completed immediately
            bytes_transfer = recv_bytes;
            build_recv_buffer();
        }
    }
    CloseHandle(send_event);
    return 0;
}

void network_start_recv() {
    DWORD thread_id;
    if (!CreateThread(0, 0, recv_thread, 0, 0, &thread_id)) {
        printf("CreateThread failed\n");
    }
}
void network_start_send() {
    DWORD thread_id;
    if (!CreateThread(0, 0, send_thread, 0, 0, &thread_id)) {
        printf("CreateThread failed\n");
    }
}
//accepting thread
DWORD WINAPI accept_connection(LPVOID param) {
    *wsa_conn_event = WSACreateEvent();

    if (WSAEventSelect(listen_socket, *wsa_conn_event, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR) {
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

    if (WSAWaitForMultipleEvents(1, wsa_conn_event, 1, WSA_INFINITE, 1) == WSA_WAIT_FAILED) {
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

//basic winsock setup
int network_start_server() {
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int rc;

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
        accept_socket = INVALID_SOCKET;
        return 1;
    }
    freeaddrinfo(result);

    //might want to later increase the number of connections to accept at once
    rc = listen(listen_socket, 1);
    if (rc == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        accept_socket = INVALID_SOCKET;
        return 1;
    }

    //create thread for handling overlapped connection requests
    DWORD thread_id;
    if (!CreateThread(0, 0, accept_connection, 0, 0, &thread_id)) {
        printf("CreateThread failed\n");
        closesocket(listen_socket);
        accept_socket = INVALID_SOCKET;
        return 1;
    }
    return 0;
}

//connection handling thread
DWORD WINAPI start_connection(LPVOID address) {
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int err = 0;
    int rc;
    // Make sure the hints struct is zeroed out
    SecureZeroMemory((PVOID) &hints, sizeof (struct addrinfo));

    // Initialize the hints to retrieve the server address for IPv4
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    rc = getaddrinfo((const char *)address, PORT, &hints, &result);
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
            accept_socket = INVALID_SOCKET;
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
    return 0;
}
//initialize winsoc in a thread
int network_start_client(const char * address) {
    DWORD thread_id;
    if (!CreateThread(0, 0, start_connection, (LPVOID)address, 0, &thread_id)) {
        printf("CreateThread failed\n");
        return 1;
    }
    return 0;
}

int network_has_data(int channel) {
    return inbound_data[channel].size();
}

void network_add_data(int channel, network_Data * data) {
    network_Data * temp = reinterpret_cast<network_Data *>(malloc(sizeof(int) + data->size));
    temp->size = data->size;
    std::memcpy(temp+sizeof(int), data->buffer, data->size);
    outbound_data[channel].push(temp);
}

void network_get_data(int channel, network_Data * data) {
    network_Data * temp = inbound_data[channel].front();
    inbound_data[channel].pop();
    data->size = temp->size;
    std::memcpy(data+sizeof(int), temp->buffer, temp->size);
}
