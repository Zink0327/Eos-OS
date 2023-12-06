CFLAGS := -mcmodel=large -fno-builtin -m64 
ASFLAGS := --64
OBJS := head.o main.o print.o 

all: kernel.elf
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary kernel.elf kernel.bin

kernel.elf:	$(OBJS)
	ld -b elf64-x86-64 -z muldefs -o kernel.elf $(OBJS) -T linkkernel.lds 

head.o:	head.S
	gcc -E  head.S > head.s
	as --64 -o head.o head.s
	
main.o:	main.c
	gcc  $(CFLAGS) -c main.c -fno-stack-protector
	
print.o: print.c
	gcc  $(CFLAGS) -c print.c -fno-stack-protector
