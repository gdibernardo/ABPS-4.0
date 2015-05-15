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
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

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
    
    close(log_descriptor);
    
    is_test_enabled = 0;
    
    return 1;
}


int prepare_for_logging(void)
{
    int return_value = lseek(log_descriptor, 0, SEEK_END);
    return return_value;
}



void ipv4_check_and_log_local_error_notify_with_test_identifier(ErrMsg *error_message, int test_identifier)
{
    for(error_message->c = CMSG_FIRSTHDR(error_message->msg); error_message->c; error_message->c = CMSG_NXTHDR(error_message->msg, error_message->c))
    {
        if((error_message->c->cmsg_level == IPPROTO_IP) && (error_message->c->cmsg_type == IP_RECVERR))
        {
            struct sockaddrin *from;
            
            error_message->ee = (struct sock_extended_err *) CMSG_DATA(error_message->c);
            
            from = (struct sockaddrin *) SO_EE_OFFENDER(error_message->ee);
            
            if((error_message->ee->ee_origin == SO_EE_ORIGIN_LOCAL_NOTIFY) && (error_message->ee->ee_errno == 0))
            {
                uint32_t identifier = ntohl(error_message->ee->ee_info);
                
                char packet_status;
                
                printf("Received notification for packet %" PRIu32 " \n", identifier);
                
                uint8_t acked = error_message->ee->ee_type;
                
                uint8_t more_frag = error_message->ee->ee_code;
                uint16_t frag_len = (error_message->ee->ee_data >> 16);
                uint16_t offset = ((error_message->ee->ee_data << 16) >> 16);
                
                if(acked)
                {
                    printf("packet with identifier %" PRIu32 " is acked \n", identifier);
                }
                else
                {
                    printf("packet with identifier %" PRIu32 " is not acked \n", identifier);
                }
                
                if(is_test_enabled)
                {
                    int return_value;
                    time_t current_time = time(NULL);
                    
                    char *log_line;
                    
                    if(acked)
                        asprintf(&log_line,"%sABPS testlib just received local notification - datagram identifier:%" PRIu32 " - more frag:%" PRIu8 " - frag length:%" PRIu16 " - offset:%" PRIu16 " test identifier:%d status:ACK\n", asctime(gmtime(&current_time)), identifier,more_frag,frag_len,offset, test_identifier);
                    else
                        asprintf(&log_line,"%sABPS testlib just received local notification - datagram identifier:%" PRIu32 " - more frag:%" PRIu8 " - frag length:%" PRIu16 " - offset:%" PRIu16 " test identifier:%d status:NACK\n", asctime(gmtime(&current_time)), identifier,more_frag,frag_len,offset, test_identifier);
                    
                    prepare_for_logging();
                    
                    return_value = write(log_descriptor, log_line, strlen(log_line));
                    
                    if(return_value == -1)
                    {
                        /* Error logging this line. */
                    }
                    
                    free(log_line);
                }
            }
        }
    }

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
                
                char packet_status;
                
                printf("Received notification for packet %d \n", identifier);
                
                uint8_t acked = error_message->ee->ee_type;
                
                if(acked)
                {
                    printf("packet with identifier %d is acked \n", identifier);
                }
                else
                {
                    printf("packet with identifier %d is not acked \n", identifier);
                }
                
                if(is_test_enabled)
                {
                    int return_value;
                    time_t current_time = time(NULL);
                    
                    char *log_line;
                    
                    if(acked)
                        asprintf(&log_line,"%sABPS testlib just received local notification - datagram identifier:%d - test identifier:%d status:ACK\n", asctime(gmtime(&current_time)), identifier, test_identifier);
                    else
                        asprintf(&log_line,"%sABPS testlib just received local notification - datagram identifier:%d - test identifier:%d status:NACK\n", asctime(gmtime(&current_time)), identifier, test_identifier);
                    
                    prepare_for_logging();
                    
                    return_value = write(log_descriptor, log_line, strlen(log_line));
                    
                    if(return_value == -1)
                    {
                        /* Error logging this line. */
                    }
                    
                    free(log_line);
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


void sent_packet_with_packet_and_test_identifier(uint32_t packet_identifier, int test_identifier)
{
    if(is_test_enabled)
    {
        int return_value;
        
        time_t current_time = time(NULL);
        char *log_line;
        asprintf(&log_line,"%sABPS testlib just sent packet - packet identifier %d - test identifier:%d \n", asctime(gmtime(&current_time)), packet_identifier,test_identifier);
        
        prepare_for_logging();
        
        return_value = write(log_descriptor, log_line, strlen(log_line));
        
        if(return_value == -1)
        {
            /* We have some error logging this line. */
            printf("testlib sent_packet_with_test_identifier error writing log line. \n");
            
        }
        
        free(log_line);
    }
}
