/*
 * ds3231.h
 *
 * DS3231 read and set functions
 *
 * HouYu Li <karajan_ii@hotmail.com>
 *
 * Does not apply any license. Use as you wish!
 */

#ifndef DS3231_H_
#define DS3231_H_

#define DS3231_i2c_addr		0x68
#define DS3231_datetime_addr 	0x00
#define DS3231_alarm1_addr	0x07
#define DS3231_alarm2_addr	0x0B
#define DS3231_ctrl_addr 	0x0E
#define DS3231_stat_addr	0x0F

//unsigned char DS3231_INIT_CONF = 0x40; // 1Hz SQW Interrupt

void DS3231_set_init();
void DS3231_set_init_time();
void DS3231_set_init_alarm();
void DS3231_set_init_conf();
void DS3231_set_time(unsigned char * clock);
void DS3231_read_time(unsigned char * clock);
void DS3231_set_alarms(unsigned char * alarm1, unsigned char * alarm2);
void DS3231_read_alarms(unsigned char * alarm1, unsigned char * alarm2);
void DS3231_set_RTCconf(unsigned char * rtc_conf);
void DS3231_read_RTCconf(unsigned char * rtc_conf);
void DS3231_clear_alarm_FG();

unsigned char DEC_to_BCD(unsigned char val);
unsigned char BCD_to_DEC(unsigned char val);

#endif /* DS3231_H_ */
