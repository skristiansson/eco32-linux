# BK Id: %F% %I% %G% %U% %#%
#
# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies. Remember to do have actions
# for "archclean" and "archdep" for cleaning up and making dependencies for
# this architecture
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 1994 by Linus Torvalds
#
# Based on:
# arch/i386/Makefile

KBUILD_DEFCONFIG := eco32_defconfig

LDFLAGS         :=
OBJCOPYFLAGS    := -O binary -R .note -R .comment -S
LDFLAGS_vmlinux :=
LIBGCC 		:= $(shell $(CC) $(KBUILD_CFLAGS) -print-libgcc-file-name)

#KBUILD_CFLAGS	+= -D__linux__ -pipe -DDEBUG
KBUILD_CFLAGS	+= -D__linux__ -pipe

head-y 		:= arch/eco32/kernel/head.o

core-y		+= arch/eco32/lib/ \
		   arch/eco32/kernel/ \
		   arch/eco32/mm/

libs-y		+= $(LIBGCC)

ifneq '$(CONFIG_ECO32_BUILTIN_DTB)' '""'
BUILTIN_DTB := y
else
BUILTIN_DTB := n
endif
core-$(BUILTIN_DTB) += arch/eco32/boot/dts/

all: vmlinux
