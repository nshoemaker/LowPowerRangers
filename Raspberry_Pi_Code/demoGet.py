import urllib, json
import time
from threading import Timer

B = 140
tagID = 1
r1 = []
r2 = []

def setR1(r):
    global r1
#    print "Setting r1 to ", r
    r1.append(r)

def setR2(r):
    global r2
#    print "Setting r2 to ", r
    r2.append(r)
    
def set_method():
    while(1):
        try:
            global r1
            global r2
            r1Avg = float(sum(r1)) / len(r1) 
            r2Avg = float(sum(r2)) / len(r2)
            r1 = []
            r2 = []
            url = "https://test-server-549.herokuapp.com/testServer/set/" + str(tagID) + "/" + str(B) + "/" + str(r1Avg) + "/" + str(r2Avg)
            
            response = urllib.urlopen(url)
            break
        except Exception,e:
            pass
    Timer(5, set_method, ()).start()

Timer(5, set_method, ()).start()
