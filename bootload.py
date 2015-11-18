#!/usr/bin/python

from intelhex import IntelHex
import sys
from serial import serialwin32 as serial
import time
import os, errno

port = serial.Serial(port='COM5')

readBuffer = ''

def doRead() :
    global readBuffer

    if (not port.inWaiting()) :
        return

    for c in port.read(port.inWaiting()) :
        readBuffer += c
        if (c == "\n") :
            print 'Reading: ', readBuffer
            readBuffer = ''

def erase() :
    print "Erasing"
    port.write("erase\n");

def insertTrampoline(hexFile) :
    # When the bootloader exits, it will execute the instruction at byte address
    # 0x17FE (the byte immediately before the bootloader). We need to arrange
    # for that instruction to be an rjmp to the application. Normally the
    # firt instruction (at address 0) is an rjmp to the application, so we can
    # determine the app start address from that.

    # Word address of the app start vector
    appStartAddress = 0xFFF & ( hexFile[0] + (hexFile[1] << 8))

    # Word address of the trampoline instruction
    trampolineAddress = (6*1024)/2 - 1

    # Determine the offset between the trampoline and app start addresses.
    # Mask it to 12 bits.
    trampoline = 0xFFF & (appStartAddress-trampolineAddress)

    # The rjmp opcode is 0xC000. The lower twelve bits are the address offset.
    trampolineInstruction = 0xC000 | trampoline

    # Insert the instruction at the trampoline address
    hexFile[trampolineAddress*2] = (trampolineInstruction & 0xFF)
    hexFile[trampolineAddress*2+1] = (trampolineInstruction >> 8)

    print 'Hex File[0,1]', hexFile[0], hexFile[1]
    print 'App Start Address', appStartAddress
    print 'Trampoline Address', trampolineAddress
    print 'Trampoline', trampoline
    print 'Trampoline Instruction', trampolineInstruction


def burnHex(cmd, hexFileName, trampoline=False, blockSize=16) :
    hexFile = IntelHex(hexFileName)

    if (trampoline) :
        insertTrampoline(hexFile)

    print hexFile, hexFile.minaddr(), hexFile.maxaddr()
    end = hexFile.maxaddr()+1
    if (end % 64 != 0) :
        end = end & ~63;
        end += 64;

    for i in range(hexFile.minaddr(), end, blockSize) :
        row = ["%s %04d %d" % (cmd, i, blockSize)] + [hexFile[x] for x in range(i,i+blockSize)]
        line = " ".join([str(x) for x in row]) + "\n"

        print 'Writing: ', line
        port.write(line)
        doRead()

def dumpHex(hexFileName) :
    hexFile = IntelHex(hexFileName)

    for i in range(hexFile.minaddr(), hexFile.maxaddr()+1, 16) :
        row = [str(i)] + [ str(hexFile[j]) for j in range(i, i+16)]
        print(" ".join(row))


#dumpHex(sys.argv[1])
#erase()

if (sys.argv[1] == '-avr') :
    burnHex('flash', sys.argv[2], trampoline=True)
    burnHex('eeprom', sys.argv[3])
    burnHex('verify', sys.argv[2], trampoline=True)
elif (sys.argv[1] == '-arm') :
    burnHex('flash', sys.argv[2])
    burnHex('verify', sys.argv[2])
elif (sys.argv[1] == '-dump') :
    dumpHex(sys.argv[2])
else :
    print 'Must use either -avr or -arm'

#port.write("exit\n");

time.sleep(1)
doRead()
