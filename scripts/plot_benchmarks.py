import os
import string
import numpy as np
import matplotlib.pyplot as plt

def parse_file(filename, what):
    timeslist = []
    with open(filename) as f:
        for line in f:
            linelist = line.split()
            if len(linelist)>4:
                temp = int(linelist[2])
                if what!="time":
                    temp = float(linelist[8].split('B/s')[0][:-1])
                    if linelist[8].split('B/s')[0][-1:]=='k':
                        temp = temp/1000.
                timeslist.append(temp)
    return np.array(timeslist)

def prep_arrays(serial_times, parallel_times, index_s, index_p, N):
    times = [x for x in parallel_times[index_p::N]]
    times.insert(0, serial_times[index_s])
    #print(times)
    return np.array(times)

if __name__ == "__main__":
     import sys
     # Input arguments:
     # Plot walltime or data processing rate
     what = sys.argv[8]
     # File names
     serial_times = parse_file(sys.argv[1], what)
     parallel_times = parse_file(sys.argv[2], what)
     # number of benchmarking parameters
     n1 = int(sys.argv[3]) # image sizes
     n2 = int(sys.argv[4]) # visibilities sizes
     n3 = int(sys.argv[5]) # kernel sizes
     # number of benchmarked functions (eg. [ctor, direct, adjoint]=3)
     nb = int(sys.argv[6])
     # number of benchmarked ncore configurations, serial included
     nconf = int(sys.argv[7])

     ylabel = ""
     if what=="time":
         ylabel = "Time (ms or us)"
     else:
         ylabel = "Data rate (MB/s)"

     # Parameter lists etc
     im_sizes = [128, 256, 512, 1024, 2048, 4096]
     vis_sizes = ['1M', '10M']
     k_sizes = [2, 4, 8, 16]
     x = range(nconf)
     x_name = ['serial',
               '1/1',
               '1/2',
               '1/8',
               '1/12',
               '2/8',
               '2/12',
               '4/12'
     ]
     colors = ['black', 'red', 'green', 'blue']
     markers = ['o', '^', 's', 'x']
     bm_name = ['Constructor', 'Apply Direct', 'Apply Adjoint']
     npar = 0
     par_name = []
     for im in range(n1):
         for vis in range(n2):
             for k in range(n3):
                 if k_sizes[k]*k_sizes[k]<im_sizes[im]:
                     npar = npar+1
                     par_name.append('im='+str(im_sizes[im])+', vis='+vis_sizes[vis]+', k='+str(k_sizes[k]))
     N = npar*nb
                     
     for bm in range(0,nb):
         fig = plt.figure()
         fig.suptitle(bm_name[bm], fontsize=16)
         ax = fig.add_subplot(1,2,1)
         for par in range(0,npar):
             index = bm*npar + par
             # For the degrid operator parallel benchmarks, there are 2 sets of plots distr+MPI
             y = prep_arrays(serial_times, parallel_times, index, index, 2*N)
             group = par_name[par]
             ax.scatter(x, y, c=colors[par%4], s=200, marker=markers[par%4], label=group)
             plt.title('Distributed')
         plt.setp(ax.get_xticklabels(), visible=True)
         plt.xticks(range(nconf), x_name)
         plt.xlabel("# nodes/# cores per node")
         plt.ylabel(ylabel)
         ax = fig.add_subplot(1,2,2)
         for par in range(0,npar):
             index = bm*npar + N + par
             # For the degrid operator parallel benchmarks, there are 2 sets of plots distr+MPI
             y = prep_arrays(serial_times, parallel_times, index-N, index, 2*N)
             group = par_name[par]
             ax.scatter(x, y, c=colors[par%4], s=200, marker=markers[par%4], label=group)
             plt.title('MPI')
         plt.setp(ax.get_xticklabels(), visible=True)
         plt.xticks(range(nconf), x_name)
         plt.xlabel("# nodes/# cores per node")
         plt.legend()
         plt.show()
