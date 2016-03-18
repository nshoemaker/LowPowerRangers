import urllib, json

def set_method():
    tagID = raw_input("Enter the tag ID")
    B = raw_input("Enter the distance between two tags")
    r1 = raw_input("Enter distance between anchor 1 and tag")
    r2 = raw_input("Enter distance between anchor 2 and tag")
    url = "https://test-server-549.herokuapp.com/testServer/set/" + tagID + "/" + B + "/" + r1 + "/" + r2
    urllib.urlopen(url)

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

read_from_cli()
