##
## (C) Copyright 2000-2006
## Wolfgang Denk, DENX Software Engineering, wd@denx.de.
##
## See file CREDITS for list of people who contributed to this
## project.
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License as
## published by the Free Software Foundation; either version 2 of
## the License, or (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston,
## MA 02111-1307 USA
##
#
##########################################################################
#
$(warning sw_debug: in /home/sw/pp_self/1/iTop4412_uboot/config.mk)
##ifneq ($(OBJTREE),$(SRCTREE))
##ifeq ($(CURDIR),$(SRCTREE))
###dir :=
##dir ?:=
##else
###dir := $(subst $(SRCTREE)/,,$(CURDIR))
##dir ?:= $(subst $(SRCTREE)/,,$(CURDIR))
##endif
##
###obj := $(if $(dir),$(OBJTREE)/$(dir)/,$(OBJTREE)/)
###src := $(if $(dir),$(SRCTREE)/$(dir)/,$(SRCTREE)/)
##obj ?:= $(if $(dir),$(OBJTREE)/$(dir)/,$(OBJTREE)/)
##src ?:= $(if $(dir),$(SRCTREE)/$(dir)/,$(SRCTREE)/)
##
##$(shell mkdir -p $(obj))
##else
###obj :=
###src :=
##obj ?:=
##src ?:=
##endif
#
## clean the slate ...
#PLATFORM_RELFLAGS =
#PLATFORM_CPPFLAGS =
#PLATFORM_LDFLAGS =
#
##########################################################################
#
#HOSTCFLAGS	= -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer \
#		  $(HOSTCPPFLAGS)
#HOSTSTRIP	= strip
#
##
## Mac OS X / Darwin's C preprocessor is Apple specific.  It
## generates numerous errors and warnings.  We want to bypass it
## and use GNU C's cpp.  To do this we pass the -traditional-cpp
## option to the compiler.  Note that the -traditional-cpp flag
## DOES NOT have the same semantics as GNU C's flag, all it does
## is invoke the GNU preprocessor in stock ANSI/ISO C fashion.
##
## Apple's linker is similar, thanks to the new 2 stage linking
## multiple symbol definitions are treated as errors, hence the
## -multiply_defined suppress option to turn off this error.
##
#
#ifeq ($(HOSTOS),darwin)
#HOSTCC		= cc
#HOSTCFLAGS	+= -traditional-cpp
#HOSTLDFLAGS	+= -multiply_defined suppress
#else
#HOSTCC		= gcc
#endif
#
#ifeq ($(HOSTOS),cygwin)
#HOSTCFLAGS	+= -ansi
#endif
#
## We build some files with extra pedantic flags to try to minimize things
## that won't build on some weird host compiler -- though there are lots of
## exceptions for files that aren't complaint.
#
#HOSTCFLAGS_NOPED = $(filter-out -pedantic,$(HOSTCFLAGS))
#HOSTCFLAGS	+= -pedantic
#
##########################################################################
##
## Option checker (courtesy linux kernel) to ensure
## only supported compiler options are used
##
cc-option = $(shell if $(CC) $(CFLAGS) $(1) -S -o /dev/null -xc /dev/null \
		> /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi ;)
#
##
# Include the make variables (CC, etc...)
#
AS	= $(CROSS_COMPILE)as
LD	= $(CROSS_COMPILE)ld
CC	= $(CROSS_COMPILE)gcc
CPP	= $(CC) -E
AR	= $(CROSS_COMPILE)ar
NM	= $(CROSS_COMPILE)nm
LDR	= $(CROSS_COMPILE)ldr
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB	= $(CROSS_COMPILE)RANLIB

#########################################################################

# Load generated board configuration
sinclude $(OBJTREE)/include/autoconf.mk

ifdef	ARCH
sinclude $(TOPDIR)/lib_$(ARCH)/config.mk	# include architecture dependend rules
endif
ifdef	CPU
sinclude $(TOPDIR)/cpu/$(CPU)/config.mk		# include  CPU	specific rules
endif
ifdef	SOC
sinclude $(TOPDIR)/cpu/$(CPU)/$(SOC)/config.mk	# include  SoC	specific rules
endif
ifdef	VENDOR
BOARDDIR = $(VENDOR)/$(BOARD)
else
BOARDDIR = $(BOARD)
endif
ifdef	BOARD
sinclude $(TOPDIR)/board/$(BOARDDIR)/config.mk	# include board specific rules
endif

#sinclude /home/sw/pp_self/1/iTop4412_uboot/board/samsung/smdkc210/config.mk	# include board specific rules
#########################################################################

ifneq (,$(findstring s,$(MAKEFLAGS)))
ARFLAGS = cr
else
ARFLAGS = crv
endif
RELFLAGS= $(PLATFORM_RELFLAGS)
DBGFLAGS= -g # -DDEBUG
OPTFLAGS= -Os #-fomit-frame-pointer
ifndef LDSCRIPT
#LDSCRIPT := $(TOPDIR)/board/$(BOARDDIR)/u-boot.lds.debug
ifeq ($(CONFIG_NAND_U_BOOT),y)
LDSCRIPT := $(TOPDIR)/board/$(BOARDDIR)/u-boot-nand.lds
else
LDSCRIPT := $(TOPDIR)/board/$(BOARDDIR)/u-boot.lds
endif
endif
OBJCFLAGS += --gap-fill=0xff $(DBGFLAGS)


gccincdir := $(shell $(CC) -print-file-name=include)

CPPFLAGS := $(DBGFLAGS) $(OPTFLAGS) $(RELFLAGS)		\
	-D__KERNEL__
ifneq ($(TEXT_BASE),)
CPPFLAGS += -DTEXT_BASE=$(TEXT_BASE)
endif

ifneq ($(RESET_VECTOR_ADDRESS),)
CPPFLAGS += -DRESET_VECTOR_ADDRESS=$(RESET_VECTOR_ADDRESS)
endif

ifneq ($(OBJTREE),$(SRCTREE))
CPPFLAGS += -I$(OBJTREE)/include2 -I$(OBJTREE)/include
endif

CPPFLAGS += -I$(TOPDIR)/include
CPPFLAGS += -fno-builtin -ffreestanding -nostdinc	\
	-isystem $(gccincdir) -pipe $(PLATFORM_CPPFLAGS)

ifdef BUILD_TAG
CFLAGS := $(CPPFLAGS) -Wall -Wstrict-prototypes \
	-DBUILD_TAG='"$(BUILD_TAG)"'
else
CFLAGS := $(CPPFLAGS) -Wall -Wstrict-prototypes
endif

CFLAGS += $(DBGFLAGS) $(call cc-option,-fno-stack-protector)

# avoid trigraph warnings while parsing pci.h (produced by NIOS gcc-2.9)
# this option have to be placed behind -Wall -- that's why it is here
ifeq ($(ARCH),nios)
ifeq ($(findstring 2.9,$(shell $(CC) --version)),2.9)
CFLAGS := $(CPPFLAGS) -Wall -Wno-trigraphs
endif
endif

# $(CPPFLAGS) sets -g, which causes gcc to pass a suitable -g<format>
# option to the assembler.
AFLAGS_DEBUG := -g

# turn jbsr into jsr for m68k
ifeq ($(ARCH),m68k)
ifeq ($(findstring 3.4,$(shell $(CC) --version)),3.4)
AFLAGS_DEBUG := -Wa,-gstabs,-S
endif
endif

AFLAGS := $(AFLAGS_DEBUG) -D__ASSEMBLY__ $(CPPFLAGS)

LDFLAGS += -Bstatic -T $(obj)u-boot.lds $(PLATFORM_LDFLAGS) $(DBGFLAGS)
ifneq ($(TEXT_BASE),)
LDFLAGS += -Ttext $(TEXT_BASE)
endif

# Location of a usable BFD library, where we define "usable" as
# "built for ${HOST}, supports ${TARGET}".  Sensible values are
# - When cross-compiling: the root of the cross-environment
# - Linux/ppc (native): /usr
# - NetBSD/ppc (native): you lose ... (must extract these from the
#   binutils build directory, plus the native and U-Boot include
#   files don't like each other)
#
# So far, this is used only by tools/gdb/Makefile.

ifeq ($(HOSTOS),darwin)
BFD_ROOT_DIR =		/usr/local/tools
else
ifeq ($(HOSTARCH),$(ARCH))
# native
BFD_ROOT_DIR =		/usr
else
#BFD_ROOT_DIR =		/LinuxPPC/CDK		# Linux/i386
#BFD_ROOT_DIR =		/usr/pkg/cross		# NetBSD/i386
BFD_ROOT_DIR =		/opt/powerpc
endif
endif

#########################################################################

export	HOSTCC HOSTCFLAGS HOSTLDFLAGS PEDCFLAGS HOSTSTRIP CROSS_COMPILE \
	AS LD CC CPP AR NM STRIP OBJCOPY OBJDUMP MAKE
export	TEXT_BASE PLATFORM_CPPFLAGS PLATFORM_RELFLAGS CPPFLAGS CFLAGS AFLAGS

#########################################################################

# Allow boards to use custom optimize flags on a per dir/file basis
BCURDIR := $(notdir $(CURDIR))
$(obj)%.s:	%.S
	$(CPP) $(AFLAGS) $(AFLAGS_$(@F)) $(AFLAGS_$(BCURDIR)) -o $@ $<

$(obj)%.o:	%.S
#	echo "-in--sdkjfer--$(obj)%.o--%.S-"
	$(CC)  $(AFLAGS) $(AFLAGS_$(@F)) $(AFLAGS_$(BCURDIR)) -o $@ $< -c

#=====[]sw2022-05-13=======good===
#$(obj)cpu/arm_cortexa9/%.o:	%.S
#	@echo "kjkjl11"
#	$(CC)  $(AFLAGS) $(AFLAGS_$(@F)) $(AFLAGS_$(BCURDIR)) -o $@ $< -c


#$(obj)cpu/arm_cortexa9/start.o:	/home/sw/pp_self/1/iTop4412_uboot/cpu/arm_cortexa9/start.S
#	@echo "kjkjl22"
#	@echo "@F= $(@F)"
#	@echo "AFLAGS = $(AFLAGS)"
#	@mkdir -p $(obj)cpu/arm_cortexa9
#	$(CC)  $(AFLAGS) $(AFLAGS_$(@F)) $(AFLAGS_$(BCURDIR)) -o $@ $< -c

#$(obj)cpu/arm_cortexa9/s5pc210/cpu_init.o:/home/sw/pp_self/1/iTop4412_uboot/cpu/arm_cortexa9/s5pc210/cpu_init_SCP.S
#	@echo "rewffsd"
#	$(CC)  $(AFLAGS) $(AFLAGS_$(@F)) $(AFLAGS_$(BCURDIR)) -o $@ $< -c

####################################
#in board/samsung/smdkc210/config.mk
####################################
#$(obj)board/samsung/smdkc210/lowlevel_init.o:/home/sw/pp_self/1/iTop4412_uboot/board/samsung/smdkc210/lowlevel_init.S
#	@echo "asdwdadd000"
#	$(CC)  $(AFLAGS) $(AFLAGS_$(@F)) $(AFLAGS_$(BCURDIR)) -o $@ $< -c
#
#$(obj)board/samsung/smdkc210/smdkc210.o:/home/sw/pp_self/1/iTop4412_uboot/board/samsung/smdkc210/smdkc210.c
#	@echo "affafasf000"
#	$(CC)  $(CFLAGS) $(CFLAGS_$(@F)) $(CFLAGS_$(BCURDIR)) -o $@ $< -c

#$(obj)cpu/arm_cortexa9/s5pc210/%.o:	%.c
#	$(CC)  $(CFLAGS) $(CFLAGS_$(@F)) $(CFLAGS_$(BCURDIR)) -o $@ $< -c


#=================================


$(obj)%.o:	%.c
	@echo "rkjklk"
	$(CC)  $(CFLAGS) $(CFLAGS_$(@F)) $(CFLAGS_$(BCURDIR)) -o $@ $< -c
$(obj)%.i:	%.c
	$(CPP) $(CFLAGS) $(CFLAGS_$(@F)) $(CFLAGS_$(BCURDIR)) -o $@ $< -c
$(obj)%.s:	%.c
	$(CC)  $(CFLAGS) $(CFLAGS_$(@F)) $(CFLAGS_$(BCURDIR)) -o $@ $< -c -S



#$(warning sw_debug: ---$(obj)%.o----%.S---- )
#$(warning sw_debug: $(@F) )
#$(warning sw_debug: $(CC) )
#$(warning sw_debug: $(AFLAGS))
#$(warning sw_debug: $(AFLAGS_$(@F)) )
#$(warning sw_debug: $(AFLAGS_$(BCURDIR)))
#$(warning sw_debug: $@ )
#$(warning sw_debug: $<)
#$(warning sw_debug: CURDIR = $(CURDIR))
#$(warning sw_debug: BCURDIR = $(BCURDIR))
#$(warning sw_debug: obj = $(obj))
#$(warning sw_debug: out /home/sw/pp_self/1/iTop4412_uboot/config.mk)
#########################################################################
