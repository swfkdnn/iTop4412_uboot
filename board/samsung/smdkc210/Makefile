#
# (C) Copyright 2000, 2001, 2002
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# (C) Copyright 2008
# Guennadi Liakhovetki, DENX Software Engineering, <lg@denx.de>
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
#===[]==sw2022-0503===
include $(TOPDIR)/config.mk
#$(warning sw_debug: $(TOPDIR)/config.mk)
#==
$(warning sw_debug: sfdsfsfdfe)
include /home/sw/pp_self/1/iTop4412_uboot/board/samsung/smdkc210/config.mk

LIB	= $(obj)lib$(BOARD).a
#LIB	= $(obj)board/samsung/smdkc210/lib$(BOARD).a

COBJS-y	:= smdkc210.o
COBJS-$(CONFIG_SAMSUNG_ONENAND)	+= onenand.o

SOBJS	:= lowlevel_init.o

SRCS    := $(SOBJS:.o=.S) $(COBJS-y:.o=.c)   #in topdir rules.mk  compile
OBJS	:= $(addprefix $(obj),$(COBJS-y))
SOBJS	:= $(addprefix $(obj),$(SOBJS))

#OBJS	:= $(addprefix $(obj)/board/samsung/smdkc210/,$(COBJS-y))
#SOBJS	:= $(addprefix $(obj)/board/samsung/smdkc210/,$(SOBJS))
$(warning sw_debug: SRCS = $(SRCS))
$(warning sw_debug: SOBJS = $(SOBJS))
$(warning sw_debug: OBJS = $(OBJS))
#$(warning sw_debug: include $(TOPDIR)/config.mk)

$(LIB):	$(obj).depend $(SOBJS) $(OBJS)
	@echo "12234534656"
	$(AR) $(ARFLAGS) $@ $(SOBJS) $(OBJS)

clean:
	rm -f $(SOBJS) $(OBJS)

distclean:	clean
	rm -f $(LIB) core *.bak $(obj).depend

#########################################################################

# defines $(obj).depend target
#=====[]=sw==2022-0503====
include $(SRCTREE)/rules.mk
#include /home/sw/pp_self/1/iTop4412_uboot/rules.mk

sinclude $(obj).depend

$(warning sw_debug: out /home/sw/pp_self/1/iTop4412_uboot/board/samsung/smdkc210/Makefile)
#########################################################################
