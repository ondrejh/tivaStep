import test as comm
import time
import serial
import ntplib

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

def testTime(filename,ntpclient,cnt):

    with serial.Serial(portname,comm.baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=comm.portTimeout) as port:

        s = ''
        t = getTime(port,modbus_adr)
        if abs(t-time.time())>100:
            while True:
                t = time.time()
                if t-round(t-0.5)>0.9:
                    setTime(port,modbus_adr,round(t+1))
                    break
            response = ntpclient.request('tik.cesnet.cz')
            cnt = 0
            s = '{},{},{:0.03f},{:0.03f}'.format(cnt,round(t),t,response.tx_time)
            file=open(filename,'w')
            file.write('{}\n'.format(s))
            file.close()
        else:
            while True:
                tn = time.time()
                tt = getTime(port,modbus_adr)
                if t!=tt:
                    tn += time.time()
                    tn /= 2
                    response = ntpclient.request('tik.cesnet.cz')
                    s = '{},{},{:0.03f},{:0.03f}'.format(cnt,t,tn,response.tx_time)
                    file=open(filename,'a')
                    file.write('{}\n'.format(s))
                    file.close()
                    break

        return s

if __name__ == "__main__":

    c = ntplib.NTPClient()
    cnt = 0

    while True:

        answ = testTime('timeLog.txt',c,cnt)
        cnt+=1
        print(answ)
        time.sleep(5)
