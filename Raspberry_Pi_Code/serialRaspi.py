import serial
import demoGet

def main():
    ser1 = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate=9600
    )
    ser1.isOpen()

    ser2 = serial.Serial(
        port='/dev/ttyUSB1',
        baudrate=9600
    )
    ser2.isOpen()

    print "Opened the port"

    out1 = ""
    out2 = ""
    while(1):
        outChar1 = ser1.read(1)
        try:
            if(outChar1 == "\n"):
                out1 += outChar1
                out1 = out1.strip()
                if(out1 != "FAIL" and out1 != ''): 
                    demoGet.setR1(int(out1))
                    
##                    print "trying to change r1 to " + str(out1)
                    out1 = ""
            else:
                out1 += outChar1
        except:
            pass

        try:
            outChar2 = ser2.read(1)
            if(outChar2 == "\n"):
                out2 += outChar2
                out2 = out2.strip()
                if(out2 != "FAIL" and out2 != ''):
                    demoGet.setR2(int(out2))
##                    print "trying to change r2 to " + str(out2)
                    out2 = ""
            else:
                out2 += outChar2
        except:
            pass


main()
