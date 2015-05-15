//
//  testlib.c
//  
//
//  Created by Gabriele Di Bernardo on 13/05/15.
//
//

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "testlib.h"


static char *log_path;

static int log_descriptor;

static int shared_test_identifier = 0;

static int is_test_enabled = 0;


int get_test_identifier(void)
{
    return ++shared_test_identifier;
}



int enable_test_mode_with_path(char *path)
{
    
    int log_file_descriptor = open(path, O_WRONLY | O_CREAT | O_APPEND, 0777);
    if(log_file_descriptor == -1)
        return log_file_descriptor;
    
    log_path = path;
    
    log_descriptor = log_file_descriptor;
    
    is_test_enabled = 1;
    
    return 1;
}



int disable_test_mode(void)
{
    log_path = NULL;
    is_test_enabled = 0;
    
    return 1;
}




void ipv4_check_and_log_local_error_notify_with_test_identifier(ErrMsg *error_message, int test_identifier)
{
    
}


void ipv6_check_and_log_local_error_notify_with_test_identifier(ErrMsg *error_message, int test_identifier)
{
    for(error_message->c = CMSG_FIRSTHDR(error_message->msg); error_message->c; error_message->c = CMSG_NXTHDR(error_message->msg, error_message->c))
    {
        if((error_message->c->cmsg_level == IPPROTO_IPV6) && (error_message->c->cmsg_type == IPV6_RECVERR))
        {
            struct sockaddrin_6 *from;
            
            error_message->ee = (struct sock_extended_err *) CMSG_DATA(error_message->c);
            
            from = (struct sockaddrin_6 *) SO_EE_OFFENDER(error_message->ee);
            
            if((error_message->ee->ee_origin == SO_EE_ORIGIN_LOCAL_NOTIFY) && (error_message->ee->ee_errno == 0))
            {
                uint32_t identifier = ntohl(error_message->ee->ee_info);
                
                printf("received notification for packet %d \n", identifier);
    
                if(is_test_enabled)
                {
                    time_t current_time = time(NULL);
                    char *log_line;
                    asprintf(&log_line,"%s - datagram identifier:%d - test identifier:%d status:\n",asctime(gmtime(&current_time)), identifier, test_identifier);
                    
                    printf("%s",log_line);
                }
            }
        }
    }
}



void check_and_log_local_error_notify_with_test_identifier(ErrMsg *error_message, int test_identifier)
{
    if(error_message->is_ipv6)
    {
        ipv6_check_and_log_local_error_notify_with_test_identifier(error_message, test_identifier);
    }
    else
    {
        ipv4_check_and_log_local_error_notify_with_test_identifier(error_message, test_identifier);
    }
}


void sent_packet_with_test_identifier(int test_identifier)
{
    if(is_test_enabled)
    {
        time_t current_time = time(NULL);
        char *log_line;
        asprintf(&log_line,"ABPS testlib just sent packet %s - test identifier:%d \n",asctime(gmtime(&current_time)), test_identifier);
        
        printf("%s",log_line);
    }
}
