#include "threads/init.h"
#include <console.h>
#include <debug.h>
#include <inttypes.h>
#include <limits.h>
#include <random.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "devices/kbd.h"
#include "devices/input.h"
#include "devices/serial.h"
#include "devices/shutdown.h"
#include "devices/timer.h"
#include "devices/vga.h"
#include "devices/rtc.h"
#include "threads/interrupt.h"
#include "threads/io.h"
#include "threads/loader.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/pte.h"
#include "threads/thread.h"

void kernel_monitor(void)
{
    //allocates space for cmd
    char cmd[128];
    int i;

    printf("Entering Pintos monitor\n");

    while (1) {

        printf("monitor> ");
        i = 0;

        //read keyboard input 
        while (1)
        {
            char c = input_getc();
            //when to end command
            if (c == '\n')
                break;
            //dont allow greater than allocated space    
            if (i < 127)
                cmd[i++] = c;
            putchar(c);
        }

        //handles commands
        cmd[i] = '\0';
        printf("\n");

        if (!strcmp(cmd, "threads")) {
            thread_print_stats();
        }
        else if (!strcmp(cmd, "exit")) {
            break;
        }
        else {
            printf("Unknown command\n");
        }
    }
}



