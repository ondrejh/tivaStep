import test as comm
import time
import serial

portname='COM1'
modbus_adr=1

RTC_SET_ADDRESS = 8
RTC_TIME_ADDRESS = 10

MOTOR_POSITION_ADDRESS = 12

def getTime(port,adr):

    answ = comm.readParams(port,adr,RTC_TIME_ADDRESS,2)
    #print(answ)
    try:
        s = (answ[1][0] | (answ[1][1]<<16))
        return s
    except:
        return answ

def setTime(port,adr,time):

    answ = comm.writeParams(port,adr,RTC_SET_ADDRESS,2,[time&0xFFFF,(time>>16)&0xFFFF])
    return answ

def getPosition(port,adr):

    answ = comm.readParams(port,adr,MOTOR_POSITION_ADDRESS,4)

    try:
        m1pos = (answ[1][0] | (answ[1][1]<<16))
        m2pos = (answ[1][2] | (answ[1][3]<<16))
        return (m1pos,m2pos)
    except:
        return answ

if __name__ == "__main__":

    with serial.Serial(portname,comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:

        print(getPosition(port,modbus_adr))

        t = round(time.time())
        print(setTime(port,modbus_adr,round(time.time())))

        for i in range(10):
            t = getTime(port,modbus_adr)
            print('{:08X}'.format(t))

