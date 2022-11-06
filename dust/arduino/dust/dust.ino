/*
   Dust collection gate controller. Each gate is controlled by a servo and has
   a corresponding button on the screen. Buttons change color from yellow to
   red when the gate is open. This sketch controls six gates.

   State of each gate is stored in EEPROM and so persists across power cycles.
   If the EEPROM signature is changed, the EEPROM bytes 0 to
   EEPROM_SIGNATURE_INDEX are initialized to zero, meaning that the gates are
   indicated closed. The setup then moves the gates to the indicated state,
   meaning that after EEPROM signature updates, all the gates will close.

   Gate servo motors are unpowered unless the gate is opening or closing.

   Arduino Nano can run up to 12 servos without additional hardware, but the
   display uses a number of pins, so there are only 7 pins remaining, assuming
   we reserve two pins for serial communications.

   By Nenad Rijavec
   There are no restrictions on this code, you are welcome to use, modify
   and share it as you see fit. Note that LCDWIKI libraries have their own
   usage policies.

   However, there are no guarantees about fitness of this code for any
   specific purpose.

*/

#include <Servo.h>
#include "gui_obj.h"

#define EEPROM_SIGNATURE 3 // to indicate if the EEPROM was loaded
#define EEPROM_SIGNATURE_INDEX 255

// set the LCD dimensions for centering
// for the constants that go into PROGMEM
#define LCD_ORIENT  LCD_ORIENT_0
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

const int GATE_OPEN = 165 ;
const int GATE_CLOSED = 10 ;
const int n_buttons = 6 ;
Servo wrk_servo ;
int servo_pins[n_buttons] = { 16, 15, 7, 8, 9, 10 } ;

const int SWEEP_DELAY = 20 ;

int get_servo_index( clickable *button )
{
	return (int)button->get_act_arg() ;
}

int gate_action( void *obj, void *ctrp ) {
	clickable *button = (clickable *)obj ;
	long index = (long) ctrp ;
	int angle ;
	Serial.println( "starting motor on pin " + String( servo_pins[index]) ) ;
	wrk_servo.attach( servo_pins[index] ) ;
	if ( button->_flags & AM_ON ) {
		EEPROM.write( get_servo_index(button), 1 ) ;
		Serial.println( "opening servo " + String( index ) ) ;
		angle = GATE_CLOSED+1 ;
		while ( angle <= GATE_OPEN ) {
			Serial.println( String( angle ) ) ;
			wrk_servo.write( angle++ ) ;
			delay( SWEEP_DELAY ) ;
		}
	} else {
		EEPROM.write( get_servo_index(button), 0 ) ;
		Serial.println( "closing servo " + String( index ) ) ;
		angle = GATE_OPEN-1 ;
		while ( angle >= GATE_CLOSED ) {
			Serial.println( String( angle ) ) ;
			wrk_servo.write( angle-- ) ;
			delay( SWEEP_DELAY ) ;
		}
	}
	wrk_servo.detach() ;
	Serial.println( "motor done" ) ;
}

// buttons are ordered from top to bottom
// in buttons below, STR_PIXW string length is one more than actual label length

const char PROGMEM label_sander_button[] = "Sander" ;
const struct clickable_prog PROGMEM buff_sander_button =
{
	(C_LCD_W-STR_PIXW(7,4))/2, 30, STR_PIXW(7,4), 70, // uint16_t
	BLUE, YELLOW, // uint16_t
	false,   // bool
	4, // uint8_t
	// if any of the strings are missing, set to NULL
	(__FlashStringHelper *)label_sander_button,
	NULL, NULL,
	6, // uint8_t
	WHITE,  // uint16_t
	gate_action, 
	(long *)0, // no need for ptr, index into EEPROM and servos
	RED  // uint16_t bg on color
} ;

clickable sander_button( &buff_sander_button, AM_ONOFF | CHG_ON ) ;

const char PROGMEM label_saw_button[] = "Table Saw" ;
const struct clickable_prog PROGMEM buff_saw_button =
{
	(C_LCD_W-STR_PIXW(10,4))/2, 130, STR_PIXW(10,4), 70, // uint16_t
	BLUE, YELLOW, // uint16_t
	false,   // bool
	4, // uint8_t
	// if any of the strings are missing, set to NULL
	(__FlashStringHelper *)label_saw_button,
	NULL, NULL,
	6, // uint8_t
	WHITE,  // uint16_t
	gate_action, 
	(long *)1, // no need for ptr, index into EEPROM and servos
	RED  // uint16_t bg on color
} ;

clickable saw_button( &buff_saw_button, AM_ONOFF | CHG_ON ) ;

const char PROGMEM label_miter_button[] = "Miter Saw" ;
const struct clickable_prog PROGMEM buff_miter_button =
{
	(C_LCD_W-STR_PIXW(10,4))/2, 230, STR_PIXW(10,4), 70, // uint16_t
	BLUE, YELLOW, // uint16_t
	false,   // bool
	4, // uint8_t
	// if any of the strings are missing, set to NULL
	(__FlashStringHelper *)label_miter_button,
	NULL, NULL,
	6, // uint8_t
	WHITE,  // uint16_t
	gate_action, 
	(long *)2, // no need for ptr, index into EEPROM and servos
	RED  // uint16_t bg on color
} ;

clickable miter_button( &buff_miter_button, AM_ONOFF | CHG_ON ) ;

const char PROGMEM label_vac1_button[] = "Vac1" ;
const struct clickable_prog PROGMEM buff_vac1_button =
{
	10, 320, STR_PIXW(5,4), 70, // uint16_t
	BLUE, YELLOW, // uint16_t
	false,   // bool
	4, // uint8_t
	// if any of the strings are missing, set to NULL
	(__FlashStringHelper *)label_vac1_button,
	NULL, NULL,
	6, // uint8_t
	WHITE,  // uint16_t
	gate_action, 
	(long *)3, // no need for ptr, index into EEPROM and servos
	RED  // uint16_t bg on color
} ;

clickable vac1_button( &buff_vac1_button, AM_ONOFF | CHG_ON ) ;

const char PROGMEM label_vac2_button[] = "Vac2" ;
const struct clickable_prog PROGMEM buff_vac2_button =
{
	(C_LCD_W-STR_PIXW(5,4))/2, 410, STR_PIXW(5,4), 70, // uint16_t
	BLUE, YELLOW, // uint16_t
	false,   // bool
	4, // uint8_t
	// if any of the strings are missing, set to NULL
	(__FlashStringHelper *)label_vac2_button,
	NULL, NULL,
	6, // uint8_t
	WHITE,  // uint16_t
	gate_action, 
	(long *)4, // no need for ptr, index into EEPROM and servos
	RED  // uint16_t bg on color
} ;

clickable vac2_button( &buff_vac2_button, AM_ONOFF | CHG_ON ) ;

const char PROGMEM label_vac3_button[] = "Vac3" ;
const struct clickable_prog PROGMEM buff_vac3_button =
{
	C_LCD_W-STR_PIXW(5,4)-10, 320, STR_PIXW(5,4), 70, // uint16_t
	BLUE, YELLOW, // uint16_t
	false,   // bool
	4, // uint8_t
	// if any of the strings are missing, set to NULL
	(__FlashStringHelper *)label_vac3_button,
	NULL, NULL,
	6, // uint8_t
	WHITE,  // uint16_t
	gate_action, 
	(long *)5, // no need for ptr, index into EEPROM and servos
	RED  // uint16_t bg on color
} ;

clickable vac3_button( &buff_vac3_button, AM_ONOFF | CHG_ON ) ;

clickable *buttons[n_buttons] = { &sander_button, &saw_button, &miter_button,
	&vac1_button, &vac2_button, &vac3_button
} ;

void render_all()
{
	int i ;
	lcd.Fill_Screen(WHITE) ;
	for ( i = 0; i < n_buttons ; i++ )
		buttons[i]->render( false, false ) ;
}

void eeprom_setup()
{
	int i ;
	// set all servos to closed. Buttons and servos initialize from
	// EEPROM later.
	// EEPROM value is 1 if the gate is OPEN
	for ( i = 0 ; i <= EEPROM_SIGNATURE_INDEX ; i++ ) 
		EEPROM.write( i, 0 ) ;
	EEPROM.write( EEPROM_SIGNATURE_INDEX, EEPROM_SIGNATURE ) ;

}

void setup()
{
	byte w ;
	int i ;

	Serial.begin( 115200 ) ;

	EEPROM.get( EEPROM_SIGNATURE_INDEX, w ) ;
	if ( w != EEPROM_SIGNATURE ) {
		Serial.println( "initalizing EEPROM" ) ;
		eeprom_setup() ;
	} else {
		Serial.println( "EEPROM already initialized" ) ;
	}

	// initalize button states from EEPROM
	for ( i = 0 ; i < n_buttons ; i++ ) {
		EEPROM.get( get_servo_index(buttons[i]), w ) ;
		if ( w != 0 )
			buttons[i]->_flags |= AM_ON ;
	}
	// make sure the gates are in the right state
	for ( i = 0 ; i < n_buttons ; i++ ) {
		wrk_servo.attach( servo_pins[get_servo_index(buttons[i])] ) ;
		if ( buttons[i]->_flags & AM_ON )
			wrk_servo.write( GATE_OPEN ) ;
		else
			wrk_servo.write( GATE_CLOSED ) ;
		delay( 300 ) ; // wait for motor to move if needed
		wrk_servo.detach() ;
	}

	lcd.Init_LCD() ;
	lcd.Set_Rotation( LCD_ORIENT ) ;
	tscreen.TP_Init( LCD_ORIENT,
		lcd.Get_Display_Width(), lcd.Get_Display_Height() ) ;
	tscreen.TP_Set_Rotation( TSC_ORIENT_0 ) ;

	render_all() ;
	am_pressed = false ;

}


void loop()
{
	int i ;
	tscreen.TP_Scan( 0 ) ;
	if ( tscreen.TP_Get_State() & TP_PRES_DOWN ) {
		if ( !am_pressed ) {
			am_pressed = true ;
			for ( i = 0 ; i < n_buttons ; i++ )
				buttons[i]->click( tscreen.x, tscreen.y ) ;
		Serial.println( "clicked" ) ;
		}
	} else {
		if ( am_pressed ) {
			am_pressed = false ;
			for ( i = 0 ; i < n_buttons ; i++ )
				buttons[i]->release() ;
		Serial.println( "released" ) ;
		}
	}
}
