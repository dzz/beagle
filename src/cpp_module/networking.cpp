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

using namespace network;

SOCKET listen_socketfd = -1, socketfd = -1;

static std::map<int, std::queue<Data *>> outbound_data{};
static std::map<int, std::queue<Data *>> inbound_data{};

static int bytes_transfer = 0;
static char network_buffer[NET_BUFF_SIZE] = {0};

void init() {
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2,2), &wsa_data);
}

void close() {
    WSACleanup();
}

int start_server(int port) {
    listen_socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, PORT, &hints, &result);

    listen_socketfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    bind( listen_socketfd, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);
    listen(listen_socketfd, 5);
    socketfd = accept(listen_socketfd,0,0);
    return 0;
}

int start_client(const char * address, int port) {
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    getaddrinfo(address, PORT, &hints, &result);
    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
        // Create a SOCKET for connecting to server
        socketfd = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        // Connect to server.
        if (connect( socketfd , ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) { 
            closesocket(socketfd);
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    return 0;
}

void send_pending() {
    bytes_transfer = 0;
    int key, size;
    for (auto entry : outbound_data) {
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
    send(socketfd, network_buffer, bytes_transfer, 0);
}

void recv_pending() {
    bytes_transfer = 0;
    int trans;
    while ((trans = recv(socketfd, network_buffer + bytes_transfer, NET_BUFF_SIZE-bytes_transfer, 0))) {
        bytes_transfer += trans;
    }

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
