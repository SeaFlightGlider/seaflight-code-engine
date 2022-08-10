#make sure to install the serial library (pyserial) using cmd

#this program prints all the files from the teensy board to serial and
#to a file labeled "oceangliderfile.txt" in C:\Users\"USER"\AppData\Local\Programs\Python\Python310

import serial
import time
from serial.tools import list_ports

print('avaliable COM ports')
print(list_ports.comports())

userCOM = input('What COM port?     (ex. COM4)')

BaudSpeed = input('What is the Baud Speed?    (ex. 9600)')

port = serial.Serial(userCOM, BaudSpeed, timeout=1)

serialoutput = ''

def writing():
    time.sleep(1)
    print('Connecting and getting files')
    this = 'allfile'
    port.write(this.encode())

def reading():
    #time.sleep(.01)
    serialoutput = port.read(size=2000).decode('ascii')
    print(serialoutput)
    with open('oceangliderfile.txt', 'a') as f:
        f.write(serialoutput)

while True:
    writing()
    while True:
        reading()
