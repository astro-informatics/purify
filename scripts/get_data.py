# Gets and treats  some data to try out purify's CASA and pyrap interfaces.
# This data is not quite adequate for testing purposes. It has W components
# which are ignored here.
# And it requires CASA itself to first treat the downloaded data (which is what
# this script does).
#
# This script is run by first launching casapy and then doing:
# >>> run -i get_data.py
import os

# needed for existence check
from os import F_OK


##########################################################################
#                                                                        
# Clear out previous run results
#rmtables('ngc4826.tutorial.*')
os.system('rm -rf ngc4826.tutorial.*')

# Sets a shorthand for the ms, not necessary
prefix='tutorial'
msfile = prefix + '.16apr98.ms'

print 'Tutorial Script for BIMASONG NGC4826 Mosaic'
print 'Version for Release 3.4.0  May-2012'
print 'Will do: import, flagging, calibration, imaging'
print ''
#
##########################################################################
#
# 
##########################################################################
#
# N4826 - BIMA SONG Data CO(1-0) 115.2712 GHz
# 16apr98
#	source=ngc4826
#	phasecal=1310+323
#	fluxcal=3c273, Flux = 23 Jy on 16apr98
#	passcal= none - data were observed with online bandpass correction.
#
# NOTE: This data has been filled into MIRIAD, line-length correction 
#	done, and then exported as separate files for each source.
#	3c273 was not line length corrected since it was observed
#	for such a short amount of time that it did not need it.  
#
# From miriad: source Vlsr = 408; delta V is 20 km/s 
#
##########################################################################
# Locate the data
##########################################################################
#
fitsdir = 'fitsfiles'
# See if this sub-directory exists
if os.access(fitsdir,F_OK):
    # already in current directory
    print "  Found "+fitsdir+" for UVFITS files"
else:
    pathname=os.environ.get('CASAPATH').split()[0]
    datapath=pathname+'/data/demo/tutorial/BIMA/NGC4826/'
    dataname='ngc4826.fitsfiles.tgz'
    datafiles = datapath+dataname
    # Path to web archive
    webfiles = 'http://casa.nrao.edu/Data/BIMA/NGC4826/'+dataname
    if os.access(dataname,F_OK):
        print '--Found data tarball in local directory--'
        print "  Using "+dataname
    elif os.access(datafiles,F_OK):
        print '--Copy data tarball to local directory--'
        print "  Using "+datafiles
        os.system("cp -r "+datafiles+" .")
        os.system('chmod -R a+wx '+datafiles)
    else:
	# try web retrieval
        print '--Trying to retrieve data from '+webfiles
        # Use curl (for Mac compatibility)
        os.system('curl '+webfiles+' -f -o '+dataname)
        # NOTE: could also use wget
        #os.system('wget '+webfiles)
        
    print '--Unpacking tarball '
    os.system('tar xzf '+dataname)
    if os.access(fitsdir,F_OK):
        # should now be in current directory
	print "  Will use "+fitsdir+" in current directory"
    else:
        raise IOError," ERROR: "+fitsdir+" not found"

##########################################################################
# Import and concatenate sources
##########################################################################
#
# USB spectral windows written separately by miriad for 16apr98
# Assumes these are in sub-directory called "fitsfiles" of working directory
print '--Importuvfits (16apr98)--'
default('importuvfits')

print "Starting from the uvfits files exported by miriad"
print "The USB spectral windows were written separately by miriad for 16apr98"

#### We could read in each of the individual fits files as example
#### below -- this works well if you only have one or two files to
#### read, but here we have many, so instead we use some useful
#### pythonease to simplify the commands.

#importuvfits(fitsfile='fitsfiles/3c273.fits5', vis='ngc4826.tutorial.3c273.5.ms')

#### Tutorial Note: For the loop to work, the high end of range must be
#### 1+ number of actual files.

for i in range(5,9):
	importuvfits(fitsfile="fitsfiles/3c273.fits"+str(i),
		vis="ngc4826.tutorial.3c273."+str(i)+".ms")

for i in range(9,17):
	importuvfits(fitsfile="fitsfiles/1310+323.ll.fits"+str(i),
		vis="ngc4826.tutorial.1310+323.ll."+str(i)+".ms")

for i in range(5,9):
	importuvfits(fitsfile="fitsfiles/ngc4826.ll.fits"+str(i),
		vis="ngc4826.tutorial.ngc4826.ll."+str(i)+".ms")
#
##########################################################################
#
print '--Concat--'
default('concat')

concat(vis=['ngc4826.tutorial.3c273.5.ms',
	    'ngc4826.tutorial.3c273.6.ms',
	    'ngc4826.tutorial.3c273.7.ms',
	    'ngc4826.tutorial.3c273.8.ms',
	    'ngc4826.tutorial.1310+323.ll.9.ms',
	    'ngc4826.tutorial.1310+323.ll.10.ms',
	    'ngc4826.tutorial.1310+323.ll.11.ms',
	    'ngc4826.tutorial.1310+323.ll.12.ms',
	    'ngc4826.tutorial.1310+323.ll.13.ms',
	    'ngc4826.tutorial.1310+323.ll.14.ms',
	    'ngc4826.tutorial.1310+323.ll.15.ms',
	    'ngc4826.tutorial.1310+323.ll.16.ms',
	    'ngc4826.tutorial.ngc4826.ll.5.ms',
	    'ngc4826.tutorial.ngc4826.ll.6.ms',
	    'ngc4826.tutorial.ngc4826.ll.7.ms',
	    'ngc4826.tutorial.ngc4826.ll.8.ms'],
       concatvis='ngc4826.tutorial.ms',
       freqtol="",dirtol="1arcsec",async=False)

##########################################################################
#
# Fix up the MS
# This ensures that the rest freq will be found for all spws. 
# NOTE: STILL NECESSARY
#
# print '--Fixing up spw rest frequencies in MS--'
vis='ngc4826.tutorial.ms'
tb.open(vis+'/SOURCE',nomodify=false)
spwid=tb.getcol('SPECTRAL_WINDOW_ID')
#spwid.setfield(-1,int)
# Had to do this for 64bit systems 08-Jul-2008
spwid.setfield(-1,'int32')
tb.putcol('SPECTRAL_WINDOW_ID',spwid)
tb.close()

##########################################################################
# 16 APR Calibration
##########################################################################
#
# List contents of MS
#
print '--Listobs--'
listobs(vis='ngc4826.tutorial.ms')

# Should see the listing included at the end of this script
#

print "There are 3 fields observed in a total of 16 spectral windows"
print "   field=0    3c273    spwids 0,1,2,3               64 chans "
print "   field=1    1310+323 spwids 4,5,6,7,8,9,10,11     32 chans "
print "   field=2~8  NGC4826  spwids 12,13,14,15           64 chans "
print ""
print "See listobs summary in logger"
#
default('flagdata')

flagdata(vis='ngc4826.tutorial.ms', mode='manual',
         spw='0~3:0;1;62;63,4~11:0;1;30;31,12~15:0;1;62;63')

print ""
print "Flagging bad correlator field 8 antenna 3&9 spw 15 all channels"
print "  timerange 1998/04/16/06:19:00.0~1998/04/16/06:20:00.0"
print ""

flagdata(vis='ngc4826.tutorial.ms', mode='manual', field='8', spw='15', antenna='3&9', 
         timerange='1998/04/16/06:19:00.0~1998/04/16/06:20:00.0')


default('flagmanager')

print "Now will use flagmanager to save a copy of the flags we just made"
print "These are named myflags"

flagmanager(vis='ngc4826.tutorial.ms',mode='save',versionname='myflags',
            comment='My flags',merge='replace')


default('setjy')

setjy(vis='ngc4826.tutorial.ms',field='0',fluxdensity=[23.0,0.,0.,0.],spw='0~3')

#
# Gain calibration
#
print '--Gaincal--'
default('gaincal')

# This should be combining all spw for the two calibrators for single
# scan-based solutions

print 'Gain calibration for fields 0,1 and spw 0~11'
print 'Using solint=inf combining over spw'
print 'Output table ngc4826.tutorial.16apr98.gcal'

gaincal(vis='ngc4826.tutorial.ms', caltable='ngc4826.tutorial.16apr98.gcal',
	field='0,1', spw='0~11', gaintype='G', minsnr=2.0,
        refant='ANT5', solint='inf', combine='spw')

#
##########################################################################
#
# Transfer the flux density scale:
#
print '--Fluxscale--'
default('fluxscale')

print ''
print 'Transferring flux of 3C273 to sources: 1310+323'
print 'Output table ngc4826.tutorial.16apr98.fcal'

fluxscale(vis='ngc4826.tutorial.ms', caltable='ngc4826.tutorial.16apr98.gcal',
	  fluxtable='ngc4826.tutorial.16apr98.fcal',
	  reference='3C273', transfer=['1310+323'])

#
# Correct the calibrater/target source data:
# Use new parm spwmap to apply gain solutions derived from spwid1
# to all other spwids... 
print '--Applycal--'
default('applycal')

print 'Applying calibration table ngc4826.tutorial.16apr98.fcal to data'

applycal(vis='ngc4826.tutorial.ms',
	 field='', spw='',
         gaintable='ngc4826.tutorial.16apr98.fcal',
	 spwmap=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0])

print "Done calibration and plotting"
#
# Split out calibrated target source and calibrater data:
#
print '--Split--'
default('split')

print 'Splitting NGC4826 data to ngc4826.tutorial.16apr98.src.split.ms'

split(vis='ngc4826.tutorial.ms',
      outputvis='ngc4826.tutorial.16apr98.src.split.ms',
      field='2~8', spw='12~15:0~63',
      datacolumn='corrected')
