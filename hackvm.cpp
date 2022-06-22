#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <array>
#include <list>

std::stack<size_t> call_stack;
size_t program_counter{0};
std::list<long long> operand_stack;
std::array<long long,16384> memory{};
std::string code{};
char opcode;
bool terminated = false;
bool endofcode = false;

long long vm_opop(){
    long long a = operand_stack.front();
    operand_stack.pop_front();
    return a;
}

void vm_opush(long long value){
    operand_stack.push_front(value);
}

void vm_add(){
    long long a,b,c;
    a = vm_opop();
    b = vm_opop();
    c = a + b;
    vm_opush(c);
}

void vm_mul(){
    long long a,b,c;
    a = vm_opop();
    b = vm_opop();
    c = a * b;
    vm_opush(c);
}

void vm_div(){
    long long a,b,c;
    a = vm_opop();
    b = vm_opop();
    c = b / a;
    vm_opush(c);
}

void vm_sub(){
    long long a,b,c;
    a = vm_opop();
    b = vm_opop();
    c = b - a;
    vm_opush(c);
}

void vm_cmp(){
    long long a,b,c;
    a = vm_opop();
    b = vm_opop();
    c = b - a;
    vm_opush(c<0?-1:c>0?1:0);
}

void vm_print_int(){
    std::cout << vm_opop() << std::flush;
}

void vm_print_char(){
    std::cout << (char) vm_opop() << std::flush;
}

void vm_goto(){
    long long offset;
    offset = vm_opop();
    program_counter += offset;
}

void vm_gotoz(){
    long long offset;
    offset = vm_opop();
    if(vm_opop() == 0){
        program_counter += offset;
    }
}

void vm_peek(){
    long long address = vm_opop();
    if(address < 0 || address >=(long long) memory.size())
        throw "memory peek invalid address";
    vm_opush(memory.at(address));
}

void vm_poke(){
    long long address = vm_opop();
    if(address < 0 || address >=(long long) memory.size())
        throw "memory poke invalid address";
    memory.at(address) = vm_opop();
}

void vm_call(){
    call_stack.push(program_counter);
    program_counter = vm_opop();
}

void vm_pick(){
    long long index = vm_opop();
    if (index < 0 || index >= (long long) operand_stack.size())
        throw "pick index invalid";
    auto it = operand_stack.begin();
    std::advance(it, index);
    long long value = *it;
    vm_opush(value);
}

void vm_roll(){
    long long index = vm_opop();
    if (index < 0 || index >= (long long) operand_stack.size())
        throw "pick index invalid";
    auto it = operand_stack.begin();
    std::advance(it, index);
    long long value = *it;
    operand_stack.erase(it);
    vm_opush(value);
}

void vm_return(){
    program_counter = call_stack.top();
    call_stack.pop();
}

void vm_drop(){
    vm_opop();
}

void vm_exit(){
    terminated = true;
}

void fetch(){
    if(program_counter < 0)
        throw "program counter must be positive";
    if(program_counter<code.size())
        opcode = code.at(program_counter);
    else{
        opcode = ' ';
        endofcode = true;
        program_counter = code.size() - 1;
    }
}

void execute_opcode(){
    switch(opcode){
        case '+': vm_add(); break;
        case '*': vm_mul(); break;
        case '-': vm_sub(); break;
        case '/': vm_div(); break;
        case ':': vm_cmp(); break;
        case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8':
        case '9': case '0':
            vm_opush((long long)opcode-48);
            break;
        case 'p': vm_print_int(); break;
        case 'P': vm_print_char(); break;
        case 'g': vm_goto(); break;
        case '?': vm_gotoz(); break;
        case '<': vm_peek(); break;
        case '>': vm_poke(); break;
        case 'd': vm_drop(); break;
        case 'c': vm_call(); break;
        case '$': vm_return(); break;
        case '^': vm_pick(); break;
        case 'v': vm_roll(); break;
        case '!': vm_exit(); break;
    }
}

void cycle(){
    try{
        fetch();
        program_counter++;
        execute_opcode();
    }
    catch(...){
       std::cerr << "opcode: " << opcode << std::endl;
       std::cerr << "Program counter: " << program_counter << std::endl;
       exit(1);
    }
}

void execute_file(const char * filename){
    char buf[1024];
    std::ifstream ifs(filename);
    while(ifs.read(buf, 1024)){
        code.append(buf,ifs.gcount());
    }
    code.append(buf, ifs.gcount());
    ifs.close();
    while(!endofcode && !terminated){
        cycle();
    }
}

void prompt(){
    std::cout << "\nHackVM>";
}

void repl(){
    std::string line{};
    prompt();
    while(!terminated && std::getline(std::cin, line)){
        code.append(line);
        endofcode = false;
        while(!endofcode && !terminated){
            cycle();
        }
        prompt();
    }
}

int main(int argc, char ** argv) {
    if(argc > 1 && argv[1][0]!='-'){
        execute_file(argv[1]);
    }else{
        std::cout << "Welcome to HackVM" << std::endl
                  << "Here is a list of all opcodes:"
                  << R"(
    
    '+': addition
    '*': multiplication
    '-': subtraction
    '/': division
    ':': comparison
    pops two operands from stack, result is pushed
    
    '1':
    '2':
    '3':
    '4':
    '5':
    '6':
    '7':
    '8':
    '9':
    '0':
    push digit as integer on the stack
    
    'p': pop integer and print it
    'P': pop character and print it
    'g': (goto) pops address offset and add it to program counter
    '?': (goto zero) pops address offset and pop again value from stack. if value is zero, add offset to program counter
    '<': (peek) pops address from stack then pushed memory address content on the stack
    '>': (poke) pops address from the stack then pops value from the stack. value is then stored at memory address.
    'd': (drop) pops one element off the stack
    'c': (call) call to subroutine. pops address from stack and saves program counter value for return later. the program counter is assigned to goto address.
    '$': return from subroutine, returns program counter to previously saved state.
    '^': pick
    'v': roll
    '!': exit

)";
        repl();
    }
    return 0;
}
