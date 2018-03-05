#!/usr/bin/python
import json
import optparse

usage = 'usage: %prog [options]'
parser = optparse.OptionParser(usage)
parser.add_option('-m', '--method',
                  dest    = 'method',   
                  default = '',
                  help    = 'provide method')
parser.add_option('--row',
                  dest    = 'row',   
                  default = 0,
                  type    = int,
                  help    ='provide row')
parser.add_option('--column',
                  dest    ='column', 
                  default = 0,
                  type    = int,
                  help    = 'provide column')
parser.add_option('-j', 
                  dest    = 'json',
                  default = '',
                  help    = 'provide the json file')
(opt, args) = parser.parse_args()

samplesList=[]
if opt.json:
    jsonList = opt.json.split(',')
    for jsonPath in jsonList:
        jsonFile = open(jsonPath, 'r')
        samplesList += json.load(jsonFile, encoding = 'utf-8').items()
        jsonFile.close()

if opt.method == 'length':
    print len(samplesList)

ind = 0
if opt.method == 'tag':
    for tag, sample in samplesList:
        if ind == opt.row:
            print tag
            break
        ind += 1

if opt.method == 'sample_element':
    for tag, sample in samplesList:
        if ind == opt.row:
            print sample[opt.column]
            break
        ind += 1

        
