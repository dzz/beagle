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
SOCKET listen_socket = INVALID_SOCKET;
SOCKET accept_socket = INVALID_SOCKET;

static std::map<uint16_t, std::queue<std::pair<uint16_t, void *>>> outbound_data{};
static std::map<uint16_t, std::queue<std::pair<uint16_t, void *>>> inbound_data{};

static char in_buffer[NET_BUFF_SIZE] = { 0 };
static char out_buffer[NET_BUFF_SIZE] = { 0 };

WSAEVENT wsa_conn_event[1] = { 0 };
HANDLE send_sem;

//channel interaction helpsers
bool network::has_data(int channel) {
    return inbound_data[channel].size() > 0;
}

void network::add_data(int channel, void * type, uint16_t size) {
    void * temp = malloc(size);
    std::memcpy(temp, type, size);
	outbound_data[channel].push({size, temp});
}

void network::get_data(int channel, void * type) {
    const auto & temp = inbound_data[channel].front();
    inbound_data[channel].pop();
    std::memcpy(type, temp.second, temp.first);
    free(temp.second);
}

//buffer building helpers
//returns size built
uint16_t build_send_buffer() {
    char * buffer = out_buffer + sizeof(uint16_t);
    uint16_t total_size = sizeof(uint16_t);
    for (auto& entry : outbound_data) { //loop all the queues
        uint16_t key = entry.first;
        auto & queue = entry.second;
        uint16_t size = queue.size();
        if (!size) {
            continue;
        }
        //write channel key
        *(uint16_t *)buffer = key;
        buffer += sizeof(uint16_t);
        total_size += sizeof(uint16_t);
        //write the total entries
        *(uint16_t *)buffer = size;
        buffer += sizeof(uint16_t);
        total_size += sizeof(uint16_t);
        while (!queue.empty()) {
            auto temp = queue.front();
            queue.pop();
            *(uint16_t *)buffer = temp.first;
            buffer += sizeof(uint16_t);
            total_size += sizeof(uint16_t);
            std::memcpy(buffer, temp.second, temp.first);
            buffer += temp.first;
            total_size += temp.first;
            free(temp.second);
        }
    }
	*(uint16_t *)out_buffer = total_size;
	return total_size;
}
//takes in size to build from
void build_recv_buffer(uint16_t total_size) {
    char * buffer = in_buffer;
    while (buffer < in_buffer + total_size) {
        uint16_t key, entries;
        key = *(uint16_t *)buffer;
        buffer += sizeof(uint16_t);
        entries = *(uint16_t *)buffer;
        buffer += sizeof(uint16_t);
        for (int i = 0; i < entries; ++i) {
            uint16_t size;
            void * temp;
            size = *(uint16_t *)buffer;
            buffer += sizeof(uint16_t);
            temp = malloc(size);
            std::memcpy(temp, buffer, size);
            buffer += size;
			inbound_data[key].push({size, temp});
        }
    }
}

//trigger a send event on the send thread
void network::send_pending() {
    ReleaseSemaphore(send_sem, 1, 0);
}

void CALLBACK send_callback(DWORD err, DWORD bytes_sent, LPWSAOVERLAPPED, DWORD) {
    if (err) {
        printf("WSASend failed with error: %d\n", err);
    }
}

DWORD WINAPI send_thread(LPVOID) {
    send_sem = CreateSemaphore(NULL, 0, 1, NULL);
    WSAOVERLAPPED overlapped = { 0 };
    overlapped.hEvent = WSACreateEvent();

    DWORD sent_bytes;
    int rc, err;
    uint16_t total;

    WSABUF message_wsa_buf;

    message_wsa_buf.buf = out_buffer;
    while (WaitForSingleObject(send_sem, INFINITE) == WAIT_OBJECT_0) {
        total = build_send_buffer();
        message_wsa_buf.len = total;
        if ((rc = WSASend(accept_socket, &message_wsa_buf, 1, &sent_bytes, 0, &overlapped, send_callback)) == SOCKET_ERROR) {
            if ((err = WSAGetLastError()) == WSA_IO_PENDING) {
                //this may break but i think it makes sense
                WaitForSingleObject(overlapped.hEvent, INFINITE);
            }
            else {
                printf("WSASend failed with error: %d\n", err);
                return 1;
            }
        }//else it completed without delay
    }
    CloseHandle(send_sem);
    return 0;
}

void CALLBACK recv_callback(DWORD err, DWORD bytes_read, LPWSAOVERLAPPED, DWORD) {
    if (err) {
        printf("WSARecv callback failed with error: %d\n", err);
    }
}

uint16_t get_message_size() {
    DWORD bytes_read;
    DWORD flags = 0;
    int rc, err;
    OVERLAPPED recv_overlapped = { 0 };
    recv_overlapped.hEvent = WSACreateEvent();
    uint16_t total_size;
    WSABUF size_buf;

    size_buf.len = sizeof(uint16_t);
    size_buf.buf = (char *)&total_size;

    if ((rc = WSARecv(accept_socket, &size_buf, 1, &bytes_read, &flags, 
                    &recv_overlapped, recv_callback)) == SOCKET_ERROR) {
        if ((err = WSAGetLastError()) == WSA_IO_PENDING) {
            //the sleep allows the callback to be called on this thread
            if (SleepEx(INFINITE, 1) != WAIT_IO_COMPLETION) {
                closesocket(accept_socket);
                accept_socket = INVALID_SOCKET;
                return 0;//something not a callback woke us, quit
            }
        } else {
            printf("WSARecv msg size failed with error: %d\n", err);
        }
    }

    WSACloseEvent(recv_overlapped.hEvent);
    return total_size;
}
void fill_buffer(uint16_t size) {
    DWORD bytes_read;
    DWORD flags = 0;
    int rc, err;
    OVERLAPPED recv_overlapped = { 0 };
    recv_overlapped.hEvent = WSACreateEvent();
    WSABUF size_buf;

    size_buf.len = size;
    size_buf.buf = in_buffer;

    if ((rc = WSARecv(accept_socket, &size_buf, 1, &bytes_read, &flags, 
                    &recv_overlapped, recv_callback)) == SOCKET_ERROR) {
        if ((err = WSAGetLastError()) == WSA_IO_PENDING) {
            //the sleep allows the callback to be called on this thread
            if (SleepEx(INFINITE, 1) != WAIT_IO_COMPLETION) {
                closesocket(accept_socket);
                accept_socket = INVALID_SOCKET;
                return;//something not a callback woke us, quit
            }
        } else {
            printf("WSARecv fill failed with error: %d\n", err);
        }
    }
    WSACloseEvent(recv_overlapped.hEvent);
}

DWORD WINAPI recv_thread(LPVOID) {
    WSAEVENT recv_event = WSACreateEvent();

    //you may be thinking, is that a spin lock on the accept socket???
    //...
    //yes. there is too much other triggering happening to want to deal with it
    while (accept_socket == INVALID_SOCKET) Sleep(1);

    if (WSAEventSelect(accept_socket, recv_event, FD_READ | FD_CLOSE) == SOCKET_ERROR) {
        printf("WSAEventSelect failed with error: %d\n", WSAGetLastError());
        WSACloseEvent(recv_event);
        return 1;
    }

    while (WSAWaitForMultipleEvents(1, &recv_event, 1, WSA_INFINITE, 1) != WSA_WAIT_FAILED) {
        uint16_t message_size = get_message_size();
        if (WSAWaitForMultipleEvents(1, &recv_event, 1, WSA_INFINITE, 1) != WSA_WAIT_FAILED) {
            fill_buffer(message_size);
            build_recv_buffer(message_size);
        }
    }
    WSACloseEvent(recv_event);
	return 0;
}

void network::start_recv() {
    DWORD thread_id;
    if (!CreateThread(0, 0, recv_thread, 0, 0, &thread_id)) {
        printf("CreateThread failed\n");
    }
}
void network::start_send() {
    DWORD thread_id;
    if (!CreateThread(0, 0, send_thread, 0, 0, &thread_id)) {
        printf("CreateThread failed\n");
    }
}


//ANYTHING BELOW THIS LINE ACTUALLY WORKS DONT CHANGE IT
void network::init() {
    WSADATA wsa_data;
    int rc;
    rc = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (rc != 0) {
        printf("Unable to load Winsock: %d\n", rc);
    }
}

void network::close() {
    WSACleanup();
}
bool network::is_connected() {
    return accept_socket != INVALID_SOCKET;
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
int network::start_server() {
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int client_addr_size = sizeof(struct sockaddr_in);
    int rc;

    //make sure the hints struct is zeroed out
    SecureZeroMemory((PVOID)&hints, sizeof(struct addrinfo));

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

    rc = bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
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
    SecureZeroMemory((PVOID)&hints, sizeof(struct addrinfo));

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
        rc = connect(accept_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
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
int network::start_client(const char * address) {
    DWORD thread_id;
    if (!CreateThread(0, 0, start_connection, (LPVOID)address, 0, &thread_id)) {
        printf("CreateThread failed\n");
        return 1;
    }
    return 0;
}
