/**
 * Originally from TI
 * Added some customized definitions
 *
 * HouYu Li <karajan_ii@hotmail.com>
 */

#ifndef USCI_LIB
#define USCI_LIB

// Customized definitions
// Houyu Li <karajan_ii@hotmail.com>
#define I2C_PSEL	P1SEL
#define I2C_PSEL2	P1SEL2

#define I2C_SDA		BIT7
#define I2C_SCL		BIT6

#define BR_100KHZ_12MHZ 0x78									// Baud Rate divider for 100kHz @12MHz
#define BR_400KHZ_12MHZ 0x1E									// Baud Rate divider for 400kHz @12MHz
#define BR_100KHZ_1MHZ 0x0B									// Baud Rate divider for 100kHz @1MHz

#define I2C_BR	BR_100KHZ_12MHZ
// ======== //

void TI_USCI_I2C_receiveinit(unsigned char slave_address, unsigned char prescale);
void TI_USCI_I2C_transmitinit(unsigned char slave_address, unsigned char prescale);


void TI_USCI_I2C_receive(unsigned char byteCount, unsigned char *field);
void TI_USCI_I2C_transmit(unsigned char byteCount, unsigned char *field);


unsigned char TI_USCI_I2C_slave_present(unsigned char slave_address);
unsigned char TI_USCI_I2C_notready();


#endif
