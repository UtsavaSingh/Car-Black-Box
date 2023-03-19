/*
 * File:   car_black_box_def.c
 * Author: Utsava
 *
 * Created on 14 March, 2023, 10:05 AM
 */

#include "main.h"

// variable declaration
char clock_reg[3]; // HH MM SS 
char *menu[] = {"View log", "Clear log", "Download log", "Set time", "Change passwd"};
char time[7];  // "HH:MM:SS"
// time, event, speed HHMMSSON99
char log[11];
extern int pos;
extern char access;
char sec = 0, min = 0;
extern int return_time;
extern unsigned char second, minute, hour;

/*Function to get time from ds1307*/
void get_time()
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
}

/*Function to display time on the clcd display*/
void display_time()
{
    get_time(); // HH MM SS  
   
    /* To store the time in HH:MM:SS format */
    // HH -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';
    
    // MM 
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    
    // SS
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
    
    // HH:MM:SS
    clcd_putch(time[0], LINE2(2)); 
    clcd_putch(time[1], LINE2(3));
    clcd_putch(':', LINE2(4)); 
    clcd_putch(time[2], LINE2(5)); 
    clcd_putch(time[3], LINE2(6));
    clcd_putch(':', LINE2(7)); 
    clcd_putch(time[4], LINE2(8)); 
    clcd_putch(time[5], LINE2(9));
}

/*Function to display Dash Board Screen*/
void display_dash_screen(char event[], unsigned char speed)
{
    clcd_print("  TIME     EV SP", LINE1(0));
      
    display_time(); // HH:MM:SS --> 13:14:15
    
    // to print event
    clcd_print(event, LINE2(11));
    // to print speed
    clcd_putch(speed/10 + '0', LINE2(14));
    clcd_putch(speed%10 + '0', LINE2(15));
}

/*Function to store car events into eeprom*/
void log_car_event(void)
{
    char addr;
    pos++;
    if(pos == 10)
    {
        pos = 0;
    }
    addr = pos * 10 + 5;  // 5 15 25 35 .....
    str_write_EEPROM(addr, log); // 5th to 14th address
    if(access < 10)
        access++;
}

/*Function to store each events in log array*/
void log_event(char event[], unsigned char speed)
{
    strncpy(log, time, 6);
    strncpy(&log[6], event, 2);
    log[8] = speed/10 + '0';
    log[9] = speed%10 + '0';
    log[10] = '\0';
    
    log_car_event();
}

/*Function to login operation*/
char login(unsigned char reset_flag, unsigned char key)
{
    static char npasswd[4];
    char spasswd[4];
    static unsigned char attempt_rem, i;
    
    if(reset_flag == RESET_PASSWORD)
    {
        npasswd[0] = '\0';
        npasswd[1] = '\0';
        npasswd[2] = '\0';
        npasswd[3] = '\0';
        
        attempt_rem = 3;
        i = 0;
        key = ALL_RELEASED;
        return_time = 5;
    }
    if(return_time == 0)
        return RETURN_BACK;
    if(key == SW4 && i < 4) // '1'
    {
        npasswd[i] = '1';
        i++;
        clcd_putch('*', LINE2(5+i));
        return_time = 5;
    }
    else if(key == SW5 && i < 4) // '0'
    {
        npasswd[i] = '0';
        i++;
        clcd_putch('*', LINE2(5+i));
        return_time = 5;
    }
    if(i == 4)
    {
        for(unsigned char j = 0; j < 4; j++)
        {
            spasswd[j] = read_EEPROM(j);
        }
        if(strncmp(npasswd, spasswd, 4) == 0)
        {
            // Correct Password
            return LOGIN_SUCCESS;
        }
        else
        {
            // Incorrect Password
            attempt_rem--;
            if(attempt_rem == 0)
            {
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("You are blocked", LINE1(0));
                clcd_print("wait for 15 min", LINE2(0));
                // 1 min
                sec = 60;
                // 15 min and reset_flag = RESET_PASSWORD
                min = 15;
                while(min)
                {
                    clcd_putch(min/10 + '0', LINE2(9));
                    clcd_putch(min%10 + '0', LINE2(10));
                }
                sec = 0;
                attempt_rem = 3;
            }
            else
            {
                clear_screen();
                clcd_print("Wrong Password", LINE1(0));
                clcd_putch(attempt_rem + '0', LINE2(0));
                clcd_print("attempt remain", LINE2(2));
                __delay_ms(3000);
            }
            
            clear_screen();
            clcd_print(" ENTER PASSWORD ", LINE1(0));
            clcd_putch(' ', LINE2(5));
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            i = 0;
            return_time = 5;
        }
    }
    return NOTHING;
}

/*Function to Show login menu*/
unsigned char login_menu(unsigned char reset_flag, unsigned char key)
{
    static char menu_pos;
    if(reset_flag == RESET_LOGIN_MENU)
    {
        menu_pos = 0;
        clear_screen();
        return_time = 5;
        key = ALL_RELEASED;
    }
    if(return_time == 0)
        return RETURN_BACK;
    if(key == SW5 && menu_pos < 4)
    {
        menu_pos++;
        clear_screen();
        return_time = 5;
    }
    else if(key == SW4 && menu_pos > 0)
    {
        menu_pos--;
        clear_screen();
        return_time = 5;
    }
    if(menu_pos < 4)
    {
        clcd_putch('*', LINE1(0));
        clcd_print(menu[menu_pos], LINE1(2));
        clcd_print(menu[menu_pos+1], LINE2(2));
    }
    else if(menu_pos == 4)
    {
        clcd_putch('*', LINE2(0));
        clcd_print(menu[menu_pos-1], LINE1(2));
        clcd_print(menu[menu_pos], LINE2(2));
    }
    return menu_pos;
}

/*Function to show logs occur*/
void view_log(unsigned char reset_flag, unsigned char key)
{ 
    char ch, i;
    static char log_pos;
    if(pos == -1)
    {
        clcd_print("No Log Exist", LINE1(1));
    }
    else
    {
        if(reset_flag == RESET_VIEW_LOG_POS)
        {
            log_pos = 0;
            clear_screen();
        }
        if(key == SW5 && (log_pos < (access-1)))
        {
            log_pos++;
            clear_screen();
        }
        else if(key == SW4 && log_pos > 0)
        {
            log_pos--;
            clear_screen();
        }
        else if(key == SW5 && (log_pos == (access-1)))
        {
            log_pos = 0;
            clear_screen();
        }
        if(log_pos < access)
        {
            clcd_print("# TIME     EV SP", LINE1(0));
            i = log_pos * 10 + 5;
            clcd_putch(log_pos + '0', LINE2(0));
            ch = read_EEPROM(i);
            clcd_putch(ch, LINE2(2));
            ch = read_EEPROM(i+1);
            clcd_putch(ch, LINE2(3));
            clcd_putch(':', LINE2(4));
            ch = read_EEPROM(i+2);
            clcd_putch(ch, LINE2(5));
            ch = read_EEPROM(i+3);
            clcd_putch(ch, LINE2(6));
            clcd_putch(':', LINE2(7));
            ch = read_EEPROM(i+4);
            clcd_putch(ch, LINE2(8));
            ch = read_EEPROM(i+5);
            clcd_putch(ch, LINE2(9));
            clcd_putch(' ', LINE2(10));
            ch = read_EEPROM(i+6);
            clcd_putch(ch, LINE2(11));
            ch = read_EEPROM(i+7);
            clcd_putch(ch, LINE2(12));
            clcd_putch(' ', LINE2(13));
            ch = read_EEPROM(i+8);
            clcd_putch(ch, LINE2(14));
            ch = read_EEPROM(i+9);
            clcd_putch(ch, LINE2(15));
        }
    }
}

/*Function to change the existing password*/
char change_password(unsigned char reset_flag, unsigned char key)
{
    static char npasswd[4];
    static char re_npasswd[4];
    static unsigned char i, j;
    static char flag = 0;
    
    if(reset_flag == RESET_PASSWORD)
    {
        npasswd[0] = '\0';
        npasswd[1] = '\0';
        npasswd[2] = '\0';
        npasswd[3] = '\0';
        
        re_npasswd[0] = '\0';
        re_npasswd[1] = '\0';
        re_npasswd[2] = '\0';
        re_npasswd[3] = '\0';
        
        i = 0;
        j = 0;
        key = ALL_RELEASED;
        clear_screen();
        clcd_print(" ENTER PASSWORD ", LINE1(0));
        clcd_putch(' ', LINE2(5));
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
        __delay_us(100);
    }
   
    if(key == SW4 && i < 4) // '1'
    {
        npasswd[i] = '1';
        i++;
        clcd_putch('*', LINE2(5+i));
        if(i == 4)
        {
            key = ALL_RELEASED;
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            flag = 1;
        }
    }
    else if(key == SW5 && i < 4) // '0'
    {
        npasswd[i] = '0';
        i++;
        clcd_putch('*', LINE2(5+i));
        if(i == 4)
        {
            key = ALL_RELEASED;
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            flag = 1;
        }
    }
    if(i == 4)
    {
        if(flag == 1)
        {
            flag = 0;
            clcd_print("RE-ENTER PASSWORD ", LINE1(0));
            clcd_putch(' ', LINE2(5));
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
        }
        if(key == SW4 && j < 4) // '1'
        {
            re_npasswd[j] = '1';
            j++;
            clcd_putch('*', LINE2(5+j));
        }
        else if(key == SW5 && j < 4) // '0'
        {
            re_npasswd[j] = '0';
            j++;
            clcd_putch('*', LINE2(5+j));
        }
             
        if(j == 4)
        {
            if(strncmp(npasswd, re_npasswd, 4) == 0)
            {
                // Both password same
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("New Password set", LINE1(0));
                for(unsigned char k = 0; k < 4; k++)
                {
                    byte_write_EEPROM(k, npasswd[k]);
                }
                __delay_ms(3000);
                return TASK_SUCCESS;
            }
            else
            {
                // Both password not same
                clear_screen();
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print(" Both Password ", LINE1(0));
                clcd_print("  Not Matched", LINE2(0));
                __delay_ms(3000);
                return TASK_FAIL;
            }
        }   
    }
    return NOTHING;
}

/*Function to set new time*/
void set_time(unsigned char reset_flag, unsigned char key)
{
    static unsigned char second_flag, minute_flag, hour_flag, delay, flag;
    
    static char new_time[7];  // "HH:MM:SS"
    if(reset_flag == RESET_TIME)
    {
        key = ALL_RELEASED;
        second_flag = 1;
        minute_flag = 0;
        hour_flag = 0;
        delay = 0;
        clear_screen();
        clcd_print("    HH:MM:SS    ", LINE1(0));
        get_time(); // HH MM SS  
   
        /* To store the time in HH:MM:SS format */
        // HH -> 
        time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
        time[1] = (clock_reg[0] & 0x0F) + '0';
    
        // MM 
        time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
        time[3] = (clock_reg[1] & 0x0F) + '0';
    
        // SS
        time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
        time[5] = (clock_reg[2] & 0x0F) + '0';
        time[6] = '\0';
        // HH:MM:SS
        clcd_putch(time[0], LINE2(4)); 
        clcd_putch(time[1], LINE2(5));
        clcd_putch(':', LINE2(6)); 
        clcd_putch(time[2], LINE2(7)); 
        clcd_putch(time[3], LINE2(8));
        clcd_putch(':', LINE2(9)); 
        clcd_putch(time[4], LINE2(10)); 
        clcd_putch(time[5], LINE2(11));
        hour = (time[0]-'0')*10+(time[1]-'0');
        minute = (time[2]-'0')*10+(time[3]-'0');
        second = (time[4]-'0')*10+(time[5]-'0');
    }
    
    
    // Increment
    if(key == SW4)
    {
        if(minute_flag)
        {
            ++minute;
            if(minute > 60)
                minute = 0;
        }
        else if(hour_flag)
        {
            ++hour;
            if(hour > 24)
                hour = 0;
        }
        else if(second_flag)
        {
            ++second;
            if(second > 60)
                second = 0;
        }
    }
    // Choose field
    else if(key == SW5)
    {
        if(second_flag)
        {
            hour_flag = 0;
            minute_flag = 1;
            second_flag = 0;
        }
        else if(minute_flag)
        {
            hour_flag = 1;
            minute_flag = 0;
            second_flag = 0;
        }
        else if(hour_flag)
        {
            hour_flag = 0;
            minute_flag = 0;
            second_flag = 1;
        }
    }
        
    // set mode
    if(second_flag)
    {
        if(flag)
        {
            new_time[4] = (second / 10) % 10 + '0';
            new_time[5] = (second / 1) % 10 + '0';
        }
        else
        {
            new_time[4] =  ' ';
            new_time[5] =  ' ';
        }
    }
    else
    {
        new_time[4] = (second / 10) % 10 + '0';
        new_time[5] = (second / 1) % 10 + '0';
    }
    if(minute_flag)
    {
        if(flag)
        {
            new_time[2] = (minute / 10) % 10 + '0';
            new_time[3] = (minute / 1) % 10 + '0';
        }
        else
        {
            new_time[2] =  ' ';
            new_time[3] =  ' ';
        }
    }
    else
    {
        new_time[2] = (minute / 10) % 10 + '0';
        new_time[3] = (minute / 1) % 10 + '0';
    }
            
    if(hour_flag)
    {
        if(flag)
        {
            new_time[0] = (hour / 10) % 10 + '0';
            new_time[1] = (hour / 1) % 10 + '0';
        }
        else
        {
            new_time[0] =  ' ';
            new_time[1] =  ' ';
        }
    }
    else
    {
        new_time[0] =  (hour / 10) % 10 + '0';
        new_time[1] =  (hour / 1) % 10 + '0';
    }
    
    // HH:MM:SS
    clcd_putch(new_time[0], LINE2(4)); 
    clcd_putch(new_time[1], LINE2(5));
    clcd_putch(':', LINE2(6)); 
    clcd_putch(new_time[2], LINE2(7)); 
    clcd_putch(new_time[3], LINE2(8));
    clcd_putch(':', LINE2(9)); 
    clcd_putch(new_time[4], LINE2(10)); 
    clcd_putch(new_time[5], LINE2(11));
    
    // logic to blink selected field
    if(delay++ == 8)
    {
        delay = 0;
        flag = !flag;
    }
}

/*Function to Download log*/
char download_log(unsigned char reset_flag)
{
    char ch, i;
    static char log_pos;
    if(pos == -1)
    {
        clcd_print("No Log Exist", LINE1(1));
        __delay_ms(2000);
        return RETURN_BACK;
    }
    else
    {
        if(reset_flag == RESET_DOWNLOAD)
        {
            log_pos = 0;
            clcd_print("Downloaded-Open", LINE1(0));
            clcd_print("Serial Terminal", LINE2(0));
            putchar('\n');
            puts("N TIME     EV SP\n");
        }   
        
        if(log_pos < access)
        {
            i = log_pos * 10 + 5;
            putchar(log_pos + '0');
            putchar(' ');
            ch = read_EEPROM(i);
            putchar(ch);
            ch = read_EEPROM(i+1);
            putchar(ch);
            putchar(':');
            ch = read_EEPROM(i+2);
            putchar(ch);
            ch = read_EEPROM(i+3);
            putchar(ch);
            putchar(':');
            ch = read_EEPROM(i+4);
            putchar(ch);
            ch = read_EEPROM(i+5);
            putchar(ch);
            putchar(' ');
            ch = read_EEPROM(i+6);
            putchar(ch);
            ch = read_EEPROM(i+7);
            putchar(ch);
            putchar(' ');
            ch = read_EEPROM(i+8);
            putchar(ch);
            ch = read_EEPROM(i+9);
            putchar(ch);
            putchar('\n');
            log_pos++;
        }
        else
            return RETURN_BACK;
    }
    return NOTHING;
}

/*Function to clear clcd screen*/
void clear_screen(void)
{
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);
}

/*Function to convert decimal number into BCD*/
unsigned char dec2bcd(unsigned char num)
{
    unsigned char ones = 0;
    unsigned char tens = 0;
    unsigned char temp = 0;
    
    ones = num % 10;
    temp = num / 10;
    tens = temp<<4;
    
    return (tens + ones);
}