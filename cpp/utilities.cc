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
      for (int k = 0; k < b.rows(); ++k)
      {
        output(k, l) = (a * b.block(k, l, a_y, a_x)).sum();
      }
    }

    return output;
  }

    Matrix<t_complex> re_sample_ft_grid(const Matrix<t_complex>& input, const t_real& re_sample_ratio){
    /*
      up samples image using by zero padding the fft
      
      input:: fft to be upsampled, with zero frequency at (0,0) of the matrix.

    */


      //sets up dimensions for old image
      t_int old_x = input.cols();
      t_int old_y = input.rows();

      t_int old_x_centre_floor = floor(input.cols() * 0.5);
      t_int old_y_centre_floor = floor(input.rows() * 0.5);
      //need ceilling in case image is of odd dimension
      t_int old_x_centre_ceil = ceil(input.cols() * 0.5);
      t_int old_y_centre_ceil = ceil(input.rows() * 0.5);

      //sets up dimensions for new image
      t_int new_x = floor(input.cols() * re_sample_ratio);
      t_int new_y = floor(input.rows() * re_sample_ratio);

      t_int new_x_centre_floor = floor(new_x * 0.5);
      t_int new_y_centre_floor = floor(new_y * 0.5);
      //need ceilling in case image is of odd dimension
      t_int new_x_centre_ceil = ceil(new_x * 0.5);
      t_int new_y_centre_ceil = ceil(new_y * 0.5);

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
      w_prog:: input fourier transform of chirp
      energy_fraction:: how much energy to keep after sparsifying 
    */
      //there is probably a way to get eigen to do this without a loop
      if ( energy_fraction == 1) 
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
        std::cout << energy_sum << '\n';
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
      std::cout << "Final energy:" << '\n';
      std::cout << final_energy / abs_row_total_energy << '\n';
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

Sparse<t_complex> convolution(const Sparse<t_complex> & Grid, const Image<t_complex>& Chirp, const t_int& Nx, const t_int& Ny, const t_int& Nvis){

        std::cout<<"Convolving Gridding matrix with Chirp"<<std::endl;
        std::cout<<"Nx = "<<Nx<<" Ny = "<<Ny<<std::endl;
        std::cout<<Chirp.rows()<<" "<<Chirp.cols()<<std::endl;
        t_int Npix=Nx*Ny;
        Sparse<t_complex> newG(Nvis, Npix);
        Image<t_complex> Gtemp_mat(Nx, Ny);

        typedef Eigen::Triplet<t_complex> T;
        std::vector<T> tripletList;

        tripletList.reserve(Nvis*Npix);

        for(int m=0; m<Nvis; m++){//chirp->M

            if(m%100==0)    std::cout<<"In vis = "<<m<<std::endl;

            //loop over every pixel

            for(int i=0; i<Nx; i++){//nx
                for(int j=0; j<Ny; j++){ //ny
                    t_complex  Gtemp0 (0.0,0.0);
                    t_complex  chirptemp (0.0,0.0);

                    //only loop over the non-zero gmat elements
                    for (Eigen::SparseMatrix<t_complex>::InnerIterator pix(Grid,m); pix; ++pix){
                    //for(int pix=from; pix<to; pix++ ){
                        //std::cout<<i<<" "<<j<<"  "<<pix.row()<<"  "<<pix.col()<<"  " <<pix.index()<<std::endl;
                        //express the column index as two-dimensional indices in image plane
                        t_int ii, jj, i_fftshift, j_fftshift;

                        jj=pix.row()% Ny; 
                        ii= (pix.row() - jj)/Ny;

                        if(ii<Nx/2) i_fftshift=ii+Nx/2;
                        if(ii>=Nx/2) i_fftshift=ii-Nx/2;
                        if(jj<Ny/2) j_fftshift=jj+Ny/2;
                        if(jj>=Ny/2) j_fftshift=jj-Ny/2;
                        

                        t_int oldpixi, oldpixj;
                        
                        //translate the chirp matrix for m to center on the pixel (i,j)
                        //store old pixel indices of Chirp 
                        oldpixi=Nx/2-i+i_fftshift;
                        oldpixj=Ny/2-j+j_fftshift;
                        //std::cout<<oldpixi<<" "<<oldpixj<<std::endl;
                        //index of the chirp which translates to (ii,jj)
                        t_int chirppixindex= oldpixi*Ny+oldpixj;


                
                        
                        //only add if within the overlap between chirp and Gmat
                        //no circular convolution

                        if(oldpixi>=0 && oldpixi<Nx){
                            if(oldpixj>=0 && oldpixj<Ny){
						        chirptemp=Chirp(m, chirppixindex);
                                Gtemp0=Gtemp0 + ( pix.value() * chirptemp );

                            }
                        }        
                    }
                    Gtemp_mat(i,j)=Gtemp0;        
                }
            }
            if(m%100==0) std::cout<<"After loop in vis = "<<m<<std::endl;

            for(int i=0; i<Nx; i++){
                for(int j=0; j<Ny; j++){
                    int ii, jj;
                    if(i>=Nx/2) ii=i-Nx/2;
                    if(i<Nx/2) ii=i+Nx/2;
                    if(j>=Ny/2) jj=j-Ny/2;
                    if(j<Ny/2) jj=j+Ny/2;
    
                    if(abs(Gtemp_mat(i, j))!=0.0){
                        tripletList.push_back(T(m,ii*Ny+jj,Gtemp_mat(i, j)));
                    }
                } 
            }

        }


    newG.setFromTriplets(tripletList.begin(), tripletList.end());

    
    if(newG.isApprox(Grid, 1e-13)){ 
        std::cout<<"Convolution works"<<std::endl;
    }else std::cout<<"Convolution does not work"<<std::endl;
    
    return newG;    

    }
    t_real upsample_ratio(const utilities::vis_params& uv_vis, const t_real& field_of_view_x, const t_real& field_of_view_y){
        /*
         returns the upsampling (in Fourier domain) ratio
         */
        const Vector<t_real> & u = uv_vis.u.cwiseAbs();
        const Vector<t_real> & v = uv_vis.v.cwiseAbs();
        const Vector<t_real> & w = uv_vis.w.cwiseAbs();
        Vector<t_real> um(2);
        t_real umax = u.maxCoeff();
        t_real vmax = v.maxCoeff();
        um(0) = umax; um(1) = vmax;
        t_real bandwidth = 2 * um.maxCoeff() ;
        
        Vector<t_real> bandwidth_vx= 2 * ( u + w * field_of_view_x * 0.5);
        Vector<t_real> bandwidth_vy= 2 * ( v + w * field_of_view_y * 0.5);
        
        t_real bx = bandwidth_vy.maxCoeff();
        t_real by = bandwidth_vx.maxCoeff();
        Vector<t_real> um_up(2);
        um_up(0) = bx; um_up(1) = by;
        t_real bandwidth_up = 2 * um_up.maxCoeff() ;
        t_real ratio = bandwidth_up / bandwidth;
        std::cout << "bandwidth" << ratio;
        return ratio;
    }
	}
}