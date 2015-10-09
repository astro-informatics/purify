====== 
PURIFY 
======

This branch of PURIFY is not for public dissemination


====== 
Prerequisite Matlab Tools
======

The Matlab Purify examples need to be linked to the matlab versions of SOPT and NUFFT.

Matlab SOPT can be found within SOPT, under sopt/matlab.

The matlab script for NUFFT can be found at http://web.eecs.umich.edu/~fessler/code/index.html. Download and extract http://web.eecs.umich.edu/~fessler/irt/fessler.tgz.

At the top of the Purify examples, you link Purify to your locations of SOPT and NUFFT. For example:

		%Linking Sopt
		addpath ../sopt/matlab/test_images/
		addpath ../sopt/matlab/misc/
		addpath ../sopt/matlab/prox_operators/
		addpath ../sopt/matlab
		%Linking Nufft
		addpath ../../Software/irt/nufft
		addpath ../../Software/irt/utilities
		addpath ../../Software/irt/systems