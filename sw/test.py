import serial
from crc16 import checkCRC16,getCRC16

#portName = '/dev/ttyUSB0'
portName = 'COM25'
baudRate = 19200
portTimeout = 0.1


def showBytes(data,end=None):

    ''' print out bytes '''
    
    for b in data:
        print('{:02X} '.format(b),end='')
    print('' if end==None else end)


def addCRC16(data):

    ''' add crc16 at the end of bytes
    Uses crc16 python module. Returns data with crc.'''

    data += bytes(getCRC16(data))

    return(data)


def readParams(port,id,addr,count):

    ''' read multiple parameters (modbus) '''

    msg = bytes([id,3,addr>>8,addr&0xFF,count>>8,count&0xFF])
    msg = addCRC16(msg)
    port.write(msg)

    answ = port.read(256)

    #print(answ)

    if len(answ)<2:
        return('ERROR: answer error !!!')

    #print(getCRC16(answ))
    if checkCRC16(answ):

        cnt = int(answ[2]/2)
        data = []
        for i in range(cnt):
            data.append(int(answ[3+2*i]<<8)+int(answ[4+2*i]))
        return (cnt,data)
        
    else:
        return('ERROR: CRC check failed !!!')

def writeParams(port,id,addr,count,data):

    ''' write multiple parameters (modbus) '''

    msg = bytes([id,16,addr>>8,addr&0xFF,count>>8,count&0xFF,count*2])
    for d in data:
        msg += bytes([d>>8,d&0xFF])
    msg = addCRC16(msg)
    port.write(msg)

    answ = port.read(256)

    if len(answ)<2:
        return('ERROR: answer error !!!')

    if checkCRC16(answ):
        return('OK')
        
    
def checkPresence(port,id):

    ''' check device presence on spec. id
    It reads 0 params starting on address 0 - works with CT unidrive.
    Return True if device answers or False if not.'''

    answ = readParams(port,id,0,0)
    if type(answ)==str:
        return(False)
    else:
        return(True)


if __name__ == "__main__":

    with serial.Serial(portName,baudRate,bytesize=8,parity=serial.PARITY_NONE,stopbits=2,timeout=portTimeout) as port:

        print(writeParams(port,1,0,3,[0x1234,0x4321,0xABCD]))
        print(readParams(port,1,0,3))
