/*
 * File:   main.c
 * Author: Hamza N, Yousef H, Rumaisa T
 * Created on November 19, 2022, 9:00 PM
 */

// MPLAB header libraries
#include <xc.h>
#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

// User header files
#include "Configure.h"
#include "GPIO.h"
#include "IOs.h"
#include "TimeDelay.h"
#include "ChangeClk.h"
#include "UART2.h"

//Preprocessor directives - Configuration bits for MCU start up
#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor disabled

// MACROS for Idle, Sleep modes
#define Nop() {__asm__ volatile ("nop");}
#define ClrWdt() {__asm__ volatile ("clrwdt");}
#define Sleep() {__asm__ volatile ("pwrsav #0");}   //Sleep() - put MCU in sleep mode - CPU and some peripherals off
#define Idle() {__asm__ volatile ("pwrsav #1");}    //Idle() - put MCU in idle mode - only CPU off
#define dsen() {__asm__ volatile ("BSET DSCON, #15");} 

void main(void) {
    NewClk(8); //8 for 8 Mhz; 32 for 32 Khz; 500 for 500 Khz
    IOinit(); //Initializes the IOs
    Config(); //Configures the interrupt enables and interrupt flags
    while(1)
    {
        IOCheck(); //Checks the IOs
    }
    return;
}
