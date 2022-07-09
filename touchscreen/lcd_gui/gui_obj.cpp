/*
   LCD touchscreen GUI class library. See gui_obj.h and sample lcd_gui.ino
   for documentation.

  By Nenad Rijavec
   There are no restrictions on this code, you are welcome to use, modify
   and share it as you see fit. Note that LCDWIKI libraries have their own
   usage policies.

   However, there are no guarantees about fitness of this code for any
   specific purpose.
*/
#include "gui_obj.h"

// strlen equivalent for strings kept in PROGMEM
uint8_t label_len( __FlashStringHelper *label )
{
	uint8_t len = 0 ;
	if ( label == NULL )
		return( 0 ) ; // no label set
	while ( pgm_read_byte_near((char *)label+len) != 0 )
		len++ ;
	return( len ) ;
}

// draw a rectangle of the specified size at specified position in a given
// color. 
void draw_rect( uint16_t color,
	uint16_t x_l, uint16_t y_l, uint16_t x_h, uint16_t y_h )
{
		lcd.Set_Draw_color( color ) ;
		lcd.Fill_Rectangle( x_l, y_l, x_h, y_h ) ;
}

// read object position and dimensions from PROGMEM
void gui_obj::get_dims( uint16_t *_x, uint16_t *_y, uint16_t *_w, uint16_t *_h )
{
	*_x = pgm_read_word_near( &(_pbuff->_x ) ) ;
	*_y = pgm_read_word_near( &(_pbuff->_y ) ) ;
	*_w = pgm_read_word_near( &(_pbuff->_w ) ) ;
	*_h = pgm_read_word_near( &(_pbuff->_h ) ) ;
}

// render the object on LCD
void gui_obj::render( bool flag, bool unused )
{
	uint8_t llen = 0, fsize ;
	uint16_t _x, _y, _w, _h ;
	uint16_t bgcolor ;
	bool am_transparent ;
	__FlashStringHelper *label = (__FlashStringHelper *)
			pgm_read_word_near( &(_pbuff->label) ) ;
	get_dims( &_x, &_y, &_w, &_h ) ;
	am_transparent = READ_TRANSPARENT ;
	bgcolor = READ_BG ;
	if ( !am_transparent )
		draw_rect( bgcolor, _x, _y, _x+_w, _y+_h ) ;
	// there may or may not be a label
	if ( label != NULL ) {
		llen = label_len( label ) ;
		fsize = READ_FSIZE ;
		lcd.Set_Text_Mode( am_transparent ) ;
		lcd.Set_Text_Size( fsize ) ;
		lcd.Set_Text_colour( READ_FG ) ;
		lcd.Set_Text_Back_colour( bgcolor ) ;
		// label is centered in the bounding box
		lcd.Print_String( label,
			_x+((_w-STR_PIXW(llen,fsize))/2),
			_y+(_h-fsize*CHAR_H_FAC )/2 ) ;
	}
}

// executed when the object is first pressed
void clickable::press_action( uint16_t cx, uint16_t cy )
{
	_flags |= AM_CLICKED ;
	render( false, false ) ;
}

// executed when the touchscreen press is released
// release_action is overloaded as necessary.
bool clickable::release()
{
	if ( _flags & AM_CLICKED ) {
		release_action() ;
		return( true ) ;
	} else
		return( false ) ;
}

// two cases. Either the click is on the button, or the click is outside
// button. In that case, if the button was previously being clicked, it must
// be released.
boolean clickable::click( uint16_t cx, uint16_t cy ) {
	uint16_t _x, _y, _w, _h ;
	get_dims( &_x, &_y, &_w, &_h ) ;
	if ( cx >= _x && cx < _x+_w && cy >= _y && cy < _y+_h ) {
		if ( !(_flags & AM_CLICKED) )
			press_action( cx, cy ) ;
	} else if ( _flags & AM_CLICKED ) {
		release_action() ;
		_flags &= (0xFF ^ AM_CLICKED) ;
	}
	
	return( (_flags & AM_CLICKED) ? true : false ) ;
}

// render the object on the LCD
void clickable::render( boolean from_release, boolean vartxt_only )
{
	uint16_t labelx, labely, llen = 0 ;
	uint8_t fsize ;
	uint16_t _x, _y, _w, _h ;
	uint16_t bgcolor, fgcolor ;
	const struct clickable_prog *lpbuff =
			(const struct clickable_prog *)_pbuff ;
	__FlashStringHelper *label = (__FlashStringHelper *)
			pgm_read_word_near( &(_pbuff->label) ) ;
	__FlashStringHelper *vartxt_pgm = NULL ;
	char *vartxt_mem = NULL ;
	void *vartxt = (void *) pgm_read_word_near( &(lpbuff->vartxt) ) ;
	__FlashStringHelper *vartxt2 = (__FlashStringHelper *)
			pgm_read_word_near( &(lpbuff->vartxt2) ) ;
	get_dims( &_x, &_y, &_w, &_h ) ;
	fgcolor = READ_FG ;
	bgcolor = READ_BG ;


	if ( vartxt != NULL && (_flags & VARTXT_MEM) ) {
		vartxt_mem = (char *) vartxt ;
		llen = strlen( vartxt_mem ) ;
	} else if ( vartxt != NULL ) {
		vartxt_pgm = (__FlashStringHelper *)vartxt ;
		llen = label_len( vartxt_pgm ) ;
	}

	if ( !READ_TRANSPARENT && !vartxt_only )
		draw_rect( bgcolor, _x, _y, _x+_w, _y+_h ) ;

	// there may or may not be a label
	if ( label != NULL || vartxt != NULL ) {
		fsize = READ_FSIZE ;
		lcd.Set_Text_Mode( true  ) ;
		lcd.Set_Text_Size( fsize ) ;
		lcd.Set_Text_colour( 
			( (_flags & AM_CLICKED) ? READ_FG_CLICKED : fgcolor)) ;
		lcd.Set_Text_Back_colour( bgcolor ) ;
		// label is centered in the bounding box
		if ( label != NULL )
			labelx = _x+((_w-STR_PIXW(label_len(label),fsize))/2);
		labely = _y+(_h-fsize*CHAR_H_FAC )/2 ;
		if ( label != NULL && !vartxt_only )
			lcd.Print_String( label, labelx, labely ) ;
		else if ( vartxt_only )
			lcd.Set_Text_Mode( false  ) ;
			
		labelx += STR_PIXW((READ_VOFF),fsize) ;
		// vartxt changes color on press/release and also text on
		// release. am_on in _flags has been updated prior to the call.
		// vartxt is for OFF, vartxt2 is for ON
		// if the object is not on/off, vartxt2 should be NULL
		if ( vartxt != NULL ) {
			if ( from_release && (_flags & AM_ONOFF) ) {
				// text changed, erase the old
				lcd.Set_Text_colour( READ_BG ) ;
				if (_flags&AM_ON)
					lcd.Print_String( vartxt2,
						labelx, labely ) ;
				else if ( _flags & VARTXT_MEM ) {
					lcd.Print_String( vartxt_mem, 
						labelx, labely ) ;
				} else {
					lcd.Print_String( vartxt_pgm, 
						labelx, labely ) ;
				}
				lcd.Set_Text_colour( fgcolor ) ;
				if ( _flags&AM_ON && _flags&VARTXT_MEM ) {
					lcd.Print_String( vartxt_mem,
						labelx, labely ) ;
				} else if ( _flags&AM_ON ) {
					lcd.Print_String( vartxt_pgm, 
						labelx, labely ) ;
				} else 
					lcd.Print_String( vartxt2, 
						labelx, labely ) ;
			} else {
				// same text, just redraw in the new color
				lcd.Set_Text_colour( 
				((_flags&AM_CLICKED)?READ_FG_CLICKED:fgcolor)
				) ;
				if ((_flags&AM_ON||vartxt2==NULL)&&
						(_flags|VARTXT_MEM)) {
					lcd.Print_String( vartxt_mem,
						labelx, labely ) ;
				} else if ( _flags&AM_ON || vartxt==NULL ) {
					lcd.Print_String( vartxt_pgm, 
						labelx, labely ) ;
				} else 
					lcd.Print_String( vartxt2, 
						labelx, labely ) ;
			}
		}
	}

}

// on release, change the state if on-off and execute the _action.
void clickable::release_action() 
{
	int (*_action)( void *obj, void *aa) =
	(int (*)( void *obj, void *aa))
	pgm_read_word_near( 
		&(((const struct clickable_prog *)_pbuff)->_action)) ;
	void *_act_arg = (void *) pgm_read_word_near(
		&(((const struct clickable_prog *)_pbuff)->_act_arg)) ;
	
	_flags &= (0xFF ^ AM_CLICKED) ;
	if ( !(_flags & AM_ONOFF ) )
		render( false, false ) ; // not onoff
	else {
		if ( _flags & AM_ON )
			_flags &= (0xFF ^ AM_ON ) ;
		else
			_flags |= AM_ON ;
		render( true, false ) ;
	}
	if ( _action != NULL )
		_action( (void *)this, _act_arg ) ;
}

// editvar class code
// editvar::edit and preceding functions and constants make and manage edit
// screen to edit the value linked to the object.
#define N_PADD 10
#define N_FSIZE 6
#define N_CHAR_W (CHAR_W_FAC * N_FSIZE)
#define N_CELL_W (N_CHAR_W + 2*N_PADD)
#define N_CHAR_H (CHAR_H_FAC * N_FSIZE)
#define N_CELL_H (N_CHAR_H + 2*N_PADD)
#define N_CELL_BG WHITE
#define N_CELL_FG BLACK
#define N_CELL_FG_CLICKED WHITE
#define N_CELL_BG_CLICKED RED

// draw a single char in the edited string, together with background
void draw_edit_char( bool am_clicked, int x, int y,
	uint8_t base_char_w, uint16_t base_w, uint8_t base_h,
	int fsize, int n_edit, uint8_t index, char to_draw )
{
	draw_rect( (am_clicked ? N_CELL_BG_CLICKED:N_CELL_BG),
		(index==0? x:(x+(index*base_char_w))), y,
		(index==(n_edit-1)?x+base_w+10:(x+((index+1)*base_char_w))),
		y+base_h ) ;
	lcd.Draw_Char( x+5+ index*base_char_w, y+5, to_draw, 
		(am_clicked ? N_CELL_FG_CLICKED:N_CELL_FG),
		(am_clicked ? N_CELL_BG_CLICKED:N_CELL_BG), fsize, 1 ) ;
}

// draw a single keypad entry
void draw_one_padval( uint16_t bgcol, uint16_t fgcol, int i, int j, char num ) 
{
	draw_rect( bgcol, i, j, i + N_CELL_W, j + N_CELL_H ) ;
	lcd.Draw_Char( i+N_PADD, j+N_PADD, num, fgcol, bgcol, N_FSIZE, 1 ) ;
}

// edit function
// draws the edit screen and runs editing loop
// on exit, if the ok button has been pressed and the variable value in the
// edit buffer has changed, updates the linked variable in the sketch and
// records the change in EEPROM
void editval::edit()
{
	int n_edit, fsize, x, y, i, j ;
	int n_pad_x, n_pad_y ;
	char editbuff[15]  ;
	char *vartxt ; // linked to in-memory vartxt
	uint8_t base_h, base_char_w, selected = 255 ;
	uint16_t base_w, lcd_w, lcd_h ;
	char *numvals = "123456789-0 " ;
	char *numvals2 = "-1230456 789" ;
	uint8_t vtxt_off, n_int, n_dec, n_clicked = 255 ;
	uint8_t n_row, n_col, n_pad_w, n_pad_h ;
	bool am_signed, done_ok=false, done_cancel=false, am_pressed=false ; 
	long *value, intval ;
	float *fvalue, new_fvalue ;
	uint16_t eeprom_addr ;
	long new_intval ;
	clickable *ok, *cancel ;
	const struct editval_prog *lpbuff =
			(const struct editval_prog *)_pbuff ;
	int (*_action)( void *obj, void *aa) ;
	vartxt = (char *)pgm_read_word_near( &(lpbuff->vartxt) ) ;
	n_int = pgm_read_byte_near( &(lpbuff->n_int) ) ;
	n_dec = pgm_read_byte_near( &(lpbuff->n_dec) ) ;
	am_signed = pgm_read_byte_near( &(lpbuff->am_signed) ) ;
	value = pgm_read_word_near( &(lpbuff->value) ) ;
	eeprom_addr = pgm_read_word_near( &(lpbuff->eeprom_address) ) ;
	ok = pgm_read_word_near( &(lpbuff->ok) ) ;
	cancel = pgm_read_word_near( &(lpbuff->cancel) ) ;
	_action = (int (*)( void *obj, void *aa))
			pgm_read_word_near( &(lpbuff->_action)) ;
#if 1
	if ( n_dec == 0 ) {
		String( *value ).toCharArray(editbuff,15) ;
	} else {
		String( *((float *)value), n_dec ).toCharArray(editbuff,15) ;
	}
#else
	strcpy( editbuff, vartxt ) ;
#endif
	if ( !am_signed )
		numvals[9] = ' ' ;
	// set up for edit screen
	n_edit = n_int + n_dec + (n_dec>0 ? 1:0) + (am_signed ? 1:0) ;
	if ( strlen(editbuff) < n_edit ) {
		// string is left-justified, needs to be right-justified
		// if float, we're guaranteed the right number of decimals
		j = n_edit - strlen(editbuff) ;
		for ( i = n_edit-1 ; i >= 0 ; i-- )
			if ( i-j >= 0 )
				editbuff[i] = editbuff[i-j] ;
			else
				editbuff[i] = ' ' ;
		i = strlen(editbuff) ;
	}
	lcd_w = lcd.Get_Display_Width() ;
	lcd_h = lcd.Get_Display_Height() ;
	if ( lcd_w < lcd_h ) {
		n_row = 4 ;
		n_col = 3 ;
		y = 80 ;
	} else {
		n_row = 3 ;
		n_col = 4 ;
		y = 10 ;
		// also need to reorder numvals
		numvals = numvals2 ;
	}
	fsize = 6 ;
	while ( (STR_PIXW(n_edit,fsize)) > lcd_w )
		fsize-- ;
	base_char_w = fsize * (CHAR_W_FAC+1) ;
	base_w = base_char_w*n_edit - fsize + 10 ; // deduct one inter-char skip
	base_h = fsize*CHAR_H_FAC+10 ;
	x = (lcd_w - base_w)/2 ;
	// redraw and edit
	lcd.Fill_Screen( BLACK ) ;
	draw_rect( N_CELL_BG, x, y, x+base_w+10, y+base_h) ;
	// text to be edited
	for ( i = 0 ; i < n_edit ; i++ )
		lcd.Draw_Char( 
			x+5+ i*base_char_w,
			y+5,
			editbuff[i], 
			N_CELL_FG, N_CELL_BG,
			fsize, 1 ) ;
	// keypad
	n_pad_w = n_col * N_CELL_W ;
	n_pad_h = n_row * N_CELL_H ;
	n_pad_x = (lcd_w-n_pad_w)/2 ;
	if ( lcd_w < lcd_h )
		n_pad_y = y+base_h+40 ;
	else
		n_pad_y = y+base_h+10 ;
	draw_rect( N_CELL_BG, n_pad_x, n_pad_y,
			n_pad_w+n_pad_x, n_pad_h+n_pad_y) ;
	for ( i = 0 ; i < 12 ; i++ )
		lcd.Draw_Char( 
			n_pad_x+N_PADD + (i%n_col)*N_CELL_W,
			n_pad_y+N_PADD + (i/n_col)*N_CELL_H,
			numvals[i], 
			N_CELL_FG, N_CELL_BG,
			N_FSIZE, 0 ) ;
	// draw OK and CANCEL buttons
	ok->render( false, false ) ;
	cancel->render( false, false ) ;
	am_pressed = false ;
	while ( !done_ok && !done_cancel ) {
	        tscreen.TP_Scan( 0 ) ;
        	if (!am_pressed && ( tscreen.TP_Get_State() & TP_PRES_DOWN) ) {
                        am_pressed = true ;
                	if ( tscreen.x >= x &&
					tscreen.x < x+base_w+10 &&
                			tscreen.y >= y &&
					tscreen.y < y+base_h ) {
				// a char was selected to be edited
				i = (tscreen.x - x - 5)/base_char_w ;
				if  ( i < 0 )
					i = 0 ;
				else if ( i >= n_edit )
					i = n_edit - 1 ;
				if ( selected < 255 && selected != i ) {
					// another char was selected,
					// deselect
 					draw_edit_char(false, x, y, base_char_w,
						base_w, base_h, fsize, n_edit,
						selected, editbuff[selected]);
					selected = 255 ;
				}
				selected = i ;
				if ( n_dec > 0 && selected == n_int+am_signed )
					// at decimal point, can't be edited
					selected = 255 ;
				else
 					draw_edit_char(true, x, y, base_char_w,
						base_w, base_h, fsize, n_edit,
						selected, editbuff[selected]);
                	} else if ( tscreen.x >= n_pad_x &&
					tscreen.x < n_pad_x+n_pad_w &&
                			tscreen.y >= n_pad_y &&
					tscreen.y < n_pad_y+n_pad_h ) {
				// a keypad number was pressed
				i = n_pad_x + (n_clicked%n_col)*N_CELL_W ;
				j = n_pad_y + (n_clicked/n_col)*N_CELL_H ;
				n_clicked=n_col*((tscreen.y-n_pad_y)/N_CELL_H)+
					(tscreen.x-n_pad_x)/N_CELL_W ;
				draw_one_padval(N_CELL_BG_CLICKED,
					N_CELL_FG_CLICKED,
					i, j, numvals[n_clicked] ) ;
			} else {
				// not in keypad or selector, deselect if
				// any char is currently being edited
				if ( selected < 255 ) {
 					draw_edit_char(false, x, y, base_char_w,
						base_w, base_h, fsize, n_edit,
						selected, editbuff[selected]);
					selected = 255 ;
				}
				ok->click( tscreen.x, tscreen.y ) ;
                		cancel->click( tscreen.x, tscreen.y ) ;
			}
			delay( 200 ) ; // or we sometimes get double press
                } else if ( am_pressed &&
        			!(tscreen.TP_Get_State() & TP_PRES_DOWN) ) {
			am_pressed = false ;
			if ( n_clicked != 255 ) {
				if ( selected < 255 ) {
					editbuff[selected] = numvals[n_clicked];
					// write the new char & background
 					draw_edit_char(true, x, y, base_char_w,
						base_w, base_h, fsize, n_edit,
						selected, editbuff[selected]);
				}
				i = n_pad_x+(n_clicked%n_col)*N_CELL_W ;
				j = n_pad_y+(n_clicked/n_col)*N_CELL_H ;
				draw_one_padval(N_CELL_BG,
					N_CELL_FG,
					i, j, numvals[n_clicked] ) ;
				n_clicked = 255 ;
			}
                	if ( ok->release() )
				done_ok = true ;
                	else if ( cancel->release() )
				done_cancel = true ;
		}
	} // end edit loop

	if ( done_ok ) {
		// OK pressed, parse the editbuff and update the value if it
		// changed
		// before parsing, eliminate any spaces
		i = 0 ;
		j = 0 ;
		while ( i < n_edit )
			if ( editbuff[i]  == ' ' )
				i++ ;
			else
				editbuff[j++] = editbuff[i++] ;
		n_edit = strlen( editbuff ) - 1 ;
		while ( n_edit >= j  )
			editbuff[n_edit--] = '\0' ;
		n_edit++ ;
		
		done_ok = false ; // reusing to track if value changed
		if ( n_dec == 0 ) {
			// integer value
			new_intval = 0 ;
			if ( editbuff[0] == '-' ) 
				i = 1 ;
			else
				i = 0 ;
			while ( i < n_edit ) {
				new_intval = 10 * new_intval +
					((long)editbuff[i]-(long)'0');
				i++ ;
			}
			if ( editbuff[0] == '-' ) 
				new_intval = -new_intval ;
			if ( new_intval != intval ) {
				done_ok = true ;
				*value = new_intval ;
			}
		} else {
			// float value
			if ( editbuff[0] == '-' )
				i = 1 ;
			else
				i = 0 ;
			new_fvalue = 0 ;
			while ( i < n_edit && editbuff[i] != '.' ) {
				new_fvalue = 10 * new_fvalue +
					((long)editbuff[i]-(long)'0');
				i++ ;
			}
			// we're at the decimal point
			j = i + 1 ;
			new_intval = 10 ;
			while ( j < n_edit  ) {
				new_fvalue += 
					((float)editbuff[j]-(float)'0') /
					(float)new_intval ;
					j++ ;
					new_intval *= 10 ;
			}
			if ( editbuff[0] == '-' )
				new_fvalue = -new_fvalue ;
			fvalue = (float *)value ;
			if ( new_fvalue != *fvalue ) {
				done_ok = true ;
				*fvalue = new_fvalue ;
			}
		}
		if ( done_ok ) {
			EEPROM.put( eeprom_addr, *value ) ;
			strcpy( vartxt, editbuff ) ;
			if ( _action != NULL ) 
				_action( (void *)this, NULL ) ;
		}
	}

}

// release_action calls edit
void editval::release_action() 
{
	_flags &= (0xFF ^ AM_CLICKED) ;
	edit() ;
}

// propagates the value to the display buffer
// sketch calls this to set the display buffer every time it changes the
// value of the external variable.
void editval::set_val_display()
{
	uint8_t n_dec = pgm_read_byte_near(&(((struct editval_prog *)_pbuff)->n_dec)); 
	long *value = pgm_read_word_near( &(((struct editval_prog *)_pbuff)->value) ) ;
	char *vartxt = pgm_read_word_near( &(((struct editval_prog *)_pbuff)->vartxt) ) ;
	if ( n_dec == 0 )
		String( *value ).toCharArray(vartxt,15) ;
	else 
		String( *((float *)value), n_dec).toCharArray(vartxt,15) ;
}
