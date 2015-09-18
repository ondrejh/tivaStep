import test as comm
import time
import serial

portname='COM1'
modbus_adr=1

RTC_SET_ADDRESS = 8
RTC_TIME_ADDRESS = 10

def getTime(port,adr):

    answ = comm.readParams(port,adr,RTC_TIME_ADDRESS,2)
    try:
        s = (answ[1][0] | (answ[1][1]<<16))
        return s
    except:
        return answ

def setTime(port,adr,time):

    answ = comm.writeParams(port,adr,RTC_SET_ADDRESS,2,[time&0xFFFF,(time>>16)&0xFFFF])
    return answ

if __name__ == "__main__":

    with serial.Serial(portname,comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:

        print(comm.readParams(port,modbus_adr,4,4))

        print(setTime(port,modbus_adr,round(time.time())))

        for i in range(10):
            print(getTime(port,modbus_adr))

