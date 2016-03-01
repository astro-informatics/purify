#include "utilities.h"

namespace purify {
	namespace utilities {

	utilities::vis_params read_visibility(const std::string& vis_name)
	  {
	    /*
	      Reads an csv file with u, v, visibilities and returns the vectors.

	      vis_name:: name of input text file containing [u, v, real(V), imag(V)] (separated by ' ').
	    */
	    std::ifstream temp_file(vis_name);
	    t_int row = 0;
	    std::string line;
	    //counts size of vis file
	    while (std::getline(temp_file, line))
	      ++row;
	    Vector<t_real> utemp(row);
	    Vector<t_real> vtemp(row);
	    Vector<t_complex> vistemp(row);
	    Vector<t_complex> weightstemp(row);
	    std::ifstream vis_file(vis_name);

	    // reads in vis file
	    row = 0;
	    t_real real;
	    t_real imag;
	    std::string s;
	    std::string  entry;
	    while (vis_file)
	    {
	      if (!std::getline(vis_file, s)) break;
	      std::istringstream ss(s);
	      std::getline(ss, entry, ' ');
	      utemp(row) = std::stod(entry);
	      std::getline(ss, entry, ' ');
	      vtemp(row) = std::stod(entry);
	      std::getline(ss, entry, ' ');
	      real = std::stod(entry);
	      std::getline(ss, entry, ' ');
	      imag = std::stod(entry);
	      vistemp(row) = t_complex(real, imag);
	      std::getline(ss, entry, ' ');
	      weightstemp(row) = 1/(std::stod(entry) * std::stod(entry));
	      ++row;
	    }
	    utilities::vis_params uv_vis;
	    uv_vis.u = utemp;
	    uv_vis.v = -vtemp; // found that a reflection is needed for the orientation of the gridded image to be correct
	    uv_vis.vis = vistemp;
	    uv_vis.weights = weightstemp;
	    return uv_vis;
	  }

	  utilities::vis_params set_cell_size(const utilities::vis_params& uv_vis, t_real cell_size_u, t_real cell_size_v)
	  {
	      /*
	        Converts the units of visibilities to units of 2 * pi, while scaling for the size of a pixel (cell_size)

	        uv_vis:: visibilities
	        cell_size:: size of a pixel in arcseconds
	      */

	      utilities::vis_params scaled_vis;

	      if (cell_size_u == 0 and cell_size_v == 0)
	      {
	        Vector<t_real> u_dist = uv_vis.u.array() * uv_vis.u.array();
	        t_real max_u = std::sqrt(u_dist.maxCoeff());
	        cell_size_u = (180 * 3600) / max_u / purify_pi / 3 * 1.02; //Calculate cell size if not given one

	        Vector<t_real> v_dist = uv_vis.v.array() * uv_vis.v.array();
	        t_real max_v = std::sqrt(v_dist.maxCoeff());
	        cell_size_v = (180 * 3600) / max_v / purify_pi / 3  * 1.02; //Calculate cell size if not given one
	        std::cout << "PSF has a FWHM of " << cell_size_u * 3 << " x " << cell_size_v * 3 << " arcseconds" << '\n';
	      }
	      if (cell_size_v == 0)
	      {
	        cell_size_v = cell_size_u;
	      }

	      
	      std::cout << "Using a pixel size of " << cell_size_u << " x " << cell_size_v << " arcseconds" << '\n';

	      t_real scale_factor_u = 180 * 3600 / cell_size_u / purify_pi;
	      t_real scale_factor_v = 180 * 3600 / cell_size_v / purify_pi;
	      scaled_vis.u = uv_vis.u / scale_factor_u * 2 * purify_pi;
	      scaled_vis.v = uv_vis.v / scale_factor_v * 2 * purify_pi;
	      scaled_vis.vis = uv_vis.vis;
	      scaled_vis.weights = uv_vis.weights;
	      return scaled_vis;
	  }

	  Vector<t_complex> apply_weights(const Vector<t_complex> visiblities, const Vector<t_complex> weights)
	  {
	    /*
	      Applies weights to visiblities, assuming they are 1/sigma^2.
	    */
	    Vector<t_complex> weighted_vis;
	    weighted_vis = (visiblities.array() * weights.array()).matrix();
	    return weighted_vis;
	  }

	  utilities::vis_params uv_scale(const utilities::vis_params& uv_vis, const t_int& ftsizeu, const t_int& ftsizev)
	  {
	    /*
	      scales the uv coordinates from being in units of 2 * pi to units of pixels.
	    */
	      utilities::vis_params scaled_vis;
	      scaled_vis.u = uv_vis.u / (2 * purify_pi) * ftsizeu;
	      scaled_vis.v = uv_vis.v / (2 * purify_pi) * ftsizev;
	      scaled_vis.vis = uv_vis.vis;
	      scaled_vis.weights = uv_vis.weights;
	      for (t_int i = 0; i < uv_vis.u.size(); ++i)
	      {
	      	scaled_vis.u(i) = utilities::mod(scaled_vis.u(i), ftsizeu);
	      	scaled_vis.v(i) = utilities::mod(scaled_vis.v(i), ftsizev);
	      }
	      return scaled_vis;
	  }

	  utilities::vis_params uv_symmetry(const utilities::vis_params& uv_vis)
	  {
	    /*
	      Adds in reflection of the fourier plane using the condjugate symmetry for a real image.

	      uv_vis:: uv coordinates for the fourier plane
	    */
	    t_int total = uv_vis.u.size();
	    Vector<t_real> utemp(2 * total);
	    Vector<t_real> vtemp(2 * total);
	    Vector<t_complex> vistemp(2 * total);
	    Vector<t_complex> weightstemp(2 * total);
	    utilities::vis_params conj_vis;
	    for (t_int i = 0; i < uv_vis.u.size(); ++i)
	    {
	      utemp(i) = uv_vis.u(i);
	      vtemp(i) = uv_vis.v(i);
	      vistemp(i) = uv_vis.vis(i);
	      weightstemp(i) = uv_vis.weights(i);
	    }
	    for (t_int i = total; i < 2 * total; ++i)
	    {
	      utemp(i) = -uv_vis.u(i - total);
	      vtemp(i) = -uv_vis.v(i - total);
	      vistemp(i) = std::conj(uv_vis.vis(i - total));
	      weightstemp(i) = uv_vis.weights(i - total);
	    }
	    conj_vis.u = utemp;
	    conj_vis.v = vtemp;
	    conj_vis.vis = vistemp;
	    conj_vis.weights = weightstemp;
	    return conj_vis;
	  }
	
	  t_int sub2ind(const t_int& row, const t_int& col, const t_int& rows, const t_int& cols) 
	  {
	    /*
	      Converts (row, column) of a matrix to a single index. This does the same as the matlab funciton sub2ind, converts subscript to index. 
	      Though order of cols and rows is probably transposed.

	      row:: row of matrix (y)
	      col:: column of matrix (x)
	      cols:: number of columns for matrix
	      rows:: number of rows for matrix
	     */
	    return row * cols + col;
	  }

	  void ind2sub(const t_int sub, const t_int cols, const t_int rows, t_int* row, t_int* col) 
	  {
	    /*
	      Converts index of a matrix to (row, column). This does the same as the matlab funciton sub2ind, converts subscript to index.
	      
	      sub:: subscript of entry in matrix
	      cols:: number of columns for matrix
	      rows:: number of rows for matrix
	      row:: output row of matrix
	      col:: output column of matrix

	     */
	    *col = sub % cols;
	    *row = (sub - *col) / cols;
	  }

	  t_real mod(const t_real& x, const t_real& y) 
	  {
	    /*
	      returns x % y, and warps circularly around y for negative values.
	    */
	      t_real r = std::fmod(x, y);
	      if (r < 0)
	        r = y + r;
	      return r;
	  }

	  t_complex mean(const Vector<t_complex> x){
	  	// Calculate mean of vector x
	  	return x.sum()/static_cast<t_real>(x.size());
	  }

	  t_real variance(const Vector<t_complex> x){
	  	//calculate variance of vector x
	  	t_complex mu = mean(x);
	  	Vector<t_complex> q = (x.array() - mu).matrix();
	  	auto var = (q.adjoint() * q).real()(0)/(q.size() - 1);
	  	return var;
	  }

  Image<t_complex> convolution_operator(const Image<t_complex>& a, const Image<t_complex>& b){
    /*
    returns the convolution of vector a with vector b
    a:: vector a, which is shifted
    b:: vector b, which is fixed
    */

    //size of a image
    t_int a_y = a.rows();
    t_int a_x = a.cols();
    //size of b image
    t_int b_y = b.rows();
    t_int b_x = b.cols();    

    Image<t_complex> output = Image<t_complex>::Zero(a_y + b_y, a_x + b_x);



    for (t_int l = 0; l < b.cols(); ++l)
    {
      for (int k = 0; k < b.rows(); ++k)
      {
        output(k, l) = (a * b.block(k, l, a_y, a_x)).sum();
      }
    }

    return output;
  }
	}
}