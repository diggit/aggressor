# Aggressor MOD
This repo contains firmware replacement for modified HITEC Aggressor SRX and documentation of this TX including full schematics (shhh).
## HW modifications
* MCU changed to atmega32
* Original display replaced with graphic LCD display from nokia 3310
 * this involves some routing pin changes (SPI SS pin can't be used as input)
* serial header for flashing FW using bootloader (optional)
* capacitor to base of force off transistor, look into schematic
<a name="crystal-mod"></a>
* 16MHz crystal for higher output resolution (0.5us)

## Capabilities
* everything can be easily reconfigured!
* easy to add any functionality and menus
* **inputs (5)**
	* inverting *[stored globally]*
	* enabling (freeze) *[stored globally]*
	* 3 analog inputs: wheel, trigger, D/R
		* all could be calibrated *[stored globally]*
        * [trim](http://www.rcheliwiki.com/Trim) - assign any trim *[stored per model]*
		* [oversampled](http://www.atmel.com/Images/doc2505.pdf)
		* deadzone *[stored globally]*
	* 1 toggling input: bug thumb button
		 * levels setup *[stored per model]*
	* 1 multi level input: ALT
	     * levels setup *[stored per model]*
* **outputs (5)**
	* 50Hz ppm
	* 0.5us pulse length precision ( [with 16MHz crystal](#crystal-mod) )
	* no jitter - hw generated pulses
	* all output config is  *[stored per model]*
	* [subtrim](http://www.rcheliwiki.com/Subtrim)
	* **inMIX** - every output get data from every input multiplied by coeficient *[stored per model]*
	* EPA, separate for each side
* **model**
	* store up to 10 models
	* model name up to 8 alphanumeric chars
	* copy actual model to another slot
	* switch to different model at runtime
	* delete model at any slot (except actual)
* **config**
	* *[stored globally]*
	* all beeps enable/diable
	* button beeps enable/disable
	* battery alarm voltage
	* enable disable PPM output

## Contents
* **rev** reverse engineered schematics of TX in KiCAD (PNG may not be up to date)
* **code** firmware code
* **doc** photos and etc...


### useful info

All SMD electronic parts are glued to PCB! If you want preserve stock FW, you have to unsolder original MCU. Stock FW can't be dumped, lockbits SET!
#####desoldering MCU
Whole MCU is glued by 4 glue dots! Add a lot of solder on all pins, then heat it above melt point and pull MCU from board. Be sure to melt all solder completely, because you have to pull hard but not pull up PCB pads!
*(I've destroyed my original MCU due to overheating chip. Then I realized, that all parts are glued to PCB. Pros: higher motivation to finish this FW)*

| VF header |	| ISP header |
|-----------|---|-----------|
| PPM |			| +5V |
| VCC |			| MISO |
| NC |			| RST |
| GND |			| SOCKET |
| ANT |			| MOSI |
| |				| GND |
