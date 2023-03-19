/* 
 * File:   main.h
 * Author: Utsava
 *
 * Created on 14 March, 2023, 10:17 AM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include "i2c.h"
#include"ds1307.h"
#include "clcd.h"
#include "EEPROM.h"
#include "adc.h"
#include "digital_keypad.h"
#include "car_black_box.h"
#include "timers.h"
#include "uart.h"
#include <string.h>

#define LOGIN_SUCCESS               0x21
#define RETURN_BACK                 0x22
#define TASK_SUCCESS                0x23
#define TASK_FAIL                   0x24

#define DASH_BOARD_FLAG             0x01
#define LOGIN_FLAG                  0x02
#define LOGIN_MENU_FLAG             0x03
#define VIEW_LOG_FLAG               0x04
#define CLEAR_LOG_FLAG              0x05
#define DOWNLOAD_LOG_FLAG           0x06
#define SET_TIME_FLAG               0x07
#define CHANGE_PASSWORD_FLAG        0x08
#define RESET_NOTHING               0x09
#define RESET_PASSWORD              0x10
#define RESET_LOGIN_MENU            0x11
#define RESET_MEMORY                0x12
#define RESET_VIEW_LOG_POS          0x13
#define RESET_TIME                  0x14
#define RESET_DOWNLOAD              0x15
#define NOTHING                     0x30                        

#endif	/* MAIN_H */

