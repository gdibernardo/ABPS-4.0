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


#include <limits.h>

#define NUMBER_OF_PACKETS 100

#define MESSAGE_LENGTH 4500




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
    instantiate_ipv4_shared_instance_by_address_and_port(address, port);
  
    int notifications = 0;
    
    for(index = 0; index < NUMBER_OF_PACKETS; index++)
    {
        int test_identifier = get_test_identifier();
        char stringaInvio[MESSAGE_LENGTH];
		memset(stringaInvio,'c',MESSAGE_LENGTH-1);
		stringaInvio[MESSAGE_LENGTH-1]='\0';
        json_object *object = json_object_new_object();
    	
        json_object *test_identifier_content = json_object_new_int(test_identifier);
        json_object *message_content = json_object_new_string(stringaInvio);


		//json_object *an_content = json_object_new_string(stringaInvio);

		json_object_object_add(object,"testIdentifier", test_identifier_content);
        json_object_object_add(object, "messageContent", message_content);
		//json_object_object_add(object, "anContent", an_content);
		

    		
        
        const char *buffer = json_object_to_json_string(object);
        uint32_t identifier;
        send_packet_with_message(buffer, strlen(buffer), &identifier);
        
        printf("Just got identifier from TED %" PRIu32 " \n", identifier);
        
        /* Log packet just sent. */
        sent_packet_with_packet_and_test_identifier(identifier, test_identifier);
    
        
        ErrMsg *error_message = alloc_init_ErrMsg();
    
        if(receive_local_error_notify_with_error_message(error_message))
        {
            /* Application does not use local notification info. */
            check_and_log_local_error_notify_with_test_identifier(error_message, test_identifier, type, fp, hopV);
            notifications++;
            printf("notification: %d \n", notifications);
        }
        
        if (hopV==1)
		{
			hopV=0;
		}        
		free(object);
    
        free(error_message);
    }
    while (1)
    {
        /*  */
        ErrMsg *error_message = alloc_init_ErrMsg();
        
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
                    
                    printf("ACK value %" PRIu8 " \n - retry count: %" PRIu8 " - MF:%" PRIu16 " - fragment length: %" PRIu16 " - offset:%" PRIu16 "\n",acked,retry_count,more_frag,frag_len,offset);
                    notifications++;
                    printf("notification: %d \n", notifications);
                }
            }
        }

        
        free(error_message);
        
    }
    
    disable_test_mode();
    
    release_shared_instance();
    
    return 0;
}
