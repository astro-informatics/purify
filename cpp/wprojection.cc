#include "wprojection.h"

namespace purify {
	namespace wprojection {
	  Matrix<t_complex> create_chirp_matrix(const Vector<t_real> & w_components, const t_real cell_x, const t_real cell_y, const t_int &ftsizeu, const t_int &ftsizev, const t_real& energy_fraction){
	  	FFTOperator fftop;
	    const t_int total_rows = w_components.size();
	    const t_int total_cols = ftsizeu * ftsizev;
	    //std::vector<t_tripletList> entries;
	    //entries.reserve(total_rows * total_cols);

	    Matrix<t_complex> chirp_matrix = Matrix<t_complex>::Zero(total_rows, total_cols);
	    std::cout << "Generating chirp matrix with " << total_rows << " x " << total_cols << '\n';
	    for (t_int m = 0; m < total_rows; ++m)
	    {
	      Image<t_complex> chirp_image = wprojection::generate_chirp(w_components(m), cell_x, cell_y, ftsizeu, ftsizev);
	      
	      Matrix<t_complex> row = fftop.forward(chirp_image);
	      row = wprojection::sparsify_chirp(row, energy_fraction);

	      row.resize(1, total_cols);
	      chirp_matrix.row(m) = row;
	      //for (t_int j = 0; j < row.size(); ++j)
	      //{
	        //if (std::abs(row(j)) == 0)
	        //{
	        //  entries.emplace_back(m, j, row(j));
	        //}
	      //}
	    }
	    //Sparse<t_complex> chirp_matrix(total_rows, total_cols);
	    //chirp_matrix.setFromTriplets(entries.begin(), entries.end());
	    
	    return chirp_matrix;
	  }

	    Matrix<t_complex> re_sample_ft_grid(const Matrix<t_complex>& input, const t_real& re_sample_ratio){
	    /*
	      up samples image using by zero padding the fft
	      
	      input:: fft to be upsampled, with zero frequency at (0,0) of the matrix.

	    */
	      if (re_sample_ratio == 1)
	      	return input;

	      //sets up dimensions for old image
	      t_int old_x = input.cols();
	      t_int old_y = input.rows();

	      t_int old_x_centre_floor = std::floor(input.cols() * 0.5);
	      t_int old_y_centre_floor = std::floor(input.rows() * 0.5);
	      //need ceilling in case image is of odd dimension
	      t_int old_x_centre_ceil = std::ceil(input.cols() * 0.5);
	      t_int old_y_centre_ceil = std::ceil(input.rows() * 0.5);

	      //sets up dimensions for new image
	      t_int new_x = std::floor(input.cols() * re_sample_ratio);
	      t_int new_y = std::floor(input.rows() * re_sample_ratio);

	      t_int new_x_centre_floor = std::floor(new_x * 0.5);
	      t_int new_y_centre_floor = std::floor(new_y * 0.5);
	      //need ceilling in case image is of odd dimension
	      t_int new_x_centre_ceil = std::ceil(new_x * 0.5);
	      t_int new_y_centre_ceil = std::ceil(new_y * 0.5);

	      Matrix<t_complex> output = Matrix<t_complex>::Zero(new_y, new_x);

	      t_int box_dim_x;
	      t_int box_dim_y;


	      //now have to move each quadrant into new grid
	      box_dim_x = std::min(old_x_centre_floor, new_x_centre_floor);
	      box_dim_y = std::min(old_y_centre_floor, new_y_centre_floor);   
	      //(0, 0)
	      output.block(0, 0, box_dim_y, box_dim_x) = input.block(0, 0, box_dim_y, box_dim_x);

	      box_dim_x = std::min(old_x_centre_floor, new_x_centre_floor);
	      box_dim_y = std::min(old_y_centre_ceil, new_y_centre_ceil);   
	      //(0, y0)
	      output.block(new_y - box_dim_y, 0, box_dim_y, box_dim_x) = input.block(old_y - box_dim_y, 0, box_dim_y, box_dim_x);

	      box_dim_x = std::min(old_x_centre_ceil, new_x_centre_ceil);
	      box_dim_y = std::min(old_y_centre_floor, new_y_centre_floor);   
	      //(x0, 0)
	      output.block(0, new_x - box_dim_x, box_dim_y, box_dim_x) = input.block(0, old_y - box_dim_x, box_dim_y, box_dim_x);

	      box_dim_x = std::min(old_x_centre_ceil, new_x_centre_ceil);
	      box_dim_y = std::min(old_y_centre_ceil, new_y_centre_ceil);  
	      //(x0, y0)
	      output.block(new_y - box_dim_y, new_x - box_dim_x, box_dim_y, box_dim_x) = input.block(old_y - box_dim_y, old_y - box_dim_x, box_dim_y, box_dim_x);

	      return output;
	  }
	  Image<t_complex> sparsify_chirp(const Image<t_complex>& row, const t_real& energy_fraction){
	    /*
	      Takes in fourier transform of chirp, and returns sparsified version
	      row:: input fourier transform of chirp
	      energy_fraction:: how much energy to keep after sparsifying 
	    */
	      //there is probably a way to get eigen to do this without a loop
	      if (energy_fraction == 1) 
	      	return row;
	      t_real tau = 0.5;
	      t_real old_tau = -1;
	      t_int niters = 100;
	      Image<t_real> abs_row = row.cwiseAbs();
	      t_real abs_row_max = abs_row.maxCoeff();
	      t_real abs_row_total_energy = (abs_row * abs_row).sum();

	      t_real min_tau = 0;
	      t_real max_tau = 1;
	      //calculating threshold
	      for (t_int i = 0; i < niters; ++i)
	      {
	        t_real energy_sum = 0;
	        for (t_int i = 0; i < abs_row.size(); ++i)
	        {
	          if (abs_row(i)/abs_row_max > tau)
	          {
	            energy_sum = energy_sum + abs_row(i) * abs_row(i) / abs_row_total_energy;
	          }
	        }

	        old_tau = tau;
	        if (energy_sum >= energy_fraction)
	        {
	          min_tau = tau;
	        }else{
	          max_tau = tau;
	        }
	        tau = (max_tau - min_tau) * 0.5 + min_tau;
	        //std::cout << energy_sum << '\n';
	        if (std::abs(tau - old_tau)/tau < 1e-6 and energy_sum > energy_fraction)
	        {
	        	tau = old_tau;
	        	break;
	        }
	        

	      }

	      Image<t_complex> output_row = Image<t_complex>::Zero(row.rows(), row.cols());
	      t_real final_energy = 0;
	      //performing clipping
	      for (t_int i = 0; i < abs_row.size(); ++i)
	      {
	        if (abs_row(i)/abs_row_max > tau)
	        {
	          output_row(i) = row(i);
	          final_energy = final_energy + abs_row(i) * abs_row(i);
	        }
	      }
	      //std::cout << "Final energy:" << '\n';
	      //std::cout << final_energy / abs_row_total_energy << '\n';
	      return output_row;
	  }

	  Image<t_complex> generate_chirp(const t_real w_term, const t_real cell_x, const t_real cell_y, const t_int x_size, const t_int y_size){
	    /*
	      return chirp image fourier transform for w component

	      w:: w-term in units of lambda
	      celly:: size of y pixel in arcseconds
	      cellx:: size of x pixel in arcseconds
	      x_size:: number of pixels along x-axis
	      y_size:: number of pixels along y-axis

	    */
	    const t_real theta_FoV_L = cell_x * x_size;
	    const t_real theta_FoV_M = cell_y * y_size;

	    const t_real L = 2 * std::sin(purify_pi / 180.* theta_FoV_L / (60. * 60.) * 0.5);
	    const t_real M = 2 * std::sin(purify_pi / 180.* theta_FoV_M / (60. * 60.) * 0.5);

	    const t_real delt_x = L / x_size;
	    const t_real delt_y = M / y_size;

	    Image<t_complex> chirp(y_size, x_size);
	    t_complex I(0, 1);
	    for (t_int l = 0; l < x_size; ++l)
	    {
	      for (t_int m = 0; m < y_size; ++m)
	      {
	        t_real x = (l + 0.5 - x_size * 0.5) * delt_x;
	        t_real y = (m + 0.5 - y_size * 0.5) * delt_y;

	        chirp(m, l) = std::exp(- 2 * purify_pi * I * w_term * (std::sqrt(1 - x*x - y*y) - 1)) * std::exp(- 2 * purify_pi * I * (l * 0.5 + m * 0.5));
	      }
	    }
	    return chirp/chirp.size();
	  }

	Sparse<t_complex> convolution(const Sparse<t_complex> & input_gridding_matrix, const Image<t_complex>& Chirp, const t_int& Nx, const t_int& Ny, const t_int& Nvis){

			const Sparse<t_complex> & Grid = input_gridding_matrix.transpose();
	        std::cout << "Convolving Gridding matrix with Chirp" << std::endl;
	        std::cout << "Nx = " << Nx << " Ny = " << Ny << std::endl;
	        std::cout << Chirp.rows() << " " << Chirp.cols() << std::endl;
	        t_int Npix = Nx * Ny;
	        Sparse<t_complex> newG(Nvis, Npix);
	        Image<t_complex> Gtemp_mat(Nx, Ny);

	        typedef Eigen::Triplet<t_complex> T;
	        std::vector<T> tripletList;
	        
	        t_int sparsity = 0;
	        
	        for(t_int m = 0; m < Nvis; m++){//chirp->M
	        	
	            //loop over every pixels
	            for(t_int i = 0; i < Nx; i++){//nx
	                for(t_int j = 0; j < Ny; j++){ //ny
	                    t_complex  Gtemp0 (0.0,0.0);
	                    t_complex  chirptemp (0.0,0.0);

	                    //only loop over the non-zero gmat elements
	                    for (Sparse<t_complex>::InnerIterator pix(Grid,m); pix; ++pix){
	                        //express the column index as two-dimensional indices in image plane
	                        t_int ii, jj, i_fftshift, j_fftshift;

	                        Vector<t_int> image_row_col = utilities::ind2sub(pix.index(), Nx, Ny); 
	                        ii = image_row_col(0);
	                        jj = image_row_col(1);

	                        if(ii < Nx/2) i_fftshift = ii + Nx / 2;
	                        if(ii >= Nx/2) i_fftshift = ii - Nx / 2;
	                        if(jj < Ny/2) j_fftshift = jj + Ny/2;
	                        if(jj >= Ny/2) j_fftshift = jj - Ny/2;
	                        t_int oldpixi, oldpixj;
	                        
	                        //translate the chirp matrix for m to center on the pixel (i,j)
	                        //store old pixel indices of Chirp 
	                        oldpixi = Nx/2 - i + i_fftshift;
	                        oldpixj = Ny/2 - j + j_fftshift;
	                       
	                        //index of the chirp which translates to (ii,jj)
	                        t_int chirppixindex = oldpixi * Ny + oldpixj;
	                        //only add if within the overlap between chirp and Gmat
	                        //no circular convolution

	                        if(oldpixi >= 0 && oldpixi < Nx){
	                            if(oldpixj >= 0 && oldpixj < Ny){
							                    chirptemp = Chirp(m, chirppixindex);
	                                Gtemp0 = Gtemp0 + ( pix.value() * chirptemp );

	                            }
	                        }        
	                    }
	                    if (std::abs(Gtemp0) < 1e-13)
	                    	sparsity++;
	                    Gtemp_mat(i,j) = Gtemp0;        
	                }
	            }
	            
	            tripletList.reserve(sparsity);
	            for(t_int i=0; i<Nx; i++){
	                for(t_int j=0; j<Ny; j++){
	                    t_int ii, jj;
	                    if(i >= Nx/2) ii = i - Nx/2;
	                    if(i < Nx/2) ii = i + Nx/2;
	                    if(j >= Ny/2) jj = j - Ny/2;
	                    if(j < Ny/2) jj = j + Ny/2;
	    
	                    if(abs(Gtemp_mat(i, j)) > 1e-13){
	                        tripletList.push_back(T(m,utilities::sub2ind(ii,jj,Nx,Ny),Gtemp_mat(i, j)));
	                    }
	                } 
	            }

	        }

	        std::cout<<"---- After convolution  ---- "<<std::endl;

	    newG.setFromTriplets(tripletList.begin(), tripletList.end());

	    
	    return newG;    

	    }

	    t_real upsample_ratio(const utilities::vis_params& uv_vis, const t_real& cell_x, const t_real& cell_y, const t_int& x_size, const t_int& y_size){
	        /*
	         returns the upsampling (in Fourier domain) ratio
	         */

	        const t_real theta_FoV_L = cell_x * x_size;
	    	const t_real theta_FoV_M = cell_y * y_size;

	    	const t_real L = 2 * std::sin(purify_pi / 180.* theta_FoV_L / (60. * 60.) * 0.5);
	    	const t_real M = 2 * std::sin(purify_pi / 180.* theta_FoV_M / (60. * 60.) * 0.5);

	    	const t_real FoV = std::max(L, M);
	        
	        const Vector<t_real> & w = uv_vis.w.cwiseAbs();
	        const Vector<t_real> uv_dist = (uv_vis.u.array() * uv_vis.u.array() + uv_vis.v.array() * uv_vis.v.array()).sqrt();
	        
	        const Vector<t_real> bandwidth_up_vector = uv_dist + w * FoV * 0.5;
	        
	        const t_real bandwidth_up = bandwidth_up_vector.maxCoeff();

	    	const t_real bandwidth = uv_dist.maxCoeff();

	        const t_real ratio = bandwidth_up / bandwidth;
	        return ratio;
	    }
	}

}



