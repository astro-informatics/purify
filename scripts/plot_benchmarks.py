import os
import string
import numpy as np
import matplotlib.pyplot as plt

def parse_file(filename):
    timeslist = []
    with open(filename) as f:
        for line in f:
            linelist = line.split()
            if len(linelist)>4:
                timeslist.append(int(linelist[2]))
    #print(np.array(timeslist))
    return np.array(timeslist)

def prep_arrays(serial_times, parallel_times, index_s, index_p, N):
    times = [x for x in parallel_times[index_p::N]]
    times.insert(0, serial_times[index_s])
    #print(times)
    return np.array(times)

if __name__ == "__main__":
     import sys
     serial_times = parse_file(sys.argv[1])
     parallel_times = parse_file(sys.argv[2])
     # number of benchmarking parameters
     n1 = int(sys.argv[3])
     n2 = int(sys.argv[4])
     n3 = int(sys.argv[5])
     npar = n1*n2*n3
     # number of benchmarked functions
     nb = int(sys.argv[6])
     N = npar*nb
     # number of benchmarked ncore configurations, serial included
     nconf = int(sys.argv[7])
     # For the degrid operator benchmarks, there are 2 sets of plots distr+MPI
     x = range(nconf)
     x_name = ['serial',
               '1',
               '2',
               '8',
               '12',
               '16',
               '24',
               '48'
     ]
     colors = ['black', 'red', 'green', 'blue']
     markers = ['o', '^', 's', 'x']
     bm_name = ['Constructor', 'Apply Direct', 'Apply Adjoint']
     par_name = ['im=128, vis=1000, k=2',
                 'im=128, vis=1000, k=4',
                 'im=256, vis=1000, k=2',
                 'im=256, vis=1000, k=4'
     ]
     for bm in range(0,nb):
         fig = plt.figure()
         fig.suptitle(bm_name[bm], fontsize=16)
         ax = fig.add_subplot(1,2,1)
         for par in range(0,npar):
             index = bm*npar + par
             y = prep_arrays(serial_times, parallel_times, index, index, 2*N)
             group = par_name[par]
             ax.scatter(x, y, c=colors[par], s=200, marker=markers[par], label=group)
             plt.title('Distributed')
         plt.setp(ax.get_xticklabels(), visible=True)
         plt.xticks(range(nconf), x_name)
         plt.xlabel("number of cores")
         plt.ylabel("time (ms or us)")
         ax = fig.add_subplot(1,2,2)
         for par in range(0,npar):
             index = bm*npar + N + par
             y = prep_arrays(serial_times, parallel_times, index-N, index, 2*N)
             group = par_name[par]
             ax.scatter(x, y, c=colors[par], s=200, marker=markers[par], label=group)
             plt.title('MPI')
         plt.setp(ax.get_xticklabels(), visible=True)
         plt.xticks(range(nconf), x_name)
         plt.xlabel("number of cores")
         plt.legend()
         plt.show()
