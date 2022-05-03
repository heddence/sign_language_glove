#ifndef F_CPU
#undef F_CPU
    #define F_CPU 16000000UL
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
    i2c_write(value);
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

/* read values from sensors */
void read_and_send_sensors() {
    int acc_gyro_sensors[16][6];
    unsigned int sensors_filtered[6];
    char msg[4 + 12 + 2];
    char buffer[32];

    for (int i = 0; i < 16; i++) {
        imu_read_acc_gyro((int*) &acc_gyro_sensors[i]);
    }

    for (int i = 0; i < 6; i++) {
        sensors_filtered[i] = 0;
        for (int j = 0; j < 16; j++) {
            sensors_filtered[i] += acc_gyro_sensors[j][i] >> 8;
        }
    }

    prepare_msg(msg, sensors_filtered);

    uart_puts("mpu data w/o header: ");
    for (int i = 0; i < 4; i++) {
        sprintf(buffer, "%x", msg[i] & 0xff);
        uart_puts(buffer);
    }
    for (int i = 4; i < 18; i++) {
        sprintf(buffer, "%u", msg[i]);
        uart_puts(buffer);
    }
    uart_putc('\n');
}

/* prepare message to send via UART */
void prepare_msg(char* msg, unsigned int* sensors) {
    // TODO: MAKE CHAR FROM INT
    msg[0] = 0xA1;
	msg[1] = 0xB2;
	msg[2] = 0xC3;
	msg[3] = 0xD4;

    for (int i = 0; i < 6; i++) {
        msg[2 * i + 4] = sensors[i];
        msg[2 * i + 5] = sensors[i] >> 8;
    }

    /* checksum */
    unsigned int sum = 0;
    for (int i = 0; i < 12; i++) {
        sum += msg[4 + i];
    }
    msg[4 + 12] = sum;
    msg[4 + 12 + 1] = sum >> 8;
}

int main(void) {
    i2c_init();
    imu_init();
    uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
    sei();
    _delay_ms(150);

    while (1) {
        read_and_send_sensors();
        _delay_ms(2000);
    }
}
