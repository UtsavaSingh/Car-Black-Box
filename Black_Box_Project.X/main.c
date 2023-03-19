/*
 * File:   main.c
 * Author: Utsava Kumar Singh
 * Description: Car Black Box Project
 * Created on 14 March, 2023, 7:08 AM
 */

#include "main.h"

#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled)

// Global variables
int pos = -1;
char access = 0;
unsigned char second, minute, hour;

static void init_config(void) {
    // initialize i2c 
    init_i2c(100000); //100K
    // initialize ds1307
    init_ds1307();
    // initialize digital keypad
    init_digital_keypad();
    // initialize clcd
    init_clcd();
    // initialize adc
    init_adc();
    // initialize timer2
    init_timer2();
    // initialize UART 
    init_uart(9600);
    GIE = 1;
    PEIE = 1;
}

void main(void) {
    // Variable declaration
    unsigned char control_flag = DASH_BOARD_FLAG, reset_flag;    // Dash Board Screen
    const char *gear[6] = {"GN", "GR", "G1", "G2", "G3", "G4"};
    unsigned char speed = 0;
    char event[] = "ON";
    unsigned char menu_pos;
    unsigned char key, key_level; 
    unsigned int gr = 0, delay1 = 0, delay2 = 0, delay3 = 0, delay4 = 0, delay5 = 0;
    init_config();
    str_write_EEPROM(0x00, "1010"); // password

    while (1) {
        // Reading key press
        key = read_digital_keypad(STATE);
        key_level = read_digital_keypad(LEVEL);
        
        if(key == SW1)
        {
            // Crash event
            strcpy(event, "C ");
            log_event(event, speed);
        }
        else if(key == SW2 && gr < 6) // increment gear
        {
            // Gear change event (increment)
            strcpy(event, gear[gr]);
            log_event(event, speed);
            gr++;
        }
        else if(key == SW3 && gr > 0) // decrement gear
        {
            // Gear change event (decrement)
            gr--;
            strcpy(event, gear[gr]);
            log_event(event, speed);
        }
        else if((control_flag == DASH_BOARD_FLAG) && (key == SW4 || key == SW5))
        {
            // Password Enter Screen
            clear_screen();
            clcd_print(" ENTER PASSWORD ", LINE1(0));
            clcd_putch(' ', LINE2(5));
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            control_flag = LOGIN_FLAG;
            reset_flag = RESET_PASSWORD;
            /* Switching on the Timer2 */
            TMR2ON = 1;
        }
        else if((control_flag == LOGIN_MENU_FLAG) && (key_level == SW4) && (delay1++ == 20))
        {
            // Long press of up key to select a particular option from login menu
            delay1 = 0;
            switch(menu_pos)
            {
                case 0 :
                    // view log
                    control_flag = VIEW_LOG_FLAG;
                    reset_flag = RESET_VIEW_LOG_POS;
                    clear_screen();
                    break;
                case 1 :
                    // clear log
                    pos = -1;
                    access = 0;
                    clear_screen();
                    clcd_print("Logs Cleared", LINE1(1));
                    __delay_ms(3000);
                    clear_screen();
                    break;
                case 2 :
                    // download log -> pc serial terminal
                    control_flag = DOWNLOAD_LOG_FLAG;
                    reset_flag = RESET_DOWNLOAD;
                    clear_screen();
                    break;
                case 3 :
                    // set time
                    control_flag = SET_TIME_FLAG;
                    reset_flag = RESET_TIME;
                    clear_screen();
                    break;
                case 4 :
                    // change password
                    control_flag = CHANGE_PASSWORD_FLAG;
                    reset_flag = RESET_PASSWORD;
                    clear_screen();
                    break;
            }
        }
        else if((control_flag == LOGIN_MENU_FLAG) && (key_level == SW5) && (delay2++ == 20))
        {
            // Return to Dash board screen from login menu screen 
            /* Switching off the Timer2 */
            TMR2ON = 0;
            delay2 = 0;
            control_flag = DASH_BOARD_FLAG;
            clear_screen();
        }
        else if((control_flag == VIEW_LOG_FLAG) && (key_level == SW5) && (delay3++ == 20))
        {
            // Return to Dash board screen from login menu screen 
            /* Switching off the Timer2 */
            TMR2ON = 0;
            delay3 = 0;
            control_flag = DASH_BOARD_FLAG;
            clear_screen();
        }
        else if((control_flag == VIEW_LOG_FLAG) && (key_level == SW4) && (delay4++ == 20))
        {
            // Return to login menu screen from View log screen
            delay4 = 0;
            control_flag = LOGIN_MENU_FLAG;
            reset_flag = RESET_LOGIN_MENU;
            clear_screen();
        }
        else if((control_flag == SET_TIME_FLAG) && (key_level == SW4) && (delay5++ == 20))
        {
            // Return to login menu screen from set time screen
            delay5 = 0;
            control_flag = LOGIN_MENU_FLAG;
            reset_flag = RESET_LOGIN_MENU;
            // Setting new time
            write_ds1307(SEC_ADDR, dec2bcd(second));
            write_ds1307(MIN_ADDR, dec2bcd(minute));
            write_ds1307(HOUR_ADDR, dec2bcd(hour));
            clear_screen();
        }
        
        // Reading speed through adc 
        speed = read_adc()/10;  //0 -1023
        if(speed > 99)
            speed = 99;
        
        // Selecting the particular operations depending on control flag
        switch(control_flag)
        {
            // Display dash board
            case DASH_BOARD_FLAG:
            {
                display_dash_screen(event, speed);
                break;
            }
            // Display login screen
            case LOGIN_FLAG:
            {
                switch(login(reset_flag, key))
                {
                    case RETURN_BACK :
                        control_flag = DASH_BOARD_FLAG;
                        /* Switching off the Timer2 */
                        TMR2ON = 0;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        break;
                    case LOGIN_SUCCESS :
                        control_flag = LOGIN_MENU_FLAG;
                        reset_flag = RESET_LOGIN_MENU;
                        continue;
                }
                break;
            }
            // Display login menu
            case LOGIN_MENU_FLAG :
                /* Switching on the Timer2 */
                TMR2ON = 1;
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                menu_pos = login_menu(reset_flag, key);
                if(menu_pos == RETURN_BACK)
                {
                    control_flag = DASH_BOARD_FLAG;
                    clear_screen();
                }
                break;
            // Display Logs
            case VIEW_LOG_FLAG :
                /* Switching off the Timer2 */
                TMR2ON = 0;
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                view_log(reset_flag, key);
                //clear_screen();
                break;
            // Display Set Time Screen
            case SET_TIME_FLAG :
                /* Switching off the Timer2 */
                TMR2ON = 0;
                set_time(reset_flag, key);
                break;
            // Download Logs to Serial Terminal
            case DOWNLOAD_LOG_FLAG :
                /* Switching off the Timer2 */
                TMR2ON = 0;
                if(download_log(reset_flag) == RETURN_BACK)
                {
                    control_flag = LOGIN_MENU_FLAG;
                    reset_flag = RESET_LOGIN_MENU;
                    continue;
                }
                break; 
            // Display change password screen
            case CHANGE_PASSWORD_FLAG :
                /* Switching off the Timer2 */
                TMR2ON = 0;
                switch(change_password(reset_flag, key))
                {
                    case TASK_SUCCESS :
                        control_flag = DASH_BOARD_FLAG;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        break;
                    case TASK_FAIL :
                        control_flag = LOGIN_MENU_FLAG;
                        reset_flag = RESET_LOGIN_MENU;
                        continue;
                }
                break;
            
        }
        reset_flag = RESET_NOTHING;
    }
    
    return;
}
