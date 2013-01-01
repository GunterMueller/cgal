#ifndef CGAL_KD_COAFFINE_H
#define CGAL_KD_COAFFINE_H
#include <vector>
#include <algorithm>
#include <iterator>
#include <CGAL/Dimension.h>
#include <CGAL/functor_tags.h>

namespace CGAL {
namespace CartesianDKernelFunctors {
struct Flat_orientation {
	std::vector<int> proj;
	std::vector<int> rest;
};

// For debugging purposes
inline std::ostream& operator<< (std::ostream& o, Flat_orientation const& f) {
  o << "Proj: ";
  for(std::vector<int>::const_iterator i=f.proj.begin();
      i!=f.proj.end(); ++i)
    o << *i << ' ';
  o << "\nRest: ";
  for(std::vector<int>::const_iterator i=f.rest.begin();
      i!=f.rest.end(); ++i)
    o << *i << ' ';
  return o << '\n';
}

namespace internal {
namespace coaffine {
template<class Mat>
inline void debug_matrix(std::ostream& o, Mat const&mat) {
  for(int i=0;i<mat.rows();++i){
  for(int j=0;j<mat.cols();++j){
    o<<mat(i,j)<<' ';
  }
  o<<'\n';
  }
}
}
}

template<class R_> struct Construct_flat_orientation : private Store_kernel<R_> {
	CGAL_FUNCTOR_INIT_STORE(Construct_flat_orientation)
	typedef R_ R;
	typedef typename Get_type<R, FT_tag>::type FT;
	typedef typename Get_type<R, Point_tag>::type Point;
	typedef typename Increment_dimension<typename R::Max_ambient_dimension>::type Dplusone;
	typedef typename R::LA::template Rebind_dimension<Dynamic_dimension_tag,Dplusone>::Other LA;
	typedef typename LA::Square_matrix Matrix;
	typedef typename Get_functor<R, Compute_point_cartesian_coordinate_tag>::type CCC;
	typedef typename Get_functor<R, Point_dimension_tag>::type PD;
	typedef Flat_orientation result_type;

	// This implementation is going to suck. Maybe we should push the
	// functionality into LA.
	template<class Iter>
	result_type operator()(Iter f, Iter e)const{
		PD pd (this->kernel());
		CCC ccc (this->kernel());
		int dim = pd(*f);
		Matrix coord (dim+1, dim+1); // use distance(f,e)? This matrix doesn't need to be square.
		int col = 0;
		Flat_orientation o;
		std::vector<int>& proj=o.proj;
		std::vector<int>& rest=o.rest; rest.reserve(dim+1);
		for(int i=0; i<dim+1; ++i) rest.push_back(i);
		for( ; f != e ; ++col, ++f ) {
			Point const&p=*f;
			// use a coordinate iterator instead?
			for(int i=0; i<dim; ++i) coord(col, i) = ccc(p, i);
			coord(col,dim)=1;
			int d = proj.size()+1;
			Matrix m (d, d);
			// Fill the matrix with what we already have
			for(int i=0; i<d; ++i)
			for(int j=0; j<d-1; ++j)
				m(i,j) = coord(i, proj[j]);
			// Try to complete with any other coordinate
			// TODO: iterate on rest by the end, or use a (forward_)list.
			for(std::vector<int>::iterator it=rest.begin();;++it) {
				CGAL_assertion(it!=rest.end());
				for(int i=0; i<d; ++i) m(i,d-1) = coord(i, *it);
				if(LA::sign_of_determinant(m)!=0) {
					proj.push_back(*it);
					rest.erase(it);
					break;
				}
			}
		}
		std::sort(proj.begin(),proj.end());
		return o;
	}
};

template<class R_> struct Contained_in_affine_hull : private Store_kernel<R_> {
        CGAL_FUNCTOR_INIT_STORE(Contained_in_affine_hull)
        typedef R_ R;
	typedef typename Get_type<R, FT_tag>::type FT;
        typedef typename Get_type<R, Point_tag>::type Point;
	typedef typename Get_type<R, Bool_tag>::type result_type;
	typedef typename Get_functor<R, Compute_point_cartesian_coordinate_tag>::type CCC;
	typedef typename Get_functor<R, Point_dimension_tag>::type PD;
	//typedef typename Increment_dimension<typename R::Default_ambient_dimension>::type D1;
	//typedef typename Increment_dimension<typename R::Max_ambient_dimension>::type D2;
	//typedef typename R::LA::template Rebind_dimension<D1,D2>::Other LA;
	typedef typename Increment_dimension<typename R::Max_ambient_dimension>::type Dplusone;
	typedef typename R::LA::template Rebind_dimension<Dynamic_dimension_tag,Dplusone>::Other LA;
	typedef typename LA::Square_matrix Matrix;

	// mostly copied from Construct_flat_orientation. TODO: dedup this code.
        template<class Iter>
        result_type operator()(Iter f, Iter e, Point const&x) const {
		PD pd (this->kernel());
		CCC ccc (this->kernel());
		int dim=pd(*f);
		Matrix coord (dim+1, dim+1); // use distance
		int col = 0;
		std::vector<int> proj;
		std::vector<int> rest; rest.reserve(dim+1);
		for(int i=0; i<dim+1; ++i) rest.push_back(i);
		for( ; f != e ; ++col, ++f ) {
			Point const&p=*f;
			for(int i=0; i<dim; ++i) coord(col, i) = ccc(p, i);
			coord(col,dim)=1;
			int d = proj.size()+1;
			Matrix m (d, d);
			for(int i=0; i<d; ++i)
			for(int j=0; j<d-1; ++j)
				m(i,j) = coord(i, proj[j]);
			for(std::vector<int>::iterator it=rest.begin();it!=rest.end();++it) {
				for(int i=0; i<d; ++i) m(i,d-1) = coord(i, *it);
				if(LA::sign_of_determinant(m)!=0) {
					proj.push_back(*it);
					rest.erase(it);
					break;
				}
			}
		}
			for(int i=0; i<dim; ++i) coord(col, i) = ccc(x, i);
			coord(col,dim)=1;
			int d = proj.size()+1;
			Matrix m (d, d);
			for(int i=0; i<d; ++i)
			for(int j=0; j<d-1; ++j)
				m(i,j) = coord(i, proj[j]);
			for(std::vector<int>::iterator it=rest.begin();it!=rest.end();++it) {
				for(int i=0; i<d; ++i) m(i,d-1) = coord(i, *it);
				if(LA::sign_of_determinant(m)!=0) return false;
			}
			return true;
        }
};

template<class R_> struct In_flat_orientation : private Store_kernel<R_> {
        CGAL_FUNCTOR_INIT_STORE(In_flat_orientation)
        typedef R_ R;
	typedef typename Get_type<R, FT_tag>::type FT;
        typedef typename Get_type<R, Point_tag>::type Point;
	typedef typename Get_type<R, Orientation_tag>::type result_type;
	typedef typename Increment_dimension<typename R::Default_ambient_dimension>::type D1;
	typedef typename Increment_dimension<typename R::Max_ambient_dimension>::type D2;
	typedef typename R::LA::template Rebind_dimension<D1,D2>::Other LA;
	typedef typename LA::Square_matrix Matrix;

        template<class Iter>
        result_type operator()(Flat_orientation const&o, Iter f, Iter e) const {
		// TODO: work in the projection instead of the ambient space.
                typename Get_functor<R, Compute_point_cartesian_coordinate_tag>::type c(this->kernel());
                typename Get_functor<R, Point_dimension_tag>::type pd(this->kernel());
                int d=pd(*f);
                Matrix m(d+1,d+1);
                int i=0;
		for(;f!=e;++f,++i) {
			Point const& p=*f;
			m(i,0)=1;
			for(int j=0;j<d;++j){
				m(i,j+1)=c(p,j);
			}
		}
		for(std::vector<int>::const_iterator it = o.rest.begin(); it != o.rest.end() /* i<d+1 */; ++i, ++it) {
			m(i,0)=1;
			for(int j=0;j<d;++j){
				m(i,j+1)=0; // unneeded if the matrix is initialized to 0
			}
			if(*it != d) m(i,1+*it)=1;
		}

                return LA::sign_of_determinant(CGAL_MOVE(m));
        }
};

template<class R_> struct In_flat_side_of_oriented_sphere : private Store_kernel<R_> {
        CGAL_FUNCTOR_INIT_STORE(In_flat_side_of_oriented_sphere)
        typedef R_ R;
	typedef typename Get_type<R, FT_tag>::type FT;
        typedef typename Get_type<R, Point_tag>::type Point;
	typedef typename Get_type<R, Orientation_tag>::type result_type;
	typedef typename Increment_dimension<typename R::Default_ambient_dimension,2>::type D1;
	typedef typename Increment_dimension<typename R::Max_ambient_dimension,2>::type D2;
	typedef typename R::LA::template Rebind_dimension<D1,D2>::Other LA;
	typedef typename LA::Square_matrix Matrix;

        template<class Iter>
        result_type operator()(Flat_orientation const&o, Iter f, Iter e, Point const&x) const {
		// TODO: can't work in the projection, but we should at least remove the row of 1s.
                typename Get_functor<R, Compute_point_cartesian_coordinate_tag>::type c(this->kernel());
                typename Get_functor<R, Point_dimension_tag>::type pd(this->kernel());
                int d=pd(*f);
                Matrix m(d+2,d+2);
                int i=0;
		for(;f!=e;++f,++i) {
			Point const& p=*f;
			m(i,0)=1;
			m(i,d+1)=0;
			for(int j=0;j<d;++j){
				m(i,j+1)=c(p,j);
				m(i,d+1)+=CGAL_NTS square(m(i,j+1));
			}
		}
		for(std::vector<int>::const_iterator it = o.rest.begin(); it != o.rest.end() /* i<d+1 */; ++i, ++it) {
			m(i,0)=1;
			for(int j=0;j<d;++j){
				m(i,j+1)=0; // unneeded if the matrix is initialized to 0
			}
			if(*it != d) m(i,d+1)=m(i,1+*it)=1;
			else m(i,d+1)=0;
		}
		m(d+1,0)=1;
		m(d+1,d+1)=0;
		for(int j=0;j<d;++j){
			m(d+1,j+1)=c(x,j);
			m(d+1,d+1)+=CGAL_NTS square(m(d+1,j+1));
		}

                return LA::sign_of_determinant(CGAL_MOVE(m));
        }
};


}
CGAL_KD_DEFAULT_FUNCTOR(In_flat_orientation_tag,(CartesianDKernelFunctors::In_flat_orientation<K>),(Point_tag),(Compute_point_cartesian_coordinate_tag,Point_dimension_tag));
CGAL_KD_DEFAULT_FUNCTOR(In_flat_side_of_oriented_sphere_tag,(CartesianDKernelFunctors::In_flat_side_of_oriented_sphere<K>),(Point_tag),(Compute_point_cartesian_coordinate_tag,Point_dimension_tag));
CGAL_KD_DEFAULT_FUNCTOR(Construct_flat_orientation_tag,(CartesianDKernelFunctors::Construct_flat_orientation<K>),(Point_tag),(Compute_point_cartesian_coordinate_tag,Point_dimension_tag));
CGAL_KD_DEFAULT_FUNCTOR(Contained_in_affine_hull_tag,(CartesianDKernelFunctors::Contained_in_affine_hull<K>),(Point_tag),(Compute_point_cartesian_coordinate_tag,Point_dimension_tag));
}
#endif
