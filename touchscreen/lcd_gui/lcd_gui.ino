/*
   LCD touchscreen GUI demo sketch.

   Sketch counts from zero, once per second, until the absolute value of
   the count is bigger than the absolute value of the target. Target can be
   integer or float, positive or negative. If the target value is negative,
   the counter counts down from zero.Target value can be edited via
   the GUI, but the int/float selection is fixed at compile time.

   Progam instantiates six objects. Four are shown on the main screen:
	label - screen title, does nothing
	sstop_button - starts and stops the count when pressed.
	ctr_disp - displays the current count, doesn't respond to a press.
	target - displays the current target. Pressing it activates the
	    target edit screen.
    Two objects are shown on the edit screen:
    	ok_button - accept changes, if any
	cancel_button - discard editing changes

    ok, cancel and sstop buttons, as well as ctr_disp are of the same
    "clickable" class. sstop button tracks an extra state - on or off,
    and behaves diferrently. 
    
    It would be cleaner to have sstop as a separate class, but doing it this
    way saves storage.

    Editing screen is managed via target object edit method. Target value is
    stored in EEPROM, so the current target value is reused the next time
    the sketch starts. To reload the starting target value, change the
    EEPROM_SIGNATURE. Note that changing the target value definition between
    integer and float requires reload of the target value, since float and
    int have different formats.

    The display can run in any of the four orientations, as set by the
    LCD_ORIENT constant below.

    To save memory, all of the constants for each object are encoded in a
    separate struct that is stored in PROGMEM, so this will compile only on
    platforms that have avr/pgmspace.h include. We have three object classes:
    gui_obj, clickable and editval. A template section below shows
    how to fill these structs and instantiate each object.

    Each class has up to three labels, one base fixed label and two labels
    used variably. Variable labels are intended to overwrite part of the
    fixed label. Fixed label and the second variable label are constant and
    kept in PROGMEM. First fixed label can be either in PROGMEM or in
    regular memory, in which case it can be changed by the sketch at
    runtime.

    This sketch has been tested on a 3.5 inch ILI9488 LCD touchscreen and
    uses LCDWIKI libraries, Demo_UNO_Hardware_SPI version. It runs on
    both Uno and Nano.

    By Nenad Rijavec
    There are no restrictions on this code, you are welcome to use, modify
    and share it as you see fit. Note that LCDWIKI libraries have their own
    usage policies.

    However, there are no guarantees about fitness of this code for any
    specific purpose.

*/

//#define DO_SERIAL    // uncomment to turn on serial connection

#include "gui_obj.h"

#define EEPROM_SIGNATURE 8 // to indicate if the EEPROM was loaded

// set the LCD dimensions for centering
// for the constants that go into PROGMEM
#define LCD_ORIENT  LCD_ORIENT_180
#if LCD_ORIENT == LCD_ORIENT_0 || LCD_ORIENT == LCD_ORIENT_180
#define C_LCD_W LCD_W 
#define C_LCD_H LCD_H 
#else
#define C_LCD_W LCD_H 
#define C_LCD_H LCD_W 
#endif

// instantiate the lcd and touchscreen, having them as globals saves space
// and makes code simpler. Note that "lcd" and "tscreen" are declared as
// extern in gui_obj.h and used in class functions, so changing names here
// requires propagating changes to gui_obj.cpp.
LCDWIKI_SPI lcd(MODEL,CS,CD,RST,LED) ;
LCDWIKI_TOUCH tscreen(TCS,TCLK,TDOUT,TDIN,TIRQ) ;

boolean am_pressed ;
long ctr = 0 ; // need this before objects
unsigned long lastctr ;


#if 0
//===== templates for each class: this section isn't compiled =====

// template for gui_obj objects
// replace <var> with variable name
const char PROGMEM label_<var>[] = "<label>" ;
const struct gui_obj_prog PROGMEM buff_<var> =
 { 
 	// replace vars with values
	_x, _y, _w, _h, // uint16_t
	fg_color, bg_color, // uint16_t
	am_transparent,   // bool
	font_size, // uint8_t
	// set to NULL if no label
	//must be cast to (__FlashStringHelper *)
	(__FlashStringHelper *) label_<var>
} ;

gui_obj <var>( &buff_<var> ) ;

// template for clickable objects
// replace <var> with variable name
const char PROGMEM label_<var>[] = "<label>" ;
// If vartxt is not PROGMEM, VARTXT_MEM must be OR'd into flag
char vartxt_<var>[] = "<vartxt>" ; // variable vartxt
// or
const char PROGMEM vartxt_<var>[] = "<vartxt>" ; // fixed vartxt
const char PROGMEM vartxt2_<var>[] = "<vartxt2>" ;
const struct clickable_prog PROGMEM buff_<var> =
{
	_x, _y, _w, _h, // uint16_t
	fg_color, bg_color, // uint16_t
	am_transparent,   // bool
	font_size, // uint8_t
	// if any of the strings are missing, set to NULL
	// must be cast to (__FlashStringHelper *)
	(__FlashStringHelper *) label_<var>,
	(void *) vartxt_<var>,
	(__FlashStringHelper *) vartxt2_<var>,
	vartxt_offset, // uint8_t
	fg_color_clicked,  // uint16_t
	_action, // int (*_action)( clickable *obj, void *aa) 
	_act_arg // void * ;
} ;

clickable <var>( &buff_<var>, flags ) ;

// template for editval objects
// replace <var> with variable name
const char PROGMEM label_<var>[] = "<label>" ;
char vartxt_<var>[] = "<vartxt>" ; // always variable vartxt
const struct editval_prog PROGMEM buff_<var> =
{
	_x, _y, _w, _h, // uint16_t
	fg_color, bg_color, // uint16_t
	am_transparent,   // bool
	font_size, // uint8_t
	// if any of the strings are missing, set to NULL
	// must be cast to (__FlashStringHelper *)
	(__FlashStringHelper *) label_<var>,
	char vartxt_target[] = "              " ;
	(void *) vartxt_<var>,
	(__FlashStringHelper *) NULL,
	vartxt_offset, // uint8_t
	fg_color_clicked,  // uint16_t
	_action, // int (*_action)( clickable *obj, void *aa) 
	*ok, // void * ;
	*cancel, // void * ;
	n_int, n_dec, // uint8_t
	am_signed,    // bool
	*value,       // long *
	eeprom_address // uint16_t
} ;

editval <var>( &buff_<var> ) ; // editval constructor sets flags internally

//================== end template section ===============
#endif

// OK and CANCEL declared first, so they are available to other objects
const char PROGMEM label_ok_button[] = "OK" ;
const struct clickable_prog PROGMEM buff_ok_button =
{
	10, C_LCD_H-50, (C_LCD_W-40)/2, 40, // uint16_t
	BLUE, YELLOW, // uint16_t
	false,   // bool
	3, // uint8_t
	// if any of the strings are missing, set to NULL
	(__FlashStringHelper *)label_ok_button,
	(void *)NULL,
	(__FlashStringHelper *)NULL,
	0, // no vartxt
	RED,  // uint16_t
	NULL, NULL // no action
} ;

clickable ok_button( &buff_ok_button, 0x0 ) ;

const char PROGMEM label_cancel_button[] = "CANCEL" ;
const struct clickable_prog PROGMEM buff_cancel_button =
{
	25+(C_LCD_W-40)/2, C_LCD_H-50, (C_LCD_W-40)/2, 40, // uint16_t
	BLUE, YELLOW, // uint16_t
	false,   // bool
	3, // uint8_t
	// if any of the strings are missing, set to NULL
	(__FlashStringHelper *)label_cancel_button,
	(void *)NULL,
	(__FlashStringHelper *)NULL,
	0, // no vartxt
	RED,  // uint16_t
	NULL, NULL // no action
} ;

clickable cancel_button( &buff_cancel_button, 0x0 ) ;

const char PROGMEM label_label[] = "GUI Demo" ;
const struct gui_obj_prog PROGMEM buff_label =
 { 
	(C_LCD_W-STR_PIXW(9,4))/2, 10, STR_PIXW(9,4), 70, // uint16_t
	WHITE, RED, // uint8_t
	false,   // bool
	4, // uint8_t
	(__FlashStringHelper *) label_label // set to NULL if no label
} ;
gui_obj label( &buff_label ) ;

// this is called after the state changes
int sstop_action( void *obj, void* ctrp )
{
	clickable *sst = (clickable *)obj ;
	if ( sst->_flags & AM_ON ) {
		ctr = 0 ;
		lastctr = millis() ;
	}
	// no need to do anything on stop
}
const char PROGMEM label_sstop_button[] = "COUNT:     " ;
// If vartxt is not PROGMEM, VARTXT_MEM must be OR'd into flag
//char vartxt_sstop_button[] = "STOP " ;
const char PROGMEM vartxt_sstop_button[] = "STOP " ;
const char PROGMEM vartxt2_sstop_button[] = "START" ;
const struct clickable_prog PROGMEM buff_sstop_button =
{
	(C_LCD_W-STR_PIXW(12,4))/2, 90, STR_PIXW(12,4), 70, // uint16_t
	BLUE, YELLOW, // uint16_t
	false,   // bool
	4, // uint8_t
	// if any of the strings are missing, set to NULL
	(__FlashStringHelper *)label_sstop_button,
	(void *)vartxt_sstop_button,
	(__FlashStringHelper *)vartxt2_sstop_button,
	6, // uint8_t
	RED,  // uint16_t
	(void *)sstop_action, NULL
} ;

clickable sstop_button( &buff_sstop_button, AM_ONOFF ) ;

// ctr display uses VARTXT_MEM
const char PROGMEM label_ctr[] = "CTR:   " ;
char ctrvarbuff[] = "   " ;
const struct clickable_prog PROGMEM buff_ctr =
{
	(C_LCD_W-STR_PIXW(9,4))/2, 170, STR_PIXW(9,4), 70, // uint16_t
	BLUE, YELLOW, // uint16_t
	false,   // bool
	4, // uint8_t
	// if any of the strings are missing, set to NULL
	(__FlashStringHelper *)label_ctr,
	(void *)ctrvarbuff,
	NULL,
	4, // uint8_t
	RED,
	NULL, NULL
} ;
clickable ctr_disp( &buff_ctr, VARTXT_MEM ) ;
// while ctr_disp is clickable, the sketch ignores any clicks on it.

// when switching between float and int, n_int and n_dec must be updated
// labels should be expanded or shrunk reflecting the display format
// PIXW call should also be set so that the length is fixed label + 1
// if the targetval type is changed, modify the EEPROM signature so the
// new value is written into EEPROM
float targetval = -123.45 ;
//long targetval = 123 ;
// this is called after the state changes
int post_edit( void *obj, void* ctrp )
{
	// stop the count if it's active
	sstop_button.set_onoff( false ) ;
}
const char PROGMEM label_target[] = "T:       " ;
char vartxt_target[] = "              " ;
const struct editval_prog PROGMEM buff_target =
{
	(C_LCD_W-STR_PIXW(10,4))/2, 260, STR_PIXW(10,4), 70, // uint16_t
	BLUE, YELLOW, // uint16_t
	false,   // bool
	4, // uint8_t
	// if any of the strings are missing, set to NULL
	// must be cast to (__FlashStringHelper *)
	(__FlashStringHelper *) label_target,
	(void *)vartxt_target,
	(__FlashStringHelper *) NULL,
	2, // uint8_t
	RED,  // uint16_t
	(void *)post_edit, // int (*_action)( clickable *obj, void *aa) 
	&ok_button, // void * ;
	&cancel_button, // void * ;
	3, 2, // uint8_t n_int, n_dec
	true,    // bool am_signed
	(void *)(&targetval),       // void *
	2 // uint16_t - needs to be the same as in setup and eeprom_setup
} ;

editval target( &buff_target ) ;

void render_all()
{
	lcd.Fill_Screen(WHITE) ;
	label.render( false, false ) ;
	sstop_button.render( false, false ) ;
	ctr_disp.render( false, false ) ;
	target.render( false, false ) ;
}

void eeprom_setup()
{
	EEPROM.put( 0, EEPROM_SIGNATURE ) ;
	EEPROM.put( 2, targetval ) ;
}

void setup()
{
	uint16_t w ;

#ifdef DO_SERIAL
	Serial.begin( 115200 ) ;
#endif

	EEPROM.get( 0, w ) ;
	if ( w != EEPROM_SIGNATURE ) {
#ifdef DO_SERIAL
		//Serial.println( "initalizing EEPROM" ) ;
#endif
		eeprom_setup() ;
	} else {
		EEPROM.get( 2, targetval ) ;
#ifdef DO_SERIAL
		//Serial.println( "EEPROM already initialized" ) ;
#endif
	}
	target.set_val_display() ;

	lcd.Init_LCD() ;
	lcd.Set_Rotation( LCD_ORIENT ) ;
	tscreen.TP_Init( LCD_ORIENT,
		lcd.Get_Display_Width(), lcd.Get_Display_Height() ) ;
	tscreen.TP_Set_Rotation( TSC_ORIENT_0 ) ;


	lastctr = millis() ;
	sprintf( ctrvarbuff, "%3d", ctr ) ;
	render_all() ;
	am_pressed = false ;

}


void loop()
{
	long ctime ;
	// only sstop and target react to clicks
	tscreen.TP_Scan( 0 ) ;
	if ( tscreen.TP_Get_State() & TP_PRES_DOWN ) {
		if ( !am_pressed )
			am_pressed = true ;
		sstop_button.click( tscreen.x, tscreen.y ) ;
		target.click( tscreen.x, tscreen.y ) ;
	} else {
		if ( am_pressed ) {
			am_pressed = false ;
			sstop_button.release() ;
			if ( target.release() )
				// taget edited, must redraw
				render_all() ;
		}
	}
	ctime = millis() ;
	if ( sstop_button.is_on() &&  ctime - lastctr > 1000 ) {
			lastctr = ctime ;
			if ( targetval < 0 )
				ctr-- ;
			else
				ctr++ ;
			sprintf( ctrvarbuff, "%3d", ctr ) ;
			ctr_disp.render( false, true ) ;
			if ( (targetval >= 0 && ctr > targetval) ||
				(targetval < 0 && ctr < targetval) ) {
				sstop_button.set_onoff( false ) ;
				sstop_button.render( true, false ) ;
			}
	}
}
