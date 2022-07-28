#!/bin/sh

INCDIR := $(shell pwd)
LIB := lib
CFLAGS := -nostdlib -nostdinc -I$(INCDIR)/$(LIB)/include
#CPPFLAGS := -Wall -O0 -fno-builtin  -fno-tree-scev-cprop 
CPPFLAGS := -Wall -O2 -fno-builtin  -fno-tree-scev-cprop 
TARGET := u-boot.bin
BL2 := bl2.bin
LOCATION := /dev/sda
#CROSS_COMPILE := arm-none-linux-gnueabi-
CROSS_COMPILE := arm-none-eabi-
Q := @

objs := start.o uart.o lowlevel_init.o sdram_init.o main.o
objs += $(LIB)/libc.a

export CFLAGS CPPFLAGS CROSS_COMPILE INCDIR Q

$(TARGET) : $(objs)
	$(Q)$(CROSS_COMPILE)ld -T link.lds -o uart.elf $^
	$(Q)$(CROSS_COMPILE)objcopy -O binary uart.elf $@
	$(Q)$(CROSS_COMPILE)objdump -D uart.elf > uart.dis

$(LIB)/libc.a:
	$(Q)cd $(LIB);make;cd ..

%.o : %.S
	$(Q)$(CROSS_COMPILE)gcc $(CPPFLAGS) $(CFLAGS)  -o $@ $< -c

%.o : %.c
	$(Q)$(CROSS_COMPILE)gcc $(CPPFLAGS) $(CFLAGS)  -o $@ $< -c

.PHONY:clean install
clean:
	$(Q)rm -rf *.o *.elf $(BL2) *.dis mkbl1 mkbl2 sw_u-boot.bin

install:
	$(Q)gcc ./mkbl1.c -static -o mkbl1
	$(Q)./mkbl1 $(TARGET) 
	$(Q)if [ -b $(LOCATION) ]; then \
	#sudo mkfs.vfat -F 32 -I $(LOCATION); \
	dd if=/dev/zero of=$(LOCATION) bs=512 seek=1 iflag=dsync oflag=dsync count=16; \
	dd if=./$(TARGET) of=$(LOCATION) bs=512 seek=1 iflag=dsync oflag=dsync; \
	fi

install2:
	$(Q)gcc ./mkbl2.c -static -o mkbl2
	$(Q)./mkbl2 $(TARGET) $(BL2) 14336
	$(Q)if [ -b $(LOCATION) ]; then \
	#sudo mkfs.vfat -F 32 -I $(LOCATION); \
	dd if=/dev/zero of=$(LOCATION) bs=512 seek=1 iflag=dsync oflag=dsync count=1100; \
#	dd if=./E4412_N.bl1.bin of=$(LOCATION) bs=512 seek=1 iflag=dsync oflag=dsync; \
	cat E4412_N.bl1.SCP2G.bin bl2.bin all00_padding.bin u-boot.bin  > sw_u-boot.bin;\
	dd if=./sw_u-boot.bin of=$(LOCATION) bs=512 seek=1 iflag=dsync oflag=dsync; \
#	dd  if=./bl2.bin of=$(LOCATION) seek=17 iflag=dsync oflag=dsync;\
	fi


