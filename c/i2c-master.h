#ifndef I2C_MASTER_H_  /* I2C_MASTER_H_ */
#define I2C_MASTER_H_

#ifndef F_CPU
    #define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#define SCL_CLK 100000L  // SCL clock frequency
#define BITRATE(TWSR)   ((F_CPU / SCL_CLK) - 16) /\
    (2 * pow(4, (TWSR & ((1 << TWPS0) | (1 << TWPS1)))))  // bit rate

void i2c_init();					/* I2C initialize function */
uint8_t  i2c_start(char);			/* I2C start function */
uint8_t  i2c_repeated_start(char);  /* I2C repeated start function */
void i2c_stop();					/* I2C stop function */
void i2c_start_wait(char);			/* I2C start wait function */
uint8_t  i2c_write(char);			/* I2C write function */
char i2c_read_ack();				/* I2C read ack function */
char i2c_read_nack();				/* I2C read nack function */

#endif                 /* I2C_MASTER_H_ */