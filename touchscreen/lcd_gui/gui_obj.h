/*
   LCD Touchscreen GUI class definitions. This is designed and has been
   tested on a 3.5inch ILI9488 LCD touchscreen, driven by Arduino Uno. It
   also works on Arduino Nano. LCDWIKI libraries, version for Arduino Uno
   Hardware SPI, are used to interface with the screen.

   There are three classes:

   gui_obj - base class. Can be used to display a label on screen. Objects
     don't respond to touchscreen clicks.
   
   clickable - child of gui_obj. Objects respond to touchscreen clicks. Each
     object may have an _action function that is run when the press is
     released.

     clickable objects come in two flavors. Both flavors change color
     when pressed and revert back when released. Base flavor doesn't offer
     any functionality beyond calling _action, if any has been provided. The
     second flavor is the on-off flavor. With each press, it toggless
     between on and off states. The state can be queried and changed via
     public methods. Flavors are selected at instantiation. Initial state
     for the on-off flavor object is also set at instantiation.

     On-off flavor objects can have two variable label parts, one displayed
     when the object is on, the other when the object is off.

     The first label part can be either in regular memory or in PROGMEM. If
     it's in regular memory, it can be updated by the sketch. VARTXT_MEM bit
     in _flags marks that the vartxt is in regular memory.

   editvar - child od clickable. Objects respond to touchscreen clicks.
     The label displays a value of an external variable,
     either long or float. Display format is fixed precision, set at
     instantiation. If the number of decimals is zero, the variable is long,
     otherwise it's float.

     Clicking the object switches the LCD to an edit screen, where the
     variable can be edited. OK and CANCEL buttons must be provided. The
     current variable value is stored in the EEPROM, so it persist across
     program cycles.

     editval vartxt is in memory so it can be used to display the
     corresponding value without having to overload clickable::render. This
     saves some PROGMEM.

   To save on memory, almost all the object parameters are kept constant in
   a separate struct - each class has its own. This struct is stored in
   PROGMEM, as are most of the labels. Any changes require recompilation.

   By Nenad Rijavec

   There are no restrictions on this code, you are welcome to use, modify
   and share it as you see fit. Note that LCDWIKI libraries have their own
   usage policies.

   However, there are no guarantees about fitness of this code for any
   specific purpose.

*/

#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_SPI.h> //Hardware-specific library
#include <LCDWIKI_TOUCH.h> //touch screen library

// define ILI9488 parameters
#define MODEL ILI9488_18

#define LCD_W 320
#define LCD_H 480

// SPI params
#define CS   A5
#define CD   A3
#define RST  A4
#define LED  A0

//touch screen SPI parameters
#define TCS   2
#define TCLK  3
#define TDOUT 4
#define TDIN  5
#define TIRQ  6

// color RGB values
// only colors defined here are available
                             /*  r     g    b */
#define BLACK        0x0000  /*   0,   0,   0 */
#define BLUE         0x001F  /*   0,   0, 255 */
#define RED          0xF800  /* 255,   0,   0 */
#define GREEN        0x07E0  /*   0, 255,   0 */
#define CYAN         0x07FF  /*   0, 255, 255 */
#define MAGENTA      0xF81F  /* 255,   0, 255 */
#define YELLOW       0xFFE0  /* 255, 255,   0 */
#define WHITE        0xFFFF  /* 255, 255, 255 */
#define NAVY         0x000F  /*   0,   0, 128 */
#define DARKGREEN    0x03E0  /*   0, 128,   0 */
#define DARKCYAN     0x03EF  /*   0, 128, 128 */
#define MAROON       0x7800  /* 128,   0,   0 */
#define PURPLE       0x780F  /* 128,   0, 128 */
#define OLIVE        0x7BE0  /* 128, 128,   0 */
#define LIGHTGREY    0xC618  /* 192, 192, 192 */
#define DARKGREY     0x7BEF  /* 128, 128, 128 */
#define ORANGE       0xFD20  /* 255, 165,   0 */
#define GREENYELLOW  0xAFE5  /* 173, 255,  47 */
#define PINK         0xF81F  /* 255,   0, 255 */

// declare the LCD and touchcreen. They are actually instantiated in the
// main sketch.

extern LCDWIKI_SPI lcd ;
extern LCDWIKI_TOUCH tscreen ;

// Factors to multiply the font size to get character bounding box.
// If background is opaque, each char gets space to the right and below,
// mostly equal to font size (for 3,4,5. Vertical pad is the same for 6,
// horizontal pad is 5, not 6 as would be expected.
// CHAR_W_FAC doesn't include the inter-character space.

#define CHAR_W_FAC 5
#define CHAR_H_FAC 7
// macro for pixel width of a string, assuming font size (f) is <= 5
#define STR_PIXW( s,f) (((s)-1)*(f)+ (s)*(f)*CHAR_W_FAC)

// orientation constants for LCD and touchscreen. "Portrait 0" is with
// the main connectors up. These apply if TP_Init was passed "0" as the LCD
// orientation.

#define LCD_ORIENT_0   0
#define LCD_ORIENT_90  1
#define LCD_ORIENT_180 2
#define LCD_ORIENT_270 3

#define TSC_ORIENT_0   1
#define TSC_ORIENT_90  0
#define TSC_ORIENT_180 3
#define TSC_ORIENT_270 2

// progmem access macroes

#define READ_FG pgm_read_word_near( &(_pbuff->fg_color ) )
#define READ_BG pgm_read_word_near( &(_pbuff->bg_color ) )
#define READ_FG_CLICKED pgm_read_word_near( &(((const struct clickable_prog *)_pbuff)->fg_color_clicked ) )
#define READ_BG_ON pgm_read_word_near( &(((const struct clickable_prog *)_pbuff)->bg_color_on ) )
#define READ_TRANSPARENT ((bool)(pgm_read_byte_near(&(_pbuff->am_transparent))))
#define READ_FSIZE ((uint8_t)(pgm_read_byte_near( &(_pbuff->font_size))))
#define READ_VOFF pgm_read_word_near( &(((const struct clickable_prog *)_pbuff)->vartxt_offset ) )
#define READ_VOFF2 pgm_read_word_near( &(((const struct vardisp_prog *)_pbuff)->vartxt_offset ) )

// clickable _flags bit values
#define AM_ONOFF   0x80  // object is on-off flavor
#define AM_ON      0x40  // object is on
#define CHG_ON     0x20  // object is on-off and change fg and bg when on
#define VARTXT_MEM 0x02  // vartxt string is stored in memory, not in PROGMEM
#define AM_CLICKED 0x01  // active click in progress

// constant data, other than labels, are kept in these structs
// each class has the corresponding struct. structs of children are
// supersets of structs of parents.

struct gui_obj_prog {
		uint16_t _x, _y, _w, _h ;
		uint16_t fg_color, bg_color ;
		bool am_transparent ;
		uint8_t font_size ;
		__FlashStringHelper *label  ;
} ;

struct clickable_prog {
		uint16_t _x, _y, _w, _h ;
		uint16_t fg_color, bg_color ;
		bool am_transparent ;
		uint8_t font_size ;
		__FlashStringHelper *label  ;
		void *vartxt  ;  // may be either in progmem or memory
		__FlashStringHelper *vartxt2  ;
		uint8_t vartxt_offset ;
		uint16_t fg_color_clicked ; // if CHG_ON set, used when on
		int (*_action)( void *obj, void *aa) ;
		void *_act_arg ;
		uint16_t bg_color_on ;
} ;

struct editval_prog {
		uint16_t _x, _y, _w, _h ;
		uint16_t fg_color, bg_color ;
		bool am_transparent ;
		uint8_t font_size ;
		__FlashStringHelper *label  ;
		void *vartxt  ;
		__FlashStringHelper *vartxt2  ;
		uint8_t vartxt_offset ;
		uint16_t fg_color_clicked ;
		int (*_action)( void *obj, void *aa) ;
		void *ok ;
		void *cancel ;
		uint8_t n_int, n_dec ;
		bool am_signed ;
		void *value ;
		uint16_t eeprom_address ;
} ;

// class declarations
class gui_obj {
	public:
		gui_obj( const struct gui_obj_prog *pbuff  )
		{
			_pbuff = pbuff ;
		}
		void get_dims( uint16_t *_x, uint16_t *_y,
				uint16_t *_w, uint16_t *_h ) ;
		virtual void render( bool flag, bool unused ) ;
		virtual boolean click( uint16_t cx, uint16_t cy )
		{
			return false ;
		}
		bool release()
		{
			return false ;
		}
	protected:
		const struct gui_obj_prog *_pbuff ;
		
} ;


class clickable : public gui_obj {
	public :
		/*
		   bitwise flag:
		     bit 0 - if 1, object is on-off flavor
		     bit 1 - if bit 0 is 1, bit 1 indicates whether the
		         object is on or off.
		     bit 2 - if 1 and the object is on-off, change color when on
		     bit 6 - if 1, vartxt is in memory, otherwise in PROGMEM
		     bit 7 - if 1, the object is currently being clicked.
		     other bits unused
		*/
		uint8_t _flags ;
		clickable( const struct clickable_prog *pbuff, uint8_t initf ) :
			gui_obj( (const struct gui_obj_prog *)pbuff   )
		{
			_flags = initf ;
		}
		virtual void render( bool flag, bool vartxt_only ) ;
		virtual void press_action( uint16_t cx, uint16_t cy ) ;
		virtual void release_action()  ;
		virtual boolean click( uint16_t cx, uint16_t cy ) ;
		virtual bool release() ;
		virtual void set_flags( uint8_t in_flag )
		{
			_flags = in_flag ;
		}
		virtual void set_onoff( bool flag )
		{
			if ( !(_flags & AM_ONOFF) )
				return ; // not an on-off object
			if ( flag )
				_flags |= AM_ON ;
			else
				_flags &= (0xff ^ AM_ON ) ;
		}
		boolean is_on()
		{
			if ( !(_flags & AM_ONOFF) || !(_flags & AM_ON) )
				return false ;
			else
				return true ;
		}
		virtual void *get_act_arg()
		{
			return (void *)pgm_read_word_near(
                	&(((const struct clickable_prog *)_pbuff)->_act_arg)) ;
		}
	protected:
} ;

class editval : public clickable {
	public :
		editval( const struct editval_prog *pbuff ) :
		// vartxt is always in memory
		clickable( (const struct clickable_prog *)pbuff, VARTXT_MEM ) 
		{
			set_val_display() ;
		}
		virtual void release_action() ;
		virtual void edit( ) ;
		virtual void set_val_display() ;
		virtual void *get_act_arg()
		{
			return (void *)0 ;
		}
	protected:
} ;
