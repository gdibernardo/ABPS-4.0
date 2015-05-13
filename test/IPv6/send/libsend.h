#include <stdio.h>
#include <unistd.h>

#include "sendrecvUDP.h"

/* Need to add proper header file. */
#define SO_EE_ORIGIN_LOCAL_NOTIFY	5

/* Initialization and release. */

int instantiate_ipv4_shared_instance_by_address_and_port(char *address, int port);


int instantiate_ipv6_shared_instance_by_address_and_port(char *address, int port);


void release_shared_instance(void);



/* Send and receive notify interfaces. */

int send_packet_with_message(char *message, int message_length, uint32_t *identifier);

int receive_local_error_notify_with_error_message(ErrMsg *error_message);