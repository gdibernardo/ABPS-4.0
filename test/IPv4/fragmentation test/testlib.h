//
//  testlib.h
//  
//
//  Created by Gabriele Di Bernardo on 13/05/15.
//
//

#ifndef ____testlib__
#define ____testlib__

#include <stdio.h>

#include "sendrecvUDP.h"


/* Time struct used in testlib. */
typedef struct 
{
    long int milliseconds_time;
    
    char *human_readable_time_and_date;
    
} testlib_time;


/* Enable log at the supplied path. */
int enable_test_mode_with_path(char *path);

int disable_test_mode(void);

int get_test_identifier(void);

void sent_packet_with_packet_and_test_identifier(uint32_t packet_identifier, int test_identifier);

void check_and_log_local_error_notify_with_test_identifier(ErrMsg *error_message, int test_identifier,  int type, FILE *fp, int hopV);


/* Set current time at the supplied testlib structure. It's not thread-safe. */
void current_time_with_supplied_time(testlib_time *time_lib);


#endif /* defined(____testlib__) */
