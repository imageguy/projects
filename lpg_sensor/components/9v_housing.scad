// Battery container to for 9V battery. Contains two versions.
// 9v_box_outline is basic and, if used, the opening and lid must be
// constructed separately (see slide_cover.scad for a lid that was used in
// the tachometer). 9v_box contains a pop-up lid and so is a bit more
// complete.

// 9v_slide_box() builds a box with a holder for the slide cover. There are
// no support plates, meaning that the cover holder protrudes on three
// sides. Builds so the cover is inserted from above.

// The battery space is designed so that the battery fits in only one way.

// Part of the components library.

// By Nenad Rijavec
// Feel free to use, share, modify, etc. as you see fit.

use <slide_cover.scad>

do_build = 0 ;
do_outline = 0 ;
do_slide_box = 1 ;
// constants as functions since modules are called externally
function 9v_w() = 27 ;
function 9v_l() = 45 ; 
function 9v_h() = 17 ;
function c_off() = 6 ;
function c_plus() = 8 ;
function c_minus() = 11 ;
function c_l() = 6 ;
function wall() = 2 ;
// inner box dimensions, we allow 1mm space extra in each dimension
function box_w() = 9v_w() + 1 ;
function box_l() = 9v_l() + c_l() + 1.0 ;
function box_h() = 9v_h() + 1 ; 


module 9v_box_outline_base()
{
	union() {
		translate( [ 0, c_l(), 0 ] )
			cube([ box_w(), box_l()-c_l(), box_h() ]);
		translate( [ c_off()-c_plus()/2, wall(), 0 ] )
			cube( [ c_plus(), c_l()+1, box_h() ] ) ;
		translate( [ 9v_w()-c_off()-c_minus()/2, wall(), 0 ] )
			cube( [ c_minus(), c_l()+1, box_h() ] ) ;
	//	translate( [ wall(), wall()+c_l(), 0 ] )
	//		cube([ box_w()-2*wall(), box_l()-2*wall()-c_l(), box_h() ]);
	//	translate( [ wall()+c_off()-c_plus()/2, wall(), 0 ] )
	//		cube( [ c_plus(), c_l()+1, box_h() ] ) ;
	//	translate( [ wall()+9v_w()-c_off()-c_minus()/2, wall(), 0 ] )
	//		cube( [ c_minus(), c_l()+1, box_h() ] ) ;

	}
}

module 9v_box_outline() 
{
	facx=(box_w()+2*wall())/box_w() ;
	facy=(box_l()+2*wall())/box_l() ;
	facz=(box_h()+2)/box_h() ;
	difference() {
		union() {
			scale( [ facx, facy, facz ] ) 
			9v_box_outline_base( ) ;
			translate( [ c_off()+5.5, wall()+0.18, 0 ] )
				cube( [ 6.2+wall(), 7.82+wall(), box_h() ] ) ;
		}
		union() {
			translate( [ wall(), wall(), 1 ] ) //scale( [ 1, 1, 2 ] )
				9v_box_outline_base( ) ;
			// modify the contact holes
			translate( [ c_off()+5.5, wall()+2, 1 ] )
				cube( [ 1, 8, box_h()+1 ] ) ;
			translate( [ c_off()+10.6, wall()+2, 1 ] )
				cube( [ 1, 8, box_h()+1 ] ) ;
			// wire passthroughs
			translate( [ c_off()+2, wall()+5, wall()+5 ] )
			rotate( [ 90, 0, 0 ] )
				cylinder( 20, d=4, $fn=100 ) ;
			translate( [ 9v_w()-c_off()+2, wall()+5, wall()+5 ] )
			rotate( [ 90, 0, 0 ] )
				cylinder( 20, d=4, $fn=100 ) ;
		}
	}
}

module 9v_box( horiz_latch = 1 )
{
	difference() {
		union() {
			cube( [ box_w(), box_l(), box_h() ] ) ;
		}
		union() {
			translate( [ wall(), wall()+c_l(), wall() ] )
				cube([ box_w()-2*wall(),
				box_l()-2*wall()-c_l(), box_h() ]);
			translate( [ wall()+c_off()-c_plus()/2, wall(), wall() ] )
				cube( [ c_plus(), c_l()+1, box_h() ] ) ;
			translate( [ wall()+9v_w()-c_off()-c_minus()/2, wall(), wall() ] )
				cube( [ c_minus(), c_l()+1, box_h() ] ) ;
			translate( [ wall()+c_off(), wall()+5, wall()+2 ] )
			rotate( [ 90, 0, 0 ] )
			cylinder( 20, d=4, $fn=100 ) ;
			translate( [ wall()+9v_w()-c_off(), wall()+5, wall()+2 ] )
			rotate( [ 90, 0, 0 ] )
			cylinder( 20, d=4, $fn=100 ) ;
			if ( horiz_latch ) {
				translate( [ 1, 1, box_h()-2 ] )
					cube( [ box_w()-2, box_l()-2, 10 ] ) ;
				translate( [ box_w()/2-4.1, 1, box_h()-10 ] )
					cube( [ 8.1, 15, 15 ] ) ;
				translate( [ wall()+4.0, box_l()-4, box_h()-3 ] )
					cube( [ 6, 5, 2 ] ) ;
				translate( [ box_w()-wall()-10.0, box_l()-4, box_h()-3 ] )
					cube( [ 6, 5, 2 ] ) ;
				translate( [ box_w()/2-4, -1, box_h()-4 ] )
				cube( [ 8, 4, 2 ] ) ;
			}

		}
	}
}

module 9v_cover() {
	difference() {
		union() {
			cube( [ box_w()-3.5, box_l()-3.5, 2 ] ) ;
		}
		union() {
			translate( [ box_w()/2-6.75, -0.5, -1 ] )
				cube( [ 10, 5.0, 4 ] ) ;
		}
	}
	translate( [ box_w()/2-4.75, 1.5, 3.0 ] )
	rotate( [ 0, 90, 0 ] )
		cylinder( 6, d=2, $fn=100 ) ;
	translate( [ box_w()/2-4.75, 4.5, 0 ] )
			cube( [ 6, 1.5, 8.5 ] ) ;
	translate( [ box_w()/2-4.75, 1, 0 ] )
			cube( [ 6, 1.5, 8.5 ] ) ;
	translate( [ box_w()/2-4.75, 1, 7.5 ] )
			cube( [ 6, 4, 1 ] ) ;
	translate( [ wall()+2.75, box_l()-4, 2 ] )
	rotate( [ 0, 90, 0 ] )
	linear_extrude( height = 5 ) polygon( 
		points = [ [ 0,0], [2,0], [0,3 ] ] ) ;
	translate( [ box_w()-wall()-11.25, box_l()-4, 2 ] )
	rotate( [ 0, 90, 0 ] )
	linear_extrude( height = 5 ) polygon( 
		points = [ [ 0,0], [2,0], [0,3] ] ) ;
}

module 9v_slide_box( )
{
	// 9v holder 
	difference() {
		//translate( [ 0, 2+box_h(), 2+box_w() ] )
		translate( [ 0, 2+box_h(), 1+2*wall()+box_w() ] )
		rotate( [ 90, 0, 0 ] )
		rotate( [ 0, 0, -90 ] )
			9v_box_outline() ;
		// 9v cover knockout
		//translate( [ box_l()-1, 1, 3 ] )
		//cube( [ 20, box_h(), box_w()-6 ] ) ;
		translate( [ box_l()+1.9, wall()-1, wall()+1 ] )
		cube( [ 20, box_h()+2*wall(), box_w() ] ) ;
		translate( [ box_l()+2.0, -1, 0 ] )
		cube( [ 20, box_h()+2*wall(), box_w()+3*wall() ] ) ;
	}
	// 9v cover frame
	//translate( [ box_l()-2.5, 24.5, 34 ] ) 
	//translate( [ box_l()+wall(), 24.5, 34 ] ) 
	//rotate( [ -90, 0, 0 ] )
	//rotate( [ 0, 90, 0 ] )
		//slide_cover_base( 19, 26, 3 ) ;
	translate( [ box_l()+2, 23.0, 34 ] ) 
	rotate( [ -90, 0, 0 ] )
	rotate( [ 0, 90, 0 ] )
		slide_cover_base( 18, 26, 3 ) ;
}


if ( do_build ) {
	if ( do_slide_box ) {
		9v_slide_box() ;
	} else {
		9v_box() ;
		translate( [ box_w()+10, 0, 0 ] ) 9v_cover() ;
	}
} else if ( do_outline ) {
	//color( "red")
	//translate( [ 20, 20, 1 ] ) cube( [ 5, 1, 9v_h+3 ] ) ;
	//intersection() { cube( [ 100, 20, 100 ] ) ;
	9v_box_outline() ;
	//}
} else if ( do_slide_box ) {
	intersection() {
		union() {
			rotate( [ 90, 0, 0 ] )
			9v_slide_box() ;
			color( "red") 
			translate( [ 8.5, -30.5, 1.5 ] )
				cube( [ 45, 27, 17 ] ) ;
		}
		translate( [ -10, -70, -10 ] )
			cube( [ 100, 150, 20 ] ) ;
	}
		color( "blue" )
		translate( [ 55.0, -34, 21.0 ] )
		rotate( [ 0, 90, 0 ] )
		slide_cover( 18.0, 30.5, 3 ) ;
} else { 
	9v_box() ;
	//translate( [ box_w()-1.8, 1.8, box_h()+0 ] )
		//rotate( [ 0, 180, 0 ] )
		//9v_cover() ;
	translate( [ box_l()+15, 0, box_h() ] )
	rotate( [ 0, -90, 0 ] ) 
	rotate( [ 0, 0, 90 ] ) 
		9v_slide_box() ;
}
