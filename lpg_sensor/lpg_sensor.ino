/*
  LPG (C3H8) concentration sensor. MQ6 sensor is used to measure the LPG
  concentration, while DHT20 is used to measure temperature and relative
  humidity. This has been designed for, and tested on, Arduino Nano.

  MQ6 must be warmed up for 30 seconds in clean air (i.e., without any
  propane content) and changes resistance with propane concentration.
  Mapping of resistance to LPG PPM (parts per million) depends on the ratio
  of the measured resistance to resistance at 1000ppm. Clean air resistance
  is 10 times the resistance at 1000ppm. The sensor can't measure
  concentrations below 200ppm, which translates to ratio of over roughly 1.4.

  Resistance to concentration curve is specified for temperature of 20C and
  65% relative humidity. We use DHT20 sensor to measure the actual
  temperature and relative humidity and correct the MQ6 readings
  accordingly.

  Results are displayed on an Adafruit 1.8" 160x128 color display, ST7735R.
  See https://learn.adafruit.com/adafruit-gfx-graphics-library for the
  library documentation.

  After warmup, we display relative humidity and temperature and the LPG
  concentration. Concentration is either in ppm or given as "< 200". In
  normal environment, concentration will always be below 200ppm, so we also
  display the ratio (r: line below ppm) so we can track how the sensor
  reading changes (corrected for the temperature and relative humidity).

  DHT20 uses I2C and has 7 bit address, just like I2C library. Address is 0x38.

  If the Nano is connected via USB to a host, more detailed record of
  measurements is output over the serial line.

  By Nenad Rijavec
  You are welcome to use, share, modify, etc. this code as you see fit. Note
  that Adafruit libraries have their own usage conditions.
*/

#include <Wire.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735

#define DHT20_addr 0x38
#define MQ6_PIN 6
#define MQ6_WARMUP_SECS 30
#define MQ6_CLEAN_AIR_RATIO 10.0 

// for temperature/humidity correction
int t_vals[4] = { -10, 5, 20, 50 } ;
float rh33[4] = { 1.33, 1.1, 1, 0.9 } ;
float rh65[4] = { 1.21, 1.0, 0.92, 0.82 } ; // computed from 33 & 85
float rh85[4] = { 1.13, 0.94, 0.87, 0.77 } ;
float rh65_20 = 0.92 ; // correction factor for 20C and 65% RH

float ref_resistance = 22.0 ; // in kOhms
float Ro ;

// linear fit with for the LPG concentration with x=log10(lpg_ppm)
// form the spec, y(3)=1, y(4)=0.4, so y(x)=-0.6(x-3)+1, where y is Rs/Ro
// ratio. Thus x = (2.8-y)/0.6 and lpg_ppm = 10^x.
float k = 0.6, y0 = 2.8 ;

// LED definitions

#define TFT_CS        10
#define TFT_RST        9 
#define TFT_DC         8

/*
   ST7735 supports 18 bit color, but the interface used uint16_t and thus
   only provides 16 bits. R and B are specified using 5 bits, while G uses 6
   bits. The bits are thus packed as RRRR RGGG GGGB BBBB. Note that this is
   logical packing. Since Arduino Nano is little endian, the bytes in the
   actual storage are reversed (GGGB BBBB RRRR RGGG). Treating RGB as an
   uint16_t removes the byte ordering from consideration.
*/
// color definitions
const uint16_t  color_black        = 0x0000;
const uint16_t  color_blue         = 0x001F;
const uint16_t  color_red          = 0xF800;
const uint16_t  color_green        = 0x07E0;
const uint16_t  color_magenta      = 0xF81F;
const uint16_t  color_yellow       = 0xFFE0;
const uint16_t  color_orange       = 0xFB40;
const uint16_t  color_white        = 0xFFFF;

uint16_t text_color         = color_blue;
uint16_t background_color    = color_yellow;
uint16_t font_size = 3 ;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// LED screen display structure
// background color is fixed for the whole LED, each text item has its own
// color, font size and position.

#define N_DISP_ITEMS 5
#define DISP_STR_LEN 20
struct led_item {
	uint16_t txt_color ;
	uint16_t font_size ;
	int16_t x_orig ; // resets at each display if centered
	int16_t y_orig ;
	bool am_centered ;
	char *txt ; // ptr to an location in a single array for all the text
	// values used to erase old item
	int16_t e_x_orig ;
	int16_t e_y_orig ;
	uint16_t e_w ;
	uint16_t e_h ;
} ;

struct led_item disp_items[N_DISP_ITEMS] ;
unsigned int n_disp_items = 0 ;

char disp_txt[N_DISP_ITEMS*DISP_STR_LEN] ;  // all the text is kept here

// a simple workaround around the fact that Arduino sprintf doesn't support
// floats. We print the float with 2 decimals into a global buffer and then
// output the buffer as needed.

char buff[20] ;

void sprintf_float( float val )
{
	int val_a, val_b ;
	val_a = trunc(val) ;
	val_b = trunc(100*val) - 100*val_a ;
	sprintf( buff, "%2d.%02d", val_a, val_b ) ;
}

// actually write the item to the lcd
void write_to_led( int i)
{
	struct led_item *curr = &disp_items[i] ;
	uint16_t w, h ;
	int16_t x1, y1 ;
	tft.setTextColor(curr->txt_color, background_color );
	tft.setTextSize(curr->font_size);
	if ( curr->am_centered ) {
		tft.getTextBounds( String(curr->txt),
			 	curr->x_orig, curr->y_orig,
			 	&x1, &y1, &w, &h ) ;
		curr->x_orig = (128-w)/2 ;
		// if it narrowed, erase
		if ( w < curr->e_w ) {
			tft.fillRect( curr->e_x_orig, curr->e_y_orig,
				curr->e_w, curr->e_h, background_color ) ;
		}
		// reset the erase area to current content
		curr->e_x_orig = curr->x_orig ;
		curr->e_w = w ;
	}
	tft.setCursor( curr->x_orig, curr->y_orig ) ;
	tft.print( curr->txt ) ;
}

// sets the text at specified location and specified font
// erases any old text on the screen and resets e_* values
// text is not displayed yet
// for changing values, we might keep the item as built here and just change
// the text and perhaps x_orig to keep the text centered.
int mk_disp_item( int n, int16_t x, int16_t y,
		uint16_t font_size, uint16_t txt_color,
		char *txt, bool am_centered )
{
	struct led_item *curr ;
	if ( n >= N_DISP_ITEMS || strlen(txt) > DISP_STR_LEN-2 )
		return( 1 ) ;
	curr = &disp_items[n] ;
	if ( curr->e_w > 0 ) {
		// something was there, erase it
		tft.fillRect( curr->e_x_orig, curr->e_y_orig,
			curr->e_w, curr->e_h, background_color ) ;
	}
	// set the item
	curr->am_centered = am_centered ;
	curr->x_orig = x ;
	curr->y_orig = y ;
	curr->font_size = font_size ;
	curr->txt_color = txt_color ;
	sprintf( curr->txt, "%s", txt ) ;
	tft.setTextSize(font_size); // must set for the erase size
	// set erase rectangle
	tft.getTextBounds( txt, x, y, 
		&curr->e_x_orig, &curr->e_y_orig, &curr->e_w, &curr->e_h ) ;

	return( 0 ) ;
}


// initialize all display items
void init_disp_txt()
{
	uint16_t y_orig = 40 ;
	int i, rc  ;
	memset( disp_items, 0x0, sizeof(disp_items) ) ;

	for ( i = 0 ; i < N_DISP_ITEMS ; i++ )
		disp_items[i].txt = disp_txt + i * DISP_STR_LEN ;

	rc = mk_disp_item( n_disp_items++, 10, y_orig, 3, text_color,
		"WARMUP", true ) ;
	if ( rc != 0 ) {
		Serial.println( "mk_disp_item failed for " +
			String(n_disp_items-1) ) ;
		n_disp_items-- ;
		return ;
	}
	y_orig += 40 ;
	rc = mk_disp_item( n_disp_items++, 35, y_orig, 4, text_color,
		"30", true ) ;
	if ( rc != 0 ) {
		Serial.println( "mk_disp_item failed for " +
			String(n_disp_items-1) ) ;
		n_disp_items-- ;
		return ;
	}
	rc = mk_disp_item( n_disp_items++, 0, 140, 2, text_color,
		"RH: xx.xx\%", false ) ;
	if ( rc != 0 ) {
		Serial.println( "mk_disp_item failed for " +
			String(n_disp_items-1) ) ;
		n_disp_items-- ;
		return ;
	}
	rc = mk_disp_item( n_disp_items++, 64, 140, 2, text_color,
		"T: xx.xxC", false ) ;
	if ( rc != 0 ) {
		Serial.println( "mk_disp_item failed for " +
			String(n_disp_items-1) ) ;
		n_disp_items-- ;
		return ;
	}
	y_orig += 30 ;
	rc = mk_disp_item( n_disp_items++, 10, y_orig, 2, text_color,
		"ratio:", true ) ;
	if ( rc != 0 ) {
		Serial.println( "mk_disp_item failed for " +
			String(n_disp_items-1) ) ;
		n_disp_items-- ;
		return ;
	}

}

// display given temp and relative humidity using prebuilt items 2 and 3
void display_t_rh( float t, float rh )
{
	int rh_a = trunc(rh), rh_b = trunc(10*rh)-10*rh_a ;
	int t_a = (int)t, t_b = (int)(10*t)-10*t_a ;
	sprintf( disp_items[2].txt, "RH:%2d", rh_a ) ;
	sprintf( disp_items[3].txt, "T:%2dC", t_a ) ;
	write_to_led( 2 ) ;
	write_to_led( 3 ) ;
}

// blocks until the sensor is initalized
// no real error handling except warning if a serial line is connected
void initialize_dht20()
{
	int wait_ctr = 1 ;
	bool is_initialized = false ;
	unsigned char res, data[7] ;
	
	// set up and connect to DHT20
	Wire.begin() ;
	delay( 1000 ) ; // wait for the sensor to initalize
	while ( !is_initialized ) {
		// ask for sensor status
		Serial.println( "reading DHT20 sensor status" ) ;
		Wire.beginTransmission( DHT20_addr ) ;
		Wire.write( 0x71 ) ;
		Wire.endTransmission() ;
		// read status byte
		Wire.requestFrom( DHT20_addr, 1 ) ;
		while ( Wire.available() < 1 ) {
			Serial.println( "waiting for DHT20: " +
				String( wait_ctr ) +
				" sec" ) ;
			wait_ctr += 1 ;
			delay( 1000 ) ;
		}
		res = Wire.read() ;
		if ( (res&0x18) == 0x18 ) {
			Serial.println( " OK" ) ;
			is_initialized = true ;
		} else {
			Serial.println( " Not initialized after " +
				String(wait_ctr) + " sec" ) ;
			wait_ctr += 1 ;
			delay( 1000 ) ;
		}
	}
}

// read DHT20 and parse the data
void get_temp_rh( float *temp, float *rh )
{

	unsigned long divisor = (unsigned long)1024 * (unsigned long)1024 ;
	unsigned char res, data[7] ;
	unsigned long Srh = 0, St = 0 ;
	int wait_ctr = 0 ;
	bool meas_ready = false ;
	int i ;
	Wire.beginTransmission( DHT20_addr ) ;
	Wire.write( 0xAC ) ;
	Wire.write( 0x33 ) ;
	Wire.write( 0x00 ) ;
	Wire.endTransmission() ;
	delay( 100 ) ;
	Wire.requestFrom( DHT20_addr, 1 ) ;
	while ( Wire.available() < 1 ) {
		Serial.println( "waiting for DHT20: " +
			String( wait_ctr ) +
			" sec" ) ;
		wait_ctr += 1 ;
		delay( 1000 ) ;
	}
	wait_ctr = 0 ;
	while ( !meas_ready ) {
		res = Wire.read() ;
		if ( (res&0x80) == 0 ) {
			meas_ready = true ;
		} else {
			Serial.println( "waiting for temp, rh " +
				String( wait_ctr ) + " sec" ) ;
		}
	}
	Wire.requestFrom( DHT20_addr, 6 ) ;
	for ( i = 0 ; i < 6 ; i++ )
		if ( Wire.available() > 0 )
			data[i] = Wire.read() ;
	Srh = data[1] ;
	Srh <<=8 ;
	Srh |= data[2] ;
	Srh <<=4 ;
	Srh |= data[3] >> 4 ;
	*rh = 100.0 * (float)Srh / divisor ;
	St = data[3] & 0x0f ;
	St <<= 8 ;
	St |= data[4] ;
	St <<= 8 ;
	St |= data[5] ;
	*temp = 200.0 * (float)St / divisor - 50.0 ;
		
}

// read MQ6
float get_mq6_resistance()
{
	int raw ;
	float voltage ;
	raw = analogRead(MQ6_PIN) ;
	return( ref_resistance * (1023-raw)/raw  ) ;
}

// MQ6 detection resistor must be warmed up. We wait for MQ6_WARMUP_SECS and
// possibly longer until the read value settles down
// timer and temp and humidity are displayed during warmup
void init_mq6()
{
	int to_wait = MQ6_WARMUP_SECS ;
	float res_val[MQ6_WARMUP_SECS] ;
	float max_val ;
	float temp, rh ;
	int i, max_loc ;
	bool mq6_init = false ;
	write_to_led( 0 ) ;  // warmup title
	// wait to warm up
	while ( to_wait > 0 ) {
		get_temp_rh( &temp, &rh ) ;
		display_t_rh( temp,rh ) ;
		sprintf( disp_items[1].txt, "%1d", to_wait ) ;
		write_to_led( 1 ) ;
		res_val[MQ6_WARMUP_SECS-to_wait] = get_mq6_resistance() ;
		Serial.println( "warming up MQ6 : " + String( to_wait ) +
			" (" + String( (int)res_val[MQ6_WARMUP_SECS-to_wait] ) +
			")" ) ;
		to_wait -= 1 ;
		delay( 1000 ) ;
	}
	sprintf( disp_items[1].txt, "%1d", to_wait ) ;
	write_to_led( 1 ) ;
	Serial.println( "MQ6 warmed up" ) ;
	// calibrate by waiting for the resistance to settle down
	while ( !mq6_init ) {
		max_val = res_val[0] ;
		max_loc = 0 ;
		for ( i = 1 ; i <  MQ6_WARMUP_SECS ; i++ )
			if ( max_val < res_val[i] ) {
				max_val = res_val[i] ;
				max_loc = i ;
			}
		if ( max_loc < MQ6_WARMUP_SECS -1 )
			mq6_init = true ;
		else {
			for ( i = 0 ; i < MQ6_WARMUP_SECS - 1 ; i++ )
				res_val[i] = res_val[i+1] ;
			res_val[MQ6_WARMUP_SECS-1] = get_mq6_resistance() ;
			Serial.println( "max: " + String( max_loc) + 
				" new: " +
				String(res_val[MQ6_WARMUP_SECS-1]) ) ;
			delay( 1000 ) ;
		}
	}
	Serial.println( "MQ6 calibrated" ) ;
}

// correct th ratio (or just the measured resistance) for the measured
// temperature and relative humidity
float t_rh_correct( float ratio, int temp, int rh )
{
	// t in c, rh in %

	int i, t_low, t_high ;
	float t_weight, val33, val85 ;
	float slope, val ;

	if ( temp < -10 || temp > 50 )
		return -1.0 ;
	t_low = 0 ;
	t_high = 3 ;

	while ( t_vals[t_low] < temp )
		t_low++ ;
	if ( t_vals[t_low] > temp )
		t_low-- ;
	while ( t_vals[t_high] > temp )
		t_high-- ;
	if ( t_vals[t_high] < temp )
		t_high++ ;
	if ( t_low == t_high )
		t_weight = 1.0 ;
	else
		t_weight = (temp-t_vals[t_low])/(t_vals[t_high]-t_vals[t_low]);
	val33 = t_weight * rh33[t_low] + (1 - t_weight) * rh33[t_high] ;
	val85 = t_weight * rh85[t_low] + (1 - t_weight) * rh85[t_high] ;
	// fit the line, anchor at 33
	slope = (val85-val33) / (85.0-33.0) ;
	val = slope * (rh-33) + val33 ;
	/*
	   val is Rs/Ro correction factor for given temperature and 
	   relative humidity relative to 20C and 33% rh. The value table for
	   the Rs/Ro in the spec is based on 20C and 65% rh, so to get to
	   the right value, we divide by val and multiply by rh65_20. 
	   That way, the Rs/Ro can be multiplied by the correction factor to
	   get it to the right point.
	*/
	val = ratio * rh65_20 / val ;

	return val ;
}

void setup()
{
	float temp, rh ;
	float r_base ;
	int i, rc ;
	Serial.begin(115200) ;
	// connect to the display
	tft.initR(INITR_BLACKTAB); // Init ST7735R chip
 
	// initialize the display
	tft.setFont();
	tft.fillScreen(background_color);
	tft.setTextColor(text_color);
	tft.setTextSize(font_size);

	// init display data
	init_disp_txt() ;

	// initialize sensors
	initialize_dht20() ; // blocks until dht20 is ready
	get_temp_rh( &temp, &rh ) ;
	display_t_rh( temp,rh ) ;
	init_mq6() ;

	// sensors ready
	r_base = get_mq6_resistance() ;
	Serial.println( " base resistance : " + String(r_base) ) ;
	get_temp_rh( &temp, &rh ) ;
	display_t_rh( temp,rh ) ;
	Serial.println( " RH : " + String( trunc(rh) ) + "%" ) ;
	Serial.println( " T : " + String( trunc(temp) ) + "C" ) ;
	Ro = t_rh_correct( r_base, temp, rh ) ;
	sprintf_float( Ro ) ;
	Serial.print( " corrected : " ) ;
	Serial.print( buff ) ;
	Ro = Ro / 10.0 ;
	sprintf_float( Ro ) ;
	Serial.print( " Ro : " ) ;
	Serial.println( buff ) ;

	// ready to measure, change LCD labels

	rc = mk_disp_item( 0, 0, 10, 3, text_color,
		"LPG PPM", true ) ;
	rc = mk_disp_item( 1, 0, 60, 4, text_color,
		"< 200", true ) ;
	// Make sure we erase the whole line. As the values change, the
	// width of the line changes, but the y offset and height don't.
	disp_items[1].e_x_orig = 0 ;
	disp_items[1].e_w = 128 ;
	write_to_led( 0 ) ;
	write_to_led( 1 ) ;

}

bool last_was_below = false ; // so we don't keep reweiting "< 200"

void loop()
{

	float temp, rh, res, ccr, ratio, x ;
	double lpg_ppm ;

	// measure and correct for temp and relative humidity
	get_temp_rh( &temp, &rh ) ;
	display_t_rh( temp, rh ) ;
	res = get_mq6_resistance() ;
	ccr =  t_rh_correct( res, temp, rh ) ;

	// process the measurement
	ratio = ccr/Ro ;
	Serial.print( "RH : " + String( trunc(rh) ) + "%" ) ;
	Serial.print( " T : " + String( trunc(temp) ) + "C" ) ;
	Serial.print( " resistance : " ) ;
	sprintf_float( res ) ;
	Serial.print( buff ) ;
	Serial.print( " corrected: " ) ;
	sprintf_float( ccr ) ;
	Serial.print( buff ) ;
	Serial.print( " ratio: " ) ;
	sprintf_float( ratio ) ;
	Serial.print( buff ) ;
	sprintf( disp_items[4].txt, "r: %s", buff ) ;
	write_to_led( 4 ) ;
	x = (y0 - ratio)/k ;
	if ( x < 2.3 ) {
		if ( !last_was_below ) {
			sprintf( disp_items[1].txt, "< 200" ) ;
			last_was_below = true ;
		}
		Serial.println( " PPM < 200" ) ;
	} else {
		last_was_below = false ;
		lpg_ppm = pow( 10.0, x ) ;
		Serial.println( " PPM: " + String((int)lpg_ppm) ) ;
		sprintf( disp_items[1].txt, "%d", (int)lpg_ppm ) ;
	}
	write_to_led( 1 ) ;


	delay( 3000 ) ;
	
}
