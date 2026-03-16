#ifndef KERNEL_MONITOR_H
#define KERNEL_MONITOR_H

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

void kernel_monitor(void) {
    char cmd[128];
    int i;

    printf("Entering Pintos monitor\n");

    while (1) {

        printf("monitor> ");
        i = 0;

        while (1) {
            char c = input_getc();

            if(c == '\n' || c == '\r') {
                break;
            }

            //handle backspace 
            if((c == '\b' || c == 127) && i > 0) {
                i--;
                printf("\b \b");
                continue;
            }

            if (i < sizeof(cmd) - 1){
                cmd[i++] = c;
            }
            putchar(c);
        }

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



#endif