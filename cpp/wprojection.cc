#include "purify/config.h"
#include "logging.h"
#include "types.h"
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
	    PURIFY_HIGH_LOG("Generating {} by {} chirp matrix", total_rows, total_cols);
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
	      PURIFY_DEBUG("Sparsify chirp-matrix final energy: {}", final_energy / abs_row_total_energy);
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

	    const t_real L = 2 * std::sin(constant::pi / 180.* theta_FoV_L / (60. * 60.) * 0.5);
	    const t_real M = 2 * std::sin(constant::pi / 180.* theta_FoV_M / (60. * 60.) * 0.5);

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

	        chirp(m, l) = std::exp(- 2 * constant::pi * I * w_term * (std::sqrt(1 - x*x - y*y) - 1)) * std::exp(- 2 * constant::pi * I * (l * 0.5 + m * 0.5));
	      }
	    }
	    return chirp/chirp.size();
	  }

	Sparse<t_complex> convolution(const Sparse<t_complex> & input_gridding_matrix, const Image<t_complex>& Chirp, const t_int& Nx, const t_int& Ny, const t_int& Nvis){

			const Sparse<t_complex> & Grid = input_gridding_matrix.transpose();
      PURIFY_LOW_LOG("Convolving {} by {} gridding matrix with {} by {} Chirp", Nx, Ny,
          Chirp.rows(), Chirp.cols());
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

          PURIFY_DEBUG("---- After convolution  ---- ");

	    newG.setFromTriplets(tripletList.begin(), tripletList.end());

	    
	    return newG;    

	    }

	    t_real upsample_ratio(const utilities::vis_params& uv_vis, const t_real& cell_x, const t_real& cell_y, const t_int& x_size, const t_int& y_size){
	        /*
	         returns the upsampling (in Fourier domain) ratio
	         */

	        const t_real theta_FoV_L = cell_x * x_size;
	    	const t_real theta_FoV_M = cell_y * y_size;

	    	const t_real L = 2 * std::sin(constant::pi / 180.* theta_FoV_L / (60. * 60.) * 0.5);
	    	const t_real M = 2 * std::sin(constant::pi / 180.* theta_FoV_M / (60. * 60.) * 0.5);

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



