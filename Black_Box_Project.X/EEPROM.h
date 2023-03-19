/* 
 * File:   EEPROM.h
 * Author: Utsava
 *
 * Created on 14 March, 2023, 4:33 AM
 */

#ifndef EEPROM_H
#define	EEPROM_H

#define EXT_EEPROM_WRITE             0b10100000 // 0xA0
#define EXT_EEPROM_READ              0b10100001 // 0xA1


//void init_EEPROM(void);
char read_EEPROM(unsigned char addr);
void byte_write_EEPROM(unsigned char addr, unsigned char data);
void str_write_EEPROM(unsigned char addr, char *data);
#endif	/* EEPROM_H */

