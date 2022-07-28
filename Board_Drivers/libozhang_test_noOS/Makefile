#!/bin/sh

INCDIR := $(shell pwd)
LIB := lib
CFLAGS := -nostdlib -nostdinc -I$(INCDIR)/$(LIB)/include
#CPPFLAGS := -Wall -O0 -fno-builtin  -fno-tree-scev-cprop 
CPPFLAGS := -Wall -O2 -fno-builtin  -fno-tree-scev-cprop 
T = swi
TARGET := $(T).bin
BL2 := bl2.bin
LOCATION := /dev/sda
#CROSS_COMPILE := arm-none-linux-gnueabihf-
#CROSS_COMPILE := arm-none-eabi-
CROSS_COMPILE := /home/sw/bb/arm-2009q3/bin/arm-none-linux-gnueabi-
NM := /home/sw/bb/arm-2009q3/bin/arm-none-linux-gnueabi-nm
Q := @
SYSTEM_MAP = \
		$(NM) $1 | \
		grep -v '\(compiled\)\|\(\.o$$\)\|\( [aUw] \)\|\(\.\.ng$$\)\|\(LASH[RL]DI\)' | \
		LC_ALL=C sort

objs := start.o uart.o lowlevel_init.o sdram_init.o $(T).o
objs += $(LIB)/libc.a

export CFLAGS CPPFLAGS CROSS_COMPILE INCDIR Q SYSTEM_MAP
#+System.map 
$(TARGET) : $(objs)
	$(Q)$(CROSS_COMPILE)ld -T link.lds -o $(T).elf $^
	$(Q)$(CROSS_COMPILE)objcopy -O binary $(T).elf $@
	$(Q)$(CROSS_COMPILE)objcopy --gap-fill=0xff -g -O ihex $(T).elf $(T).hex
	$(Q)$(CROSS_COMPILE)objdump -D $(T).elf > $(T).dis
	$(Q)$(call SYSTEM_MAP,$(T).elf) > $(T)_System.map

$(LIB)/libc.a:
	$(Q)cd $(LIB);make;cd ..

%.o : %.S
	$(Q)$(CROSS_COMPILE)gcc $(CPPFLAGS) $(CFLAGS)  -o $@ $< -c

%.o : %.c
	$(Q)$(CROSS_COMPILE)gcc $(CPPFLAGS) $(CFLAGS)  -o $@ $< -c

.PHONY:clean install
clean:
	$(Q)cd $(LIB);make clean; cd ..
	$(Q)rm -rf *.o *.elf $(TARGET) $(BL2) $(T)_download.bin $(T)_System.map *.dis *.map mkbl1 mkbl2

instal_bl1:
	$(Q)gcc ./mkbl1.c -static -o mkbl1
	$(Q)./mkbl1 $(TARGET) 
	$(Q)if [ -b $(LOCATION) ]; then \
	#sudo mkfs.vfat -F 32 -I $(LOCATION); \
	dd if=/dev/zero of=$(LOCATION) bs=512 seek=1 iflag=dsync oflag=dsync count=16; \
	dd if=./$(TARGET) of=$(LOCATION) bs=512 seek=1 iflag=dsync oflag=dsync; \
	fi

install_bl2:
	$(Q)gcc ./mkbl2.c -static -o mkbl2
	$(Q)./mkbl2 $(TARGET) $(BL2) 14336
	$(Q)if [ -b $(LOCATION) ]; then \
	#sudo mkfs.vfat -F 32 -I $(LOCATION); \
#	dd if=/dev/zero of=$(LOCATION) bs=512 seek=1 iflag=dsync oflag=dsync count=16; \
	dd if=/dev/zero of=$(LOCATION) bs=512 seek=1 iflag=dsync oflag=dsync count=62; \
#	dd if=./E4412_N.bl1.bin of=$(LOCATION) bs=512 seek=1 iflag=dsync oflag=dsync; \
	cat E4412_N.bl1.SCP2G.bin bl2.bin  > $(T)_download.bin;\
	dd if=./$(T)_download.bin of=$(LOCATION) bs=512 seek=1 iflag=dsync oflag=dsync; \
#	dd  if=./bl2.bin of=$(LOCATION) seek=17 iflag=dsync oflag=dsync;\
	fi

