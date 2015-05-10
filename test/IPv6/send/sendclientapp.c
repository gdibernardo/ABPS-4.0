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

int main(int argc, char ** argv)
{
    if(argc < 3)
    {
        return 1;
    }
    
    char *address = argv[1];
    
    int port = atoi(argv[2]);
    
    printf("Ready to alloc shared instance with parameters %s %d",address,port);
    
    
    /* check for errror */
    instantiate_ipv6_shared_instance_by_address_and_port(address, port);
    char buffer[2000];
    memset(buffer,0,2000);
    strcpy(buffer,"hello from client app");
    send_packet_with_message(buffer, 2000);
    sleep(3);
    receive_local_error_notify();
    return 0;
}
