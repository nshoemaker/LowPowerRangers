import urllib, json, time

tagID = 1

def setValues(B, R1, R2):
    url = "https://test-server-549.herokuapp.com/testServer/set/" + str(tagID) + "/" + str(B) + "/" + str(R1) + "/" + str(R2)
    response = urllib.urlopen(url)
    url = "https://test-server-549.herokuapp.com/testServer/get/" + str(tagID)
    response = urllib.urlopen(url)
    data = json.loads(response.read())
    print data

def runTest1():
    setValues(3,2,1)
    time.sleep(1)
    setValues(3,2,3)
    time.sleep(1)
    setValues(3,3,3)
    time.sleep(1)
    setValues(3,4,3)

def runTest2():
    B = 3
    r1 = 2.0
    r2 = 3.0
    for x in xrange(10):
        setValues(B, r1, r2)
        r1 += 0.2
        time.sleep(1)

def main():
    #runTest1()
    runTest2()

main()