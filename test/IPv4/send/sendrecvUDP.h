
/* sendrecvUDP.h */

#ifndef ____sendrecvUDP__
#define ____sendrecvUDP__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h> /* SOL_UDP define. */
#include <arpa/inet.h>
#include <linux/errqueue.h>
#include <errno.h>


#define SOCKET_ERROR   ((int)-1)

#define SIZEBYTE 70000

typedef unsigned int u32;


#define HAVE_IP_RECVERR 1

#if HAVE_IP_RECVERR

#include <sys/uio.h>
#else
#warning "HAVE_IP_RECVERR not defined"
		   printf ("setsockopt() IP_PROTO IP_RECVERR not defined\n");
#endif

typedef struct struct_errormessage
{
    union
    {
        struct sockaddr_in6	ipv6_name;
        struct sockaddr_in ipv4_name;
    } name;
    
    unsigned int namelen;
    
    struct cmsghdr *c;
    
    struct sock_extended_err *ee;
    
    char control[512];
    
    int	len_control;
    
    char errmsg[64 + 768];
    
    int	len_errmsg;
    
    struct msghdr msg[1];
    
    int	lenrecv;
    int	myerrno;
    
    int is_ipv6;
    
} ErrMsg;


int ipv4_sendmsg_udp(int udp_file_descriptor, const char *buffer, int length, struct sockaddr_in destination_address, uint32_t *pointer_for_identifier);

int ipv6_sendmsg_udp(int udp_file_descriptor, const char *buffer, int length, struct sockaddr_in6 destination_address, uint32_t *pointer_for_identifier);


ErrMsg* alloc_init_ErrMsg(void);


int ipv6_receive_error_message_no_wait(int descriptor, ErrMsg *em);

int ipv6_receive_error_message_wait(int descriptor, ErrMsg *em);



int ipv4_receive_error_message_no_wait(int descriptor, ErrMsg *em);

int ipv4_receive_error_message_wait(int descriptor, ErrMsg *em);



#endif /* defined(____sendrecvUDP__) */


