#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "sendrecvUDP.h"




int shared_descriptor;

int is_shared_instance_instantiated;

int is_shared_instance_ipv6;



struct sockaddr_in ipv4_destination_address;

struct sockaddr_in6 ipv6_destination_address;


/* create IPv4 socket */

int create_ipv4_socket(char *address, int port, int *file_descriptor, struct sockaddr_in *destination_address)
{
    int error, option_value;
    
    struct sockaddr_in local_address;
    
    
    /* get datagram socket */
    
    *file_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    
    if(*file_descriptor == SOCKET_ERROR)
    {
        printf("Failed getting socket in function create_socket %s \n", strerror(errno));
        return errno;
    }
    
    local_address.sin_family = AF_INET;
    local_address.sin_addr.s_addr = htonl(INADDR_ANY);
    local_address.sin_port = htons(0);
    
    
    /* prepare destination sockaddr_in */
    
    destination_address->sin_family = AF_INET;
    inet_pton(AF_INET, address, &(destination_address->sin_addr));
    destination_address->sin_port = htons(port);
    
    
    /* configuring the socket */
    
    option_value = 1;
    error = setsockopt(*file_descriptor, SOL_SOCKET, SO_REUSEADDR, (char *)&option_value, sizeof(option_value));
    if(error == SOCKET_ERROR)
    {
        printf("setsockopt for reuse address failed \n");
        return errno;
    }
    
    error = setsockopt(*file_descriptor, IPPROTO_IP, IP_RECVERR, (char *)&option_value, sizeof(option_value));
    if(error == SOCKET_ERROR)
    {
        printf("setsockopt for IP_RECVERR failed \n");
        return errno;
    }
    
    return 1;
}



/* create IPv6 socket */

int create_ipv6_socket(char *address, int port, int *file_descriptor, struct sockaddr_in6 *destination_address)
{
    int error, option_value;
    
    struct sockaddr_in6 local_address;
    
    /* get datagram socket */
    
    *file_descriptor = socket(AF_INET6, SOCK_DGRAM, 0);
    
    if(*file_descriptor == SOCKET_ERROR)
    {
        printf("Failed getting socket in function create_socket %s \n", strerror(errno));
        return errno;
    }
    
    local_address.sin6_family = AF_INET6;
    local_address.sin6_addr = in6addr_any;
    local_address.sin6_port = htons(0);
    
    
    /* prepare destination sockaddr_in */
    
    destination_address->sin6_family = AF_INET6;
    inet_pton(AF_INET6, address, &(destination_address->sin6_addr));
    destination_address->sin6_port = htons(port);
    
    
    /* Probably this is not the proper way. */
    
    destination_address->sin6_scope_id = if_nametoindex("wlan0");
    
    
    /* configuring the socket */
    
    option_value = 1;
    error = setsockopt(*file_descriptor, SOL_SOCKET, SO_REUSEADDR, (char *)&option_value, sizeof(option_value));
    if(error == SOCKET_ERROR)
    {
        printf("setsockopt for reuse address failed \n");
        return errno;
    }
    
    error = setsockopt(*file_descriptor, IPPROTO_IP, IP_RECVERR, (char *)&option_value, sizeof(option_value));
    if(error == SOCKET_ERROR)
    {
        printf("setsockopt for IP_RECVERR failed \n");
        return errno;
    }
    
    return 1;
}


int instantiate_ipv4_shared_instance_by_address_and_port(char *address, int port)
{
    int error;
    
    is_shared_instance_ipv6 = 0;
    is_shared_instance_instantiated = 1;
    
    error = create_ipv4_socket(address,port,&shared_descriptor, &ipv4_destination_address);
    return error;
}


int instantiate_ipv6_shared_instance_by_address_and_port(char *address, int port)
{
    int error;
    
    is_shared_instance_ipv6 = 1;
    is_shared_instance_instantiated = 1;
    
    error = create_ipv6_socket(address,port,&shared_descriptor, &ipv6_destination_address);
    return shared_descriptor;
}


void release_shared_instance(void)
{
    shared_descriptor = -1;
    is_shared_instance_instantiated = 0;
    is_shared_instance_ipv6 = 0;
    
//    ipv4_destination_address = NULL;
//    ipv6_destionation_address = NULL;
}






/* Send and receive. */

int send_packet_with_message(const char *message, int message_length, uint32_t *identifier)
{
    int result_value;
    
    if(!is_shared_instance_instantiated)
        return -1;
    
    if(is_shared_instance_ipv6)
        result_value = ipv6_sendmsg_udp(shared_descriptor, message, message_length, ipv6_destination_address, identifier);
    else
        result_value = ipv4_sendmsg_udp(shared_descriptor, message, message_length, ipv4_destination_address, identifier);
    
    return result_value;
}


int receive_local_error_notify_with_error_message(ErrMsg *error_message)
{
    printf("receive local invoked \n");
    if(!is_shared_instance_instantiated)
        return -1;
    int return_value;
    
    if(is_shared_instance_ipv6)
        return_value = ipv6_receive_error_message_wait(shared_descriptor, error_message);
    else
        return_value = ipv4_receive_error_message_wait(shared_descriptor, error_message);
    
    return return_value;
}





