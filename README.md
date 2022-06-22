# hackvm
HackVM as seen from hacker.org .  Reads a file as command line arg or reads live input in a REPL .

# hardware
HackVM is a stack based machine. It has a stack with which it does arithmetic operations. It also has memory blocks for loading value to and from the stack. No registers. The code is loaded into memory as a string of chars. Each char is an instruction.

# compile
There are no dependencies really besides plain stdlib++. So compilation can be as easy as:
```bash
g++ hackvm.cpp -o hvm
```

