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
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <linux/errqueue.h>


#include "testlib.h"


static char *log_path;

static int log_descriptor;

static int shared_test_identifier = 0;

static int is_test_enabled = 0;







typedef struct testlib_list
{
	int id_test;
	testlib_time time_list;
    
	struct testlib_list * next;
    
}testlib_list;


static testlib_list *head = NULL;


void add_in_list(testlib_list *list)
{
	if(head == NULL)
		head = list;
	else
	{
		testlib_list *temp=head;
		while(temp->next != NULL)
			temp=temp->next;
        
		temp->next = list; 
    }
	return;
}


testlib_list* search_in_list(int id)
{
	testlib_list *return_value;
    
	if(head->id_test == id)
	{
		return_value = head;
		head = head->next;
		return return_value;
	}

	testlib_list * temp = head;
	
	while(temp->next != NULL)	
	{
		if((temp->next)->id_test == id)
		{
			return_value = temp->next;
			temp->next = (temp->next)->next;
			
			return_value->next = NULL;
			return return_value;
		}
	}
	return NULL;
}



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
                
                
                printf("Received notification for packet %" PRIu32 " \n", identifier);
                
                uint8_t acked = error_message->ee->ee_type;
                uint8_t retry_count = error_message->ee->ee_retry_count;
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
                    
                    testlib_time current_time;
                    
                    current_time_with_supplied_time(&current_time);
                    
					testlib_list *ptr = search_in_list(test_identifier);
                    
					long int difference = current_time.milliseconds_time - ptr->time_list.milliseconds_time;                    
                    
                    char *log_line;
                    
                    if(acked)
                        asprintf(&log_line,"%sABPS testlib just received local notification msec: %ld - difference: %ld  - datagram identifier:%" PRIu32 " - more frag:%" PRIu8 " - frag length:%" PRIu16 " - offset:%" PRIu16 " test identifier:%d  , retrycount:%" PRIu8 "  , status:ACK\n", current_time.human_readable_time_and_date, current_time.milliseconds_time,  difference, identifier,more_frag, frag_len, offset, test_identifier, retry_count);
                    else
                        asprintf(&log_line,"%sABPS testlib just received local notification msec: %ld - difference: %ld  - datagram identifier:%" PRIu32 " - more frag:%" PRIu8 " - frag length:%" PRIu16 " - offset:%" PRIu16 " test identifier:%d  , retrycount:%" PRIu8 "  ,status:NACK\n", current_time.human_readable_time_and_date, current_time.milliseconds_time, difference, identifier, more_frag, frag_len, offset, test_identifier, retry_count);
                    
                    prepare_for_logging();
                    
                    return_value = write(log_descriptor, log_line, strlen(log_line));
                    
                    if(return_value == -1)
                    {
                        /* Error logging this line. */
                    }
                    
                    free(log_line);
                    free(ptr);
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
				uint8_t retry_count = error_message->ee->ee_retry_count;
                
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
                    testlib_time current_time;
                    current_time_with_supplied_time(&current_time);
                    
					testlib_list *ptr = search_in_list(test_identifier);
                    
					long int difference = current_time.milliseconds_time - ptr->time_list.milliseconds_time;
                    char *log_line;
                    
                    if(acked)
                        asprintf(&log_line,"%sABPS testlib just received local notification msec: %ld - difference: %ld datagram identifier:%" PRIu32 " - test identifier:%d ,retry count:%" PRIu8 "  ,status:ACK\n", current_time.human_readable_time_and_date, current_time.milliseconds_time, difference, identifier, test_identifier, retry_count);
                    else
                        asprintf(&log_line,"%sABPS testlib just received local notification msec: %ld - difference: %ld datagram identifier:%" PRIu32 " - test identifier:%d ,retry count:%" PRIu8 "  ,status:NACK\n", current_time.human_readable_time_and_date, current_time.milliseconds_time, difference, identifier, test_identifier, retry_count);
                    
                    prepare_for_logging();
                    
                    return_value = write(log_descriptor, log_line, strlen(log_line));
                    
                    if(return_value == -1)
                    {
                        /* Error logging this line. */
                    }
                    
                    free(log_line);
					free(ptr);
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
        
        testlib_time current_time;
        
        current_time_with_supplied_time(&current_time);
        testlib_list *ptr = (testlib_list *) malloc(sizeof(testlib_list));
		ptr->next = NULL;
		ptr->time_list = current_time;
		ptr->id_test = test_identifier;

		add_in_list(ptr);
        
        char *log_line;
        asprintf(&log_line,"%sABPS testlib just sent packet msec: %ld - packet identifier %d - test identifier:%d \n", current_time.human_readable_time_and_date,current_time.milliseconds_time, packet_identifier,test_identifier);
        
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


void current_time_with_supplied_time(testlib_time *time_lib)
{
    struct timeval current_time;
   	gettimeofday(&current_time, NULL);

    time_lib->human_readable_time_and_date = asctime(gmtime(&current_time.tv_sec));
    
    time_lib->milliseconds_time = ((current_time.tv_sec) * 1000000L + current_time.tv_usec) / 1000;
}
