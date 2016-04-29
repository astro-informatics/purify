import msparser
import numpy as np
import matplotlib.pyplot as plt
import os
import glob


opts = ['kb_interp 1.375', 'kb 2']
for  o in opts:
	os.system("valgrind --tool=massif  ./cpp/example/gridding_mem " + o)

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

		plt.plot(x, np.array(y)/ 1e9)
		os.system('rm '+f)

plt.legend(opts)
plt.ylabel('Total Mem. (GB)')
plt.xlabel('Time (not in an obvious unit of time)')
plt.show()