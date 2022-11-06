// various LEDs and touchscreens

module ILI9488()
{
	difference() {
		union() {
			cube( [ 98, 56.24, 2 ] ) ;
			translate( [ 1, 10.39, -7 ] ) cube( [ 2.54, 35.56, 8 ]) ;
			translate( [ 94.46, 23.09, -7 ] )
				cube( [ 2.54, 10.16, 8 ]) ;
			translate( [ 52, 38, -3 ] ) cube( [ 27, 17, 4 ] ) ;
			translate( [ 6.5, 1, 1.99 ] )
				cube( [ 83.00, 54.24, 3.5] ) ;
		}
		union() {
			translate( [3, 3.42, -1] ) cylinder( 5, d=3, $fn=50) ;
			translate( [95, 3.42, -1] ) cylinder( 5, d=3, $fn=50) ;
			translate( [3, 52.82, -1] ) cylinder( 5, d=3, $fn=50) ;
			translate( [95, 52.82, -1] ) cylinder( 5, d=3, $fn=50) ;
		}
	}
}

ILI9488() ;
