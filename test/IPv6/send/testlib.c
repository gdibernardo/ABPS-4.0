//
//  testlib.c
//  
//
//  Created by Gabriele Di Bernardo on 13/05/15.
//
//

#include "testlib.h"


static char *log_path;

static int shared_test_identifier = 0;

static int is_test_enabled = 0;


int get_test_identifier(void)
{
    return ++shared_test_identifier;
}



int enable_test_mode_with_path(char *path)
{
    log_path = path;
    is_test_enabled = 1;
    
    return 1;
}


int disable_test_mode(void)
{
    log_path = NULL;
    is_test_enabled = 0;
}


void check_and_log_local_error_notify_with_test_identifier(ErrMsg *error_message, int test_identifier)
{

}