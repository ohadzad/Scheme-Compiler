#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: 	compile	main
tsah: compilet main
ohad: compileo main
hadar: compileh main

# Tool invocations
# Executable "hello" depends on the files hello.o and run.o.
compile: 
	rm -f main.c
	petite makecompile.scm

compilet:
	rm -f main.c
	petite makecompilet.scm


compileo:
	rm -f main.c
	petite makecompileo.scm

compileh:
	rm -f main.c
	petite makecompileh.scm

main: main.o
	gcc -m32 -g -Wall -Wno-unused-label -o main main.o

# Depends on the source and header files
main.o: main.c cisc.h
	gcc -g -Wall -Wno-unused-label -m32 -c -o main.o main.c 

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o main