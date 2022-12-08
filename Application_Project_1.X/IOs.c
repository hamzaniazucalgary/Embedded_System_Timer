/*
 * File:   IOs.c
 * Author: Hamza N, Yousef H, Rumaisa T
 * Created on November 19, 2022, 9:12 PM
 */

#include "xc.h"
#include "TimeDelay.h"
#include "GPIO.h"

int PBstate = 0b000;
int Mode = 1; //default mode - countdown
int min = 0; //minutes
int sec = 0; //seconds
int hsec = 0; //hundreth of a second
int pressedTime = 0; //how long button has been pressed in seconds
int countStop = 0; //countdown stop flag
int timerStop = 0; //timer stop flag
int alarm = 0; //alarm flag
char str[30];
extern int PB1; //PB1 from GPIO.c
extern int PB2; //PB2 from GPIO.c
extern int PB3; //PB3 from GPIO.c

void IOinit() {
    TRISAbits.TRISA2 = 1; // Set RA2 as input (B1)
    TRISAbits.TRISA4 = 1; // Set RA4 as input (B2)
    TRISBbits.TRISB4 = 1; // Set RB4 as input (B3)
    TRISBbits.TRISB8 = 0; // Set RB8 as output (LED)

    CNPU2bits.CN30PUE = 1; // RA2 Pull up
    CNPU1bits.CN0PUE = 1; // RA4 Pull up
    CNPU1bits.CN1PUE = 1; // RB4 Pull up

    AD1PCFG = 0xFFFF; //Makes all inputs analog
}

void IOCheck() {
    //PB1 : RA2
    //PB2 : RA4
    //PB3 : RB4

    //Check if any pushbuttons are pressed
    if (PB1 == 1 || PB2 == 1 || PB3 == 1) {
        NewClk(8); //Set clock to 8MHz (Speed up clock after input is registered) 
    }
    PBstate = 0b000;

    if (PB1 == 1) { //If PB1 == 1
        PBstate += 0b001; //Changes the PBstate if PB1 == 1
    }
    if (PB2 == 1) { //If PB2 == 1
        PBstate += 0b010; //Changes the PBstate if PB2 == 1
    }
    if (PB3 == 1) { //If PB3 == 1
        PBstate += 0b100; //Changes the PBstate if PB3 == 1
    }
    switch (PBstate) { //We use case switch to get the right input/output
        case 0b001: //PB1 pressed
            switch (Mode) { //A case switch inside a case switch
                case 1: //Countdown - adjusts the minutes
                    if (alarm == 1) { //If the alarm == 1
                        alarm = 0; //Reset the aram
                        countStop = 0; //Make the counter stop
                    }
                    if (min < 59) { //If the min < 59
                        min = min + 1; //Increment the minutes by 1 as long as PB1 is pressed
                        Delay_ms(200); //Adds a delay so the incrementation moves a bit slow
                    }
                    sprintf(str, "  %02dm : %02ds         ", min, sec); //Formats the mins and sec in a string
                    Disp2String(str); //Displays the string
                    XmitUART2('\r', 1); //Prints a \r character
                    break; //Break for the case
                case 2: //Timer - start stop
                    Delay_ms(300); //Debounce delay
                    if (timerStop == 0) { //If the timestop == 0
                        Timer(); //Call the timer function
                    } else if (timerStop == 1) { //Else if the timestop == 1
                        timerStop = 0; //Make the timer stop
                        sprintf(str, "  %02dm : %02d.%02ds      ", min, sec, hsec); //Format the string with minutes and sec
                        Disp2String(str); //Display the string
                        XmitUART2('\r', 1); //Print a \r character
                    }
            }
            break; //Break for the case
        case 0b010: //PB2 pressed
            switch (Mode) { //Switch case with in a switch case
                case 1: //countdown - adjust seconds
                    if (alarm == 1) { //If the alarm == 1
                        alarm = 0; //Reset the alarm to 0
                        countStop = 0; //Make the timer stop
                    }
                    if (sec < 59) { //If the sec < 59
                        sec = sec + 1; //Increment the sec as long as PB2 is pressed
                        Delay_ms(200); //Adds a delay so the incrementation moves a bit slow
                    }
                    sprintf(str, "  %02dm : %02ds         ", min, sec); //Format the string with min and sec
                    Disp2String(str); //Prints the string
                    XmitUART2('\r', 1); //Prints a \r character
                    break; //Break the case
                case 2: //timer - reset
                    timerStop = 0; //Reset the timer
                    min = 0; //Reset the min
                    sec = 0; //Reset the sec
                    hsec = 0; //Reset the hsec
                    sprintf(str, "  %02dm : %02d.%02ds      ", min, sec, hsec); //Format the string with min, sec and hsec
                    Disp2String(str); //Display the string
                    XmitUART2('\r', 1); //Print a \r character
            }
            break; //Break the case
        case 0b100: //PB3 pressed
            switch (Mode) {
                case 1: //countdown - PB3 long or short press
                    while (PB3 == 1) { //If PB3 == 1
                        sprintf(str, "  %02dm : %02ds", min, sec); //Format the string with min and sec
                        Disp2String(str); //Display the string
                        XmitUART2('\r', 1); //Print a \r character
                        pressedTime = pressedTime + 1; //Keep track of how long the button is pressed
                        Delay_ms(500);  //Delay for debounce
                    }
                    if (pressedTime <= 3) //countdown - start stop
                    {
                        pressedTime = 0; //Reset the pressed time
                        if (min == 0 && sec == 0) { //If the sec and min == 0
                            countStop = 1; //Stop the count down
                        }
                        if (countStop == 0) { //If count stop = 0
                            Countdown(); //Call the function
                        } else if (countStop == 1) { //If the count stop is 1
                            countStop = 0; //Reset the flag
                            sprintf(str, "  %02dm : %02ds         ", min, sec); //Format the string with min and sec
                            Disp2String(str); //Display the string
                            XmitUART2('\r', 1); //Print a \r character
                        }
                    }
                    if (pressedTime > 3) //countdown - reset
                    {
                        pressedTime = 0; //Reset everything
                        alarm = 0; //Alarm rest
                        countStop = 0; //Count stop rest
                        min = 0; //Min reset
                        sec = 0; //Sec reset
                        sprintf(str, "  %02dm : %02ds         ", min, sec); //Format the string with min and sec
                        Disp2String(str); //Display the string
                        XmitUART2('\r', 1); //Print a \r character
                    }
                    break; //Break the case
            }
            break; //Break the case
        default: //no pushbuttons
            if (alarm == 1) { //LED on when ALARM
                LATBbits.LATB8 = 1; //LED on forever
            } else if (alarm == 0) { //If no ALARM
                LATBbits.LATB8 = 0; //LED off forever
            }
            NewClk(32); //Slow down clock when stop watch is not active
            Delay_ms(100); //Debounce delay
            break; //Break the case
    }
}
void Countdown() {
    int blink = 0; //Keeps track of the blink

    while (countStop == 0) { //While the count stop is 0
        if (sec >= 0) { //If the sec is >= 0
            sec = sec - 1; //countdown seconds
            if (blink == 0) { //1 sec blink rate
                blink = 1; //LED on
                LATBbits.LATB8 = 1; //LED on
                Delay_ms(1000); //1 sec delay
            } else if (blink == 1) { //If blink is 1
                blink = 0; //LED off
                LATBbits.LATB8 = 0; //LED off
                Delay_ms(1000); //1 sec delay
            }
            if (sec < 0) { //If the sec is less than 0
                min = min - 1; //countdown minutes
                sec = 59; //make the sec 59
            }
            if (PB3 == 1) { //countdown stop flag
                countStop = 1; //Stop the timer
                min = min; // min is min
                sec = sec + 1; //sec is sec + 1
            }
            sprintf(str, "  %02dm : %02ds         ", min, sec); //Format the string with min and sec
            Disp2String(str); //Display the string
            if (min == 0 && sec == 0) { //ALARM
                XmitUART2('\r', 1); //Printa \r character
                sprintf(str, "  %02dm : %02ds -- ALARM", min, sec); //Format the string with min and sec and alaram
                Disp2String(str); //Display the string
                alarm = 1; //Alaram flag is 1
                countStop = 1; //Countstop flag is 1
                LATBbits.LATB8 = 1; //LED on
            }
            XmitUART2('\r', 1); //Print the \r character
        }
    }
}

void Timer() {
    while (timerStop == 0) {
        LATBbits.LATB8 = 1; //Turn LED on
        hsec = hsec + 1; //increment hsec twice to account for display delays
        Delay_ms(1); //Delay for 1 ms
        hsec = hsec + 1; //Increment hsec
        Delay_ms(1); //Delay for 1 ms
        if (hsec == 100) { //When hsec is 100
            hsec = 0; //Reset hsec
            sec = sec + 1; //increment seconds
            if (sec > 59) { //If sec is > 59
                sec = 0; //Reset it to 0
                min = min + 1; //increment minutes
            }
        }
        if (PB1 == 1) { //timer stop flag
            timerStop = 1; //Time stop flag is 1
            min = min; //min = min
            sec = sec; //sec = sec
            hsec = hsec; //hsec = hsec
            LATBbits.LATB8 = 0; //LED off
        }
        if (PB2 == 1) { //timer reset flag
            timerStop = 1; //Time stop flag is 0
            min = 0; //min = 0
            sec = 0; //sec = 0
            hsec = 0; //hsec = 0
        }
        sprintf(str, "  %02dm : %02d.%02ds", min, sec, hsec); //Format the string with min sec and hsec
        Disp2String(str); //Display the string
        XmitUART2('\r', 1); //Print he \r character
    }
}
