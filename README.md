# RISC-V Simulator

Multi-part application that creates a computing machine capable of executing real programs using C++ compiled with gcc. The purpose is to gain an understanding of a computing machine (RISC-V) and its instruction set. The application has the ability to simulate the execution of a binary file, as well as simulate how the file is stored in memory and generating a dump of the memory. More details and documentation can be found in the source files. 

Usage: rv32i [-m hex-mem-size] [-l execution-limit] [-dirz] infile
     
     -m specify memory size (default = 0x10000)
     
     -l specify execution limit (default = infinite)
     
     -d show disassembly before program simulation
     
     -i show instruction printing during execution
     
     -r show a dump of the hart status before each exec
     
     -z show a dump of the hart and memory after simulation

Commands used to run the program:
```
g++ -g -ansi -pedantic -Wall -Werror -std=c++14 -c -o main.o main.cpp

g++ -g -ansi -pedantic -Wall -Werror -std=c++14 -c -o rv32i.o rv32i.cpp

g++ -g -ansi -pedantic -Wall -Werror -std=c++14 -c -o memory.o memory.cpp

g++ -g -ansi -pedantic -Wall -Werror -std=c++14 -c -o registerfile.o registerfile.cpp

g++ -g -ansi -pedantic -Wall -Werror -std=c++14 -c -o hex.o hex.cpp

g++ -g -ansi -pedantic -Wall -Werror -std=c++14 -o rv32i main.o rv32i.o memory.o registerfile.o hex.o
```
Commands used to generate the output:
```
# Run the application
g++ -g -ansi -pedantic -Wall -Werror -std=c++14 -c -o main.o main.cpp
g++ -g -ansi -pedantic -Wall -Werror -std=c++14 -c -o rv32i.o rv32i.cpp
g++ -g -ansi -pedantic -Wall -Werror -std=c++14 -c -o memory.o memory.cpp
g++ -g -ansi -pedantic -Wall -Werror -std=c++14 -c -o registerfile.o registerfile.cpp
g++ -g -ansi -pedantic -Wall -Werror -std=c++14 -c -o hex.o hex.cpp
g++ -g -ansi -pedantic -Wall -Werror -std=c++14 -o rv32i main.o rv32i.o memory.o registerfile.o hex.o

# Try to run without arguments
./rv32i

# Run with -i
./rv32i -i testfiles/allinsns5.bin

# Run with invalid
./rv32i -X testfiles/allinsns5.bin

# Run with other options
./rv32i -dirz testfiles/allinsns5.bin > allinsns5-dirz.out
./rv32i -dz testfiles/allinsns5.bin > allinsns5-dz.out
./rv32i -m100 -iz testfiles/allinsns5.bin > allinsns5-iz-m100.out
./rv32i -m100 -ir testfiles/allinsns5.bin > allinsns5-ir-m100.out
./rv32i -m100 -irl2 testfiles/allinsns5.bin > allinsns5-irl2-m100.out
./rv32i -dirz -m8500 testfiles/torture5.bin > torture5-dirz-m8500.out
./rv32i -iz -m8500 testfiles/torture5.bin > torture5-iz-m8500.out
./rv32i -z -m8500 testfiles/torture5.bin > torture5-z-m8500.out
./rv32i -m8500 testfiles/torture5.bin > torture5-m8500.out
./rv32i -z -m50000 testfiles/sieve.bin | head -10 > sieve-z-m50000-head-10.log
./rv32i -z -m50000 testfiles/sieve.bin | grep "^00034[01]" > sieve-z-m50000-grep-0003401.log
```
