void imu_init();  // init IMU function
unsigned char imu_read(unsigned char);  // read method
void imu_write(unsigned char, unsigned char);  // write method
void imu_read_acc_gyro(int*);  // read accel and gyro registers of IMU
void read_and_send_sensors();  // read values from sensors
void prepare_msg(char*, unsigned int*);  // prepare message to send via UART
