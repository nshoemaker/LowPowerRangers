import urllib, json
import time

B = 140
tagID = 1
r1 = []
r2 = []
arraySize = 5

def setR1(r):
    global r1
#    print "Setting r1 to ", r
    if(len(r1) >= arraySize):
        r1.pop(0)
        r1.append(r)
    else:
        r1.append(r)

def setR2(r):
    global r2
#    print "Setting r2 to ", r
    if(len(r2) >= arraySize):
        r2.pop(0)
        r2.append(r)
    else:
        r2.append(r)
    
def set_method():
    while(1):
        try:
            global r1
            global r2
            r1Avg = 0
            r2Avg = 0
            if (len(r1) > 0):
                r1Avg = float(sum(r1)) / len(r1) 
            if (len(r2) > 0):
                r2Avg = float(sum(r2)) / len(r2)
#            r1 = []
#            r2 = []
            url = "https://test-server-549.herokuapp.com/testServer/set/" + str(tagID) + "/" + str(B) + "/" + str(r1Avg) + "/" + str(r2Avg)
            print r1Avg, r2Avg
            response = urllib.urlopen(url)
#            break
        except Exception,e:
            print e
            pass
