CFLAGS := -nostdlib -O0
TARGET := led.bin
LOCATION := /dev/sda 
CROSS_COMPILE := arm-none-eabi-
Q := @
$(TARGET) : start.o
	$(Q)$(CROSS_COMPILE)ld -T link.lds -o led.elf $^
	$(Q)$(CROSS_COMPILE)objcopy -O binary led.elf $@
	$(Q)$(CROSS_COMPILE)objdump -D led.elf > led.dis

%.o : %.S
	$(Q)$(CROSS_COMPILE)gcc -o $@ $< -c

%.o : %.c
	$(Q)$(CROSS_COMPILE)gcc $(CFLAGS) -o $@ $< -c

.PHONY:clean install
clean:
	rm -rf *.o *.elf *.bin *.dis mkbl1


