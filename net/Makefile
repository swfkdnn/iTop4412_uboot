include $(TOPDIR)/config.mk

# CFLAGS += -DDEBUG

LIB	= $(obj)libnet.a

#COBJS-$(CONFIG_CMD_NET)  += bootp.o
#COBJS-$(CONFIG_CMD_DNS)  += dns.o
#COBJS-$(CONFIG_CMD_NET)  += eth.o
#COBJS-$(CONFIG_CMD_NET)  += net.o
#COBJS-$(CONFIG_CMD_NFS)  += nfs.o
#COBJS-$(CONFIG_CMD_NET)  += rarp.o
#COBJS-$(CONFIG_CMD_SNTP) += sntp.o
#COBJS-$(CONFIG_CMD_NET)  += tftp.o

COBJS-y += bootp.o
COBJS-y += dns.o
COBJS-y += net.o
COBJS-y += eth.o
COBJS-y += nfs.o
COBJS-y += rarp.o
COBJS-y += sntp.o
COBJS-y += tftp.o

COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB):	$(obj).depend $(OBJS)
	@echo "~~~in -/net/Makefile a5463621  ar?-"
	@echo "~~~OBJS = $(OBJS)"
	@echo "~~~@    = $(@)"
	@echo "~~~ COBJS-(CONFIG_CMD_NET)   = COBJS-$(CONFIG_CMD_NET)"
	@echo "~~~ COBJS-(CONFIG_CMD_DNS)   = COBJS-$(CONFIG_CMD_DNS)"
	@echo "~~~CONFIG_CMD_NET = $(CONFIG_CMD_NET)"
	@echo "~~~COBJS = $(COBJS) "
	@echo "~~~ARFLAGS = $(ARFLAGS) "
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
