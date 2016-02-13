//********************************************************
//****  Header file for 3310_routines.c  *****
//********************************************************
//Controller:	ATmega32 (Clock: 1 Mhz-internal)
//Compiler:		AVR_GCC
//Author:		CC Dharmani, Chennai (India)
//		www.dharmanitech.com
//Date:			Sep 2008
//
//Modifed by Michael Spiceland (http://tinkerish.com) to
//pixel level functions with lcd_buffer[][].
//Jan 2009
//
//********************************************************
#ifndef _3310_ROUTINES_H_
#define _3310_ROUTINES_H_

    #include <avr/pgmspace.h>
    #include <avr/io.h>
    #include <inttypes.h>
    #include "config.h"

    #include "misc.h"

    //uncomment if LCD row should be inverted on every update
	//#define LCD_UPDATE_DEBUG
	//just for worst case vwrification
	//#define LCD_FORCE_TOTAL_UPDATE
    //update even when nothing changed
    //#define LCD_NO_SKIP_UPDATE



    #define DC_PIN  (1<<PD3)
    #define RST_PIN (1<<PD4)
    #define SCE_PIN (1<<PD6)

    #define SET_DC_PIN                 PORTD |= DC_PIN
    #define CLEAR_DC_PIN               PORTD &= ~DC_PIN
    #define SET_SCE_PIN                PORTD |= SCE_PIN
    #define CLEAR_SCE_PIN              PORTD &= ~SCE_PIN
    #define SET_RST_PIN                PORTD |= RST_PIN
    #define CLEAR_RST_PIN              PORTD &= ~RST_PIN

    #define HW_ROWS     6
    #define HW_COLUMNS  84
    #define RES_MAX_X       (HW_COLUMNS-1)
    #define RES_MAX_Y       (HW_ROWS*8-1)

    #define CHAR_WIDTH  6//how many columns 1 char takes (incl space cols)

    #define CHAR_SPACE(N)   (CHAR_WIDTH*N)

    uint8_t lcd_buffer[HW_ROWS][HW_COLUMNS];


    /*--------------------------------------------------------------------------------------------------
                                     Public function prototypes
    --------------------------------------------------------------------------------------------------*/


    //draw flags
    #define OVERWRITE   (1<<0)
    #define INVERT      (1<<1)
    #define FILL        (1<<2)
    #define CLEAR       (1<<3)
    #define RELATIVE    (1<<4) //for text, X an Y coordinates reused

    //BAR TYPES
    #define BAR_VERTICAL	1
    #define BAR_HORIZONTAL	2

    typedef struct{
        uint8_t update_rows:6;
    }lcd_status_t;

    typedef struct{
        uint8_t draw_outside:1;
        uint8_t text_wrap:1;
    }lcd_error_t;

    extern lcd_error_t LCD_error;

    #define ALL_ROWS_MODIFIED ((1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5))


    void spi_init(void);
    void LCD_init ( void );
    void LCD_writeCommand ( unsigned char command );
    void LCD_writeData ( unsigned char Data );
    void LCD_clear ( void );
    void LCD_update( void );
    void LCD_gotoXY ( unsigned char x, unsigned char y);
    void LCD_writeChar ( unsigned char character, uint8_t flags );
    void LCD_writeString_XY ( uint8_t x, uint8_t y, const char *string, uint8_t flags );
    void LCD_drawBorder (void );
    void LCD_drawHline( uint8_t X, uint8_t Y, int8_t width);
    void LCD_drawRectangle(uint8_t x_start, uint8_t y_start, int8_t width, int8_t height, uint8_t flags);
    void LCD_clear_partial(void);
    void LCD_bitmap_pgm(const uint8_t *pgm_array);
    void LCD_clear_rows(uint8_t rows);
    void LCD_clear_header_text(void);
    void LCD_write_header_text(const char *string);
    void LCD_drawBar(uint8_t x_start, uint8_t y_start, int8_t width, int8_t height, int8_t perc, uint8_t bar_type);
    #define LCD_clear_row(S) LCD_clear_rows(1<<S)

#endif  //  _3310_ROUTINES_H_
