#include "i2c-master.h"

/* I2C initialize function */
void i2c_init() {												
	TWBR = BITRATE(TWSR = 0x00);
}	

/* I2C start function */
uint8_t i2c_start(char slave_write_address) {
	uint8_t status;
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);  // Enable TWI, generate start condition and clear interrupt flag
	while (!(TWCR & (1 << TWINT)));					   // Wait until TWI finish its current job
	status = TWSR & 0xF8;							   // Read TWI status register with masking lower three bits
	if (status != 0x08)							   	   // Check whether start condition transmitted successfully
		return 0;									   // Return 0 if not
	TWDR = slave_write_address;						   // Write SLA+W in TWI data register
	TWCR = (1 << TWEN) | (1 << TWINT);				   // Enable TWI and clear interrupt flag
	while (!(TWCR & (1 << TWINT)));
	status = TWSR & 0xF8;
	if (status == 0x18)								   // Check whether SLA+W transmitted & ack received
		return 1;									   // Return 1 if yes
	if (status == 0x20)								   // Check whether SLA+W transmitted & nack received
		return 2;									   // Return 2 if yes (busy device)
	else
		return 3;									   // SLA+W failed
}

/* I2C repeated start function */
uint8_t i2c_repeated_start(char slave_read_address) {
	uint8_t status;
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);  // Enable TWI, generate start condition and clear interrupt flag
	while (!(TWCR & (1 << TWINT)));					   // Wait until TWI finish its current job

	status = TWSR & 0xF8;							   // Read TWI status register with masking lower three bits
	if (status != 0x10)							   	   // Check whether repeated start condition transmitted successfully
		return 0;									   // Return 0 if not
	TWDR = slave_read_address;						   // Write SLA+W in TWI data register
	TWCR = (1 << TWEN) | (1 << TWINT);				   // Enable TWI and clear interrupt flag
	while (!(TWCR & (1 << TWINT)));

	status = TWSR & 0xF8;
	if (status == 0x40)								   // Check whether SLA+R transmitted & ack received
		return 1;									   // Return 1 if yes
	if (status == 0x20)								   // Check whether SLA+R transmitted & nack received
		return 2;									   // Return 2 if yes (busy device)
	else
		return 3;									   // SLA+R failed
}

/* I2C stop function */
void i2c_stop() {
	TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);  // Enable TWI, generate stop condition and clear interrupt flag
	while(TWCR & (1 << TWSTO));						   // Wait until stop condition execution
}

/* I2C start wait function */
void i2c_start_wait(char slave_write_address)				
{
	uint8_t status;
	while (1)
	{
		TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);  // Enable TWI, generate start condition and clear interrupt flag */
		while (!(TWCR & (1 << TWINT)));					   // Wait until TWI finish its current job

		status = TWSR & 0xF8;							   // Read TWI status register with masking lower three bits
		if (status != 0x08)								   // Check whether repeated start condition transmitted successfully
			continue;									   // If not, continue
		TWDR = slave_write_address;						   // Write SLA+W in TWI data register
		TWCR = (1 << TWEN) | (1 << TWINT);				   // Enable TWI and clear interrupt flag
		while (!(TWCR & (1<<TWINT)));

		status = TWSR & 0xF8;
		if (status != 0x18) {							   // Check whether SLA+W transmitted & ack received
			i2c_stop();									   // If not, generate stop condition
			continue;
		}
		break;
	}
}

/* I2C write function */
uint8_t i2c_write(char data) {
	uint8_t status;
	TWDR = data;						// Copy data in TWI data register
	TWCR = (1 << TWEN) | (1 << TWINT);  // Enable TWI and clear interrupt flag
	while (!(TWCR & (1 << TWINT)));		// Wait until TWI finish its current job

	status = TWSR & 0xF8;				// Read TWI status register with masking lower three bits
	if (status == 0x28)					// Check weather data transmitted & ack received
		return 0;						// Return 0 if yes
	if (status == 0x30)					// Check weather data transmitted & nack received
		return 1;						// Return 1 if yes
	else
		return 2;						// Transmission failed
}

/* I2C read ack function */
char i2c_read_ack() {
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);  // Enable TWI, generation of ack and clear interrupt flag
	while (!(TWCR & (1 << TWINT)));					  // Wait until TWI finish its current job
	return TWDR;									  // Return received data
}

/* I2C read nack function */
char i2c_read_nack() {
	TWCR = (1 << TWEN) | (1 << TWINT);  // Enable TWI and clear interrupt flag
	while (!(TWCR & (1 << TWINT)));		// Wait until TWI finish its current job
	return TWDR;						// Return received data
}