/*
   fence handle
   
   By Nenad Rijavec.
   You are welcome to use, share and modify this code as you see fit.
*/

module handle()
{
	difference() {
		hull() {
		translate( [ 8, 9.75, 0.1 ] ) scale( [ 3.5, 5, 1 ] ) 
			cylinder(  1, d=10, $fn=100 ) ;
		translate( [ 8, 9.75, 65.1 ] ) scale( [ 2.5, 5, 1 ] ) 
			sphere(  d=10, $fn=100 ) ;
		}
		union() {
			cube( [ 16, 19.5, 25 ] ) ;
			translate( [ 0, 6, 24 ] ) cube( [ 16, 7.5, 33 ] ) ;
			translate( [ 3.5, -3, 0 ] ) cube( [ 7, 24, 21 ] ) ;
			translate( [ -9, 9.75, 23 ] ) rotate( [ 0, 90, 0 ] )
				cylinder( 30, d=3.5, $fn=50 ) ;
			translate( [ -9, 9.75, 23 ] ) rotate( [ 0, 90, 0 ] )
				cylinder( 5, d=6.5, $fn=50 ) ;
			translate( [ 0, 13.4, 27.9 ] ) rotate( [ 0, 90, 0 ] )
				linear_extrude( height=16 ) 
					polygon( [ [0,0], [3,0], [3,6] ] ) ;
			translate( [ 16, 6.1, 27.9 ] ) rotate( [ 0, -90, 0 ] )
				linear_extrude( height=16 ) 
					polygon( [ [0,0], [-3,0], [-3,-6] ] ) ;
			translate( [ 16, 6, 56.9 ] ) rotate( [ 0, -90, 0 ] )
				linear_extrude( height=16 ) 
					polygon( [ [0,0], [0,7.5], [3,3.75] ] );
		}
	}

}


handle() ;
