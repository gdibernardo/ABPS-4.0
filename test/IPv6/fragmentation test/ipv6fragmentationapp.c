//
//  ipv6fragmentationapp.c
//
//
//  Created by Gabriele Di Bernardo on 02/07/15.
//
//


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <json/json.h>

#include "libsend.h"
#include "testlib.h"


#define NUMBER_OF_PACKETS 5000

#define MESSAGE_LENGTH 1050


int main(int argc, char ** argv)
{
    if(argc < 4)
    {
        return 1;
    }
    
    char *address = argv[1];
    
    int port = atoi(argv[2]);
    
    int number_of_packets = 0;
    
    int stop_sending = 0;
    
    /* check for error */
    int file_descriptor = instantiate_ipv6_shared_instance_by_address_and_port(address, port);
    
    while (1)
    {
        if(!stop_sending)
        {
            int test_identifier = get_test_identifier();
            char stringaInvio[MESSAGE_LENGTH];
            memset(stringaInvio,'c',MESSAGE_LENGTH-1);
            stringaInvio[MESSAGE_LENGTH-1]='\0';
            
            json_object *object = json_object_new_object();
            
            json_object *test_identifier_content = json_object_new_int(test_identifier);
            json_object *message_content = json_object_new_string(stringaInvio);
            
            json_object_object_add(object,"testIdentifier", test_identifier_content);
            json_object_object_add(object, "messageContent", message_content);
            
            const char *buffer = json_object_to_json_string(object);
            
            uint32_t identifier;
            
            send_packet_with_message(buffer, strlen(buffer), &identifier);
            
            printf("Just got identifier from TED %" PRIu32 " \n", identifier);
            
            number_of_packets++;
        }
            if(number_of_packets == NUMBER_OF_PACKETS)
                stop_sending = 1;
                
            struct msghdr message[1];
            struct cmsghdr *cmsg;
            
            struct sock_extended_err *first_hop_transmission_notification;
            
            int return_value, current_errno;
            printf("read \n");
            do
            {
                return_value = recvmsg(file_descriptor, message, MSG_ERRQUEUE|MSG_DONTWAIT);
                
                current_errno = errno;
                
            } while ((return_value < 0) && (current_errno == EINTR));
        
            printf(strerror(errno));
            if(return_value < 0)
                continue;
            
            for(cmsg = CMSG_FIRSTHDR(&message); cmsg; cmsg = CMSG_NXTHDR(&message, cmsg))
            {
                /* IPv6 socket */
                
                if((cmsg->cmsg_level == IPPROTO_IPV6) && (cmsg->cmsg_type == IPV6_RECVERR))
                {
                    
                    first_hop_transmission_notification = (struct sock_extended_err *) CMSG_DATA(cmsg);
                    
                    
                    switch (first_hop_transmission_notification->ee_origin)
                    {
                            
                            /* new origin type introduced  */
                        case SO_EE_ORIGIN_LOCAL_NOTIFY:
                        {
                            if(first_hop_transmission_notification->ee_errno == 0)
                            {
                                uint32_t identifier = get_ted_identfier(first_hop_transmission_notification);
                                
                                uint8_t retry_count = get_ted_retry_count(first_hop_transmission_notification);
                                
                                printf("Received notification for packet %" PRIu32 " \n", identifier);
                                
                                uint8_t acked = get_ted_status(first_hop_transmission_notification);
                                
                                if(acked)
                                {
                                    printf("Your message with identifier %" PRIu32 " has been delivered to the first hop!", identifier);
                                }
                                else
                                {
                                    printf("Ups.. something went wrong! Your message with identifier %" PRIu32" has been lost. \n", identifier);
                                }
                            }
                        }
                        default:
                        {
                            if(first_hop_transmission_notification->ee_errno != 0)
                            {
                                printf(strerror(first_hop_transmission_notification->ee_errno));
                            }
                            break;
                        }
                    }
                    
                }
            }
        }
        
        return 0;
}