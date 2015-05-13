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



/* Enable log at the supplied path. */
int enable_test_mode_with_path(char *path);

int get_test_identifier(void);


void check_and_log_local_error_notify_with_test_identifier(ErrMsg *error_message, int test_identifier);


#endif /* defined(____testlib__) */
