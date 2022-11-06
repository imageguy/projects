// mockup of a MG996R servo motor

use <plib/screw_threads.scad>

module MG996R( show_attachment )
{
	cube( [ 40.3, 20, 36.6 ] ) ;
	translate( [ 10, 10, 36.59 ] )
	union() {
		cylinder( 2.01, d=20, $fn =50 ) ;
		cylinder( 3.01, d=13, $fn =50 ) ;
		cylinder( 4.01, d=11, $fn =50 ) ;
		difference() {
			cylinder( 8.01, d=6, $fn =50 ) ;
			translate( [ 0, 0, 4.01 ] ) 
				cylinder( 4.1, d=3.5, $fn=50 ) ;
		}
		translate( [ 0, 0, 4.01 ] ) nut_core( 3, 0.5, 4 ) ;
	}
	translate( [ 10, 1, 36.59 ] ) cube( [ 25, 18, 2.01 ] ) ;
	translate( [ 31, 10, 36.59 ] ) 
		cylinder( 1.80, d=13, $fn =50 ) ;
	translate( [ -6.8, 0, 26.6 ] ) difference() {
		union() {
			cube( [ 6.8, 20, 2.5 ] ) ;
			translate( [ 0, 9.4, 1.5 ] ) hull() {
				cube( [ 1, 1.2, 1 ] ) ;
				translate( [ 6.8, 0, 0 ] )
				cube( [ 1, 1.2, 1 ] ) ;
				translate( [ 6.8, 0, 1 ] )
				cube( [ 1, 1.2, 1 ] ) ;
			}
		}
		translate( [ 2.0, 5.5, -0.1 ] )
		union() {
			cylinder( 5, d=4.5, $fn=50 ) ;
			translate( [ 0, 9, 0 ] )
			cylinder( 5, d=4.5, $fn=50 ) ;
		}
	}
	translate( [ 40.299+6.8, 0, 26.6 ] ) 
	mirror( [ 1, 0, 0 ] ) difference() {
		union() {
			cube( [ 6.8, 20, 2.5 ] ) ;
			translate( [ 0, 9.4, 1.5 ] ) hull() {
				cube( [ 1, 1.2, 1 ] ) ;
				translate( [ 6.8, 0, 0 ] )
				cube( [ 1, 1.2, 1 ] ) ;
				translate( [ 6.8, 0, 1 ] )
				cube( [ 1, 1.2, 1 ] ) ;
			}
		}
		translate( [ 2.0, 5.5, -0.1 ] )
		union() {
			cylinder( 5, d=4.5, $fn=50 ) ;
			translate( [ 0, 9, 0 ] )
			cylinder( 5, d=4.5, $fn=50 ) ;
		}
	}

	if ( show_attachment )
		translate( [ 10, 10, 41.00 ] ) difference()  {
		union()  {
			cylinder( 3, d= 8.7, $fn=50 ) ;
			translate( [ 0, 0, 3 ] ) cylinder( 2.5, d=20, $fn=50 ) ;
		}
		translate( [ 0, 0, -0.1 ] ) cylinder( 15, d=4.5, $fn=40 ) ;
	}
}


MG996R( true ) ;
