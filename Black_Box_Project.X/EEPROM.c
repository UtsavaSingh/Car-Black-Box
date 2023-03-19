/*
 * File:   EEPROM.c
 * Author: Utsava
 *
 * Created on 14 March, 2023, 4:35 AM
 */


#include <xc.h>
#include "i2c.h"
#include "EEPROM.h"
/*
void init_EEPROM(void)
{
    unsigned char dummy;
   
    dummy = read_EEPROM(SEC_ADDR);
    dummy = dummy & 0x7F;
    write_EEPROM(SEC_ADDR, dummy);// ch = 0
}
*/
char read_EEPROM(unsigned char addr)
{
    // declaration of local variable
    unsigned char data;
    
    // sending start bit in i2c bus
    i2c_start();
    // sending the slave address byte with write bit LSB
    i2c_write(EXT_EEPROM_WRITE);
    // writing the address to the I2C line to read the data from that address
    i2c_write(addr);
    // restarting the I2C bus
    i2c_rep_start();
    // sending the slave address byte with read bit LSB
    i2c_write(EXT_EEPROM_READ);
    
    // reading the data from SSPBUFF
    data = i2c_read(0);
    // sending stop bit to make ready the I2C bus for further transmission
    i2c_stop();
    
    return data;
}

void byte_write_EEPROM(unsigned char addr, unsigned char data) // SEC_ADDR, data
{
    // sending start bit in i2c bus
    i2c_start();
    // sending the slave address byte with write bit LSB
    i2c_write(EXT_EEPROM_WRITE);
    // writing the address to the slave to write the data in that address
    i2c_write(addr);
    // writing the data to the slave
    i2c_write(data);
    // sending stop bit to make ready the I2C bus for further transmission
    i2c_stop();
}

void str_write_EEPROM(unsigned char addr, char *data)
{
    while(*data != 0)
    {
        byte_write_EEPROM(addr, *data);
        data++;
        addr++;
    }
}

