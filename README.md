Quick Guide of c8c Compiler
==================
This is a quick guide of c8c compiler. It would help you to get start with it.

##Compiler Description

This compiler(c8c) is the final project of '**COMP3235 Compiling Techniques(2013-2014 Sem 1)**' in the University of Hong Kong. Its lexical analyser is implemented in **flex**; parser is implemented in **bison**; code generater is implemented in pure **C**.

c8c has the following features:

- Variable declaration (including local variable and global variable) -- case1
- Function (recursive)call -- case1
- Pointer and reference -- case2
- Function overload -- case3
- High demensional array -- case4
- Funciton recursive -- case6
- String -- case7
- String build-in library -- case8
- Runtime error detection --case4

## File Description

[c8.l](./c8.l)
:   This file is the lexical analyser which is implemented in Flex. It extracts useful tokens from the input code but ignores comments and formats. 

[c8.y](./c8.y)
:   This file is the parser which gets tokens from **c8.l** and constructs abstract syntax trees. Gramma checking is also processed in this file.

[c8c.c](./c8c.c)
:   This file is the assembly code generater. It gets the syntax tree generated by **c8.y** and prints assembly code in **nas** grammar, which is an assembler developed by [Professor Francis Lau](http://i.cs.hku.hk/~fcmlau/), the lecturer of COMP3235 in HKU. 

[nas.l](./nas.l)
:   Nas is short for 'New Assembler-Simulater'. This file is the lexical analyser which is also implemented in Flex, just like the function of **[c8.l](./c8.l)**.

[nas.y](./nas.y)
:   This file is the parser of nas, just like the function of **[c8.y](./c8.y)**.

[calc3.h](./calc3.h)
:    Some global structures are declared in this file.

[makefile](./makefile)
:   This is the makefile for c8c. It provides following commands:
 - **make c8c** compile c8c compiler
 - **make nas** compile nas assembler
 - **make cleanc8c** delete c8c compiler
 - **make cleannas** delete nas assembler

c8c
:    This is the executable program of c8c compiler. It reads **c8c code** and prints **nas assembly code** to standard output stream in default. 
- usage: 
	./c8c [inputfile] > [outputfile]
	./c8c case1.sc > case1.as

nas
:   This is the executable program of nas assembler. It reads **nas assembly code** and execute it. 
- usage:
	./nas [inputfile]
	./nas case1.as

case1~9.sc
:   These are test cases for this compiler. All of them are written in **c8c** grammar which is quite similar to **standard C**.

##How to use it

1. download all the **[source codes](https://github.com/zjusbo/c8)** in this repository.

2. extract them to a **Linux operating system**. A Linux virtual machine also works.

3. install **flex** and **bison** in your Linux operating system. If you haven't installed them or it bothers you to do that, you can also safely skip step 4 because they have been pre-compiled for you. 

4. compile c8c and nas 
	- ./make c8c
	- ./make nas

5. run test cases
To see the assemble code on the screen, type
	- ./c8c case1.sc
To redirect the assemble code for future running, type(**recommanded**)
	- ./c8c case1.sc > case1.as

6. run assemble codes
	- ./nas case1.as

7. check the fascinating results and admire them.

##Author

[Song Bo](mailto:sbo@zju.edu.cn), [Wang Yue](mailto:3110101447@zju.edu.cn) 
Zhejiang University

