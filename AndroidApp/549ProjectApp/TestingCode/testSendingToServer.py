import urllib, json, time

tagID = 1

def setValues(B, R1, R2):
    url = "https://test-server-549.herokuapp.com/testServer/set/" + str(tagID) + "/" + str(B) + "/" + str(R1) + "/" + str(R2)
    response = urllib.urlopen(url)
    print response.read()
    url = "https://test-server-549.herokuapp.com/testServer/get/" + str(tagID)
    response = urllib.urlopen(url)
    data = json.loads(response.read())
    print data

def runTest1():
    setValues(300,200,100)
    time.sleep(1)
    setValues(300,200,300)
    time.sleep(1)
    setValues(300,300,300)
    time.sleep(1)
    setValues(300,400,300)

def runTest2():
    B = 300
    r1 = 200.0
    r2 = 300.0
    for x in xrange(10):
        setValues(B, r1, r2)
        r1 += 20
        time.sleep(1)

def main():
##    runTest1()
    runTest2()

main()
