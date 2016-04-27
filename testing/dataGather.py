import urllib2
import json
import csv
from time import sleep

NUM_POINTS = 50

def readInput(prompt):
    s = raw_input(prompt)
    return float(s)

def getTopRow(d):
    for i in d:
        yield '% R error(' + str(i) + ")"
        yield 'theta error(' + str(i) + ")"

def getDataRow(d, i):
    for b in d:
        if i < len(d[b]['rs']):
            yield d[b]['rs'][i]
            yield d[b]['thetas'][i]
        else:
            yield ""
            yield ""

with open('data.csv', 'wb') as csvfile:
    csvwriter = csv.writer(csvfile)
    dataDict = {}
    testNum = 0
    while True:
        try:
            realR = readInput('Real R: ')
            realTh = readInput('Real Theta: ')
            b = readInput('Baseline: ')
            if b not in dataDict:
                dataDict[b] = {}
                dataDict[b]['rs'] = []
                dataDict[b]['thetas'] = []

            successes = 0
            while (successes < NUM_POINTS):
                try:
                    data = json.load(urllib2.urlopen('https://test-server-549.herokuapp.com/testServer/get/1/'))
                    dataDict[b]['rs'] += [100 * abs(data['R'] - realR) / realR]
                    dataDict[b]['thetas'] += [abs(data['theta'] - realTh)]
                    successes += 1
		    sleep(.2)
                except Exception:
                    pass
        except Exception:
            break
        testNum += 1

    topRow = list(getTopRow(dataDict))
    csvwriter.writerow(topRow)
    numRows = max([len(dataDict[b]['rs']) for b in dataDict])
    for i in xrange(numRows):
        row = list(getDataRow(dataDict, i))
        csvwriter.writerow(row)
