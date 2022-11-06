// controller housing

use <plib/screw_threads.scad>
use <components/displays.scad> 
module mockup_pcb()
{
	cube( [ 70, 50, 2 ] ) ;
	translate( [ 0, 2.5, 1 ] ) cube( [ 20, 45, 8 ] ) ;
	translate( [ 28, 2.5, 1 ] ) cube( [ 20, 45, 20 ] ) ;
	translate( [ 55, 5.5, 1 ] ) cube( [ 10, 42, 20 ] ) ;
}
// switch mockup

module mockup_switch()
{
	// body and space for the actual switch on top
	cube( [ 19, 12, 33.0 ] ) ;
	// mounting lip
	translate( [ -1, -1, 27 ] )
		cube( [ 21, 15, 2 ] ) ;
}

module slide_wedge( len, pad, height = 4 )
{
	// make as a polyhedron with 6 sides
	w_up = 0 ;
	w_down = 4 ;
	w_delta = (w_down-w_up)/2 ;
	vertices = [ 
		//[0, w_down+pad, 0], [0, -pad, 0], 
		//[0, w_up+w_delta+pad, height+pad], [0, w_delta-pad, height+pad],
		//[len, w_down+pad, 0], [len, -pad, 0], 
		//[len, w_up+w_delta+pad, height+pad], [len, w_delta-pad, height+pad]
		[0, w_down+pad, 0], [0, -pad, 0], 
		[0, w_up+w_delta+pad, height+pad], [0, -pad, height+pad],
		[len, w_down+pad, 0], [len, -pad, 0], 
		[len, w_up+w_delta+pad, height+pad], [len, -pad, height+pad]
		     ] ;
	faces = [ [1,0,2,3], [4,5,7,6],
		  [0,1,5,4], [2,0,4,6], [6,7,3,2], [1,3,7,5] ] ;
	polyhedron( vertices, faces ) ;
}

module housing_cover()
{
	difference() {
		minkowski() {
			cube( [ 130, 80, 35 ] ) ;
			sphere( d=10, $fn=50 ) ;
		}
		union() {
			translate( [ 2, 2, -3 ] ) minkowski() {
				cube( [ 126, 76, 36 ] ) ;
				sphere( d=8, $fn=50 ) ;
			}
			translate( [ -5, -5, -5 ] ) cube( [ 150, 150, 5 ] ) ;
			translate( [ 7.5, 12, 30 ] )
                                cube( [ 84, 56, 13] ) ;
			translate( [ 120.0, 30.12, 17 ] )
			rotate( [ 0, 0, 90 ] ) cube( [ 20, 13, 33.0 ] ) ;
			translate( [ 128, -5, -1 ] ) cube( [ 20, 150, 5.5 ] ) ;
			translate( [ 128, -1.5, -1 ] ) cube( [ 20, 83, 6.5 ] ) ;
			translate( [ 120,32.5, 0 ] ) cube( [ 20, 15, 5 ] ) ;
			translate( [ 120, 40, 5 ] )
			rotate( [ 0, 90, 0 ] ) cylinder( 20, d=15, $fn=50 ) ;
				
		}
	}
	translate( [ -2, -4.55, 0 ] ) slide_wedge( 133.0, 0.5 ) ;
	translate( [ 129, 84.55, 0 ] ) rotate( [ 0, 0, 180 ] ) slide_wedge( 128, 0.5 ) ;
	translate( [ 12, 72.5, 25 ] ) 
		screw_segment( 6, 1, 15, diam_adj=-0.5,
			lead_bottom=true, chamfer_bottom=true ) ;
	translate( [ 12, 8, 25 ] ) 
		screw_segment( 6, 1, 15, diam_adj=-0.5,
			lead_bottom=true, chamfer_bottom=true ) ;
	translate( [ 35, 72.5, 10 ] ) 
		screw_segment( 6, 1, 30, diam_adj=-0.5,
			lead_bottom=true, chamfer_bottom=true ) ;
	translate( [ 83, 72.5, 10 ] ) 
		screw_segment( 6, 1, 30, diam_adj=-0.5,
			lead_bottom=true, chamfer_bottom=true ) ;
	translate( [ 35, 8, 10 ] ) 
		screw_segment( 6, 1, 30, diam_adj=-0.5,
			lead_bottom=true, chamfer_bottom=true ) ;
	translate( [ 83, 8, 10] ) 
		screw_segment( 6, 1, 30, diam_adj=-0.5,
			lead_bottom=true, chamfer_bottom=true ) ;
	//color( "red") translate( [ 10, 68, 30 ] ) cube( [ 5, 17, 20 ] ) ;
}

module screw_cutout()
{
	cylinder( 3.5, d=4.0, $fn=50 ) ;
	translate( [ 0, 0, 3.1 ] )
	cylinder( 4, d1 = 4.0, d2 = 8.5, $fn =50 ) ;
}


module housing_base()
{
	difference() {
		minkowski() {
			cube( [ 130, 80, 8 ] ) ;
			sphere( d=10, $fn=50 ) ;
		}
		union() {
			translate( [ 3, 7.5, -3 ] ) minkowski() {
				cube( [ 125, 65, 36 ] ) ;
				sphere( d=8, $fn=50 ) ;
			}
			translate( [ -10, -10, -10 ] ) cube( [ 200, 110, 10 ]) ;
			translate( [ -10, -10, 5 ] ) cube( [ 200, 110, 10 ]) ;
			translate( [ 120, 40, 10 ] )
			rotate( [ 0, 90, 0 ] ) cylinder( 20, d=15, $fn=50 ) ;
		}
	}
	difference() {
	translate( [ 132, 2, 5 ] ) cube( [ 3, 77, 4.5 ] ) ;
			translate( [ 120, 40, 10 ] )
			rotate( [ 0, 90, 0 ] ) cylinder( 20, d=15, $fn=50 ) ;
	}
	translate( [ 135.0, 77.0, 9.5 ] ) 
		rotate( [0,180,0] ) slide_wedge( 136, 0.5 ) ;
	translate( [ -1, 3.0, 9.5 ] )
		rotate( [ 0, 180, 180 ] ) slide_wedge( 136, 0.5 ) ;

	translate( [ 20, 0, 0 ] )
	difference() {
		cube( [ 15, 80, 5 ] ) ;
		translate( [ 7.5, 40, -1 ] ) screw_cutout() ;
	}
	translate( [ 80, 0, 0 ] )
	difference() {
		cube( [ 15, 80, 5 ] ) ;
		translate( [ 7.5, 40, -1 ] ) screw_cutout() ;
	}
}

// wide strut, make 4
module strut()
{
	difference() {
		union() {
			cylinder( 3, d=10, $fn=50 ) ;
			translate( [0, 64.5, 0 ] ) cylinder( 3, d=10, $fn=50 ) ;
			translate( [ -5, 0, 0 ] ) cube( [ 10, 64.5, 3 ] ) ;
		}
		translate( [ 0, 0, -1 ] ) 
		union() {
			cylinder( 6, d=6.5, $fn=50 ) ;
			translate( [0, 64.5, 0 ] ) cylinder( 6, d=6.5, $fn=50 ) ;
		}
	}
}

// narrow strut, make 1
module strut2()
{
	difference() {
		union() {
			cylinder( 3, d=10, $fn=50 ) ;
			translate( [0, 64.5, 0 ] ) cylinder( 3, d=10, $fn=50 ) ;
			translate( [ -4, 0, 0 ] ) cube( [ 5, 64.5, 3 ] ) ;
		}
		translate( [ 0, 0, -1 ] ) 
		union() {
			cylinder( 6, d=6.5, $fn=50 ) ;
			translate( [0, 64.5, 0 ] ) cylinder( 6, d=6.5, $fn=50 ) ;
		}
	}
}


if ( false ) {
//intersection() {
union() {
color( "teal" )
translate( [ 1.5, 10, 35 ] ) ILI9488() ;
//translate( [ 30, 12, 10 ] ) mockup_pcb() ;
color("green")translate( [ 119.5, 28.62, 13 ] )
	rotate( [ 0, 0, 90 ] ) mockup_switch() ;

//translate( [ 0, -2, 0 ] ) housing_cover() ;
//color( "teal" )
//translate( [ 0, -2, -5 ] ) housing_base() ;
translate( [ 35, 6, 30 ] ) strut() ;
//translate( [ 10, 6, 24 ] ) hex_nut( 6, 1, diam_adj = 0.3 ) ;
}
//translate( [ -10, -10, -10 ] ) cube( [ 80, 150, 150 ] ) ;
//}
} else {

translate( [ 0, 0, 40 ] ) rotate( [ 180, 0, 0 ] )  housing_cover() ;
//housing_base() ;
//translate( [ 0, 0, 0 ] ) strut2() ;
//translate( [ 0, 0, 0 ] ) strut() ;
//translate( [ 13, 0, 0 ] ) strut() ;
//translate( [ 25, 0, 0 ] ) rotate( [ 0, 0, 30 ] ) hex_nut( 6, 1, diam_adj = 0.3 ) ;
//translate( [ 25, 12, 0 ] ) rotate( [ 0, 0, 30 ] ) hex_nut( 6, 1, diam_adj = 0.3 ) ;
//translate( [ 25, 24, 0 ] ) rotate( [ 0, 0, 30 ] ) hex_nut( 6, 1, diam_adj = 0.3 ) ;
//translate( [ 25, 36, 0 ] ) rotate( [ 0, 0, 30 ] ) hex_nut( 6, 1, diam_adj = 0.3 ) ;
}
