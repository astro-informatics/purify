import csv
import h5py

names = ["input_data"]
for name in names:
    input_name = f"{name}.vis"
    h5_name = input_name[:input_name.rfind('.')] + '.h5'

    udata = []
    vdata = []
    rdata = []
    idata = []
    sdata = []
    with open(input_name, mode ='r') as file:
        csvFile = csv.reader(file)
        for line in csvFile:
            u, v, re, im, sigma = line[0].split()
            udata.append(float(u))
            vdata.append(float(u))
            rdata.append(float(u))
            idata.append(float(u))
            sdata.append(float(u))

    f = h5py.File(h5_name, 'w')
    f.create_dataset('u', data=udata)
    f.create_dataset('v', data=vdata)
    #f.create_dataset('w', data=wdata) # no w column here
    f.create_dataset('re', data=rdata)
    f.create_dataset('im', data=idata)
    f.create_dataset('sigma', data=sdata)
    f.close()

    print(f"saved {h5_name}")
