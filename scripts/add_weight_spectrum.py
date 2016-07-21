## Script to add WEIGHT_SPECTRUM column which is needed for WSclean

from pyrap import tables
import pyrap

ms_file = "0332-391.ms";
t = tables.table(ms_file, readonly=False);
t.removecols('WEIGHT_SPECTRUM');
print "Before:" 
print t.colnames();
cd1 = tables.makecoldesc('WEIGHT_SPECTRUM', t.getcoldesc('WEIGHT'))
td = tables.maketabdesc(cd1)
t.addcols(td);
tc = t.col('WEIGHT_SPECTRUM')
for i in range(len(tc)):
	tc[i] = [0];
print "After:"
print t.colnames();
t.flush()