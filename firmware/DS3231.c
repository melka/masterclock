/*
 * ds3231.c
 *
 * DS3231 read and set functions
 *
 * HouYu Li <karajan_ii@hotmail.com>
 *
 * Does not apply any license. Use as you wish!
 */

#include <msp430g2553.h>
#include "DS3231.h"
#include "TI_USCI_I2C_master.h"

void DS3231_set_init() {
	DS3231_set_init_time();
	DS3231_set_init_alarm();
	DS3231_set_init_conf();
}

void DS3231_set_init_time() {
	unsigned char init_clock[7];

	init_clock[0] = DEC_to_BCD(59);
	init_clock[1] = DEC_to_BCD(59);
	init_clock[2] = DEC_to_BCD(23);
	init_clock[3] = DEC_to_BCD(2); // Week start on Sunday : 1
	init_clock[4] = DEC_to_BCD(31);
	init_clock[5] = DEC_to_BCD(12);
	init_clock[6] = DEC_to_BCD(12); // 2012

	DS3231_set_time(init_clock);
}

void DS3231_set_init_alarm() {
	unsigned char alarm1[4] = {0, 0, 0, 0x80}; // Default Alarm 1 matches hour and minute while second default to 0
	unsigned char alarm2[3] = {0, 0, 0x80}; // Default Alarm 2 matches hour and minute

	DS3231_set_alarms(alarm1, alarm2);
}

void DS3231_set_init_conf() {
    unsigned char rtc_conf = 0x40;
	DS3231_set_RTCconf(&rtc_conf);
}

void DS3231_set_time(unsigned char * clock) {
	unsigned char trans_data[8], data_bytes;
	trans_data[0] = DS3231_datetime_addr;
	data_bytes = 7;
	while (data_bytes > 0) {
		trans_data[8 - data_bytes] = *clock;
		clock++;
		data_bytes--;
	}

	__enable_interrupt();
	TI_USCI_I2C_transmitinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_transmit(8, trans_data);
	while ( TI_USCI_I2C_notready() );
}

void DS3231_read_time(unsigned char * clock) {
	unsigned char reg_addr[1];
	reg_addr[0] = DS3231_datetime_addr;

	__enable_interrupt();
	TI_USCI_I2C_transmitinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_transmit(1, reg_addr);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_receiveinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_receive(7, clock);
	while ( TI_USCI_I2C_notready() );
}

void DS3231_read_alarms(unsigned char * alarm1, unsigned char * alarm2) {
	unsigned char reg_addr1[1];
	unsigned char reg_addr2[1];
	reg_addr1[0] = DS3231_alarm1_addr;
	reg_addr2[0] = DS3231_alarm2_addr;

	__enable_interrupt();
	TI_USCI_I2C_transmitinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_transmit(1, reg_addr1);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_receiveinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_receive(4, alarm1);
	while ( TI_USCI_I2C_notready() );

	TI_USCI_I2C_transmitinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_transmit(1, reg_addr2);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_receiveinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_receive(3, alarm2);
	while ( TI_USCI_I2C_notready() );
}

void DS3231_set_alarms(unsigned char * alarm1, unsigned char * alarm2) {
	unsigned char data_bytes;
	unsigned char trans_data[5];
	trans_data[0] = DS3231_alarm1_addr;
	data_bytes = 4;
	while (data_bytes > 0) {
		trans_data[5 - data_bytes] = *alarm1;
		alarm1++;
		data_bytes--;
	}

	__enable_interrupt();
	TI_USCI_I2C_transmitinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_transmit(5, trans_data);
	while ( TI_USCI_I2C_notready() );

	trans_data[0] = DS3231_alarm2_addr;
	data_bytes = 3;
	while (data_bytes > 0) {
		trans_data[4 - data_bytes] = *alarm2;
		alarm2++;
		data_bytes--;
	}

	__enable_interrupt();
	TI_USCI_I2C_transmitinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_transmit(4, trans_data);
	while ( TI_USCI_I2C_notready() );
}

void DS3231_read_RTCconf(unsigned char * rtc_conf) {
	unsigned char reg_addr[1];
	reg_addr[0] = DS3231_ctrl_addr;

	__enable_interrupt();
	TI_USCI_I2C_transmitinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_transmit(1, reg_addr);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_receiveinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_receive(1, rtc_conf);
	while ( TI_USCI_I2C_notready() );
}

void DS3231_set_RTCconf(unsigned char * rtc_conf) {
	unsigned char trans_data[2];
	trans_data[0] = DS3231_ctrl_addr;
	trans_data[1] = *rtc_conf;

	__enable_interrupt();
	TI_USCI_I2C_transmitinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_transmit(2, trans_data);
	while ( TI_USCI_I2C_notready() );
}

void DS3231_clear_alarm_FG() {
	unsigned char rtc_stat;
	unsigned char trans_data[2];
	trans_data[0] = DS3231_stat_addr;

	__enable_interrupt();
	TI_USCI_I2C_transmitinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_transmit(1, trans_data);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_receiveinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_receive(1, &rtc_stat);
	while ( TI_USCI_I2C_notready() );

	rtc_stat &= ~0x03;
	trans_data[1] = rtc_stat;
	__enable_interrupt();
	TI_USCI_I2C_transmitinit(DS3231_i2c_addr, I2C_BR);
	while ( TI_USCI_I2C_notready() );
	TI_USCI_I2C_transmit(2, trans_data);
	while ( TI_USCI_I2C_notready() );
}

/**
 * Following functions are copied from
 * D3232 RTC Example Using MSP430G2452
 * by Kerr D. Wong
 * http://www.kerrywong.com
 */

// Convert normal decimal numbers to binary coded decimal
unsigned char DEC_to_BCD(unsigned char val) {
  return ( (val/(char)10*(char)16) + (val%(char)10) );
}

// Convert binary coded decimal to normal decimal numbers
unsigned char BCD_to_DEC(unsigned char val) {
  return ( (val/(char)16*(char)10) + (val%(char)16) );
}
