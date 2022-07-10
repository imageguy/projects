/*
   This program is used for testing the breadboard connections between
   Arduino Uno and ILI9844 3.5in TFT screen. Use it without the actual
   display being plugged into the breadboard.
   
   The program sets all the pins used by the display to LOW.  Enter a 
   number <=-2 into the serial monitor to turn all the pins to HIGH.
   Enter -1 to turn them all to LOW. Positive numbers 0-13 turn on one pin
   in turn, where VCC is pin 0. Too large port number results in a warning
   message.

   Pins 0, 1 and 8 can't be set and will return a warning mesage. Pin 0 is
   VCC, pin 1 is GND and pin 8 is the return line from the LCD which is not
   needed and has not been connected to the Arduino.

   Using a voltmeter, you can check that all the ports are connected and the
   voltages are valid. Turning each port in turn, you can check that all the
   Arduino ports are corrected to the right LCS pins.

   By Nenad Rijavec
   Feel free to use, share and modify this program as you see fit.
*/

int used_pins[] = { -1, -1, A5, A4, A3, 11, 13, A0, -1, 3, 2, 5, 4, 6 } ;
int n_pins = sizeof( used_pins ) / sizeof( int ) ;

void write_all( int val ) 
{
	int i ;
	for ( i = 0 ; i < n_pins ; i++ )
		if ( used_pins[i] != -1 ) {
			digitalWrite(used_pins[i],val);
			Serial.println( String(i) +
				" : " + String(used_pins[i]) ) ;
		}
}

void setup()
{
	
	Serial.begin( 115200 ) ;
	Serial.println( "ready" ) ;
	pinMode(A0,OUTPUT);
	pinMode(A3,OUTPUT);
	pinMode(A4,OUTPUT);
	pinMode(A5,OUTPUT);
	pinMode(11,OUTPUT);
	pinMode(13,OUTPUT);
	pinMode(2,OUTPUT);
	pinMode(3,OUTPUT);
	pinMode(4,OUTPUT);
	pinMode(5,OUTPUT);
	pinMode(6,OUTPUT);
	Serial.println( "Setting all pins to LOW" ) ;
	write_all( LOW ) ;
}

void loop()
{
	int val, tmp ;
	if ( Serial.available() > 0 ) {
		val = Serial.parseInt() ;
		while ( Serial.available() > 0 )
			tmp = Serial.read() ;
		if ( val == -1 ) {
			Serial.println( "Setting all pins to LOW" ) ;
			write_all( LOW ) ;
		} else if ( val < -1) {
			Serial.println( "Setting all pins to HIGH" ) ;
			write_all( HIGH ) ;
		} else if ( val >= n_pins ) {
			Serial.println( "Invalid pin, must be less than " +
			String(n_pins) ) ;
		} else if ( used_pins[val] == -1 ) {
			Serial.println( "This pin can't be set" ) ;
		} else {
			write_all( LOW ) ;
			digitalWrite(used_pins[val],HIGH);
			Serial.println( "Set " + String(val) +
				"(" + String(used_pins[val]) + ") to HIGH" ) ;
		}
		while ( Serial.available() > 0 )
			tmp = Serial.read() ;
	}
}
