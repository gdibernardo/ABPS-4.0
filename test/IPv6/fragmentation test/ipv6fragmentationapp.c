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
            if(number_of_packets == NUMBER_OF_PACKETS)
            {
                stop_sending = 1
            }
        }
        
        
        
    }
    
    return 0;
}