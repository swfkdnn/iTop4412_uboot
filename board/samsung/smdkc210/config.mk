$(warning sw_debug: in--/home/sw/pp_self/1/iTop4412_uboot/board/samsung/smdkc210--)
TEXT_BASE = 0xc3e00000
//TEXT_BASE = 0x41000000
# =========================================================================
#
# Supply options according to compiler version
#
# =========================================================================
PLATFORM_RELFLAGS +=$(call cc-option,-mshort-load-bytes,\
		    $(call cc-option,-malignment-traps,))

$(warning sw_debug: out--/home/sw/pp_self/1/iTop4412_uboot/board/samsung/smdkc210--)
