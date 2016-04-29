import msparser
import numpy as np
import matplotlib.pyplot as plt
import os
import glob


opts = ['kb_interp 1.375 5', 'kb 2 4', 'kb 1.375 5']
M_N_ratios = np.arange(1, 11)/10. * 2.
i = 1
for m in M_N_ratios:
	for  o in opts:
		os.system("valgrind --tool=massif  ../build/cpp/example/gridding_mem " + o + " " +str(m))

		massif_files = glob.glob('massif.out.*')

		for f in massif_files:
			print "Creating plot for " + o
			data = msparser.parse_file(f)
			#print "# id", "time", "heap", "extra", "total", "stack"

			x = []
			y = []

			for snapshot in data['snapshots']:
			    id = snapshot['id']
			    time = snapshot['time']
			    heap = snapshot['mem_heap']
			    extra = snapshot['mem_heap_extra']
			    total = heap + extra
			    stack = snapshot['mem_stack']
			    y.append(total)
			    x.append(time)
			    #print '  '+str(id), time, heap, extra, total, stack

			plt.plot(x, np.array(y)/ 1e6)
			os.system('rm '+f)

	plt.legend(opts)
	plt.ylabel('Total Mem. (MB) for $M/N$ = ' + str(m))
	plt.xlabel('Time (not in an obvious unit of time)')
	plt.savefig("Gridding_Memory_test_" + str(i) + ".png")
	plt.clf()
	i = i + 1