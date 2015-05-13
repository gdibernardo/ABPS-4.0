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


int get_test_identifier(void)
{
    return ++shared_test_identifier;
}


