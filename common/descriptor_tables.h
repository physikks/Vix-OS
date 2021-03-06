
#ifndef DESCRI_TABLES_H
#define DESCRI_TABLES_H

struct gdt_entry_struct
{
	uint16 limit_low;
	uint16 base_low;
	uint8 base_middle;
	uint8 access;
	uint8 granularity;
	uint8 base_high;
} __attribute__((packed));
typedef struct gdt_entry_struct gdt_entry_t;


struct gdt_ptr_struct
{
	uint16 limit;
	uint32 base;
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;

#ifdef __cplusplus
extern "C" {
#endif
void init_descriptor_tables();
#ifdef __cplusplus
}
#endif


//#include <stdint.h>


// A struct describing an interrupt gate.
struct idt_entry_struct
{
   uint16 base_lo;             // The lower 16 bits of the address to jump to when this interrupt fires.
   uint16 sel;                 // Kernel segment selector.
   uint8  always0;             // This must always be zero.
   uint8  flags;               // More flags. See documentation.
   uint16 base_hi;             // The upper 16 bits of the address to jump to.
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

// A struct describing a pointer to an array of interrupt handlers.
// This is in a format suitable for giving to 'lidt'.
struct idt_ptr_struct
{
   uint16 limit;
   uint32 base;                // The address of the first element in our idt_entry_t array.
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

// These extern directives let us access the addresses of our ASM ISR handlers.

void idt_init();

extern void isr0() ;
extern void isr1() ;
extern void isr2() ;
extern void isr3() ;
extern void isr4() ;
extern void isr5() ;
extern void isr6() ;
extern void isr7() ;
extern void isr8() ;
extern void isr9() ;
extern void isr10() ;
extern void isr11() ;
extern void isr12() ;
extern void isr13() ;
extern void isr14() ;
extern void isr15() ;
extern void isr16() ;
extern void isr17() ;
extern void isr18() ;
extern void isr19() ;
extern void isr20() ;
extern void isr21() ;
extern void isr22() ;
extern void isr23() ;
extern void isr24() ;
extern void isr25() ;
extern void isr26() ;
extern void isr27() ;
extern void isr28() ;
extern void isr29() ;
extern void isr30() ;
extern void isr31() ;
extern void isr66() ;

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
#endif
