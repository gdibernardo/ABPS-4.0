/* sendrecvUDP.c */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h> /* SOL_UDP */
#include <arpa/inet.h>
#include <errno.h>
#include <linux/types.h>
#include <linux/errqueue.h>
#include <linux/socket.h> /* ABPS_CMSG_TYPE */
#include <sys/uio.h>

#include "sendrecvUDP.h"

#define ABPS_CMSG_TYPE 111

int ipv4_sendmsg_udp(int udp_file_descriptor, const char *buffer, int length, struct sockaddr_in destination_address, uint32_t *pointer_for_identifier)
{
    int result_value;
    
    char *pointer;
    
    char ancillary_buffer[CMSG_SPACE(sizeof (pointer_for_identifier))];
    
    struct iovec iov[3];
    
    struct msghdr message_header;
    
    struct cmsghdr *cmsg;
    
    iov[0].iov_base = (void *) buffer;
    iov[0].iov_len = length;
    
    
    message_header.msg_name = (void *) &destination_address;
    message_header.msg_namelen = sizeof (destination_address);
    message_header.msg_iov = iov;
    message_header.msg_iovlen = 1;
    
    message_header.msg_control = ancillary_buffer;
    message_header.msg_controllen = sizeof(ancillary_buffer);
    
    cmsg = CMSG_FIRSTHDR(&message_header);
    
    cmsg->cmsg_level = SOL_UDP;
    cmsg->cmsg_type = ABPS_CMSG_TYPE;
    
    cmsg->cmsg_len = CMSG_LEN(sizeof(pointer_for_identifier));
    
    pointer = (char *) CMSG_DATA(cmsg);
    
    memcpy(pointer, &pointer_for_identifier, sizeof(pointer_for_identifier));
    
    message_header.msg_controllen = cmsg->cmsg_len;
    
    
    /* Prepare for sending. */
    
    result_value = sendmsg(udp_file_descriptor, &message_header, MSG_NOSIGNAL);
    if(result_value < 0)
        printf("Error sending packt in ipv4_sendmsg_udp. \n");
    
    return result_value;
}


int ipv6_sendmsg_udp(int udp_file_descriptor, const char *buffer, int length, struct sockaddr_in6 destination_address, uint32_t *pointer_for_identifier)
{
    int result_value;
    
    char *pointer;
    
    char ancillary_buffer[CMSG_SPACE(sizeof (pointer_for_identifier))];
    
    struct iovec iov[3];
    
    struct msghdr message_header;
    
    struct cmsghdr *cmsg;
    
    iov[0].iov_base = (void *) buffer;
    iov[0].iov_len = length;
    
    
    message_header.msg_name = (void *) &destination_address;
    message_header.msg_namelen = sizeof (destination_address);
    message_header.msg_iov = iov;
    message_header.msg_iovlen = 1;
    
    message_header.msg_control = ancillary_buffer;
    message_header.msg_controllen = sizeof(ancillary_buffer);
    
    cmsg = CMSG_FIRSTHDR(&message_header);
    
    cmsg->cmsg_level = SOL_UDP;
    cmsg->cmsg_type = ABPS_CMSG_TYPE;
    
    cmsg->cmsg_len = CMSG_LEN(sizeof(pointer_for_identifier));
    
    pointer = (char *) CMSG_DATA(cmsg);
    
    memcpy(pointer, &pointer_for_identifier, sizeof(pointer_for_identifier));
    
    message_header.msg_controllen = cmsg->cmsg_len;
    
    
    /* Prepare for sending. */
    
    result_value = sendmsg(udp_file_descriptor, &message_header, MSG_NOSIGNAL);
    if(result_value < 0)
        printf("Error sending packt in ipv4_sendmsg_udp. \n");
    
    return result_value;
}




ErrMsg *alloc_init_ErrMsg(void)
{
    ErrMsg *em;
    em = (ErrMsg*) malloc(sizeof(ErrMsg));
    if(em == NULL)
        return(NULL);
    else
    {
        memset(&(em->msg),0,sizeof(em->msg));
        em->lenrecv=0;
        em->myerrno=0;
        
        return(em);
    }
}


int ipv4_receive_error_message_no_wait(int descriptor, ErrMsg *em)
{
    struct iovec iov[1];
    int return_value;
    
    memset(&(em->name.ipv4_name),0,sizeof(em->name.ipv4_name));
    em->name.ipv4_name.sin_family = AF_INET;
    em->namelen = sizeof(em->name.ipv4_name);
    
    em->is_ipv6 = 0;
    
    return_value = getsockname(descriptor, (struct sockaddr *)&(em->name.ipv4_name), &(em->namelen));
    if(return_value != 0)
    {
        perror("getsockname failed in ipv4_receive_error_message_no_wait. \n");
        return 0;
    }
    
    do
    {
        memset(&(em->msg),0,sizeof(em->msg));
        em->msg->msg_name = &(em->name);
        em->msg->msg_namelen = sizeof(em->name);
        em->msg->msg_iov = iov;
        em->msg->msg_iovlen = 1;
        iov->iov_base = em->errmsg;
        iov->iov_len = sizeof(em->errmsg);
        em->msg->msg_control = em->control;
        em->msg->msg_controllen = sizeof(em->control);
        
        return_value = recvmsg(descriptor, em->msg, MSG_ERRQUEUE|MSG_DONTWAIT);
        em->myerrno=errno;
        
    } while ((return_value < 0) && (em->myerrno == EINTR));
    
    if(return_value < 0)
    {
        if(em->myerrno == EAGAIN)
        {
            /* No message available on error queue. */
            return 0;
        }
        else
        {
            errno = em->myerrno;
            perror("ipv4_receive_error_message_no_wait failed with error \n");
            return -1;
        }
    }
    else
    {
        if ((em->msg->msg_flags & MSG_ERRQUEUE) != MSG_ERRQUEUE)
        {
            printf("recvmsg: no errqueue\n");
            return(0);
        }
        else
            if (em->msg->msg_flags & MSG_CTRUNC)
            {
                printf("recvmsg: extended error was truncated\n");
                return(0);
            }
            else
                return(1); // read, ok
    }

}


int ipv4_receive_error_message_wait(int descriptor, ErrMsg *em)
{
    struct iovec iov[1];
    int return_value;
    
    memset(&(em->name.ipv4_name),0,sizeof(em->name.ipv4_name));
    em->name.ipv4_name.sin_family = AF_INET;
    em->namelen = sizeof(em->name.ipv4_name);
    
    em->is_ipv6 = 0;
    
    return_value = getsockname(descriptor, (struct sockaddr *)&(em->name.ipv4_name), &(em->namelen));
    if(return_value != 0)
    {
        perror("getsockname failed in ipv4_receive_error_message_no_wait. \n");
        return 0;
    }
    
    do
    {
        memset(&(em->msg),0,sizeof(em->msg));
        em->msg->msg_name = &(em->name);
        em->msg->msg_namelen = sizeof(em->name);
        em->msg->msg_iov = iov;
        em->msg->msg_iovlen = 1;
        iov->iov_base = em->errmsg;
        iov->iov_len = sizeof(em->errmsg);
        em->msg->msg_control = em->control;
        em->msg->msg_controllen = sizeof(em->control);
        
        return_value = recvmsg(descriptor, em->msg, MSG_ERRQUEUE);
        em->myerrno=errno;
        
    } while ((return_value < 0) && ((em->myerrno == EINTR) || (em->myerrno == EAGAIN)));
    
    if(return_value < 0)
    {
        if(em->myerrno == EAGAIN)
        {
            /* No message available on error queue. */
            return 0;
        }
        else
        {
            errno = em->myerrno;
            perror("ipv4_receive_error_message_no_wait failed with error \n");
            return -1;
        }
    }
    else
    {
        if ((em->msg->msg_flags & MSG_ERRQUEUE) != MSG_ERRQUEUE)
        {
            printf("recvmsg: no errqueue\n");
            return(0);
        }
        else
            if (em->msg->msg_flags & MSG_CTRUNC)
            {
                printf("recvmsg: extended error was truncated\n");
                return(0);
            }
            else
                return(1); // read, ok
    }
    
}


int ipv6_receive_error_message_no_wait(int descriptor, ErrMsg *em)
{
    struct iovec iov[1];
    int return_value;
    
    memset(&(em->name.ipv6_name),0,sizeof(em->name.ipv6_name));
    em->name.ipv6_name.sin6_family = AF_INET6;
    em->namelen = sizeof(em->name.ipv6_name);
   
    em->is_ipv6 = 1;
    
    return_value = getsockname(descriptor, (struct sockaddr *)&(em->name.ipv6_name), &(em->namelen));
    if(return_value != 0)
    {
        perror("getsockname failed in ipv6_receive_error_message_no_wait. \n");
        return 0;
    }
    
    do
    {
        memset(&(em->msg),0,sizeof(em->msg));
        em->msg->msg_name = &(em->name);
        em->msg->msg_namelen = sizeof(em->name);
        em->msg->msg_iov = iov;
        em->msg->msg_iovlen = 1;
        iov->iov_base = em->errmsg;
        iov->iov_len = sizeof(em->errmsg);
        em->msg->msg_control = em->control;
        em->msg->msg_controllen = sizeof(em->control);
        
        return_value = recvmsg(descriptor, em->msg, MSG_ERRQUEUE|MSG_DONTWAIT);
        em->myerrno=errno;
        
    } while ((return_value < 0) && (em->myerrno == EINTR));
    
    if(return_value < 0)
    {
        if(em->myerrno == EAGAIN)
        {
            /* No message available on error queue. */
            return 0;
        }
        else
        {
            errno = em->myerrno;
            perror("ipv6_receive_error_message_no_wait failed with error \n");
            return -1;
        }
    }
    else
    {
        if ((em->msg->msg_flags & MSG_ERRQUEUE) != MSG_ERRQUEUE)
        {
            printf("recvmsg: no errqueue\n");
            return(0);
        }
        else
            if (em->msg->msg_flags & MSG_CTRUNC)
            {
                printf("recvmsg: extended error was truncated\n");
                return(0);
            }
            else
                return(1); // read, ok
    }
}


int ipv6_receive_error_message_wait(int descriptor, ErrMsg *em)
{
    struct iovec iov[1];
    int return_value;
    
    memset(&(em->name.ipv6_name),0,sizeof(em->name.ipv6_name));
    em->name.ipv6_name.sin6_family = AF_INET6;
    em->namelen = sizeof(em->name.ipv6_name);
    
    em->is_ipv6 = 1;
    
    return_value = getsockname(descriptor, (struct sockaddr *) &(em->name.ipv6_name), &(em->namelen));
    if(return_value != 0)
    {
        perror("getsockname failed in ipv6_receive_error_message_no_wait. \n");
        return 0;
    }
    
    do
    {
        memset(&(em->msg),0,sizeof(em->msg));
        em->msg->msg_name = &(em->name);
        em->msg->msg_namelen = sizeof(em->name);
        em->msg->msg_iov = iov;
        em->msg->msg_iovlen = 1;
        iov->iov_base = em->errmsg;
        iov->iov_len = sizeof(em->errmsg);
        em->msg->msg_control = em->control;
        em->msg->msg_controllen = sizeof(em->control);
        
        return_value = recvmsg(descriptor, em->msg, MSG_ERRQUEUE);
        em->myerrno=errno;
        
    } while ((return_value < 0) && ((em->myerrno == EINTR) || (em->myerrno == EAGAIN)));
    
    if(return_value < 0)
    {
        if(em->myerrno == EAGAIN)
        {
            /* No message available on error queue. */
            return 0;
        }
        else
        {
            errno = em->myerrno;
            perror("ipv6_receive_error_message_no_wait failed with error \n");
            return -1;
        }
    }
    else
    {
        if ((em->msg->msg_flags & MSG_ERRQUEUE) != MSG_ERRQUEUE)
        {
            printf("recvmsg: no errqueue\n");
            return(0);
        }
        else
            if (em->msg->msg_flags & MSG_CTRUNC)
            {
                printf("recvmsg: extended error was truncated\n");
                return(0);
            }
            else
                return(1); // read, ok
    }
}



