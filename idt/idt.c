
#include <stdint.h>
#include <string.h>
#include "idt.h"

idt_gate_t idt_gates[256] ;
idtr_t idtr ;

static void gate_set(uint8_t n, uint32_t adrs, uint16_t slct, uint8_t flags)
{
	idt_gates[n].adrs = adrs ;
	idt_gates[n].slct = slct ;
	idt_gates[n].zeros = 0 ;
	idt_gates[n].flags = flags ;
}

static void idt_init()
{
	idtr.limit = sizeof(idt_gate_t)*256-1 ;
	idtr.base = (uint32_t) &idt_gates ;
	memset(&idt_gates, 0, idtr.limit+1) ;
	uint32_t idtr_adrs = (uint32_t) &idtr ;
	asm("lidt %0" : "=g" (idtr_adrs)) ; 

	gate_set(0, (uint32_t)isr0, 0x08, 0x8E) ;
	gate_set(1, (uint32_t)isr1, 0x08, 0x8E) ;
	gate_set(2, (uint32_t)isr2, 0x08, 0x8E) ;
	gate_set(3, (uint32_t)isr3, 0x08, 0x8E) ;
	gate_set(4, (uint32_t)isr4, 0x08, 0x8E) ;
	gate_set(5, (uint32_t)isr5, 0x08, 0x8E) ;
	gate_set(6, (uint32_t)isr6, 0x08, 0x8E) ;
	gate_set(7, (uint32_t)isr7, 0x08, 0x8E) ;
	gate_set(8, (uint32_t)isr8, 0x08, 0x8E) ;
	gate_set(9, (uint32_t)isr9, 0x08, 0x8E) ;
	gate_set(10, (uint32_t)isr10, 0x08, 0x8E) ;
	gate_set(11, (uint32_t)isr11, 0x08, 0x8E) ;
	gate_set(12, (uint32_t)isr12, 0x08, 0x8E) ;
	gate_set(13, (uint32_t)isr13, 0x08, 0x8E) ;
	gate_set(14, (uint32_t)isr14, 0x08, 0x8E) ;
	gate_set(15, (uint32_t)isr15, 0x08, 0x8E) ;
	gate_set(16, (uint32_t)isr16, 0x08, 0x8E) ;
	gate_set(17, (uint32_t)isr17, 0x08, 0x8E) ;
	gate_set(18, (uint32_t)isr18, 0x08, 0x8E) ;
	gate_set(19, (uint32_t)isr19, 0x08, 0x8E) ;
	gate_set(20, (uint32_t)isr20, 0x08, 0x8E) ;
	gate_set(21, (uint32_t)isr21, 0x08, 0x8E) ;
	gate_set(22, (uint32_t)isr22, 0x08, 0x8E) ;
	gate_set(23, (uint32_t)isr23, 0x08, 0x8E) ;
	gate_set(24, (uint32_t)isr24, 0x08, 0x8E) ;
	gate_set(25, (uint32_t)isr25, 0x08, 0x8E) ;
	gate_set(26, (uint32_t)isr26, 0x08, 0x8E) ;
	gate_set(27, (uint32_t)isr27, 0x08, 0x8E) ;
	gate_set(28, (uint32_t)isr28, 0x08, 0x8E) ;
	gate_set(29, (uint32_t)isr29, 0x08, 0x8E) ;
	gate_set(30, (uint32_t)isr30, 0x08, 0x8E) ;
	gate_set(31, (uint32_t)isr31, 0x08, 0x8E) ;
}

