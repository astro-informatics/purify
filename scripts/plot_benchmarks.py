import os
import string
import numpy as np
import matplotlib.pyplot as plt

def parse_file(filename):
    timeslist = []
    meanlist = []
    stddevlist = []
    with open(filename) as f:
        for line in f:
            linelist = line.split()
            if linelist[0].split('/')[-1]=='manual_time':
                timeslist.append(int(linelist[2]))
            if linelist[0].split('/')[-1]=='manual_time_mean':
                meanlist.append(int(linelist[2]))
            if linelist[0].split('/')[-1]=='manual_time_stddev':
                stddevlist.append(int(linelist[2]))
    return np.array(timeslist), np.array(meanlist), np.array(stddevlist)

def prep_arrays(serial_times, parallel_times, index_s, index_p, N):
    times = [x for x in parallel_times[index_p::N]]
    times.insert(0, serial_times[index_s])
    # print(times)
    return np.array(times)

if __name__ == "__main__":
     import sys
     # Input arguments:
     # File names
     serial_times, serial_means, serial_stddevs = parse_file(sys.argv[1])
     parallel_times, parallel_means, parallel_stddevs = parse_file(sys.argv[2])
     # benchmarking parameters
     n1 = int(sys.argv[3]) # number of image sizes
     n01 = int(sys.argv[4]) # index of first image size
     n2 = int(sys.argv[5]) # number of visibilities sizes
     n02 = int(sys.argv[6]) # index of first visibility size
     n3 = int(sys.argv[7]) # number of kernel sizes
     n03 = int(sys.argv[8]) # index of first kernel size
     # number of benchmarked functions (eg. [ctor, direct, adjoint]=3)
     nb = int(sys.argv[9])
     # number of benchmarked algorithms (eg. for the measurement operator, [distr, MPI]=2)
     na = int(sys.argv[10])
     # number of benchmarked ncore configurations, serial included
     nconf = int(sys.argv[11])

     ylabel = "Time (ms or us)"

     # Parameter lists etc
     im_sizes = [128, 256, 512, 1024, 2048, 4096]
     vis_sizes = ['1M', '10M']
     k_sizes = [2, 4, 8, 16]
     x = range(nconf)
     x_name = ['serial',
               '1',
               '2',
               '3',
               '4',
               '8',
               '12'
     ]
     colors = ['black', 'red', 'green', 'blue']
     markers = ['o', '^', 's', 'x']
     bm_name = ['Constructor', 'Apply Direct', 'Apply Adjoint']
     al_name = ['Distributed', 'MPI']
     npar = 0
     par_name = []
     for im in range(n01,n01+n1):
         for vis in range(n02,n02+n2):
             for k in range(n03,n03+n3):
                 if k_sizes[k]*k_sizes[k]<im_sizes[im]:
                     npar = npar+1
                     par_name.append('im='+str(im_sizes[im])+', vis='+vis_sizes[vis]+', k='+str(k_sizes[k]))
     N = npar*nb
                     
     for bm in range(0,nb):
         fig = plt.figure()
         fig.suptitle(bm_name[bm], fontsize=16)

         for al in range(1,na+1):
             print(al,na)
             ax = fig.add_subplot(1,na,al)
             for par in range(0,npar):
                 index = bm*npar + (al-1)*N + par
                 y = prep_arrays(serial_means, parallel_means, index-(al-1)*N, index, na*N)
                 err = prep_arrays(serial_stddevs, parallel_stddevs, index-(al-1)*N, index, na*N)
                 print(x, y, err) 
                 group = par_name[par]
                 ax.errorbar(x, y, yerr=err, c=colors[par%4], fmt='o', label=group)
                 plt.title(al_name[al-1])
             plt.setp(ax.get_xticklabels(), visible=True)
             plt.xticks(range(nconf), x_name)
             plt.xlabel("# nodes (with 16 threads per node)")
             plt.ylabel(ylabel)
         plt.legend()
         plt.show()
