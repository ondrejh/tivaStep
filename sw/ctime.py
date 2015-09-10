import test as comm
import time
import serial

portname='COM1'
modbus_adr=1

RTC_SET_ADDRESS = 8
RTC_TIME_ADDRESS = 10

def getTime(port,adr):

    answ = comm.readParams(port,adr,RTC_TIME_ADDRESS,3)
    try:
        s = (answ[1][0] | (answ[1][1]<<16)) + (answ[1][2]/0x8000)
        return s
    except:
        return answ

def setTime(port,adr,time):

    answ = comm.writeParams(port,adr,RTC_SET_ADDRESS,2,[time&0xFFFF,(time>>16)&0xFFFF])
    return answ

with serial.Serial(portname,comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:

    #print(setTime(port,modbus_adr,round(time.time())))

    for i in range(100):
        print(getTime(port,modbus_adr))

