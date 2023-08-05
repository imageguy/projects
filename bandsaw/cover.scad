/*
  Cover and cover base for the bandsaw fence. Prints both, comment out what
  you don't need at the bottom if you only want to print on.

  By Nenad Rijavec.
  You are welcome to use, share and modify this code as you see fit.
*/

module cover_base()
{
	difference() {
		cube( [ 145, 33, 5 ] ) ;
		union() {
			translate( [ 5, 5, -1 ] ) cube( [ 135, 23, 7 ] ) ;
			translate( [ 130, 6, 2 ] ) cube( [40, 21, 10 ] ) ;
		}
	}
	for ( x = [ 10, 75, 138 ] )
		for ( y = [ 8 , 25 ] ) 
			translate( [ x, y, 0 ] )
			difference() {
				cylinder( 5, d = 7, $fn = 50 ) ;
				translate( [ 0, 0, -1 ] )
					cylinder( 7, d = 3.2, $fn = 100 ) ;
			}

}

module cover()
{
	difference() {
		union() {
		difference() {
			translate( [ 1, 1, -2 ] )
			minkowski() {
				cube( [ 201, 36, 46 ] ) ;
				sphere( d=2, $fn=50 ) ;
			}
			// inner cavity
			translate( [ 3, 3, -3, ] )
			minkowski() {
				cube( [ 197, 32, 45 ] ) ;
				sphere( d=2, $fn=50 ) ;
			}
		}
			// screw head holders
			for ( x = [ 10, 75, 138 ] )
				for ( y = [ 8 , 25 ] ) 
					translate( [ x+35, y+1, 38] )
					union() {
						cylinder( 7, d = 10, $fn = 100 ) ;
					}
		}
		union() {
			// removes lower bevel
			translate( [ -5, -5, -5 ] ) cube( [ 250, 100, 5 ] ) ;
			// vertical angle
			translate( [ -5, 32, -1 ] ) cube( [ 220, 20, 36 ] ) ;
			// front lock
			translate( [ -1, -1, -1 ] ) cube( [ 36, 40, 27 ] ) ;
			translate( [ -1, 14, -1 ] ) cube( [ 6, 8, 31 ] ) ;
			// front base space
			translate( [ 34.5, 1, -1 ] ) cube( [ 147, 2, 6.5 ] ) ;
			// back hinge
			translate( [ 200, 8, -1 ] ) cube( [ 20, 100, 43 ] ) ;
			// screw holes
			for ( x = [ 10, 75, 138 ] )
				for ( y = [ 8 , 25 ] ) 
					translate( [ x+35, y+1, 0 ] )
					union() {
						cylinder( 50, d=3.2, $fn=100) ;
						translate( [ 0, 0, 41 ] )
						cylinder( 50, d=7, $fn=100) ;
					}
		}
	}
}


translate( [ 35, 1, 0 ] ) cover_base() ;
translate( [ 0, 0, 45 ] ) rotate( [ 180, 0, 0 ] ) cover() ;
