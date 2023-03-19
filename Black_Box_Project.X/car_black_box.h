/* 
 * File:   car_black_box.h
 * Author: Utsava
 *
 * Created on 14 March, 2023, 10:21 AM
 */

#ifndef CAR_BLACK_BOX_H
#define	CAR_BLACK_BOX_H

void display_dash_screen(char event[], unsigned char speed);
void log_event(char event[], unsigned char speed);
void clear_screen(void);
char login(unsigned char reset_flag, unsigned char key);
unsigned char login_menu(unsigned char reset_flag, unsigned char key);
void view_log(unsigned char reset_flag, unsigned char key);
char change_password(unsigned char reset_flag, unsigned char key);
void set_time(unsigned char reset_flag, unsigned char key);
unsigned char dec2bcd(unsigned char num);
char download_log(unsigned char reset_flag);

#endif	/* CAR_BLACK_BOX_H */

