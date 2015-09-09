import test as comm
import time
import serial

portname='COM1'

with serial.Serial(portname,comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:
    print(comm.readParams(port,1,8,2))

    #t = round(time.time())
    #print(comm.writeParams(port,1,10,2,[t&0xFFFF,(t>>16)&0xFFFF]))
