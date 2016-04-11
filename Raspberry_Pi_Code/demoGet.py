import urllib, json

B = 140
tagID = 1
r1 = 0
r2 = 0

def setR1(r):
    global r1
    r1 = r
    print "R1 changed to " + str(r)
    set_method()

def setR2(r):
    global r2
    r2 = r
    print "R2 changed to " + str(r)
    set_method()
    
def set_method():
    ## tagID = raw_input("Enter the tag ID")
    ## B = raw_input("Enter the distance between two tags")
    ## r1 = raw_input("Enter distance between anchor 1 and tag")
    ## r2 = raw_input("Enter distance between anchor 2 and tag")
    
    url = "https://test-server-549.herokuapp.com/testServer/set/" + str(tagID) + "/" + str(B) + "/" + str(r1) + "/" + str(r2)
    print str(B) + "/" + str(r1) + "/" + str(r2)
    response = urllib.urlopen(url)
    print response.read()

def get_method():
    tagID = raw_input("Enter the tag ID")
    url = 'https://test-server-549.herokuapp.com/testServer/get/' + tagID
    response = urllib.urlopen(url)
    data = json.loads(response.read())
    print data

def read_from_cli():
    while(1):
        print "Enter either set or get -"
        command = raw_input()
        if command == "set":
            set_method()
        elif command == "get":
            get_method()
        else:
            print "Your input is invalid"

#read_from_cli()
