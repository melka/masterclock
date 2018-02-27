/* MASTERCLOCK
 * Master clock for Lepaute 30 seconds alternating pulse slave clock and possibly other models.
 * Goal is minimal power consumption on a single 1.5V (AA) battery.
 */

#include <msp430g2553.h>
#include "DS3231.h"
#include "TI_USCI_I2C_master.h"

int invert_phase = 0;

int main(void) {
  // Stop watchdog timer
  WDTCTL = WDTPW | WDTHOLD;

  // Set DCO
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;

  P1DIR = 0xFF; // All P1.x outputs
  P2DIR = 0xFF; // All P2.x outputs
  P3DIR = 0xFF; // All P3.x outputs
  // ~360uA
  P1OUT = 0xFF; // All Off
  P2OUT = 0xFF; // All Off
  P3OUT = 0xFF; // All Off
  // ~340uA

  // Set P1.0 at 0 > turn off power to DRV8838
  P1OUT &= ~BIT0;

  // RTC Alarm Interrupt
  P1DIR &= ~BIT4; // Set P1.4 to input
  P1REN |= BIT4; // Enable pullup resistor of P1.4 (default: GND)
  P1OUT |= BIT4; // Set pullup resistor to active (+3.3V) mode
  P1IE |= BIT4; // P1.4 interrupt enabled
  P1IFG &= ~BIT4; // P1.4 interrupt flag cleared

  // Set initial time
  // This is not really important, the only parameter
  // interesting to change would be the second, to have
  // the first pulse earlier or later during boot
  unsigned char start_time[7];
  start_time[0] = DEC_to_BCD(25); // Second
  start_time[1] = DEC_to_BCD(59); // Minute
  start_time[2] = DEC_to_BCD(23); // Hour
  start_time[3] = DEC_to_BCD(2);  // Day of Week / Week start on Sunday : 1
  start_time[4] = DEC_to_BCD(31); // Day
  start_time[5] = DEC_to_BCD(12); // Month
  start_time[6] = DEC_to_BCD(12); // Year

  DS3231_set_time(start_time);

  // Set RTC Alarms
  // Alarm 1 : We want alarm 1 to generate the interrupt
  // eeach time the time goes past HH:MM:30s
  // This would give is a pulse every minute
  unsigned char alarm1_conf[4] = {
      0b00110000,    // A1M1 0 : 30s
      0b10000000,    // A1M2 1 : 0m
      0b10000000,    // A1M3 1 : 0h
      0b10000000     // A1M4 1 : 0d
  };
  // Alarm 2 : Alarm 2 will generate the interrupt
  // every time the time goes past HH:MM:00s
  // This will give us a pulse every minute
  unsigned char alarm2_conf[3] = {
      0b10000000,    // A2M1 1 : 0m
      0b10000000,    // A2M2 1 : 0h
      0b10000000     // A2M3 1 : 0d seconds
  };
  // Alarm 1 + Alarm 2 totals a pulse every 30 seconds

  DS3231_set_alarms(alarm1_conf, alarm2_conf);

  // Set RTC configuration
  // Enable Alarm 1 & 2 on /SQW/INT pin
  unsigned char rtc_conf = 0b00000111;
  DS3231_set_RTCconf(&rtc_conf);

  // Clear alarm flags
  DS3231_clear_alarm_FG();

  // Let's get some zzzz
  __bis_SR_register(LPM4_bits+GIE);
  // 105nA on Sleep, 370uA on wake for MSP430
  // 915nA DS3231 + MSP430 on
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void) {
    // Go back to sleep when interrupt routine ends
    __bis_SR_register_on_exit(LPM4_bits+GIE);
    // P1.4 interrupt flag cleared
    P1IFG &= ~BIT4;
    // Clear alarm flags
    DS3231_clear_alarm_FG();

    // Power up DRV8838
    // Toggle P1.0 high
    P1OUT |= BIT0;

    // Check if we need to pull P1.1 high
    // to invert the phase of the pulse
    invert_phase = invert_phase == 0 ? 1 : 0;
    if (invert_phase == 1) {
      P1OUT |= BIT1;
    }

    // Enable DRV8838 pulse for 200ms
    P1OUT |= BIT2;
    __delay_cycles(200000);
    P1OUT &= ~BIT2;

    // Pull phase down
    P1OUT &= ~BIT1;

    // Shut down DRV8838
    P1OUT &= ~BIT0;
}
