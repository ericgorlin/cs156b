#!/usr/bin/env python
# Separate the data in all.dta into training, probe, and qual
# sets.

data = open('all.dta')
index = open('all.idx')
train = open('trainprobe.dta', 'w')

data_val = data.readline()
index_val = index.readline().strip()

while data_val:
    if index_val == '1' or index_val == '2' or index_val == '3' or index_val == '4':
        train.write(data_val)
    
    # Go to next line in each file
    data_val = data.readline()
    index_val = index.readline().strip()

if index_val:
    print 'Flies different lengths?????'

data.close()
index.close()
train.close()

