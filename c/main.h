#define UART_BAUD_RATE 9600
#define Dev24C02 0xD0  // device address of EEPROM 24C02, see datasheet

#define read_sensor(reg_H) ((imu_read(reg_H) << 8) | imu_read(reg_H + 1))

void imu_init();                               // init IMU function
unsigned char imu_read(unsigned char);         // read method
void imu_write(unsigned char, unsigned char);  // write method
void init_flex_sensors();                      // initialize flex sensors
void read_flex_sensors(uint16_t*);             // read flex sensors values
void imu_read_acc_gyro(int*);                  // read accel and gyro registers of IMU
void read_and_send_sensors();                  // read values from sensors
void prepare_msg(char*, unsigned int*);        // prepare message to send via UART
