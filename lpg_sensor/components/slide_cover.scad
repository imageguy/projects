// Cover assmebly for a battery or similar compartment. slide_cover_base
// should be included in the object, since it provides the rails to accpet
// the cover. slide_cover makes the actual lid.

// Part of the components library.

// By Nenad Rijavec
// Feel free to use, share, modify, etc. as you see fit.

do_build = 1 ;
bld_base = 0 ;
bld_cover = 1 ;
cover_w = 19 ;
cover_l = 27 ;
lid_h = 3 ;
tab_w = 6 ;

module slide_cover( cover_w, cover_l, lid_h  )
{
	// sliding plate
	hull() union() {
		cube( [ cover_w+4, cover_l, 0.1 ] ) ;
		translate( [ 2, 0, lid_h-0.1 ] ) 
		cube( [ cover_w, cover_l, 0.1 ] ) ;
	}
	// lock tab
	translate( [ 2+(cover_w-tab_w)/2, cover_l-1, 0 ] )
		cube( [ tab_w, 4, 1 ] ) ;
	// friction area for opening
	f_w = cover_w - 4 ;
	f_l = cover_l > 10 ? cover_l-5 : cover_l-2 ;

	pts = concat( 
		[ [0,0] ],
		[ for (i=[0:1:f_l])
			[for (j=[0:1])
				j==0 ? 
					(i+i%2) : (i%2==0?0.1:2)
		] ],
		[ [f_l,0]]
	) ;
	translate( [ 4, 0, lid_h-0.3 ] )
	rotate( [ 90, 0, 90 ] )
	linear_extrude( f_w ) polygon( pts ) ;


}

module slide_cover_base( cover_w, cover_l, lid_h )
{
	frame_w = 4 ;
	difference() {
		cube( [ cover_w + 2*frame_w, cover_l+2*frame_w, lid_h+1 ] ) ; 
		union() {
			translate( [ frame_w, frame_w-1, -1 ] )
				cube( [ cover_w, cover_l+2, lid_h+4 ] ) ;
			translate( [ frame_w-2.5, -1, 1 ] )
			slide_cover( cover_w+1, cover_l+frame_w+2, lid_h + 0.8 ) ;
			translate( [ frame_w+(cover_w-(tab_w+2))/2,
			frame_w+cover_l-1, 0.5 ] )
				cube( [ tab_w+2, 4.5, 2.1 ] ) ;
		}
	}
	
}

if ( do_build ) {
	if ( bld_cover )
		slide_cover( cover_w-1, cover_l+3.5, lid_h ) ;
	if ( bld_base ) 
		translate( [ bld_cover * (cover_w+5 ), 0, 0 ] )
			slide_cover_base( cover_w, cover_l, lid_h ) ;
			
} else {
	color("red")translate( [ 2.0, -0.1, 1.1 ] )
		slide_cover( cover_w, cover_l+3.5, lid_h ) ;
	slide_cover_base( cover_w, cover_l, lid_h ) ;
}
