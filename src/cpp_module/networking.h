#ifndef NETWORKING_H
#define NETWORKING_H

#define NETWORK_SERVER 1
#define NETWORK_CLIENT 2
#define PORT "14332"
#define NET_BUFF_SIZE 65535

namespace network {
    struct Data {
        int size;
        void * buffer;
    };

    void init();
    void close();
    int start_server(int port);
    int start_client(const char * address, int port);
    void send_pending();
    void recv_pending();
    bool has_data(int channel);
    void add_data(int channel, Data * data);
    void get_data(int channel, Data * data);
}

#endif
