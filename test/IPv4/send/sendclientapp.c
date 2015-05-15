//
//  sendclientapp.c
//  
//
//  Created by Gabriele Di Bernardo on 10/05/15.
//
//

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <json/json.h>

#include "libsend.h"
#include "testlib.h"


#define NUMBER_OF_PACKETS 1000

#define MESSAGE_LENGTH 2000


int main(int argc, char ** argv)
{
    if(argc < 3)
    {
        return 1;
    }
    
    char *address = argv[1];
    
    int port = atoi(argv[2]);
    
    char *test_path;
    
    if(argc > 3)
    {
        test_path = argv[3];
        enable_test_mode_with_path(test_path);
    }
    
    int index;
    
    /* check for error */
    instantiate_ipv4_shared_instance_by_address_and_port(address, port);
    
    
    for(index = 0; index < 100; index++)
    {
        int test_identifier = get_test_identifier();
        
        json_object *object = json_object_new_object();
    
        json_object *test_identifier_content = json_object_new_int(test_identifier);
        json_object *message_content = json_object_new_string("hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!hello from client app!");
    
        json_object_object_add(object,"testIdentifier", test_identifier_content);
        json_object_object_add(object, "messageContent", message_content);
    
        const char *buffer = json_object_to_json_string(object);
    
//      char buffer[2000];
//    
//      memset(buffer,0,2000);
//    
//      strcpy(buffer,"hello from client app");
        
        uint32_t identifier;
        
        send_packet_with_message(buffer, strlen(buffer), &identifier);
        
        /* Log packet just sent. */
        sent_packet_with_packet_and_test_identifier(identifier, test_identifier);
        
    
        ErrMsg *error_message = alloc_init_ErrMsg();
    
        if(receive_local_error_notify_with_error_message(error_message))
        {
            /* Application does not use local notification info. */
            check_and_log_local_error_notify_with_test_identifier(error_message, test_identifier);
        }
        
        
        free(object);
    
        free(error_message);
    }
    
    disable_test_mode();
    
    release_shared_instance();
    
    return 0;
}
