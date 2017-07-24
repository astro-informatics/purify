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
    print(np.array(timeslist))
    return np.array(timeslist)

def prep_arrays(serial_times, parallel_times, index, N):
    #times = [serial_times[index], parallel_times[index], parallel_times[index+N]]
    times = [x for x in parallel_times[index::N]]
    times.insert(0, serial_times[index])
    print(times)
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
     N = n1*n2*n3*nb
     # number of benchmarked ncore configurations: serial not included
     nconfP = int(sys.argv[7])
     # total number of configurations = 1 (for serial) + 2*nconf (for parallel)
     # The *2 is for the degrid operator benchmarks: distr+MPI
     nconf = 1 + nconfP*2
     x = range(nconf)
     x_name = ['serial',
               'distr/1', 'MPI/1',
               'distr/2', 'MPI/2',
               'distr/8', 'MPI/8',
               'distr/12', 'MPI/12',
               'distr/16', 'MPI/16',
               'distr/24', 'MPI/24',
               'distr/48', 'MPI/48'
     ]
     colors = ['black', 'red', 'green', 'blue']
     markers = ['o', '^', 's']
     bm_name = ['ctor ', 'dir ', 'adj ']
     par_name = ['im=128, vis=1000, k=2',
                 'im=128, vis=1000, k=4',
                 'im=256, vis=1000, k=2',
                 'im=256, vis=1000, k=4'
     ]
     for bm in range(0,nb):
         fig = plt.figure()
         ax = fig.add_subplot(1,1,1)
         for par in range(0,npar):
             index = bm*npar + par
             y = prep_arrays(serial_times, parallel_times, index, N)
             group = bm_name[bm] + par_name[par]
             ax.scatter(x, y, c=colors[par], marker=markers[bm], label=group)
         plt.setp(ax.get_xticklabels(), visible=True)
         plt.xticks(range(nconf), x_name)
         plt.xlabel("parallelization scheme/number of cores")
         plt.ylabel("time (ms or us)")
         plt.legend()
         plt.show()
