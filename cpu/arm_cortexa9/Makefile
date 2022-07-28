#
# (C) Copyright 2000-2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#
#[]sw2022=0501====
$(warning sw_debug:  in-/home/sw/pp_self/1/iTop4412_uboot/cpu/arm_cortexa9/Makefile--)
$(warning sw_debug:  -TOPDIR = $(TOPDIR)-)
include $(TOPDIR)/config.mk
#include /home/sw/pp_self/1/iTop4412_uboot/config.mk

$(warning sw_debug:  obj = $(obj))
LIB	= $(obj)lib$(CPU).a

START	:= start.o
COBJS	:= cpu.o

SRCS	:= $(START:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))
START	:= $(addprefix $(obj),$(START))
#START	:= $(addprefix $(obj)/cpu/$(CPU),$(START))
$(warning sw_debug:  obj= $(obj)-  START = $(START)-)

#SUBDIRS = s5pc210

all:	$(obj).depend $(START) $(LIB)
	@echo "ksecdc"
	@echo "START = $(START)"


$(LIB):	$(OBJS)
	@echo "in LIB LIB= $(LIB)--OBJS=$(OBJS)--AR= $(AR)--ARFLAG=$(ARFLAGS)"
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
#include $(SRCTREE)/rules.mk
include $(TOPDIR)/rules.mk
#include /home/sw/pp_self/1/iTop4412_uboot/rules.mk


sinclude $(obj).depend
#sinclude /home/sw/pp_self/1/iTop4412_uboot/sw_build/.depend
$(warning sw_debug: SRCTREE = $(SRCTREE)-------)
$(warning sw_debug:  out-/home/sw/pp_self/1/iTop4412_uboot/cpu/arm_cortexa9/Makefile--)
#########################################################################

