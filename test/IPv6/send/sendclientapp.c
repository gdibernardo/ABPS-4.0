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

#include "libsend.h"


void check_and_log_local_error_notify(ErrMsg *error_message)
{
    
}


int ipv4_perform_log()
{

}



int ipv6_perform_log()
{
    
}



int main(int argc, char ** argv)
{
    if(argc < 3)
    {
        return 1;
    }
    
    char *address = argv[1];
    
    int port = atoi(argv[2]);
    
    printf("Ready to alloc shared instance with parameters %s %d",address,port);
    /* check for error */
    instantiate_ipv6_shared_instance_by_address_and_port(address, port);

    unsigned char buffer[2000];
    
    memset(buffer,0,2000);
    
    strcpy(buffer,"hello from client app");
    
    send_packet_with_message(buffer, 2000);
    
    sleep(2);
    
    ErrMsg *error_message = allocinit_ErrMsg();
    
    receive_local_error_notify_with_error_message(error_message);
    
    free(error_message);
    
    release_shared_instance();
    
    return 0;
}
