// birdhouse internal camera mount

// default diam is undersized so M3 screw holds
// assumes that PCB is at [0,0]
// use larger diam and appropriate len to build posts
module mount_holes( diam=2.8, len=30 )
{
	translate( [ 3.6, 3.6, -1 ] )
	union() {
		cylinder( len, d=diam, $fn=50 ) ;
		translate( [ 28.8, 0, 0 ] ) cylinder( len, d=diam, $fn=50 ) ;
		translate( [ 0, 28.8, 0 ] ) cylinder( len, d=diam, $fn=50 ) ;
		translate( [ 28.8, 28.8, 0 ] ) cylinder( len, d=diam, $fn=50 ) ;
	}
}

// corner screws to hold the box together
// default diam is undersized so M3 screw holds
// use larger diam and appropriate len to build posts
module corner_holes( diam=2.8, len=30 )
{
	translate( [ 3.6, 3.6, -1 ] )
	union() {
		cylinder( len, d=diam, $fn=50 ) ;
		translate( [ 58.8, 0, 0 ] ) cylinder( len, d=diam, $fn=50 ) ;
		translate( [ 0, 34.8, 0 ] ) cylinder( len, d=diam, $fn=50 ) ;
		translate( [ 58.8, 34.8, 0 ] ) cylinder( len, d=diam, $fn=50 ) ;
	}
}

module half_box( wall_h = 6, screw_diam = 2.8 )
{
	difference() {
		union() {
			// base and outer walls
			cube( [ 66, 42, 1 ] ) ;
			cube( [ 66, 2, wall_h ] ) ;
			cube( [ 2, 42, wall_h ] ) ;
			translate( [ 0, 40, 0 ] ) cube( [ 66, 2, wall_h ] ) ;
			translate( [ 65, 0, 0 ] ) cube( [ 2, 42, wall_h ] ) ;

			// camera rest frame
			translate( [ 0, 0, 1 ] ) corner_holes( diam=7.2,
				len=wall_h ) ;
			translate( [ 15, 3, 1 ] ) mount_holes( diam=7.2, len=4 ) ;
			translate( [ 15, 6.6, 0 ] ) cube( [6.5, 28.8, 2] ) ;
			translate( [ 15.7+28.8, 6.6, 0] ) cube([ 6.5, 28.8, 2 ]) ;
			translate( [ 18.6, 3, 0 ] ) cube( [28.8, 6.5, 2] ) ;
			translate( [ 18.6, 3.7+28.8, 0 ] ) cube([28.8, 6.5, 2] ) ;
		}
		union() {
			// camera screw mount_holes 
			translate( [ 0, 0, -1 ] )
				corner_holes( diam=screw_diam ) ;
			translate( [ 15, 3, -1 ] )
				mount_holes( diam=screw_diam ) ;
			// cable notch through outer wall
			translate( [ -1, 11, wall_h-2 ] ) cube( [ 10, 20, 3 ] ) ;
		}
	}
}

module camera_base()
{
	difference() {
		half_box( wall_h = 6, screw_diam=2.8 ) ;
		// housing mount hole
		translate( [ 33, 21, -1 ] ) cylinder(d=3.5,h=4,$fn=50) ;
	}
}

// cover, with led mount_holes
module camera_leds()
{
	difference() {
		half_box( wall_h = 4.2, screw_diam=3.5 ) ;
		union() {
			// lens hole
			translate( [ 33, 21, -1 ] )
				cylinder( 10, d=18, $fn=50 ) ;
			// led mount_holes 
			translate( [12, 10.5, -1] ) cylinder( 4, d=3, $fn=50 ) ;
			translate( [12, 21, -1] ) cylinder( 4, d=3, $fn=50 ) ;
			translate( [12, 31.5, -1] ) cylinder( 4, d=3, $fn=50 ) ;
			translate( [6, 14, -1] ) cylinder( 4, d=3, $fn=50 ) ;
			translate( [6, 28, -1] ) cylinder( 4, d=3, $fn=50 ) ;
			translate( [54, 10.5, -1] ) cylinder( 4, d=3, $fn=50 ) ;
			translate( [54, 21, -1] ) cylinder( 4, d=3, $fn=50 ) ;
			translate( [54, 31.5, -1] ) cylinder( 4, d=3, $fn=50 ) ;
			translate( [60, 14, -1] ) cylinder( 4, d=3, $fn=50 ) ;
			translate( [60, 28, -1] ) cylinder( 4, d=3, $fn=50 ) ;
		}
	}
}

if ( true ) {
//camera_base() ;
camera_leds() ;
} else 
//intersection() { union() 
{
camera_base() ;
translate( [ 0, 42, 10.2 ] ) 
rotate( [ 180, 0, 0 ] ) camera_leds() ;
} 
//translate( [ -5, -5, -1 ] ) cube ( [ 38, 50, 10 ] ) ; 
//}
