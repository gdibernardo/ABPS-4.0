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
    if(argc < 4)
    {
        return 1;
    }
    
    char *address = argv[1];
    
    int port = atoi(argv[2]);
    int type = atoi(argv[3]); /* type of connection, ex: with/without hosts, indoor/outdoor */
    int hopV = 0;  /* for json file. If the file is empty, hop comma in the first row */
	int len=0; /* length of file */
    char *test_path;
    FILE *fp=NULL;
    if(argc > 4)
    {
        test_path = argv[4];
        enable_test_mode_with_path(test_path);
		fp=fopen(test_path, "r+");
    }
    
    int index;
    /* control size of file to iniziliaze json format. TODO: could make parsing */
	fseek(fp, 0, SEEK_END);
    len = (int)ftell(fp);
	if (len==0)
	{ 
		fseek( fp, 0, SEEK_SET  );
		fputs("{ \"pacchetti\": [ \n \n ]}", fp);
		hopV=1;
	}

    /* check for error */
    instantiate_ipv6_shared_instance_by_address_and_port(address, port);
    
    
    for(index = 0; index < 5000; index++)
    {
        int test_identifier = get_test_identifier();
        
        json_object *object = json_object_new_object();
    
        json_object *test_identifier_content = json_object_new_int(test_identifier);
        json_object *message_content = json_object_new_string("hello from client app!");
    
        json_object_object_add(object,"testIdentifier", test_identifier_content);
        json_object_object_add(object, "messageContent", message_content);
    
        const char *buffer = json_object_to_json_string(object);
    
        uint32_t identifier;
        
        send_packet_with_message(buffer, strlen(buffer), &identifier);
        
        /* Log packet just sent. */
        sent_packet_with_packet_and_test_identifier(identifier, test_identifier);
        
    
        ErrMsg *error_message = alloc_init_ErrMsg();
    
        if(receive_local_error_notify_with_error_message(error_message))
        {
            /* Application does not use local notification info. */
            check_and_log_local_error_notify_with_test_identifier(error_message, test_identifier, type, fp, hopV);
        }
        
        if (hopV==1)
		{
			hopV=0;
		}        

        free(object);
    
        free(error_message);
    }
    
    disable_test_mode();
    
    release_shared_instance();
    
    return 0;
}
