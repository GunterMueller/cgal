// ============================================================================
//
// Copyright (c) 1997-2000 The CGAL Consortium
//
// This software and related documentation is part of an INTERNAL release
// of the Computational Geometry Algorithms Library (CGAL). It is not
// intended for general use.
//
// ----------------------------------------------------------------------------
//
// release       : $CGAL_Revision$
// release_date  : $CGAL_Date$
//
// file          : include/CGAL/Extended_cartesian.h
// package       : Nef_2 
// chapter       : Nef Polyhedra
//
// source        : nef_2d/Simple_extended_kernel.lw
// revision      : $Revision$
// revision_date : $Date$
//
// author(s)     : Michael Seel <seel@mpi-sb.mpg.de>
// maintainer    : Michael Seel <seel@mpi-sb.mpg.de>
// coordinator   : Michael Seel <seel@mpi-sb.mpg.de>
//
// implementation: Extended cartesian kernel
// ============================================================================
 
#ifndef CGAL_EXTENDED_CARTESIAN_H
#define CGAL_EXTENDED_CARTESIAN_H

#include <CGAL/Cartesian.h>
#include <CGAL/Point_2.h> 
#include <CGAL/Line_2_Line_2_intersection.h>
#ifndef _MSC_VER
#include <CGAL/RPolynomial.h>
#else
#include <CGAL/RPolynomial_MSC.h>
#define RPolynomial RPolynomial_MSC
#endif
#undef _DEBUG
#define _DEBUG 51
#include <CGAL/Nef_2/debug.h>
#include <CGAL/Nef_2/Line_to_epoint.h>

CGAL_BEGIN_NAMESPACE

template <class T> class Extended_cartesian;

/*{\Xanpage {Extended_cartesian}{}{An extended geometric kernel model}{K}}*/

template <class pFT>
class Extended_cartesian : public 
  CGAL::Cartesian< CGAL::RPolynomial<pFT> > { public:
typedef CGAL::Cartesian< CGAL::RPolynomial<pFT> > Base;
typedef Extended_cartesian<pFT> Self;

/*{\Xdefinition |\Mname| is a kernel model realizing the concept
  extended geometry. }*/

/*{\Xtypes 6.5}*/
/*{\Xtext \headerline{Affine kernel and types}}*/

typedef CGAL::Cartesian<pFT> sKernel;
/*{\Xtypemember the standard affine kernel.}*/

typedef typename sKernel::RT sRT;
/*{\Xtypemember the standard ring type.}*/

typedef typename sKernel::FT sFT;
/*{\Xtypemember the field type.}*/

typedef typename sKernel::Point_2           sPoint_2;
/*{\Xtypemember standard points.}*/

typedef typename sKernel::Segment_2         sSegment_2;
/*{\Xtypemember standard segments.}*/

typedef typename sKernel::Line_2            sLine_2;
/*{\Xtypemember standard oriented lines.}*/

typedef typename sKernel::Direction_2       sDirection_2;
/*{\Xtypemember standard directions.}*/

typedef typename sKernel::Ray_2             sRay_2;
/*{\Xtypemember standard rays.}*/

typedef typename sKernel::Aff_transformation_2 sAff_transformation_2;
/*{\Xtypemember standard affine transformations.}*/

/*{\Xtext \headerline{Extended kernel types}}*/

typedef typename Base::RT RT;
/*{\Xtypemember the ring type of our extended kernel.}*/

typedef typename Base::FT FT;
/*{\Xtypemember the ring type of our extended kernel.}*/

typedef typename Base::Point_2      Point_2;
/*{\Xtypemember extended points.}*/

typedef typename Base::Segment_2    Segment_2;
/*{\Xtypemember extended segments.}*/

typedef typename Base::Line_2       Line_2;
/*{\Xtypemember extended lines.}*/

typedef typename Base::Direction_2  Direction_2;
/*{\Xtypemember extended directions.}*/

enum point_type { SWCORNER=1, LEFTFRAME, NWCORNER, 
                  LOWERFRAME, STANDARD, UPPERFRAME,
                  SECORNER, RIGHTFRAME, NECORNER };
/*{\Xenum a type descriptor for extended points.}*/

Point_2 epoint(const sFT& m1, const sFT& n1, 
               const sFT& m2, const sFT& n2) const
{ return Point_2(FT(n1,m1),FT(n2,m2)); }

public:
/*{\Xoperations 2}*/
/*{\Xtext \headerline{Interfacing the affine kernel types}}*/

Point_2 construct_point(const sPoint_2& p) const
/*{\Xop creates an extended point |Point_2| and initializes it to the 
standard point |p|.}*/
{ return Point_2(p.x(), p.y()); }

Point_2 construct_point(const sLine_2& l, point_type& t) const
/*{\Xop creates an extended point initialized to the equivalence
class of all the rays underlying the oriented line |l|. 
|t| returns the type of the new extended point.}*/
{ 
  t = (point_type)Line_to_epoint<sKernel>::determine_type(l);
  Point_2 res;
  switch (t) {
    case SWCORNER:   res = epoint(-1, 0, -1, 0); break;
    case NWCORNER:   res = epoint(-1, 0,  1, 0); break;
    case SECORNER:   res = epoint( 1, 0, -1, 0); break; 
    case NECORNER:   res = epoint( 1, 0,  1, 0); break;  
    case LEFTFRAME:  
      res = epoint(-1, 0,  l.a()/l.b(), -l.c()/l.b()); break; 
    case RIGHTFRAME: 
      res = epoint( 1, 0, -l.a()/l.b(), -l.c()/l.b()); break; 
    case LOWERFRAME: 
      res = epoint( l.b()/l.a(), -l.c()/l.a(), -1, 0); break; 
    case UPPERFRAME: 
      res = epoint(-l.b()/l.a(), -l.c()/l.a(),  1, 0); break; 
    default: CGAL_assertion_msg(0,"EPoint type not correct!");
  }
  return res;
}

Point_2 construct_point(const sPoint_2& p1, const sPoint_2& p2, 
                        point_type& t) const
/*{\Xop creates an extended point and initializes it to the equivalence
class of all the rays underlying the oriented line |l(p1,p2)|. 
|t| returns the type of the new extended point.}*/
{ return construct_point(sLine_2(p1,p2),t); }

Point_2 construct_point(const sLine_2& l) const
/*{\Xop creates an extended point and initializes it to the equivalence
class of all the rays underlying the oriented line |l|. }*/
{ point_type dummy; return construct_point(l,dummy); }

Point_2 construct_point(const sPoint_2& p1, const sPoint_2& p2) const
/*{\Xop creates an extended point and initializes it to the equivalence
class of all the rays underlying the oriented line |l(p1,p2)|.}*/
{ return construct_point(sLine_2(p1,p2)); }

Point_2 construct_point(const sPoint_2& p, const sDirection_2& d) const
/*{\Xop creates an extended point and initializes it to the equivalence
class of all the rays underlying the ray starting in |p| in direction |d|.}*/
{ return construct_point(sLine_2(p,d)); }

Point_2 construct_opposite_point(const sLine_2& l) const
/*{\Xop creates an extended point and initializes it to the equivalence
class of all the rays underlying the oriented line opposite to |l|. }*/
{ point_type dummy; return construct_point(l.opposite(),dummy); }

point_type type(const Point_2& p) const
/*{\Xop determines the type of |p| and returns it.}*/
{
  CGAL_assertion(p.x().degree()>=0 && p.y().degree()>=0 );
  if ( p.x().degree() == 0 && p.y().degree() == 0) 
    return STANDARD;
  // now we are on the square frame
  FT rx = p.x();
  FT ry = p.y();
  int sx = sign(rx);
  int sy = sign(ry);
  if (sx < 0) rx = -rx;
  if (sy < 0) ry = -ry;
  if (rx>ry) {
    if (sx > 0) return RIGHTFRAME;
    else        return LEFTFRAME;
  }
  if (rx<ry) {
    if (sy > 0) return UPPERFRAME;
    else        return LOWERFRAME;
  }
  // now (rx == ry) 
  if (sx==sy) {
    if (sx < 0) return SWCORNER;
    else        return NECORNER;
  } else { CGAL_assertion(sx==-sy);
    if (sx < 0) return NWCORNER;
    else        return SECORNER;
  }
}


bool is_standard(const Point_2& p) const
/*{\Xop returns |true| iff |p| is a standard point.}*/
{ return (type(p)==STANDARD);  }

sPoint_2 standard_point(const Point_2& p) const
/*{\Xop returns the standard point represented by |p|.
\precond |\Mvar.is_standard(p)|.}*/
{ CGAL_assertion( type(p)==STANDARD );
  return sPoint_2(p.x()[0],p.y()[0]);
}

sLine_2 standard_line(const Point_2& p) const
/*{\Xop returns the oriented line representing the 
bundle of rays defining |p|.
\precond |!\Mvar.is_standard(p)|.}*/
{ CGAL_assertion( type(p)!=STANDARD );
  FT x = p.x(), y = p.y();
  sFT dx = x.degree()>0 ? x[1] : sFT(0);
  sFT dy = y.degree()>0 ? y[1] : sFT(0);
  sPoint_2 p0(x[0],y[0]);
  sPoint_2 p1(x[0]+dx,y[0]+dy);
  return sLine_2(p0,p1);
}

sRay_2 standard_ray(const Point_2& p) const
/*{\Xop a ray defining |p|. \precond |!\Mvar.is_standard(p)|.}*/
{ sLine_2 l = standard_line(p);
  sDirection_2 d = l.direction();
  sPoint_2 q = l.point(0);
  return sRay_2(q,d);
}

Point_2 NE() const { return construct_point(sLine_2(-1, 1,0)); }
/*{\Xop returns the point on the north east frame corner.}*/

Point_2 SE() const { return construct_point(sLine_2( 1, 1,0)); }
/*{\Xop returns the point on the south east frame corner.}*/

Point_2 NW() const { return construct_point(sLine_2(-1,-1,0)); }
/*{\Xop returns the point on the north west frame corner.}*/

Point_2 SW() const { return construct_point(sLine_2( 1,-1,0)); }
/*{\Xop returns the point on the south west frame corner.}*/


Line_2 upper() const { return construct_line(NW(),NE()); }
/*{\Xop returns the line underlying the upper frame segment.}*/

Line_2 lower() const { return construct_line(SW(),SE()); }
/*{\Xop returns the line underlying the lower frame segment.}*/

Line_2 left()  const { return construct_line(SW(),NW()); }
/*{\Xop returns the line underlying the left frame segment.}*/

Line_2 right() const { return construct_line(SE(),NE()); }
/*{\Xop returns the line underlying the right frame segment.}*/


/*{\Xtext \headerline{Geometric kernel calls}}*/

Point_2 source(const Segment_2& s) const
/*{\Xop returns the source point of |s|.}*/
{ typename Base::Construct_source_point_2 _source =
    construct_source_point_2_object();
  return _source(s); }

Point_2 target(const Segment_2& s) const
/*{\Xop returns the target point of |s|.}*/
{ typename Base::Construct_target_point_2 _target =
    construct_target_point_2_object();
  return _target(s); }

Segment_2 construct_segment(const Point_2& p, const Point_2& q) const
/*{\Xop constructs a segment |pq|.}*/
{ typename Base::Construct_segment_2 _segment =
    construct_segment_2_object();
  return _segment(p,q); }

Line_2 construct_line(const sLine_2& l)  const
/*{\Xop returns an extended line.}*/
{ return Line_2(l.a(),l.b(),l.c()); }

Line_2 construct_line(const Point_2& p1, const Point_2& p2) const
/*{\Xop returns a line through the two extended points |p1| and |p2|.}*/
{ Line_2 l(p1,p2);
    TRACEN("eline("<<p1<<p2<<")="<<l);
  RT a=l.a(), b=l.b(), c=l.c();
  l =  Line_2(a,b,c);
  return l; 
}


int orientation(const Segment_2& s, const Point_2& p) const
/*{\Xop returns the orientation of |p| with respect to the line
through |s|.}*/
{ typename Base::Orientation_2 _orientation =
    orientation_2_object();
  return static_cast<int> ( _orientation(source(s),target(s),p) ); 
}

int orientation(const Point_2& p1, const Point_2& p2, const Point_2& p3) 
const
/*{\Xop returns the orientation of |p2| with respect to the line
through |p1p2|.}*/
{ typename Base::Orientation_2 _orientation =
    orientation_2_object();
  return static_cast<int> ( _orientation(p1,p2,p3) ); 
}

bool leftturn(const Point_2& p1, const Point_2& p2, const Point_2& p3) 
const
/*{\Xop return true iff the |p3| is left of the line through |p1p2|.}*/
{ return orientation(p1,p2,p3) > 0; }
 
bool is_degenerate(const Segment_2& s) const
/*{\Xop return true iff |s| is degenerate.}*/
{ typename Base::Is_degenerate_2 _is_degenerate =
    is_degenerate_2_object();
  return _is_degenerate(s); }

int compare_xy(const Point_2& p1, const Point_2& p2) const
/*{\Xop returns the lexicographic order of |p1| and |p2|.}*/
{ typename Base::Compare_xy_2 _compare_xy =
    compare_xy_2_object();
  return static_cast<int>( _compare_xy(p1,p2) );
}

int compare_x(const Point_2& p1, const Point_2& p2) const
/*{\Xop returns the order on the $x$-coordinates of |p1| and |p2|.}*/
{ typename Base::Compare_x_2 _compare_x =
    compare_x_2_object();
  return static_cast<int>( _compare_x(p1,p2) );
}

int compare_y(const Point_2& p1, const Point_2& p2) const
/*{\Xop returns the order on the $y$-coordinates of |p1| and |p2|.}*/
{ typename Base::Compare_y_2 _compare_y =
    compare_y_2_object();
  return static_cast<int>( _compare_y(p1,p2) );
}


Point_2 intersection(
  const Segment_2& s1, const Segment_2& s2) const
/*{\Xop returns the point of intersection of the lines supported by |s1| 
and |s2|.}*/
{ typename Base::Intersect_2 _intersect =
    intersect_2_object();
  typename Base::Construct_line_2 _line =
    construct_line_2_object();
  Point_2 p; 
  CGAL::Object result =
    _intersect(_line(s1),_line(s2));
  if ( !CGAL::assign(p, result) )
  CGAL_assertion_msg(false,"intersection: no intersection.");
  return p;
}

Direction_2 construct_direction(
  const Point_2& p1, const Point_2& p2) const
/*{\Xop returns the direction of the vector |p2| - |p1|.}*/
{ typename Base::Construct_direction_of_line_2 _direction =
    construct_direction_of_line_2_object();
  return _direction(construct_line(p1,p2)); }

bool strictly_ordered_ccw(const Direction_2& d1, 
  const Direction_2& d2, const Direction_2& d3) const
/*{\Xop returns |true| iff |d2| is in the interior of the
counterclockwise angular sector between |d1| and |d3|.}*/
{ 
  if ( d1 < d2 )  return ( d2 < d3 )||( d3 <= d1 );
  if ( d1 > d2 )  return ( d2 < d3 )&&( d3 <= d1 );
  return false;
}

bool contains(const Segment_2& s, const Point_2& p) const
/*{\Xop returns true iff |s| contains |p|.}*/
{ typename Base::Has_on_2 _contains = has_on_2_object();
  return _contains(s,p);
}

bool strictly_ordered_along_line(
  const Point_2& p1, const Point_2& p2, const Point_2& p3) const
/*{\Xop returns |true| iff |p2| is in the relative interior of the
segment |p1p3|.}*/
{ typename Base::Are_strictly_ordered_along_line_2 _ordered =
    are_strictly_ordered_along_line_2_object();
  return _ordered(p1,p2,p3);
}

bool first_pair_closer_than_second(
  const Point_2& p1, const Point_2& p2, 
  const Point_2& p3, const Point_2& p4) const
{ return ( squared_distance(p1,p2) < squared_distance(p3,p4) ); }

template <class Forward_iterator>
void determine_frame_radius(Forward_iterator start, Forward_iterator end,
                            sRT& R0) const
{ sRT R;
  while ( start != end ) {
    Point_2 p = *start++;
    if ( is_standard(p) ) {
      R = CGAL_NTS max(CGAL_NTS abs(p.x()[0]), CGAL_NTS abs(p.y()[0]));
    } else {
      RT rx = CGAL_NTS abs(p.x()), ry = CGAL_NTS abs(p.y());
      if ( rx[1] > ry[1] )      R = CGAL_NTS abs(ry[0]-rx[0])/(rx[1]-ry[1]);
      else if ( rx[1] < ry[1] ) R = CGAL_NTS abs(rx[0]-ry[0])/(ry[1]-rx[1]);
      else /* rx[1] == ry[1] */ R = CGAL_NTS abs(rx[0]-ry[0])/2;
    }
    R0 = CGAL_NTS max(R+1,R0);
  }
}



const char* output_identifier() const { return "Extended_cartesian"; }

};



#undef RPolynomial
CGAL_END_NAMESPACE
#endif // CGAL_EXTENDED_CARTESIAN_H


