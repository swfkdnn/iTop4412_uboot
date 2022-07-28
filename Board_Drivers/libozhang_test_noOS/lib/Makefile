objs := div64.o ctype.o string.o vsprintf.o

libc.a: $(objs)
	$(Q)${CROSS_COMPILE}ar -r -o $@ $^
	
%.o:%.c
	$(Q)${CROSS_COMPILE}gcc $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	$(Q)${CROSS_COMPILE}gcc $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	$(Q)rm -f libc.a *.o		
	
