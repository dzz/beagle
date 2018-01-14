#ifndef NETWORKING_H
#define NETWORKING_H
#include <cstdint>

#define NETWORK_SERVER 1
#define NETWORK_CLIENT 2
#define PORT "14332"
#define NET_BUFF_SIZE 4096

namespace network {
    void init();
    void close();
    int start_server();
    int start_client(const char * address);
    void start_send();
    void start_recv();
    void send_pending();
    bool is_connected();
    //void recv_pending(); implicitly called whenever network data is avalible
    bool has_data(int channel);
    void add_data(int channel, void * type, uint16_t size);
    void get_data(int channel, void * type);
}

#endif
