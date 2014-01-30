#
# Makefile for the linux kernel.
#

extra-y	:= head.o vmlinux.lds

#SJK obj-y	:= setup.o or32_ksyms.o process.o dma.o \
#	   traps.o time.o irq.o entry.o ptrace.o signal.o \
#	   sys_call_table.o
obj-y	:= setup.o process.o dma.o traps.o irq.o entry.o ptrace.o \
	sys_call_table.o signal.o
obj-$(CONFIG_MODULES)		+= module.o
obj-$(CONFIG_OF)		+= prom.o
obj-$(CONFIG_EARLY_PRINTK)	+= early_printk.o

clean:
