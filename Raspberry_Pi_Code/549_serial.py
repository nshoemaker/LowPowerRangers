import serial

def main():
    ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate=9600
    )

    ser.isOpen()

    print "Opened the port"

    out = ""
    while(1):
        outChar = ser.read(1)
        if(outChar == "\n"):
            print out + outChar
        else:
            out += outChar


main()
