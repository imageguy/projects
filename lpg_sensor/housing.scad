// Housing for the handheld LPG sensor. Includes mockups of all the
// contents, but note this is just for rough fit.

// Set do_build=1 to build and set the relevant build_* to 1. Only one
// component should be built at the time, since the locations may overlap.

// By Nenad Rijavec

// Feel free to use, modify and share as you see fit.

use <components/9v_housing.scad>
use <components/slide_cover.scad>

module nut_m3( thickness=2.8 ) 
{
	linear_extrude( thickness )
	circle( 3.6, $fn=6  ) ;
}

housing_x = 135 ;
housing_y = 55 ;
housing_z = 40 ;
h_bottom_z = 42 ;
offset_x = -13 ;
offset_y = -4 ;
offset_z = -4 ;
h_top_z = housing_z - h_bottom_z ;
h_minkowski_r = 1.5 ;
wall_t = 2 ;
pcb_base_t = 8.0 ;

do_build = 1 ;
build_top = 1 ;
build_bottom = 0 ;
build_strut = 0 ;
build_door = 0 ;

// 1.8" TFT Display mockup

module mockup_display()
{
	// mounting PCB
	difference() {
		translate( [ 1.5, 1.5, 0.1 ] )
		minkowski() {
			cube( [ 54, 33, 1 ] ) ;
			cylinder( 0.1, d = 3, $fn = 50 ) ;
		}
		union() {
			translate( [ 1.5, 1.5, -1 ] )
				cylinder( 5, d=2, $fn=30 ) ;
			translate( [ 55.5, 1.5, -1 ] )
				cylinder( 5, d=2, $fn=30 ) ;
			translate( [ 1.5, 34.5, -1 ] )
				cylinder( 5, d=2, $fn=30 ) ;
			translate( [ 55.5, 34.5, -1 ] )
				cylinder( 5, d=2, $fn=30 ) ;
		}
	}
	// screen
	translate( [ 5.5, 0, 1.2 ] )
		cube( [ 46, 36, 3 ] ) ;
}

// Arduino Nano mockup, bare board only (there is space in the housing to fit
// in Nano with headers)

module mockup_controller()
{
	// Nano board, plugged in, little extra on top for components
	translate( [ 0, 0, 0 ] )
		cube( [ 18, 45, 3 ] ) ;
	// Nano USB socket
	translate( [ 5, 37, 3.1 ] )
		cube( [ 8, 10, 4 ] ) ;
}

// switch mockup

module mockup_switch()
{
	// body and space for the actual switch on top
	cube( [ 20, 13, 16.0 ] ) ;
	// mounting lip
	translate( [ -1, -1, 10 ] )
		cube( [ 22, 15, 2 ] ) ;
}

// whole housing, top and bottom are via intersection
module housing()
{
	r = 4 ;
	r_inner = r - wall_t ;
	wx = housing_x - 2 * r ;
	wy = housing_y - 2 * r ;
	wz = housing_z - 2 * r ;
	base_y = wall_t + box_w()+2*wall()+2.5 ;
	y_pos = [ base_y-0.3, 
		base_y-2.2,
		base_y-8.0,
		base_y-13.8,
		base_y-21.0,
		base_y-25.0,
		base_y-29.1,
		base_y-31.6 ] ;

	difference() {
		translate( [ r, r, r ] ) 
		minkowski() {
			cube( [ wx, wy, wz ] ) ;
			sphere( r = r, $fn=30 ) ;
		}
		union() {
			// cavity for hollow box
			translate( [ wall_t+r_inner, 
				wall_t+r_inner, wall_t+r_inner ] )
			minkowski() {
				cube( [ wx, wy, wz ] ) ;
				sphere( r = r_inner, $fn=30 ) ;
			}
			// battery opening
			translate( [ housing_x-wall_t-1.0, -1, 5 ] )
				cube( [ 20, box_w()+10.5, box_h()+6 ] ) ;
			translate( [ housing_x-wall_t-5, r+wall_t, 5 ] )
				cube( [ 20, box_w()-3, box_h()+6 ] ) ;
			// LED opening
			translate( [ 48, 8.5, housing_z-wall_t-2 ] )
				cube( [ 47, 37, 20 ] ) ;
			// switch opening
			translate( [ housing_x-34.5,
				housing_y-18.5, housing_z-10] ) 
				cube( [ 21, 14, 200 ] ) ;
			// USB opening
			translate( [ 43.5, housing_y-wall_t-5, wall_t+6.5 ] )
				cube( [ 5, wall_t+10, 9 ] ) ;
			// sensor space ventilation
			for ( x=[10,22] ) for ( z=[8,19,30] ) {
			translate( [ x, -0.5, z ] )
				rotate( [ -90, 0, 0 ] )
				cylinder( housing_y+1, d=8, $fn=50 ) ;
			}
			for ( x=[10,22] ) for ( y=[10,22,34,46] ) {
			translate( [ x, y, -0.5 ] )
				cylinder( housing_z+1, d=8, $fn=50 ) ;
			}
			for ( y=[10,22,34,46] ) for ( z=[8,19,30]) {
			translate( [ -0.5, y, z ] )
				rotate( [ 0, 90, 0 ] )
				cylinder( housing_z+1, d=8, $fn=50 ) ;
			}
			// screw heads to connect halves
			translate( [ 33, 5+housing_y/2, -0.5 ] )
				cylinder( 4, d=6, $fn=50 ) ;
			translate( [ housing_x-7.0, housing_y-7.0, -0.5 ] )
				cylinder( 14, d=6, $fn=50 ) ;
		}
	}
	// Nano frame
	translate( [ 44.5, 0, wall_t-0.1 ] ) {
		translate( [ 0, wall_t-0.1, 0 ] )
			cube( [ 11, housing_y-2*wall_t+0.2, 2 ] ) ;
		translate( [ 0, wall_t-0.1, 0 ] )
			cube( [ 11, 7, 7 ] ) ;
		translate( [ 0, wall_t-0.1, 0 ] )
			cube( [ 4, 9, 7 ] ) ;
		translate( [ 7, wall_t-0.1, 0 ] )
			cube( [ 4, 9, 7 ] ) ;
		translate( [ 0, housing_y-wall_t-2, 0 ] )
			cube( [ 4, 2.1, 5 ] ) ;
		translate( [ 7, housing_y-wall_t-2, 0 ] )
			cube( [ 4, 2.1, 8 ] ) ;
	}
	// battery holder and supports
	translate( [ housing_x-box_l()-6,
			box_w()+wall_t+8, 7 ] )
		rotate( [ 90, 0, 0 ] )
		9v_slide_box() ;
	// vertical struts
	translate( [ housing_x-r-2, wall_t, wall_t ] )
		cube( [ 3, 4, housing_z-2*wall_t+0.2 ] ) ;
	translate( [ housing_x-box_l()+5, wall_t, wall_t-0.1 ] ) 
		cube( [ 3, 4, housing_z-2*wall_t+0.2 ] ) ;
	// horizontal around the entrance
	translate( [ housing_x-r, wall_t+3, wall_t] )
		cube( [ 3, box_w()+3, 3 ] ) ;
	translate( [ housing_x-r, wall_t+3, wall_t+box_h()+8] )
		cube( [ 2, box_w()+3, 9 ] ) ;
	// supports below and above the box
	// some supports above are removable for display and switch
	difference() {
		union() {
			translate( [ housing_x-wall_t-box_l()-2, 0, 0 ] ) 
			for ( y = y_pos ) {
				translate( [ 0, y, wall_t-0.1] )
					cube( [ box_l()-3, 1, 6.0 ] ) ;
				translate( [ 0, y, 27.2] )
					cube( [ box_l()-3, 1, housing_z-27.2]);
			}
		}
		union() {
			translate( [ 94, 8.5, housing_z-wall_t-0.3 ] )
				cube( [ 5, 37, 0.3 ] ) ;
			translate( [ 94, 8.5, housing_z-wall_t-0.3 ] )
				cube( [ 1, 37, 3 ] ) ;
			translate( [ 104, 8.5, 27 ] )
				cube( [ 1, 37, 11 ] ) ;
			translate( [ housing_x-34, housing_y-24,
					housing_z-wall_t-0.3 ] )
				cube( [ 5, 12, 0.3 ] ) ;
			translate( [ housing_x-wall_t-3, housing_y-24, 29 ] )
				cube( [ 1, 15, 30 ] ) ;
		}
	}
	// final support below for the wire outlet
	translate( [ housing_x+0.7-wall_t-box_l(), 
		wall_t+box_w()-10.8, wall_t-0.1] )
		cube( [ 7, 1, 6 ] ) ;
	// sensor wall and supports
	translate( [ 30, wall_t-0.1, wall_t-0.1 ] )
	difference() {
		union() {
			cube( [ wall_t, housing_y-2*wall_t+0.2,
					housing_z-2*wall_t+0.2 ] ) ;
			translate( [ wall_t-0.1, 0, 0 ] )
			cube( [ 3.1, 30, 33 ] ) ;
			// screw hole padding
			translate( [ 3, 5+housing_y/2-wall_t-0.1, 0 ] )
				cylinder( housing_z-2*wall_t, d=8, $fn=50 ) ;
		}
		union() {
			// mq6 base plate holder
			translate( [ -2, 15, housing_z/2-wall_t-0.1 ] )
			rotate( [ 0, 90, 0 ] )
			cylinder( 10, d=12, $fn=50 ) ;
			translate( [ wall_t-1, 15, housing_z/2-wall_t-0.1 ] )
			rotate( [ 0, 90, 0 ] )
			cylinder( 3, d=19, $fn=50 ) ;
			translate( [ 2, 13, housing_z/2-wall_t-9.5 ] )
				cube( [ 20, 4, 20 ] ) ;
			translate( [ -2, 40, (housing_z/2)-wall_t-0.1-6 ] )
				cube( [ 8, 3, 12 ] ) ;
			// screw hole
			translate( [ 3, 5+housing_y/2-wall_t-0.1, -3.5 ] )
				cylinder( housing_z-5, d=3, $fn=50 ) ;
			translate( [ 3, 5+housing_y/2-wall_t-0.1, -0.1 ] )
				cylinder( 2, d=6, $fn=50 ) ;
		}
	}
	// final connecting screw
	translate( [ housing_x-7.0, housing_y-7.0, 0 ] )
	difference() {
		union() {
			translate( [ 0, 0, wall_t-0.1 ] )
			cylinder( housing_z-wall_t-1, d=8, $fn=50 ) ;
			translate( [ 0, 0, wall_t-0.1 ] )
			cylinder( 5, d=8, $fn=50 ) ;

		}
		union() {
			translate( [ 0, 0, -0.1 ] )
			cylinder( housing_z-5, d=3, $fn=50 ) ;
			translate( [ 0, 0, -0.1 ] )
			cylinder( 4, d=6, $fn=50 ) ;
		}
	}

}

// bottom half has alignment tabs

module housing_bottom()
{
	intersection() {
		housing() ;
		translate( [ -0.1, -0.1, -0.1 ] )
			cube( [ housing_x+1, housing_y+1, housing_z/2+0.1 ]) ;
	}
	// alignment tabs
	translate( [ 37, wall_t-0.1, wall_t ] ) 
		cube( [ 10, 2, housing_z/2 ] ) ;
	translate( [ housing_x/2, housing_y-wall_t-2, wall_t ] ) 
		cube( [ 10, 2.1, housing_z/2 ] ) ;
}

// top half should be flipped for building

module housing_top()
{
	translate( [ 0, 0, -housing_z/2 ] ) 
	intersection() {
		housing() ;
		translate( [ -0.1, -0.1, housing_z/2 ] )
			cube( [ housing_x+1, housing_y+1, housing_z/2+0.1 ]) ;
	}
}

// slide_cover, extended a bit to fit well in the opening

module slide_cover_modified()
{
	slide_cover( 17, 30.5, 3 ) ;
	translate( [ -1, 0, 4 ] ) 
	rotate( [ 0, 90, 0 ] )
	intersection() {
		translate( [ 4, 0, 0 ] )
		cylinder( 25, d=6, $fn=50 ) ;
		translate( [ 0, -5, -1 ] )
			cube( [ 4, 5, 50 ] ) ;
	}
}

// it jams into walls and bends to hold the display
module display_strut()
{
	cube( [ 10, housing_y - 2 * wall_t, 2 ] ) ;
}


if ( !do_build ) {
	intersection() {
		housing() ;
		translate( [ -2, -2, -2 ] ) 
			// change dimensions to see a cutout
			cube( [ housing_x+15, housing_y+10, housing_z+15 ] ) ;
	}
	color( "green" ) 
	translate( [100, 45, housing_z-3 ] ) rotate( [0, 0, 180] )
		mockup_display() ;
	color("red")
	translate( [ 51, 7.5, wall_t+2 ] )
		rotate( [ 0, -90, 0 ] )
		mockup_controller() ;
	color( "blue" )
	translate( [ housing_x-32, housing_y-17.5, housing_z-10] )
		mockup_switch() ;
	color( "red" )
	translate( [ housing_x-3, 3.3, 28.5 ] )
	rotate( [ 0, 90, 0 ] )
	slide_cover_modified() ;
	color( "red" )
	translate( [ housing_x-49.0, 7.5, 8.5 ] ) 
	cube( [ 45.0, 27, 17 ] ) ;

} else {
	if ( build_top )
		translate( [ 0, 0, housing_z/2 ] )
		rotate( [ 180, 0, 0 ] )
		housing_top() ;
	else if ( build_bottom ) 
		housing_bottom() ;
	else if ( build_strut ) 
		display_strut() ;
	else if ( build_door ) 
		slide_cover_modified() ;
}
