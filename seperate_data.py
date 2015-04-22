#!/usr/bin/env python
# Separate the data in all.dta into training, probe, and qual
# sets.

data = open('all.dta')
index = open('all.idx')
train = open('train.dta', 'w')
probe = open('probe.dta', 'w')
qual = open('qual2.dta', 'w')

data_val = data.readline()
index_val = index.readline().strip()

while data_val:
    if index_val == '1' or index_val == '2' or index_val == '3':
        train.write(data_val)
    elif index_val == '4':
        probe.write(data_val)
    elif index_val == '5':
        qual.write(data_val)
    else:
        print 'Some voodoo shit happened'
        print index_val
    
    # Go to next line in each file
    data_val = data.readline()
    index_val = index.readline().strip()

if index_val:
    print 'Flies different lengths?????'

data.close()
index.close()
train.close()
probe.close()
qual.close()

