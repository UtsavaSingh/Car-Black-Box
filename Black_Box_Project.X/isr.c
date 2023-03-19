#include <xc.h>
#include "main.h"

extern char sec, min;
int return_time;

void __interrupt() isr(void)
{
    static unsigned int count = 0;
    
    if (TMR2IF == 1)
    {
        if (++count == 1250)
        {
            count = 0;
            if(sec > 0)
                sec--;
            if(sec == 0 && return_time > 0)
                return_time--;
            if(sec == 0 && min > 0)
            {
                min--;
                sec = 60;
            }
        }
        
        TMR2IF = 0;
    }
}