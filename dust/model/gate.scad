
include <BOSL2/std.scad>
include <BOSL2/gears.scad>
use <plib/MG996R_servo.scad>


module gear1()
{
	spur_gear( pitch = 6, teeth=22, thickness=5, shaft_diam=3.3,
		anchor = BOTTOM ) ;
}

module gears2_3()
{
	spur_gear( pitch = 6, teeth=20, thickness=10, shaft_diam=5.3,
		anchor = BOTTOM ) ;
	color("blue")translate( [ 0, 0, 9.9 ] )
	spur_gear( pitch = 6, teeth=10, thickness=6.1, shaft_diam=5.3,
		anchor = BOTTOM ) ;
}


module slide_wedge( len, pad, height = 4 )
{
	// make as a polyhedron with 6 sides
	w_up = 10 ;
	w_down = 12 ;
	w_delta = (w_down-w_up)/2 ;
	vertices = [ 
		[0, w_down+pad, 0], [0, -pad, 0], 
		[0, w_up+w_delta+pad, height+pad], [0, w_delta-pad, height+pad],
		[len, w_down+pad, 0], [len, -pad, 0], 
		[len, w_up+w_delta+pad, height+pad], [len, w_delta-pad, height+pad]
		     ] ;
	faces = [ [1,0,2,3], [4,5,7,6],
		  [0,1,5,4], [2,0,4,6], [6,7,3,2], [1,3,7,5] ] ;
	polyhedron( vertices, faces ) ;
}

module slide_base( len ) {
	pad = 0.5 ;
	difference() {
		cube( [ len, 20, 6 ] ) ;
		translate( [ -0.5, 4-pad, 2.1 ] )
		slide_wedge( len+1, pad ) ;
	}
}

module rack_assembly()
{
	translate( [ 0, 4, 2.4 ] ) 
	rack( pitch=6, teeth=22, thickness=10, height=7, anchor=BOTTOM ) ;
	translate( [ -66, -2, 0 ] ) slide_wedge( 132, 0, height=4 ) ;
	translate( [ 66, -37, 0 ] ) 
	difference() {
		cube( [ 15, 47, 3 ] ) ;
		union() {
			translate( [7.5, 8, -1] ) cylinder( 10, d=3.2, $fn=50 );
			translate( [7.5,27, -1] ) cylinder( 10, d=3.2, $fn=50 );
		}
	}
}

module spacer_block( height = 18.5)
{
	difference() {
		union() {
			cube( [ 15, 35, height ] ) ;
		}
		union() {
			translate([7.5,-10,-1]) cylinder(height+2, d=3.6, $fn=50);
			translate([7.5, 8, -1]) cylinder(height+2, d=3.6, $fn=50);
			translate([7.5,27, -1]) cylinder(height+2, d=3.6, $fn=50);
		}
	}
}

module motor_frame()
{
	difference() {
		union() {
			translate( [ 5, 0, 0 ] )
			cube( [ 90, 5, 46 ] ) ;
			//translate( [ 10, -5, 0 ] ) cube( [80, 6, 3 ] ) ;
			translate( [ 20, -5, 0 ] ) cube( [5, 6, 46 ] ) ;
			translate( [ 90, -5, 0 ] ) cube( [5, 6, 46 ] ) ;
		}
		union() {
			// gears2_3 axle hole
			translate( [ 15, 30, 29.5 ] ) rotate( [ 90, 0, 0 ] )
				cylinder( 70, d=5, $fn=50 ) ;
			//servo cutout
			translate( [ 34.0, -18.0, 22.0 ] ) 
			cube( [ 42.0, 100, 21 ] ) ;
			// bottom cutout
			translate( [ 26, -10, 0.5 ] )
				cube( [ 100, 30, 7.5] ) ;
			// servo screw holes
				hull() {
				translate( [ 80.0, 30, 36.9 ] )
					rotate( [ 90, 0, 0 ] )
					cylinder( 70, d=5, $fn=50 ) ;
				translate( [ 81.0, 30, 36.9 ] )
					rotate( [ 90, 0, 0 ] )
					cylinder( 70, d=5, $fn=50 ) ;
				}
				hull() {
				translate( [ 80.0, 30, 27.9 ] )
					rotate( [ 90, 0, 0 ] )
					cylinder( 70, d=5, $fn=50 ) ;
				translate( [ 81.0, 30, 27.9 ] )
					rotate( [ 90, 0, 0 ] )
					cylinder( 70, d=5, $fn=50 ) ;
				}
				hull() {
				translate( [ 30.0, 30, 36.9 ] )
					rotate( [ 90, 0, 0 ] )
					cylinder( 70, d=5, $fn=50 ) ;
				translate( [ 31.0, 30, 36.9 ] )
					rotate( [ 90, 0, 0 ] )
					cylinder( 70, d=5, $fn=50 ) ;
				}
				hull() {
				translate( [ 30.0, 30, 27.9 ] )
					rotate( [ 90, 0, 0 ] )
					cylinder( 70, d=5, $fn=50 ) ;
				translate( [ 31.0, 30, 27.9 ] )
					rotate( [ 90, 0, 0 ] )
					cylinder( 70, d=5, $fn=50 ) ;
				}
		}

	}
	// servo cutout supports
	for ( x = [ 0: 1 : 7 ] )
		translate( [ 37 + 5 *x, 0, 22.5 ] ) cube( [ 1, 5, 20 ] ) ;
	// bottom cutout supports
	for ( x = [ 0: 1 : 11 ] )
		translate( [ 30.0 + 5 *x, 0, 1.0 ] ) cube( [ 1, 5, 6.5 ] ) ;
	translate( [ 90.0, -5, 1.0 ] ) cube( [ 1, 10, 6.5 ] ) ;
	translate( [ 94.0, -5, 1.0 ] ) cube( [ 1, 10, 6.5 ] ) ;

}

module axle_support()
{
	difference() {
		union() {
			cube( [ 20, 5, 40 ] ) ;
		}
		union() {
			// gears2_3 axle hole
			translate( [ 10, 30, 29.5 ] ) rotate( [ 90, 0, 0 ] )
				cylinder( 70, d=5, $fn=50 ) ;
		}

	}
}
module screw_cutout()
{
	cylinder( 3.5, d=4.0, $fn=50 ) ;
	translate( [ 0, 0, 3.1 ] )
	cylinder( 4, d1 = 4.0, d2 = 8.5, $fn =50 ) ;
}

module frame()
{
	translate( [ 79, -46, 0 ] )
	difference() {
		cube( [ 21, 47, 6 ] ) ;
		union() {
			translate( [ 9, 38, -1 ] ) screw_cutout() ;
			translate( [ 9, 8, -1 ] ) screw_cutout() ;
		}
	}
	translate( [ 74, -26.6, 0 ] ) motor_frame() ;
	slide_base( 100 );
	translate( [ 79, 20, 0 ] ) axle_support() ;
	translate( [ 10, 19 ] ) difference() {
		cube( [ 12, 12, 6 ] ) ;
		translate( [ 6, 6, -1 ] ) screw_cutout() ;
	}
}

module cover()
{
	difference() {
		cube( [ 103, 57, 53 ] ) ;
		union() {
			translate( [ 1.5, 1.5, 1.5 ] ) cube( [ 99, 54, 60 ] ) ;
			translate( [ -1, 12, 46 ] ) cube( [ 110, 15, 20 ] ) ;
			translate( [ 29, 49.2, 17 ] ) cube( [ 80, 15, 100 ] ) ;
			translate( [ 33, 50, 13.8 ] ) cube( [ 7, 15, 100 ] ) ;
			translate( [ 80, 49, 13.8 ] ) cube( [ 70, 6, 100 ] ) ;
			translate( [ 29, 100, 30.5 ] ) rotate( [ 90, 0, 0 ] )
				cylinder( 170, d=5, $fn=50);
			translate( [ -5, 62, -1 ] ) 
				cylinder( 100, d=45, $fn=50 ) ;
		}
	}
	translate( [ -5, 62, 0 ] ) difference() {
		cylinder( 53, d=45, $fn=50 ) ;
		union() { 
			translate( [ 0, 0, -1 ] ) 
				cylinder( 100, d=42, $fn=50 ) ;
			translate( [ 0, -5, -1 ] ) cube( [ 30, 30, 100 ] ) ;
			translate( [ -30, -30, -1 ] ) cube( [ 35, 90, 100 ] ) ;
		}
	}
	//translate( [ 0, 42, 55 ] ) cube( [ 15, 15, 10 ] ) ;
}


// if "true", makes complete gate. Otherwise, selected module or modules for
// STL generation

if ( false ) {
frame() ;
color( "teal" )
translate( [ 60, 28, 60 ] ) rotate( [ 180, 0, 0 ] ) cover() ;
color("red") 
translate( [ 35, 5.5, 2.1 ] ) 
	rack_assembly() ;
translate( [ 89, 14.5, 29.5 ] )
rotate( [ 90, 0, 0 ] )
rotate( [ 0, 0, 4 ] ) 
gears2_3() ;
color("red")
translate( [ 120.8, 3.5, 32.5 ] )
rotate( [ 90, 0, 0 ] )
rotate( [ 0, 0, 14 ] )
gear1() ;
translate( [ 89, 30, 29.5 ] ) rotate( [ 90, 0, 0 ] ) cylinder( 70, d=5, $fn=50);
color( "green" )
translate( [ 109.5, -48.0, 42.5 ] ) rotate( [ -90, 0, 0 ] ) MG996R( true ) ;
translate( [ 101.0, -31.55, -16.5 ] ) spacer_block() ;
} else {
spacer_block() ;
//spacer_block( height = 3) ;
//cover() ;
}
