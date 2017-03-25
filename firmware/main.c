/*
    MSP430 POWER CONSUMPTION
    SLEEP MODE      PEAK        SLEEP       DIFF
    -----------------------------------------------
    GPIO /init      2.7mA       0.431mA
    GPIO init       3.7mA       0.411mA     -0.02mA
    LPM0            3.7mA       0.090mA      -0.321mA
    LPM1            3.7mA       0.090mA      0.0mA
    LPM2            3.7mA       0.034mA     -0.056mA
    LPM3            3.7mA       0.012mA     -0.022mA
    LPM4            3.7mA       0.000mA     ?
*/

/*
    DS3231 
    P1.4    SQW
    P1.6    SCL (I2C)
    P1.7    SDA (I2C)
    REGULAR         2.033mA
    NO LED          0.562mA
    VBAT            0.2mA
    NOTHING VBAT    0.0001m1 ?

*/

// TODO : POWER DS3231 BY VBAT ONLY

#include "msp430g2553.h"
#include "DS3231.h"
#include "TI_USCI_I2C_master.h"

unsigned char rtc_conf = 0b01000000; //0x40;

unsigned int secondsCount = 0;

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
    DCOCTL = 0; // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ; // Set DCO
    DCOCTL = CALDCO_1MHZ;
    
    P1DIR = 0xFF; // All P1.x outputs
    P2DIR = 0xFF; // All P2.x outputs
    P3DIR = 0xFF; // All P2.x outputs
    P1OUT = 0x00; // All Off
    P2OUT = 0x00; // All Off
    P3OUT = 0x00; // All Off
    
    // DS3231 POWER PIN
    DS3231_set_RTCconf(&rtc_conf);
	
    // RTC SQW INTERRUPT
    P1DIR &= ~BIT4; // Set P1.4 to input
    P1REN |= BIT4; // Enable pullup resistor of P1.4 (default: GND)
    P1OUT |= BIT4; // Set pullup resistor to active (+3.3V) mode
    P1IE |= BIT4; // P1.4 interrupt enabled
    P1IFG &= ~BIT4; // P1.4 interrupt flag cleared
    
    __bis_SR_register(LPM4_bits+GIE); // Enter LPM0 w/ int until Byte RXed
}

#pragma vector=PORT1_VECTOR
__interrupt void PORT_1(void) {
    secondsCount++;
    if (secondsCount == 2) {
        P1OUT ^= BIT0; // Toggle P1.0
        P1OUT ^= BIT1; // Toggle P1.1
        P1OUT ^= BIT2; // Toggle P1.2
        __delay_cycles(100000);
        P1OUT ^= BIT2; // Toggle P1.2
        secondsCount = 0;
        P1OUT ^= BIT0; // Toggle P1.0
    }
    P1IFG &= ~BIT4; // P1.4 interrupt flag cleared
}
