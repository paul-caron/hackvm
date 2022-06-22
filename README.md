# HackVM
HackVM as seen from hacker.org .  Reads a file as command line arg or reads live input in a REPL .

## Hardware
HackVM is a stack based machine. It has a stack with which it does arithmetic operations. It also has memory blocks for loading value to and from the stack. No registers. The code is loaded as a string of chars. Each char is an instruction.

## Compile
There are no dependencies really besides plain stdlib++. So compilation can be as easy as:
```bash
g++ hackvm.cpp -o hvm
```

## Executing a script file
```bash
./hvm myscript.hvm
```

## REPL
We can also enter one line of code at a time, in REPL mode.
```bash
./hvm
```
It goes into REPL mode when no filename is specified.

Once in REPL mode, the hackvm prompt should show up.
```bash
HackVM> 
```
