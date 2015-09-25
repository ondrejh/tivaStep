import test as comm
import time
import serial

portname='COM1'
modbus_adr=1

RTC_SET_ADDRESS = 8
RTC_TIME_ADDRESS = 10

MOTOR_POSITION_ADDRESS = 12
MOTOR_GOTOPOS_ADDRESS = 16

TIME_ZERO = 10
STEPS_ZERO = 12
TURN_STEPS = 14
TURN_TIME = 16
DESIRED_TIME = 20
DESIRED_POS = 22

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

def getPosition(port,adr):

    answ = comm.readParams(port,adr,MOTOR_POSITION_ADDRESS,4)

    try:
        m1pos = (answ[1][0] | (answ[1][1]<<16))
        m2pos = (answ[1][2] | (answ[1][3]<<16))
        return (m1pos,m2pos)
    except:
        return answ

def setGotoPosition(port,adr,pos1,pos2):

    answ = comm.writeParams(port,adr,MOTOR_GOTOPOS_ADDRESS,4,[pos1&0xFFFF,(pos1>>16)&0xFFFF,pos2&0xFFFF,(pos2>>16)&0xFFFF])
    return answ

def set32bitReg(port,adr,vadr,value):

    answ = comm.writeParams(port,adr,vadr*2,2,[value&0xFFFF,(value>>16)&0xFFFF])
    return answ

def get32bitReg(port,adr,vadr):

    answ = comm.readParams(port,adr,vadr*2,2)
    try:
        val = (answ[1][0] | (answ[1][1]<<16))
        return val
    except:
        return answ

if __name__ == "__main__":

    with serial.Serial(portname,comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:

        '''print(getPosition(port,modbus_adr))
        print(setGotoPosition(port,modbus_adr,3000,-10000))

        t = round(time.time())
        print(setTime(port,modbus_adr,round(time.time())))

        for i in range(100):
            t = getTime(port,modbus_adr)
            print('{:08X}'.format(t))
            print(getPosition(port,modbus_adr))'''

        '''set32bitReg(port,modbus_adr,TURN_STEPS,10000)
        set32bitReg(port,modbus_adr,TURN_TIME,1000)

        for i in range(100):
            print(get32bitReg(port,modbus_adr,MOTOR_GOTOPOS_ADDRESS>>1))'''


        with open('timeLog.txt','w') as file:

            tStart = time.clock()

            while True:

                tim = getTime(port,modbus_adr)
                tN = time.clock()
                file.write('{},{}\n'.format(tN,tim))

                if tN-tStart>600:
                    break

            file.close()
            

