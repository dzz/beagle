#include "networking.h"
#include <windows.h>
#include <string>
#include <map>
#include <queue>

using namespace network;

int listen_socketfd = -1, socketfd = -1;

static std::map<int, std::queue<Data *>> outbound_data{};
static std::map<int, std::queue<Data *>> inbound_data{};

static int bytes_transfer = 0;
static char network_buffer[NET_BUFF_SIZE] = {0};

int start_server(int port) {
    return 0;
}

int start_client(const char * address, int port) {
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
    //send bytes_transfer of network_buffer
}

void recv_pending() {
    //read it into network_buffer and set bytes_transfer

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
