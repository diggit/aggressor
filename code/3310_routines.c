//********************************************************
//**** Functions for Interfacing NOKIA 3310 Display *****
//********************************************************
//Controller:	ATmega32 (Clock: 1 Mhz-internal)
//Compiler:		AVR-GCC
//Author:		CC Dharmani, Chennai (India)
//		www.dharmanitech.com
//Date:			Sep 2008
//
//Modifed by Michael Spiceland (http://tinkerish.com) to
//pixel level functions with lcd_buffer[][].
//Jan 2009
//
//********************************************************


#include "3310_routines.h"

//#define MEGA_FONT

//global variable for remembering where to start writing the next text string on 3310 LCD
unsigned char char_start;

lcd_error_t LCD_error={0,0};

lcd_status_t LCD={0};

/* current cursor */
static unsigned char cursor_row = 0; /* 0-6 */
static unsigned char cursor_col = 0; /* 0-83 */
unsigned char lcd_buffer[HW_ROWS][HW_COLUMNS];


//***** Small fonts (5x7) **********

static const unsigned char smallFont[] PROGMEM =
{
	0x00, 0x00, 0x00, 0x00, 0x00,   // sp
	0x00, 0x00, 0x2f, 0x00, 0x00,    // !
	0x00, 0x07, 0x00, 0x07, 0x00,   // "
	0x14, 0x7f, 0x14, 0x7f, 0x14,   // #
	0x24, 0x2a, 0x7f, 0x2a, 0x12,   // $
	0xc4, 0xc8, 0x10, 0x26, 0x46,   // %
	0x36, 0x49, 0x55, 0x22, 0x50,   // &
	0x00, 0x05, 0x03, 0x00, 0x00,   // '
	0x00, 0x1c, 0x22, 0x41, 0x00,   // (
	0x00, 0x41, 0x22, 0x1c, 0x00,   // )
	0x14, 0x08, 0x3E, 0x08, 0x14,   // *
	0x08, 0x08, 0x3E, 0x08, 0x08,   // +
	0x00, 0x00, 0x50, 0x30, 0x00,   // ,
	0x10, 0x10, 0x10, 0x10, 0x10,   // -
	0x00, 0x60, 0x60, 0x00, 0x00,   // .
	0x20, 0x10, 0x08, 0x04, 0x02,   // /
	0x3E, 0x51, 0x49, 0x45, 0x3E,   // 0
	0x00, 0x42, 0x7F, 0x40, 0x00,   // 1
	0x42, 0x61, 0x51, 0x49, 0x46,   // 2
	0x21, 0x41, 0x45, 0x4B, 0x31,   // 3
	0x18, 0x14, 0x12, 0x7F, 0x10,   // 4
	0x27, 0x45, 0x45, 0x45, 0x39,   // 5
	0x3C, 0x4A, 0x49, 0x49, 0x30,   // 6
	0x01, 0x71, 0x09, 0x05, 0x03,   // 7
	0x36, 0x49, 0x49, 0x49, 0x36,   // 8
	0x06, 0x49, 0x49, 0x29, 0x1E,   // 9
	0x00, 0x36, 0x36, 0x00, 0x00,   // :
	0x00, 0x56, 0x36, 0x00, 0x00,   // ;
	0x08, 0x14, 0x22, 0x41, 0x00,   // <
	0x14, 0x14, 0x14, 0x14, 0x14,   // =
	0x00, 0x41, 0x22, 0x14, 0x08,   // >
	0x02, 0x01, 0x51, 0x09, 0x06,   // ?
	0x32, 0x49, 0x59, 0x51, 0x3E,   // @
	0x7E, 0x11, 0x11, 0x11, 0x7E,   // A
	0x7F, 0x49, 0x49, 0x49, 0x36,   // B
	0x3E, 0x41, 0x41, 0x41, 0x22,   // C
	0x7F, 0x41, 0x41, 0x22, 0x1C,   // D
	0x7F, 0x49, 0x49, 0x49, 0x41,   // E
	0x7F, 0x09, 0x09, 0x09, 0x01,   // F
	0x3E, 0x41, 0x49, 0x49, 0x7A,   // G
	0x7F, 0x08, 0x08, 0x08, 0x7F,   // H
	0x00, 0x41, 0x7F, 0x41, 0x00,   // I
	0x20, 0x40, 0x41, 0x3F, 0x01,   // J
	0x7F, 0x08, 0x14, 0x22, 0x41,   // K
	0x7F, 0x40, 0x40, 0x40, 0x40,   // L
	0x7F, 0x02, 0x0C, 0x02, 0x7F,   // M
	0x7F, 0x04, 0x08, 0x10, 0x7F,   // N
	0x3E, 0x41, 0x41, 0x41, 0x3E,   // O
	0x7F, 0x09, 0x09, 0x09, 0x06,   // P
	0x3E, 0x41, 0x51, 0x21, 0x5E,   // Q
	0x7F, 0x09, 0x19, 0x29, 0x46,   // R
	0x46, 0x49, 0x49, 0x49, 0x31,   // S
	0x01, 0x01, 0x7F, 0x01, 0x01,   // T
	0x3F, 0x40, 0x40, 0x40, 0x3F,   // U
	0x1F, 0x20, 0x40, 0x20, 0x1F,   // V
	0x3F, 0x40, 0x38, 0x40, 0x3F,   // W
	0x63, 0x14, 0x08, 0x14, 0x63,   // X
	0x07, 0x08, 0x70, 0x08, 0x07,   // Y
	0x61, 0x51, 0x49, 0x45, 0x43,   // Z
	0x00, 0x7F, 0x41, 0x41, 0x00,   // [
	0x55, 0x2A, 0x55, 0x2A, 0x55,   // 55
	0x00, 0x41, 0x41, 0x7F, 0x00,   // ]
	0x04, 0x02, 0x01, 0x02, 0x04,   // ^
	0x40, 0x40, 0x40, 0x40, 0x40,   // _
	0x00, 0x01, 0x02, 0x04, 0x00,   // '
	0x20, 0x54, 0x54, 0x54, 0x78,   // a
	0x7F, 0x48, 0x44, 0x44, 0x38,    // b
	0x38, 0x44, 0x44, 0x44, 0x20,   // c
	0x38, 0x44, 0x44, 0x48, 0x7F,   // d
	0x38, 0x54, 0x54, 0x54, 0x18,   // e
	0x08, 0x7E, 0x09, 0x01, 0x02,   // f
	0x0C, 0x52, 0x52, 0x52, 0x3E,   // g
	0x7F, 0x08, 0x04, 0x04, 0x78,   // h
	0x00, 0x44, 0x7D, 0x40, 0x00,   // i
	0x20, 0x40, 0x44, 0x3D, 0x00,   // j
	0x7F, 0x10, 0x28, 0x44, 0x00,   // k
	0x00, 0x41, 0x7F, 0x40, 0x00,   // l
	0x7C, 0x04, 0x18, 0x04, 0x78,   // m
	0x7C, 0x08, 0x04, 0x04, 0x78,   // n
	0x38, 0x44, 0x44, 0x44, 0x38,   // o
	0x7C, 0x14, 0x14, 0x14, 0x08,   // p
	0x08, 0x14, 0x14, 0x18, 0x7C,   // q
	0x7C, 0x08, 0x04, 0x04, 0x08,   // r
	0x48, 0x54, 0x54, 0x54, 0x20,   // s
	0x04, 0x3F, 0x44, 0x40, 0x20,   // t
	0x3C, 0x40, 0x40, 0x20, 0x7C,   // u
	0x1C, 0x20, 0x40, 0x20, 0x1C,   // v
	0x3C, 0x40, 0x30, 0x40, 0x3C,   // w
	0x44, 0x28, 0x10, 0x28, 0x44,   // x
	0x0C, 0x50, 0x50, 0x50, 0x3C,   // y
	0x44, 0x64, 0x54, 0x4C, 0x44,   // z
	0x00, 0x06, 0x09, 0x09, 0x06    // º
};




//******* VERY LARGE FONTS **********
//used here for displaying temperature

#ifdef MEGA_FONT
	static const unsigned char number[13][3][16] PROGMEM =
	{
	{{0,128,192,224,224,96,224,224,  //'0'
	192,128,0,0,0,0,0,0}
	,
	{112,255,255,1,0,0,0,0,
	255,255,254,0,0,0,0,0}
	,
	{0,15,31,60,56,48,56,56,
	31,15,3,0,0,0,0,0}
	},
	{
	{0,0,0,0,128,224,224,0, 		   //'1'
	0,0,0,0,0,0,0,0}
	,
	{0,0,3,3,3,255,255,0,
	0,0,0,0,0,0,0,0}
	,
	{0,0,56,56,56,63,63,56,
	56,56,0,0,0,0,0,0}
	},
	{
	{0,192,192,224,96,96,224,224,   //'2'
	192,128,0,0,0,0,0,0}
	,
	{0,1,0,0,128,192,224,249,
	63,31,0,0,0,0,0,0}
	,
	{0,60,62,63,63,59,57,56,
	56,56,56,0,0,0,0,0}
	},
	{
	{0,192,224,224,96,96,224,224,   //'3'
	192,192,0,0,0,0,0,0}
	,
	{0,1,0,0,48,48,56,125,
	239,207,0,0,0,0,0,0}
	,
	{0,28,56,56,48,48,56,60,
	31,15,1,0,0,0,0,0}
	},
	{
	{0,0,0,0,0,128,192,224, 		   //'4'
	224,0,0,0,0,0,0,0}
	,
	{224,240,248,222,207,199,193,255,
	255,192,192,0,0,0,0,0}
	,
	{0,0,0,0,0,0,0,63,
	63,0,0,0,0,0,0,0}
	},
	{
	{0,224,224,224,224,224,224,224,	//'5'
	224,224,224,0,0,0,0,0}
	,
	{0,63,63,63,56,56,48,112,
	240,224,0,0,0,0,0,0}
	,
	{0,28,56,56,48,48,56,60,
	31,15,1,0,0,0,0,0}
	},
	{
	{0,0,128,192,192,224,96,96,		//'6'
	224,224,0,0,0,0,0,0}
	,
	{224,254,255,55,57,24,24,56,
	240,240,192,0,0,0,0,0}
	,
	{0,15,31,28,56,48,48,56,
	31,15,7,0,0,0,0,0}
	},
	{
	{0,224,224,224,224,224,224,224,		 //'7'
	224,224,224,0,0,0,0,0}
	,
	{0,0,0,0,128,224,248,126,
	31,7,1,0,0,0,0,0}
	,
	{0,0,56,62,31,7,1,0,
	0,0,0,0,0,0,0,0}
	},
	{
	{0,128,192,224,224,96,96,224,		 //'8'
	192,192,0,0,0,0,0,0}
	,
	{0,207,255,127,56,48,112,112,
	255,239,199,0,0,0,0,0}
	,
	{3,15,31,60,56,48,48,56,
	31,31,15,0,0,0,0,0}
	},
	{
	{0,128,192,224,224,96,224,224,		 //'9'
	192,128,0,0,0,0,0,0}
	,
	{12,63,127,241,224,192,192,225,
	255,255,254,0,0,0,0,0}
	,
	{0,0,56,48,48,56,56,30,
	15,7,0,0,0,0,0,0}
	},
	{

	{0,0,0,0,0,0,0,0,	  		  		 //'.'
	0,0,0,0,0,0,0,0}
	,
	{0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0}
	,
	{60,60,60,0,0,0,0,0,
	0,0,0,0,0,0,0,0}
	},
	{
	{0,0,0,0,0,0,0,0,   					 //'+'
	0,0,0,0,0,0,0,0}
	,
	{0,0,64,64,64,64,64,254,
	254,64,64,64,64,64,0,0}
	,
	{0,0,0,0,0,0,0,15,
	15,0,0,0,0,0,0,0}
	},
	{
	{0,0,0,0,0,0,0,0, 	   				 //'-'
	0,0,0,0,0,0,0,0}
	,
	{0,64,64,64,64,64,64,0,
	0,0,0,0,0,0,0,0}
	,
	{0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0}
	}};
#endif

/*--------------------------------------------------------------------------------------------------
  Name         :  spi_init
  Description  :  Initialising atmega SPI for using with 3310 LCD
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
//SPI initialize
//clock rate: 4Mhz
void spi_init(void)
{
	SPSR |=(1<<SPI2X);//double rate from default FOSC/4 to FOSC/2
	SPCR = (1<<SPE)|(1<<MSTR);
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_init
  Description  :  LCD controller initialization.
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_init ( void )
{
	SET_RST_PIN;
	SET_SCE_PIN;
	CLEAR_RST_PIN;
	delay_ms(100);
	SET_RST_PIN;


	LCD_writeCommand( 0x21 );  // LCD Extended Commands.
	//LCD_writeCommand( 0x04 );  // Set Temp coefficent.
	LCD_writeCommand( 0x13 );  // LCD bias mode 1:48.
	LCD_writeCommand( 0xC2 );  // Set LCD Vop (Contrast).
	LCD_writeCommand( 0x20 );  // LCD Standard Commands, Horizontal addressing mode.

	LCD_writeCommand( 0x09 );  // all display segments on
	delay_ms(1);

	LCD_clear();
	LCD_writeCommand( 0x08 );  //blank
	LCD_writeCommand( 0x0c );  //normal operation
	LCD_gotoXY(0,0);
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_writeCommand
  Description  :  Sends command to display controller.
  Argument(s)  :  command -> command to be sent
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_writeCommand ( unsigned char command )
{
	CLEAR_SCE_PIN;	  //enable LCD
	CLEAR_DC_PIN;	  //set LCD in command mode
	SPDR = command;//  Send data to display controller.

	// delay_us(1);
	while ( !(SPSR & 0x80) );//  Wait until Tx register empty.

	SET_SCE_PIN;   	 //disable LCD
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_writeData
  Description  :  Sends Data to display controller.
  Argument(s)  :  Data -> Data to be sent
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_writeData ( unsigned char Data )
{
	CLEAR_SCE_PIN;	  //enable LCD
	SET_DC_PIN;	  //set LCD in Data mode
	SPCR|=(1<<DORD);//switch data as part of upside-down display mod
	SPDR = Data;//  Send data to display controller.

	while ( !(SPSR & 0x80) );//  Wait until Tx register empty.

	SPCR&=~(1<<DORD);//switch data back to normal order
	SET_SCE_PIN;   	 //disable LCD
}

void LCD_schedule_update(uint8_t rows)
{
	LCD.update_rows|=rows;
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_clear
  Description  :  Clears the display
  Argument(s)  :  None.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_clear ( void )
{
	int i,j;

	LCD_gotoXY (0,0);  	//start with (0,0) position

	for(i=0; i<HW_ROWS; i++)
	{
		for(j=0; j<HW_COLUMNS; j++)
		{
			if ((i < HW_ROWS) && (j < HW_COLUMNS))
				lcd_buffer[i][j] = 0x00;
		}
	}

	LCD_gotoXY (0,0);	//bring the XY position back to (0,0)
	LCD_schedule_update(ALL_ROWS_MODIFIED);
}

// void LCD_rowUpdate( void )
// {
// 	int j;
//
// 	LCD_gotoXY_ex (RES_MAX_X,cursor_row,0);  	//start with (0,0) position
//
//
// 	for(j=HW_COLUMNS; j>0; j--)
// 	{
// 		LCD_writeData(lcd_buffer[cursor_row][j-1]);
// 	}
//
// }

#ifdef LCD_UPDATE_DEBUG
	uint8_t lcd_update_toggler=0;
#endif

void LCD_update( void )
{
	int i,j;

	#ifdef LCD_NO_SKIP_UPDATE
	if(1)
	#else
	if(LCD.update_rows) //if any rows to update
	#endif
	{
		for(i=0; i<HW_ROWS; i++)
		{
			#ifdef LCD_FORCE_TOTAL_UPDATE
			if(1)
			#else
			if(LCD.update_rows & (1<<i))
			#endif
			{
				#ifdef LCD_UPDATE_DEBUG
					bit_flip(lcd_update_toggler,1<<i);
				#endif
				LCD_writeCommand (0x80);   //column
				LCD_writeCommand (0x40|(HW_ROWS-1-i));   //row

				for(j=HW_COLUMNS; j>0; j--)
				{
					#ifdef LCD_UPDATE_DEBUG
					if(lcd_update_toggler&(1<<i))
						LCD_writeData(~lcd_buffer[i][j-1]);
					else
					#endif
						LCD_writeData(lcd_buffer[i][j-1]);
				}
			}
		}
		LCD.update_rows=0;
		LCD_gotoXY (0,0);	//bring the XY position back to (0,0)
	}

}


/*void LCD_setPixel( unsigned char x, unsigned char y )
	{
		unsigned char value;
		unsigned char row;
		row = y / 8;
		value = lcd_buffer[row][x];
		value |= (1 << (y % 8));
		lcd_buffer[row][x] = value;
		//LCD_update();
		LCD_gotoXY (x,row);
		LCD_writeData(value);
	}
*/

/*void LCD_clearPixel( unsigned char x, unsigned char y )
	{
		unsigned char value;
		unsigned char row;
		row = y / 8;
		value = lcd_buffer[row][x];
		value &= ~(1 << (y % 8));
		lcd_buffer[row][x] = value;
		//LCD_update();
		LCD_gotoXY (x,row);
		LCD_writeData(value);
	}
*/

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_gotoXY
  Description  :  Sets cursor location to xy location corresponding to basic font size.
  Argument(s)  :  x - range: 0 to 84
  			   	  y -> range: 0 to 6
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_gotoXY ( unsigned char x, unsigned char y)
{
	cursor_row = y;
	cursor_col = x;
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_writeChar
  Description  :  Displays a character at current cursor location and increment cursor location.
  Argument(s)  :  ch   -> Character to write.
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_writeChar ( unsigned char character, uint8_t flags )
{
	unsigned char j, col=0;

	if(flags & INVERT)
	{
		lcd_buffer[cursor_row][cursor_col]=0xFF;
		//lcd_buffer[cursor_row][cursor_col+6]=0xFF;
	}
	else if(flags & OVERWRITE)
	{
		lcd_buffer[cursor_row][cursor_col]=0;
		//lcd_buffer[cursor_row][cursor_col+6]=0;
	}
	//else do not touch column before and after character

	cursor_col++;


	for(j=0; j<5; j++)
	{
		if (!(flags & OVERWRITE))
		{
			col=lcd_buffer[cursor_row][cursor_col];
		}
		else
		{
			col=0;
		}
		col |= pgm_read_byte(&(smallFont [(character-32)*5 + j] ));

		if (flags & INVERT)
			col = ~col;

		lcd_buffer[cursor_row][cursor_col++] = col;
	}
	LCD_schedule_update(1<<cursor_row);
	//cursor_col++;
}

#ifdef MEGA_FONT

	/*--------------------------------------------------------------------------------------------------
	  Name         :  LCD_writeChar_megaFont
	  Description  :  Displays a character in large fonts, used here for displatying temperature
			  (for displaying '.', '+', '-', and '0' to '9', stored
			  in 3310_routines.h as three dimensional array, number[][][])
	  Argument(s)  :  ch   -> Character to write.
	  Return value :  None.
	--------------------------------------------------------------------------------------------------*/
	void LCD_writeChar_megaFont (unsigned char ch)
	{
		unsigned char i, j;

		if(ch == '.')
			ch = 10;
		else if (ch == '+')
			ch = 11;
		else if (ch == '-')
			ch = 12;
		else
			ch = ch & 0x0f;

		for(i=0;i<3;i++)
		{
			LCD_gotoXY (4 + char_start, i+2);

			for(j=0; j<16; j++) {
				lcd_buffer[cursor_row][cursor_col + j] |=  pgm_read_byte(&(number[ch][i][j]));
				LCD_writeData(lcd_buffer[cursor_row][cursor_col + j]);
			}
		}

		if(ch == '.')
			char_start += 5;
		else
			char_start += 12;
	}

	/*--------------------------------------------------------------------------------------------------
	  Name         :  LCD_writeString_megaFont
	  Description  :  Displays a string at current location, in large fonts
	  Argument(s)  :  string -> Pointer to ASCII string (stored in RAM)
	  Return value :  None.
	--------------------------------------------------------------------------------------------------*/
	void LCD_writeString_megaFont ( char *string )
	{
		char_start = 0;

		while ( *string )
			LCD_writeChar_megaFont( *string++ );

		LCD_gotoXY(char_start+6, 3);
		//LCD_writeChar('z'+1); 			  //symbol of Degree
		//LCD_writeChar('F');
	}
#endif

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_writeString_F
  Description  :  Displays a string stored in FLASH, in small fonts (refer to 3310_routines.h)
  Argument(s)  :  string -> Pointer to ASCII string (stored in FLASH)
  Return value :  None.
--------------------------------------------------------------------------------------------------*/
void LCD_writeString_XY ( uint8_t x, uint8_t y, const char *string, uint8_t flags )
{
	if (flags & RELATIVE)
		LCD_gotoXY(cursor_col+x,cursor_row+y);
	else
		LCD_gotoXY(x,y);

	while ( *string )
		LCD_writeChar( *string++,flags );
}

/*--------------------------------------------------------------------------------------------------
  Name         :  LCD_drawBorder
  Description  :  Draws rectangle border on the display
  Argument(s)  :  None
  Return value :  None
--------------------------------------------------------------------------------------------------*/
void LCD_drawBorder (void )
{
	unsigned char i, j;

	for(i=0; i<HW_ROWS; i++)
	{
		LCD_gotoXY (0,i);

		for(j=0; j<HW_COLUMNS; j++)
		{
			if(j == 0 || j == HW_COLUMNS-1)
			{
				lcd_buffer[cursor_row][cursor_col + j] |= 0xff;
			}
			else if(i == 0)
			{
				lcd_buffer[cursor_row][cursor_col + j] |= 0x01;
			}
			else if(i == HW_ROWS-1)
			{
				lcd_buffer[cursor_row][cursor_col + j] |= 0x80;
			}
			// else
			// {
			// 	lcd_buffer[cursor_row][cursor_col + j] |= 0x00;
			// }
		}
	}
	LCD_schedule_update(ALL_ROWS_MODIFIED);
}

void LCD_drawHline( uint8_t X, uint8_t Y, int8_t width)
{
	if(width==0)
		return;

	if(X>=HW_COLUMNS || Y>=HW_ROWS*8 )
	{
		LCD_error.draw_outside=1;
		return;
	}
	if(X+width>HW_COLUMNS)
	{
		LCD_error.draw_outside=1;
		width=HW_COLUMNS-X;//limit width to LCD
	}

	unsigned char row;

	row = Y / 8;

	uint8_t mask=(1 << (Y % 8));

	for (uint8_t x = X; x < X+width; x++)
	{
		lcd_buffer[row][x] |= mask;
	}
	LCD_schedule_update(1<<row);
}



void LCD_drawRectangle(uint8_t x_start, uint8_t y_start, int8_t width, int8_t height, uint8_t flags)
{
	if(width==0 || height==0)//nothing to draw
		return;
	//TL to BR
	uint8_t mask;

	if(height<0)
	{
		y_start+=height;
		height=-height;
	}
	if(width<0)
	{
		x_start+=width;
		width=-width;
	}

	if(x_start>=HW_COLUMNS || y_start>=HW_ROWS*8)
	{
		LCD_error.draw_outside=1;
		return;
	}

	uint8_t x,y;
	uint8_t x_end=x_start+width-1;
	uint8_t y_end=y_start+height-1;

	if(x_end>=HW_COLUMNS)
	{
		LCD_error.draw_outside=1;
		x_end=RES_MAX_X;//limit max X to LCD
	}
	if(y_end>=HW_ROWS*8)
	{
		LCD_error.draw_outside=1;
		y_end=RES_MAX_Y;
	}

	uint8_t start_row=y_start/8;
	uint8_t end_row=(y_end)/8;

	for(y=start_row; y<=end_row; y++)
	{
		LCD_schedule_update(1<<y);
		//prepare pattern for filled areas and vertical boreder lines
		mask=0xff;

		//generate mash, which bits will be FILLed or OVERWRITtEn
		if(y==start_row)
			mask&=FILL_BITS_FROM_MSB(8-(y_start%8));//MSB is at BOOTOM end of byteline!!!!

		if(y==end_row)
			mask&=FILL_BITS_FROM_LSB(y_end%8+1);


		if(flags & FILL) //only if FILL wanted
		{
			for (x = x_start+1; x < x_end; x++)
			{
				//write whole area occupied by this row of rectangle
				lcd_buffer[y][x]|=mask;
			}
		}
		else //otherwise normal draw (not clearing already set pixels)
		{
			if(flags & (CLEAR|OVERWRITE)) //only if CLEAR wanted
			{
				for (x = x_start+1; x < x_end; x++)
				{
					//write whole area occupied by this row of rectangle
					lcd_buffer[y][x]&=~mask;
				}
			}

			if(!(flags & CLEAR))
			{
				if(y==start_row)
				{
					for (uint8_t i = x_start; i < x_end; i++)
					{
						lcd_buffer[y][i]|= (1<<(y_start%8)); //border line
					}
				}

				if(y==end_row)
				{
					for (uint8_t i = x_start+1; i < x_end; i++)
					{
						lcd_buffer[y][i]|= (1<<(y_end)%8); //border line
					}
				}
			}
		}

		lcd_buffer[y][x_start]|=mask;//left border line - draw always
		lcd_buffer[y][x_end]|=mask;//right border line - draw always
	}
}

void LCD_clear_partial()
{
	for(uint8_t row=2;row<HW_ROWS;row++)
	{
		LCD_schedule_update(1<<row);
		for(uint8_t col=0;col<HW_COLUMNS;col++)
		{
			lcd_buffer[row][col]=0;
		}
	}
}

void LCD_clear_header_text()
{
	for (uint8_t x = 0; x < MENU_HEADER_LENGTH*6; x++)
	{
		lcd_buffer[0][x]=0;
	}
	LCD_schedule_update(1<<0);
}

void LCD_write_header_text(const char *string)
{
	LCD_clear_header_text();

	LCD_gotoXY(0,0);
	for( uint8_t limiter=0; *string && limiter<MENU_HEADER_LENGTH; limiter++ )
		LCD_writeChar( *string++,0);
	LCD_schedule_update(1<<0);
}

void LCD_bitmap_pgm(const uint8_t *pgm_array)
{
	for(uint16_t i=0;i<(HW_ROWS*HW_COLUMNS);i++)
		*(&lcd_buffer[0][0]+i)=pgm_read_byte(pgm_array+i);
	LCD_schedule_update(ALL_ROWS_MODIFIED);
}

void LCD_clear_rows (uint8_t rows)
{
	for(uint8_t row=0;row<HW_ROWS;row++)
	{
		if(rows & (1<<row))
		{
			for(uint8_t col=0; col<HW_COLUMNS; col++)
			{
				lcd_buffer[row][col] = 0x00;
			}
			LCD_schedule_update(1<<row);
		}
	}
}

#define BAR_VERTICAL	1
#define BAR_HORIZONTAL	2

void LCD_drawBar(uint8_t x_start, uint8_t y_start, int8_t width, int8_t height, int8_t perc, uint8_t bar_type)
{
	//perc=-perc;
	LCD_drawRectangle(x_start,y_start,width,height,OVERWRITE);//outer rectangle
	//for filler, -1px from app sides
	x_start++;
	y_start++;
	width-=2;
	height-=2;
	if(bar_type==BAR_VERTICAL)
	{
		if(perc<0)
		{
			perc=-perc;
			LCD_drawRectangle(x_start,y_start+height,width, -(int8_t)((uint16_t)(height)*perc/100) ,FILL);
		}
		else if(perc>0)
		{
			LCD_drawRectangle(x_start,y_start,width,(uint16_t)(height)*perc/100,FILL);
		}
	}
	else if(bar_type==BAR_HORIZONTAL)
	{
		if(perc<0)
		{
			perc=-perc;
			LCD_drawRectangle(x_start+width,y_start, -(int8_t)((uint16_t)(width)*perc/100) ,height,FILL);
		}
		else if(perc>0)
		{
			LCD_drawRectangle(x_start,y_start, (uint16_t)(width)*perc/100 ,height,FILL);
		}
	}
}

/*--------------------------------------------------------------------------------------------------
                                         End of file.
--------------------------------------------------------------------------------------------------*/
