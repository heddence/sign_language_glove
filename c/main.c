#ifndef F_CPU
#undef F_CPU
    #define F_CPU 16000000UL
#endif

#ifndef __AVR_ATmega328P__
    #define __AVR_ATmega328P__
#endif

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "i2cmaster.h"
#include "mpu6050-reg.h"
#include "uart.h"
#include "main.h"

/* init IMU function */
void imu_init() {
    _delay_ms(150);

    imu_write(SMPLRT_DIV, 0x07);
    imu_write(PWR_MGMT_1, 0x01);
    imu_write(CONFIG, 0x00);
    imu_write(GYRO_CONFIG, 0x18);
    imu_write(INT_ENABLE, 0x01);
}

/* read method */
unsigned char imu_read(unsigned char reg) {
    unsigned char ret;
    i2c_start_wait(Dev24C02+I2C_WRITE);  // set device address and write mode
    i2c_write(reg);                      // write address = reg
    i2c_rep_start(Dev24C02+I2C_READ);    // set device address and read mode
    ret = i2c_readNak();                 // read one byte
    i2c_stop();

    return ret;
}

/* write method */
void imu_write(unsigned char reg, unsigned char value) {
    i2c_start_wait(Dev24C02+I2C_WRITE);  // set device address and write mode
    i2c_write(reg);                      // write address = reg
    i2c_write(value);                    // write value to reg
    i2c_stop();
}

/* read accel and gyro registers of IMU */
void imu_read_acc_gyro(int sensor_arr[6]) {
    sensor_arr[0] = read_sensor(ACCEL_XOUT_H);
    sensor_arr[1] = read_sensor(ACCEL_YOUT_H);
    sensor_arr[2] = read_sensor(ACCEL_ZOUT_H);
    sensor_arr[3] = read_sensor(GYRO_XOUT_H);
    sensor_arr[4] = read_sensor(GYRO_YOUT_H);
    sensor_arr[5] = read_sensor(GYRO_ZOUT_H);
}

/* initialize flex sensors */
void init_flex_sensors() {
    DDRD |= 0b11111000;                        // configure PORTB to an OUTPUT
    PORTD |= 0b11111000;                       // turn LEDs ON

    DDRC = 0x00;                               // configure PORTC to an INPUT
    /* ADMUX register description:
     * =======================================
     * REFS1 REFS0 ADLAR - MUX3 MUX2 MUX1 MUX0
     * =======================================
     * REFS1:0 - Reference Selection Bits (01 is for Vcc)
     * ADLAR - ADC Left Adjust Result
     * MUX3:0 - Analog Channel Selection Bits
     */
	ADMUX = (1 << REFS0) | (1 << ADLAR);       // select Vref = AVcc
    /* ADCSRA register description:
     * ===========================================
     * ADEN ADSC ADATE ADIF ADIE ADPS2 ADPS1 ADPS0
     * ===========================================
     * ADEN - ADC Enable
     * ADSC - ADC Start Conversion
     * ADATE - ADC Auto Trigger Enable
     * ADIF - ADC Interrupt Flag (whenever a conversion is finished, this bit is set to 1)
     * ADIE - ADC Interrupt Enable
     * ADPS2:0 - ADC Prescaler Select Bits (111 for division factor of 128)
     */
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // enable ADC and set prescaler to 128
}

/* read ADC value */
uint16_t adc_read(uint8_t adc_port) {
    adc_port &= 0b00000111;
    ADMUX = (ADMUX & 0xF8) | adc_port;  // clear the last 3 bits of ADMUX register
    ADCSRA |= (1 << ADSC);              // start single conversion
    while(ADCSRA & (1 << ADSC));        // wait until the ADSC bit has been cleared
    return ADC;
}

/* read flex sensors values */
void read_flex_sensors(uint16_t* array) {
    for (int i = 0; i < 5; i++) {
        if (i == 4)
            array[i] = adc_read(i + 2);  // skip SCL and SDA ports
        array[i] = adc_read(i);
    }
}

/* read values from sensors */
void read_and_send_sensors() {
    int acc_gyro_sensors[16][6];        // array of accelerometer/gyroscope values
    uint16_t flex_sensors[16][5];       // array of flex sensors values
    unsigned int sensors_filtered[13];  // array of filtered values
    char msg[4 + 22 + 2];               // final packet to send
    char buffer[32];                    // buffer array for UART

    for (int i = 0; i < 16; i++) {
        imu_read_acc_gyro((int*) &acc_gyro_sensors[i]);  // read data from acc/gyro and write it to array
        read_flex_sensors((uint16_t*) &flex_sensors[i]);      // read data from flex sensors and write it to array
    }

    for (int i = 0; i < 6; i++) {
        sensors_filtered[i] = 0;
        for (int j = 0; j < 16; j++) {
            sensors_filtered[i] += acc_gyro_sensors[j][i] >> 8;  // sum all 16 reads from acc/gyro and shift values by 8 bits
        }
    }

    for (int i = 0; i < 5; i++) {
        sensors_filtered[6 + i] = 0;
        for (int j = 0; j < 16; j++) {
            sensors_filtered[6 + i] += flex_sensors[j][i];  // sum all 16 reads from flex sensors
        }
    }

    for (int i = 0; i < 5; i++) {
        sprintf(buffer, "%d:\t", i + 1);
        uart_puts(buffer);
        for (int j = 0; j < 16; j++) {
            sprintf(buffer, "%u", flex_sensors[j][i]);
            uart_puts(buffer);
            uart_putc('\t');
        }
        uart_putc('\n');
    }
    uart_puts("==============================================================================\n");

    /* UNCOMMENT THIS WHEN TEST COMPLETE
    prepare_msg(msg, sensors_filtered);

    for (int i = 0; i < 4; i++) {
        sprintf(buffer, "%x", msg[i] & 0xff);  // sends header output with mask to buffer
        uart_puts(buffer);                     // sends buffer via UART
    }
    for (int i = 4; i < 24; i++) {
        sprintf(buffer, "%u", msg[i]);         // sends data output to buffer
        uart_puts(buffer);                     // sends buffer via UART
    }
    uart_putc('\n');
    */
}

/* prepare message to send via UART */
void prepare_msg(char* msg, unsigned int* sensors) {
    /* header part */
    msg[0] = 0xA1;
	msg[1] = 0xB2;
	msg[2] = 0xC3;
	msg[3] = 0xD4;

    /* data part */
    for (int i = 0; i < 11; i++) {
        msg[2 * i + 4] = sensors[i];       // take value from sensors array and write it to msg array
        msg[2 * i + 5] = sensors[i] >> 8;  // take value from sensors array, shift it by 8 bits and write it to msg array
    }

    /* checksum part */
    unsigned int sum = 0;
    for (int i = 0; i < 22; i++) {
        sum += msg[4 + i];       // compute checksum by adding all values from data part of msg array
    }
    msg[4 + 22] = sum;           // write checksum value to msg array
    msg[4 + 22 + 1] = sum >> 8;  // shift checksum value by 8 bits and write it to msg array
}

int main(void) {
    sei();
    i2c_init();
    imu_init();
    init_flex_sensors();
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    _delay_ms(150);

    while (1) {
        read_and_send_sensors();
        _delay_ms(2000);
    }
}
