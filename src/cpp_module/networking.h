#ifndef NETWORKING_H
#define NETWORKING_H

#define NETWORK_SERVER 1
#define NETWORK_CLIENT 2
#define PORT "14332"
#define NET_BUFF_SIZE 4096

#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#endif

typedef struct network_Data {
    uint8_t size;
    void * buffer;
} network_Data;

void network_init();
void network_close();
int network_start_server();
int network_start_client(const char * address);
void network_start_send();
void network_start_recv();
void network_send_pending();
//void recv_pending(); implicitly called whenever network data is avalible
int network_has_data(int channel);
void network_add_data(int channel, network_Data * data);
void network_get_data(int channel, network_Data * data);

#ifdef __cplusplus
}
#endif

#endif
