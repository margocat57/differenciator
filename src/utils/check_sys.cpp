#include "string.h"
#include <stdio.h>

bool is_cmd_for_sys_correct(const char* cmd_buffer, const char* allowed){
    if(!cmd_buffer || !allowed){    
        return false;
    }
    size_t allowed_len = strspn(cmd_buffer, allowed);
    return (allowed_len == strlen(cmd_buffer));
}