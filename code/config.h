#ifndef _CHARGER_DEFS
	#define _CHARGER_DEFS

	#include <avr/io.h>
	#include <inttypes.h>

	//versioning numbers & info
	#define _ver	0 //every change indicates incompatible change in EEPROM stored data
	#define _subver	2
	#define ID		"Aggressor MOD"
	//#define author	"xorly"

	#define VENDOR_ID	0xaa
	#define FW_ID 		0xbb

	//freq of MCU clocking
	#define FOSC 16000000UL

	#define BAUD 9600UL
	#define UART_TX
	#define UART_RX
	#define UART_RX_INT_EN

	#define MODEL_MAX_NAME_LENGTH	8 //NOT including terminating 0

	#define PPM_FREQUENCY		50

	//#define BEEP_FREQ_MAX		5000

	#define ADC_SAMPLES	10
	#define ADC_DIVIDER	5  //oversample

	#define HW_INPUTS			5
	#define PPM_OUTPUT_COUNT	5

	#define PPM_PULSE_CENTER	1500 // servo center position pulse length
	#define PPM_PULSE_DEFLECTION	500


	#define VCC_MAX_VOLTAGE_mV	24400

	//normalization constant for analog inputs
	//eg. if 1000, value of input will be normalized to be within range -1000 , 1000
	#define IN_NORM	3000

	#define OUTPUT_SUBTRIM_PATH_DIV	10
	// #define OUTPUT_FORCE_1US_RESOLUTION

	#define INPUT_TRIM_USER_MULTIPLIER	2
	#define OUTPUT_SUBTRIM_USER_MULTIPLIER 2



	//uncomment if LCD row should be inverted on every update
	// #define LCD_UPDATE_DEBUG
	//just for worst case vwrification
	// #define LCD_FORCE_TOTAL_UPDATE
	//#define LCD_MAX_FPS_DIVIDE 1
	// #define LCD_NO_SKIP_UPDATE

	#define MENU_HEADER_LENGTH	8 // in characters (6px)


	// config
	#define ONE_SECOND_TICK_CALIBRATION	PPM_FREQUENCY //50

	//clock have prescaler separately
	#define CLOCK_PRESCALE PPM_FREQUENCY //50

	typedef struct {
		uint8_t low_batt;
		uint8_t beep_keys:1;
		uint8_t beep:1;
		uint8_t beep_clock:1;
		uint8_t backlight:1;
	}config_t;

	config_t config;

	//							freq,duration
	#define BEEP_IN_BOUNDS		2000,100
	#define BEEP_IN_COMMON		1000,100
	#define BEEP_TRIM_COMMON	1000,20
	#define BEEP_TRIM_ZERO		2000,20
	#define BEEP_KEY_LONG		500,20
	#define BEEP_KEY_COMMON		300,20
	#define BEEP_NOTIF			1000,40
	#define BEEP_ERROR			500,1000
	#define BEEP_CLOCK_MINUTE	1000,100

	//#define STACK_MONITOR_ENABLED


#endif
