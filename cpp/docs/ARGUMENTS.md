Arguments {#arguments}
-----------

Important arguments:

* `--help` will print basic help information, showing what arguments are possible (more than this list).
* `--measurement_set` is the path to the CASA measurement set folder. `(required argument)`
* `--name` is the prefix name used to save the output model, residual, and dirty map. `(required argument)`
* `--diagnostic` will record variables and output images with each iteration. This is useful for testing or trial runs, but will take up some more computation in calculating and saving diagnostic updates. The update images and diagnostic file will be used as a checkpoint, in the case that purify locates the images from a previous run.
* `--l2_bound` this value can be used to scale the error on the model matching the measurements. `Default value is 1.4`.
* `--power_iterations` number of iterations needed to normalize the measurement operator. This is needed to ensure that the measurement operator reconstruct a model to the correct flux scale. `Default value is 100`.
* `--noadapt` will turn off the adapting step size.
* `--size` the height and width of the output image in pixels (only powers of 2 supported at present).
* `--cellsize` is the width and height of a pixel in arcseconds. If not specified, this is determined from the Nyquist limit.
* `--niters` max number of iterations. Default is max unsigned integer.
* `--relative_variation` the relative difference of the model between iterations needed for convergence.
* `--residual_convergence` the upper bound on the residual norm needed for convergence.
