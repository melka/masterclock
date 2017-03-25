# masterclock

Master clock for Lepaute 30 seconds alternating pulse slave clock and possibly other models

## background

Slave clocks are old school clocks mostly used in train stations, hospitals, factories, etc... places that needed to have accurate timekeeping and synchronized clocks in a lot of places. A master clock was responsible for keeping time and controlling all the slave clocks. There were a number of clock manufacturers, and there were a lot of way of synching the slave clocks to the master clock.

If you find a beautiful industrial slave clock, there are several options :
* Find the master clock to go with it, but you'll need space, money and time to take care of it
* Remove the old mechanical movement and replace it with a quartz one (shame on you)
* Buy one of [these](http://www.bmumford.com/clocks/driver/driver.html), but what's the fun in that ?
* Make a teeny-tiny master clock.

So here comes this project.

## goal

![clock](https://raw.githubusercontent.com/melka/masterclock/master/pictures/clock.jpg)

I got hold of two slave clocks that were salvaged from an hospital that was being destroyed in France. One of them is a Lepaute clock. 

Those clocks use an alternating pulse every 30 seconds to make the minute hand advance half a minute. The pulse is ~100ms and a 1.5V battery is enough to make it go.

I wanted to make a board that could run for more than a year on a single AA battery, be highly accurate and that would be as small as possible (in this case, just enough space to fit the battery and a screw terminal to plug the clock mechanism).

When all the chips are sleeping, my multimeter shows nothing, and the wake-up time is so short that I can't measure it precisely. I'm planning on getting or building a [uCurrent Gold](https://www.eevblog.com/projects/ucurrent/) to measure those and have an accurate estimation of the run time.

## BOM

Full BOM can be found on [mouser](https://www.mouser.fr/ProjectManager/ProjectDetail.aspx?AccessID=3831f781b1)

* [TPS60313](http://www.ti.com/product/TPS60313) : Charge Pump
* [MSP430G2553](http://www.ti.com/product/MSP430G2553) : MCU
* [DS3231SN](https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS3231.html) : I2C RTC
* [DRV8838](http://www.ti.com/product/DRV8838) : Brushed DC Motor Driver

## circuit

Power is provided by a single AA battery.

![PSU](https://raw.githubusercontent.com/melka/masterclock/master/pictures/PSU.png)

A TPS60313 converts this voltage to 3V in order to power the MCU and RTC. Input voltage can go as low as 0.9V in order to get 3V output. Quiescent current at no-load is 35uA.

![MCU](https://raw.githubusercontent.com/melka/masterclock/master/pictures/MCU.png)

MCU is an MSP430G2553 spending most of its time in LPM4 (consumption should be about 0.15uA)

![RTC](https://raw.githubusercontent.com/melka/masterclock/master/pictures/RTC.png)

RTC is a DS3231SN, powered only via VBAT to reduce the current consumption of the device. According to the datasheet, Ibatt goes up to 70uA when active, while Ivcc goes up to 200uA. Ibatt when only timekeeping is 0.84uA typical.
A DS3231M could be used, but the SN is a tad bit more accurate (+/-2PPM against +/-5PPM for the M).

![MOTOR](https://raw.githubusercontent.com/melka/masterclock/master/pictures/MOTOR.png)

To drive the mechanism, an alternating pulse is necessary. This could be done with a discrete H-bridge, but I opted for a DRV8838, directly powered from an MCU pin. This way, the DRV isn't even sleeping, but only switched on when needed, once every 30 seconds. Vmotor comes directly from the AA battery.

## on soldering

![board](https://raw.githubusercontent.com/melka/masterclock/master/pictures/soldered.jpg)

This board being an exercise in miniaturisation, all the resistors and caps are 0402... I advise you not to try to solder this board without a good-ish soldering iron and an hot air gun. I'm using of the cheap chinese all-in-one stations, but without hot air, some components are next to impossible, especially the DRV8838.

![soldering](http://www.telesailor.de/ImageHandler/350-350/UploadFiles/Images/2014-04/Scotle-8586D.jpg)

## firmware

Firmware was developped using TI's Cloud IDE since I'm using a Mac and it was the simplest solution.

On boot, the MSP430 runs at 1MHZ and sets some configuration on the RTC :
 * Enable Oscillator
 * Battery-Backed Square-Wave Enable allows the interrupt to be sent when powered by VBAT
 * Rate Select set to 1Hz
 * Interrupt Control set to Square-Wave
 * Disable Alarms

P1.4 of the MSP430 is set as an interrupt pin, and then MSP430 is put to sleep with external interrupts enabled (LPM4 + GIE).

Each second, the RTC sends a pulse on P1.4, which wakes up the MSP430. A counter is incremented and when this seconds counter == 30, power is applied to the DRV8838 by putting P1.0 high (DRV_VCC), pulse direction is reversed by toggling P1.1 (DRV_PH) and a 100ms pulse is sent via P1.2 (DRV_EN). Counter is reset and the MSP430 goes back to sleep.

### references
 * [Horloge Mère (in French)](http://f5mna.free.fr/h_mere.htm)
 * [Powering an MSP430 From a Single Battery Cell](http://www.ti.com/lit/an/slaa398/slaa398.pdf)