#include "utilities.h"

namespace purify {
	namespace utilities {
	utilities::vis_params random_sample_density(const t_int& vis_num, const t_real& mean, const t_real& standard_deviation){
		/*
			Generates a random sampling density for visibility coverage
			vis_num:: number of visibilities
			mean:: mean of distribution
			standard_deviation:: standard deviation of distirbution
		*/
			auto sample = [&mean, &standard_deviation] (t_real x) { 
				std::random_device rd;
				std::mt19937_64 rng(rd());
				t_real output = 4 * standard_deviation + mean;
				static std::normal_distribution<> normal_dist(mean, standard_deviation);
				//ensures that all sample points are within bounds
				while(std::abs(output - mean) > 3 * standard_deviation){
					output = normal_dist(rng);
				}
				if (output != output) std::cout << output << '\n';
				return output;
			 };

			utilities::vis_params uv_vis;
			uv_vis.u = Vector<t_real>::Zero(vis_num).unaryExpr(sample);
			uv_vis.v = Vector<t_real>::Zero(vis_num).unaryExpr(sample);
			uv_vis.w = Vector<t_real>::Zero(vis_num).unaryExpr(sample);
			uv_vis.weights = Vector<t_complex>::Constant(vis_num, 1);
			uv_vis.vis = Vector<t_complex>::Constant(vis_num, 1);
			uv_vis.phase_centre = 0;
			return uv_vis;
	}
	utilities::vis_params read_visibility(const std::string& vis_name, const bool w_term)
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
	    Vector<t_real> utemp = Vector<t_real>::Zero(row);
	    Vector<t_real> vtemp = Vector<t_real>::Zero(row);
	    Vector<t_real> wtemp = Vector<t_real>::Zero(row);
	    Vector<t_complex> vistemp = Vector<t_complex>::Zero(row);
	    Vector<t_complex> weightstemp = Vector<t_complex>::Zero(row);
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
	      
	      if (w_term)
	      {
	      	std::getline(ss, entry, ' ');
	      	wtemp(row) = std::stod(entry);
	      }

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
	    uv_vis.w = wtemp;
	    uv_vis.vis = vistemp;
	    uv_vis.weights = weightstemp;
		
	    
	    return uv_vis;
	  }

	  void write_visibility(const utilities::vis_params& uv_vis, const std::string & file_name, const bool w_term)
	  {
	  	/*
			writes visibilities to output text file (currently ignores w-component)
			uv_vis:: input uv data
			file_name:: name of output text file
	  	*/
		std::ofstream out(file_name);
		out.precision(13);
		for (t_int i = 0; i < uv_vis.u.size(); ++i)
		{
			out << uv_vis.u(i) << " " << -uv_vis.v(i) << " ";
			if (w_term)
				out << uv_vis.w(i) << " ";
			out << std::real(uv_vis.vis(i)) << " " << std::imag(uv_vis.vis(i)) << " " << 1./std::sqrt(std::real(uv_vis.weights(i))) << std::endl;
		}
		out.close();
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
	        cell_size_u = (180 * 3600) / max_u / purify_pi / 3; //Calculate cell size if not given one

	        Vector<t_real> v_dist = uv_vis.v.array() * uv_vis.v.array();
	        t_real max_v = std::sqrt(v_dist.maxCoeff());
	        cell_size_v = (180 * 3600) / max_v / purify_pi / 3; //Calculate cell size if not given one
	        std::cout << "PSF has a FWHM of " << cell_size_u * 3 << " x " << cell_size_v * 3 << " arcseconds" << '\n';
	      }
	      if (cell_size_v == 0)
	      {
	        cell_size_v = cell_size_u;
	      }

	      
	      std::cout << "Using a pixel size of " << cell_size_u << " x " << cell_size_v << " arcseconds" << '\n';
	      t_real scale_factor_u = 1;
	      t_real scale_factor_v = 1;
	      if (uv_vis.units == "lambda")
	      {
	      	scale_factor_u = 180 * 3600 / cell_size_u / purify_pi;
	      	scale_factor_v = 180 * 3600 / cell_size_v / purify_pi;
	      	scaled_vis.w = uv_vis.w;
	      }
	      if (uv_vis.units == "radians")
	      {
	      	scale_factor_u = 180 * 3600 / purify_pi;
	      	scale_factor_v = 180 * 3600 / purify_pi;
	      	scaled_vis.w = uv_vis.w;	      	
	      }
	      scaled_vis.u = uv_vis.u / scale_factor_u * 2 * purify_pi;
	      scaled_vis.v = uv_vis.v / scale_factor_v * 2 * purify_pi;
	      
	      scaled_vis.vis = uv_vis.vis;
	      scaled_vis.weights = uv_vis.weights;
	      scaled_vis.units = "radians";
	      scaled_vis.phase_centre = uv_vis.phase_centre;
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

	  utilities::vis_params uv_scale(const utilities::vis_params& uv_vis, const t_int& sizex, const t_int& sizey)
	  {
	    /*
	      scales the uv coordinates from being in units of 2 * pi to units of pixels.
	    */
	      utilities::vis_params scaled_vis;
	      scaled_vis.u = uv_vis.u / (2 * purify_pi) * sizex;
	      scaled_vis.v = uv_vis.v / (2 * purify_pi) * sizey;
	      scaled_vis.vis = uv_vis.vis;
	      scaled_vis.weights = uv_vis.weights;
	      for (t_int i = 0; i < uv_vis.u.size(); ++i)
	      {
	      	scaled_vis.u(i) = utilities::mod(scaled_vis.u(i), sizex);
	      	scaled_vis.v(i) = utilities::mod(scaled_vis.v(i), sizey);
	      }
	      scaled_vis.w = uv_vis.w;
	      scaled_vis.units = "pixels";
	      scaled_vis.phase_centre = uv_vis.phase_centre;
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
	    Vector<t_real> wtemp(2 * total);
	    Vector<t_complex> vistemp(2 * total);
	    Vector<t_complex> weightstemp(2 * total);
	    
	    for (t_int i = 0; i < uv_vis.u.size(); ++i)
	    {
	      utemp(i) = uv_vis.u(i);
	      vtemp(i) = uv_vis.v(i);
	      wtemp(i) = uv_vis.w(i);
	      vistemp(i) = uv_vis.vis(i);
	      weightstemp(i) = uv_vis.weights(i);
	    }
	    for (t_int i = total; i < 2 * total; ++i)
	    {
	      utemp(i) = -uv_vis.u(i - total);
	      vtemp(i) = -uv_vis.v(i - total);
	      wtemp(i) = uv_vis.w(i - total);
	      vistemp(i) = std::conj(uv_vis.vis(i - total));
	      weightstemp(i) = uv_vis.weights(i - total);
	    }
	    utilities::vis_params conj_vis;
	    conj_vis.u = utemp;
	    conj_vis.v = vtemp;
	    conj_vis.w = wtemp;
	    conj_vis.vis = vistemp;
	    conj_vis.weights = weightstemp;
	    conj_vis.units = uv_vis.units;
	    conj_vis.phase_centre = uv_vis.phase_centre;

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

	  Vector<t_int> ind2sub(const t_int& sub, const t_int& cols, const t_int& rows) 
	  {
	    /*
	      Converts index of a matrix to (row, column). This does the same as the matlab funciton sub2ind, converts subscript to index.
	      
	      sub:: subscript of entry in matrix
	      cols:: number of columns for matrix
	      rows:: number of rows for matrix
	      row:: output row of matrix
	      col:: output column of matrix

	     */
	    Vector<t_int> row_col(2);
	    row_col(1) = sub % cols;
	   	row_col(0) = floor((sub - row_col(1)) / cols);
	   	return row_col;
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


	  Image<t_complex> convolution_operator(const Image<t_complex>& a, const Image<t_complex>& b){
	    /*
	    returns the convolution of images a with images b
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
	      for (t_int k = 0; k < b.rows(); ++k)
	      {
	        output(k, l) = (a * b.block(k, l, a_y, a_x)).sum();
	      }
	    }

	    return output;
	  }

	    utilities::vis_params whiten_vis(const utilities::vis_params& uv_vis){
	    	/*
				A function that whitens and returns the visibilities.

				vis:: input visibilities
				weights:: this expects weights that are the inverse of the complex variance, they are converted th RMS for whitenning.
	    	*/
				auto output_uv_vis = uv_vis;
	    		output_uv_vis.vis = uv_vis.vis.array() * uv_vis.weights.array().cwiseAbs().sqrt();
	    		return output_uv_vis;
	    }
	    t_real calculate_l2_radius(const Vector<t_complex> & y, const t_real& sigma, const t_real& n_sigma, const std::string distirbution){
	    	/*
				Calculates the epsilon, the radius of the l2_ball in sopt
				y:: vector for the l2 ball
	    	*/
			if (distirbution == "chi^2"){
				if (sigma == 0)
				{
					return std::sqrt(2 * y.size() + n_sigma * std::sqrt(4 * y.size()));
				}
		    	return std::sqrt(2 * y.size() + n_sigma * std::sqrt(4 * y.size())) * sigma;
	    	}
	    	if (distirbution == "chi")
	    	{
	    		auto alpha =  1./(8 * y.size()) - 1./(128 * y.size() * y.size()); //series expansion for gamma relation
	    		auto mean = std::sqrt(2) * std::sqrt(y.size()) * (1 - alpha); //using Gamma(k+1/2)/Gamma(k) asymptotic formula
	    		auto standard_deviation = std::sqrt(2 * y.size() * alpha * (2 - alpha));
	    		if (sigma == 0)
				{
					return mean + n_sigma * standard_deviation;
				}
		    	return (mean + n_sigma * standard_deviation) * sigma;
	    	}

	    	return 1.;
	    }
	   	t_real SNR_to_standard_deviation(const Vector<t_complex>& y0, const t_real& SNR){
	   		/*
			Returns value of noise rms given a measurement vector and signal to noise ratio
			y0:: complex valued vector before noise added
			SNR:: signal to noise ratio

			This calculation follows Carrillo et al. (2014), PURIFY a new approach to radio interferometric imaging
	   		*/
	    	return y0.stableNorm() / std::sqrt(y0.size()) * std::pow(10.0, -(SNR / 20.0));
	    }

		Vector<t_complex> add_noise(const Vector<t_complex>& y0, 
				const t_complex& mean, const t_real& standard_deviation){
			/*
				Adds complex valued Gaussian noise to vector
				y0:: vector before noise
				mean:: complex valued mean of noise
				standard_deviation:: rms of noise
			*/
			auto sample = [&mean, &standard_deviation] (t_complex x) { 
				std::random_device rd_real;
				std::random_device rd_imag;
				std::mt19937_64 rng_real(rd_real());
				std::mt19937_64 rng_imag(rd_imag());
				static std::normal_distribution<t_real> normal_dist_real(std::real(mean), standard_deviation / std::sqrt(2));
				static std::normal_distribution<t_real> normal_dist_imag(std::imag(mean), standard_deviation / std::sqrt(2));
				t_complex I(0,1.);
				return normal_dist_real(rng_real) + I * normal_dist_imag(rng_imag);
			 };

			auto noise = Vector<t_complex>::Zero(y0.size()).unaryExpr(sample);

			return y0 + noise;
		}
		
		bool file_exists(const std::string& name) {
			/*
				Checks if a file exists
				name:: path of file checked for existance.

				returns true if exists and false if not exists
			*/
  			struct stat buffer;   
  			return (stat (name.c_str(), &buffer) == 0); 
		}

		Vector<t_real> fit_fwhm(const Image<t_real> & psf, const t_int & size){
			/*
				Find FWHM of point spread function, using least squares.

				psf:: point spread function, assumed to be normalised.

				The method assumes that you have sampled at least 
				3 pixels across the beam.
			*/

			
			auto x0 = std::floor(psf.cols() * 0.5);
			auto y0 = std::floor(psf.rows() * 0.5);

			//finding patch
			Image<t_real> patch = psf.block(std::floor(y0 - size * 0.5) + 1, std::floor(x0 - size * 0.5) + 1, size, size);
			std::cout << "Fitting to Patch:\n " << patch << '\n';

			//finding values for least squares

			std::vector<t_tripletList> entries;
			auto total_entries = 0;

			for (t_int i = 0; i < patch.cols(); ++i)
			{
				for (t_int j = 0; j < patch.rows(); ++j)
				{	
					
					entries.emplace_back(j, i, std::log(patch(j, i))); total_entries++;
				}
			}
			Matrix<t_real> A = Matrix<t_real>::Zero(total_entries, 4);
			Vector<t_real> q = Vector<t_real>::Zero(total_entries);
			//putting values into a vector and matrix for least squares
			for (t_int i = 0; i < total_entries; ++i)
			{
				t_real x = entries.at(i).col() - size * 0.5 + 0.5;
				t_real y = entries.at(i).row() - size * 0.5 + 0.5;
				A(i, 0) = static_cast<t_real>(x * x); // x^2
				A(i, 1) = static_cast<t_real>(x * y); // x * y
				A(i, 2) = static_cast<t_real>(y * y); // y^2
				q(i) = std::real(entries.at(i).value());
			}
			//least squares fitting
			const auto solution = static_cast<Vector<t_real>>(A.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(q));
			t_real const a = - solution(0);
			t_real const b = + solution(1) * 0.5;
			t_real const c = - solution(2);
			//std::cout << a << " " << b << " " << c << '\n';
			//parameters of Gaussian
			t_real theta = std::atan2(b, std::sqrt(std::pow(2 * b, 2) + std::pow(c - a, 2)) + (c - a) * 0.5); // some relatively complicated trig identity to go from tan(2theta) to tan(theta).
			t_real t = 0.;
			t_real s = 0.;
			if (std::abs(b) <1e-13)
			{
				t = a;
				s = c;
				theta = 0;
			} else {
				t = (a + c - 2 * b / std::sin(2 * theta)) * 0.5;
				s = (a + c + 2 * b / std::sin(2 * theta)) * 0.5;
			}

			t_real const sigma_x = std::sqrt(1/(2 * t));
			t_real const sigma_y = std::sqrt(1/(2 * s));

			Vector<t_real> fit_parameters = Vector<t_real>::Zero(3);

			//fit for the beam rms, used for FWHM
			fit_parameters(0) = sigma_x;
			fit_parameters(1) = sigma_y;
			fit_parameters(2) = theta; // because 2*theta is periodic with pi.
			return fit_parameters;
		}

		t_real median(const Vector<t_real> &input){
			//Finds the median of a real vector x
			auto size = input.size();
			Vector<t_real> x(size);
			std::copy(input.data(), input.data() + size, x.data());
			std::sort(x.data(), x.data() + size);
			if (std::floor(size / 2) - std::ceil(size / 2) == 0)
				return (x(std::floor(size / 2) - 1) + x(std::floor(size / 2)))/ 2;
			return x(std::ceil(size /2 ));
		}

	   	t_real dynamic_range(const Image<t_complex>& model, const Image<t_complex>& residuals, const t_real& operator_norm){
	   		/*
			Returns value of noise rms given a measurement vector and signal to noise ratio
			y0:: complex valued vector before noise added
			SNR:: signal to noise ratio

			This calculation follows Carrillo et al. (2014), PURIFY a new approach to radio interferometric imaging
	   		*/
	    	return std::sqrt(model.size()) * (operator_norm * operator_norm) / residuals.matrix().norm() * model.cwiseAbs().maxCoeff();
	    }

		Array<t_complex> init_weights(const Vector<t_real>& u, const Vector<t_real>& v, 
			const Vector<t_complex>& weights, const t_real & oversample_factor, 
			const std::string& weighting_type, const t_real& R, const t_int & ftsizeu, const t_int & ftsizev)
		  {
		    /*
		      Calculate the weights to be applied to the visibilities in the measurement operator. 
		      It does none, whiten, natural, uniform, and robust.
		    */
		    Vector<t_complex> out_weights(weights.size());
		    t_complex const sum_weights = weights.sum();
		    if (weighting_type == "none")
		    {
		      out_weights = weights.array() * 0 + 1;
		    } else if (weighting_type == "whiten"){
		      out_weights = weights.array().sqrt();
		    }
		    else if (weighting_type == "natural")
		    {
		      out_weights = weights;
		    } else {
		      auto step_function = [&] (t_real x) { return 1; };
		      t_real scale = 1./oversample_factor; //scale for fov
		      Matrix<t_complex> gridded_weights = Matrix<t_complex>::Zero(ftsizev, ftsizeu);
		      for (t_int i = 0; i < weights.size(); ++i)
		      {
		        t_int q = utilities::mod(floor(u(i) * scale), ftsizeu);
		        t_int p = utilities::mod(floor(v(i) * scale), ftsizev);
		        gridded_weights(p, q) += 1; //I get better results assuming all the weights are the same. I think miriad does this.
		      }
		      t_complex const sum_grid_weights2 = (gridded_weights.array() * gridded_weights.array()).sum();
		      t_complex const sum_grid_weights = gridded_weights.array().sum();
		      t_complex const robust_scale = sum_weights/sum_grid_weights2 * 25. * std::pow(10, -2 * R); // Following standard formula, a bit different from miriad.
		      
		      for (t_int i = 0; i < weights.size(); ++i)
		      {
		        t_int q = utilities::mod(floor(u(i) * scale), ftsizeu);
		        t_int p = utilities::mod(floor(v(i) * scale), ftsizev);
		        if (weighting_type == "uniform")
		          out_weights(i) = weights(i) / gridded_weights(p, q);
		        if (weighting_type == "robust"){
		          out_weights(i) = weights(i) /(1. + robust_scale * gridded_weights(p, q));
		        }
		      }
		      out_weights = out_weights/out_weights.sum();
		    }
		    return out_weights.array();
		  }

		  Vector<t_complex> sparse_multiply_matrix(const Sparse<t_complex> & M, const Vector<t_complex> & x){
		    Vector<t_complex> y = Vector<t_complex>::Zero(M.rows());
		    //parallel sparse matrix multiplication with vector.
		    #pragma omp parallel for
		    //#pragma omp simd
			for (t_int k=0; k < M.outerSize(); ++k)
			  for (Sparse<t_complex>::InnerIterator it(M,k); it; ++it)
			  {
			  	
			    y(k) += it.value() * x(it.index());
			  }
		    return y;
		  }

		  void checkpoint_log(const std::string& diagnostic, t_int * iters, t_real * gamma){
		  	//reads a log file and returns the latest parameters
		    if (!utilities::file_exists(diagnostic)){
		    	*iters = 0;
		    	return;
		    }
		    std::ifstream log_file(diagnostic);
		    std::string entry;
		    std::string s;
		    
		    std::getline(log_file, s);
		    
		    while (log_file)
		    {
		      if (!std::getline(log_file, s)) break;
		      std::istringstream ss(s);
		      std::getline(ss, entry, ' ');
		      *iters = std::stoi(entry);
		      std::getline(ss, entry, ' ');
		      *gamma = std::stod(entry);
		    }
		   log_file.close();

		  }

	}

}
