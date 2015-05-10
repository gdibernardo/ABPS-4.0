#include <stdio.h>
#include <unistd.h>



/* Initialization and release. */

int instantiate_ipv4_shared_instance_by_address_and_port(char *address, int port);


int instantiate_ipv6_shared_instance_by_address_and_port(char *address, int port);


void release_shared_instance(void);



/* Send and receive interfaces. */

uint32_t send_packet_with_message(char *message, int message_length);

uint32_t receive_local_error_notify();