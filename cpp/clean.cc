#include "clean.h"

namespace purify {

	namespace clean{
		Image<t_complex> clean(MeasurementOperator & op, const utilities::vis_params & uv_vis, 
				const t_int & niters, const t_real& gain, const std::string & mode, const t_real clip){
			/*
				hogbom and sdi clean algorithm
			*/

			Vector<t_complex> residual = uv_vis.vis;
			Image<t_complex> res_image = op.grid(uv_vis.vis);
			Image<t_complex> clean_model = res_image * 0;
			Image<t_complex> temp_model = clean_model * 0;

			//should add a method to calculate clean sdi clip automatically

			for (t_int i = 0; i < niters; ++i)
			{
				//finding peak in residual image
				t_int max_x;
				t_int max_y;
				res_image = op.grid(residual);
				res_image.abs().maxCoeff(&max_y, &max_x);

				//generating clean model
				if ( mode == "hogbom")
					temp_model(max_y, max_x) = gain * res_image(max_y, max_x);
					
				if (mode == "steer"){
					t_real max = std::abs(res_image(max_y, max_x));
					temp_model = res_image;
					t_complex var = 0;
					t_complex corr = 0;
					//clipping residual map for clean model
					for (t_int i = 0; i < temp_model.size(); ++i)
					{
						if (std::abs(temp_model(i)) < max * clip)
							temp_model(i) = 0;
					}
					//need to write in correction factor for beam volume, eta
					auto dirty_model = op.grid(op.degrid(temp_model));
					t_complex eta = (res_image * dirty_model.conjugate()).sum()/(dirty_model * dirty_model.conjugate()).sum();
					if (0 < std::abs(eta) < 0.02)
						eta = 0.02 * eta / std::abs(eta); //imperical way to stop a semi-infinite loop, following miriad
					
					temp_model = eta * gain * temp_model;
				}
				//add components to clean model
				clean_model = clean_model + temp_model;
				//subtract model from data
				residual = residual - op.degrid(temp_model);
				//clear temp model for next iteration
				temp_model = temp_model * 0;
			}
			return clean_model;
		}

		Image<t_complex> restore(MeasurementOperator & op, const utilities::vis_params & uv_vis, const Image<t_complex> & clean_model){
			/*
				Produces the final image given a clean model
			*/
			FFTOperator fft;
			
			
			const t_real fwhm = 3.; //assuming 3 pixels in image domain for fwhm of point spread function
			const t_real sigma = fwhm / 2.355; // resolution of point source, determined by fwhm of point spread function
			auto gaussian = [&sigma] (t_real x) {
        		t_real a = x * sigma * purify_pi;
        		return std::sqrt(purify_pi / 2) / sigma * std::exp(-a * a * 2);
        	};
        	//constructing guassian to do convolution
        	Array<t_real> range;
        	auto x_size = clean_model.cols();
        	auto y_size = clean_model.rows();
        	range.setLinSpaced(std::max(y_size, x_size), 0.5, std::max(y_size, x_size) - 0.5);
    		const Image<t_real> point_source = (1e0 / range.segment(0, x_size).unaryExpr(gaussian)).matrix() 
    												* (1e0 / range.segment(0, y_size).unaryExpr(gaussian)).matrix().transpose();
    		
    		const Image<t_complex> clean_model_fft = fft.forward(clean_model);

    		//need to workout correction factor to multiply residuals by...
    		t_real residual_correction_factor = 1.;
    		//add convolved clean model to residual image
    		auto residual = uv_vis.vis - op.degrid(clean_model);
    		Image<t_complex> restored_image = op.grid(residual) * residual_correction_factor 
    													+ fft.inverse(clean_model_fft * point_source).array();
    		return restored_image;

		}
	}
}