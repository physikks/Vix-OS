#include "paging.h"
#include "kheap.h"
#include "../video/vga_driver.h"

page_directory_t *kernel_directory=0;
page_directory_t *current_directory=0;

uint32 *frames;
uint32 nframes;

void panic(char* msg, char* file, int line){
	print_string(msg);
	print_string(" in file : ");
	print_string(file);
	print_string(" at line : ");
	print_int(line);
	print_new_line();
	while(1);
}

extern uint32 placement_adress;
// ON va utiliser des bitmaps
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

static void set_frame(uint32 frame_addr){
	uint32 frame = frame_addr/0x1000;
	uint32 idx = INDEX_FROM_BIT(frame);
	uint32 off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}


static void clear_frame(uint32 frame_addr){
	uint32 frame = frame_addr/0x1000;
	uint32 idx = INDEX_FROM_BIT(frame);
	uint32 off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}


static uint32 test_frame(uint32 frame_addr){
	uint32 frame = frame_addr/0x1000;
	uint32 idx = INDEX_FROM_BIT(frame);
	uint32 off = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x1 << off));
}


static uint32 first_frame(){ // On cherche la premiere frame libre
	uint32 i, j;
	for(i = 0; i < INDEX_FROM_BIT(nframes); i++){
		if(frames[i] != 0xFFFFFFFF){
			for(j = 0; j < 32; j++){
				uint32 to_test = 0x1 << j;
				if(!(frames[i] & to_test)){
					return i*4*8 +j;
				}
			}
		}
	}
	return (uint32)-1;
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable,uint32 page_virtual_address,bool is_identity)
{
	if(page->frame != 0)
	{
		return; // La frame a déjà été allouée
	}
	uint32 idx = (is_identity)?page_virtual_address/0x1000:first_frame();
	if(idx == (uint32) -1){
		PANIC("No free frames"); // Faut gerer les erreurs
	}
	if(!is_identity) set_frame(idx * 0x1000);
	page->present=1;
	page->rw = (is_writeable)?1:0;
	page->user =(is_kernel)?0:1;
	page->frame = idx;
}

void free_frame(page_t *page){

	uint32 frame=page->frame;
	if(!frame)
		return;
	else
	{
		clear_frame(frame*0x1000);
		page->frame = 0x0;
	}
}

uint32 memory_use()
{
	int mem_use=0;
	for(int i = 0; i < INDEX_FROM_BIT(nframes); i++){
		for(int j = 0; j < 32; j++){
			uint32 to_test = 0x1 << j;
			if(frames[i] & to_test){
				mem_use++;
			}
		}
	}
	return mem_use;
}

void free_dir(page_directory_t *dir)
{
	for(int i=0;i<1024;i++)
	{
		for(int j=0;j<1024;j++)
		{
			if(dir->tables[i]->pages[j].frame<<12>placement_adress)
			{
				free_frame(&dir->tables[i]->pages[j]);
				*(uint32*)&dir->tables[i]->pages[j]=0;
			}
		}
	}	
}

// On va faire le paging

void init_paging(uint32 mem_size){
	uint32 mem_end_page = mem_size+0x100000;
	nframes = mem_end_page / 0x1000;//nombre de frames
	frames = (uint32*)kmalloc(INDEX_FROM_BIT(nframes)*4);// je sais pas pourquoi index_from_bit donc je rajoute *4 (du coup je l'ai enlevé, ca a l'air de mieux marcher...) (pas du tout donc je l'ai remis)
	memset(frames, 0, INDEX_FROM_BIT(nframes));
	kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
	memset(kernel_directory, 0, sizeof(page_directory_t));
	current_directory = kernel_directory;

	// On doit identifier et allouer les frames du kernel
	int i = 0;
	while(i < placement_adress)
	{
		alloc_frame( get_page(i, 1, kernel_directory), 1, 1,i,false);
		i += 0x1000;
	}
	//on identifie le reste sans allouer
	while(i<mem_end_page)
	{
		alloc_frame(get_page(i,1,kernel_directory),1,1,i,true);
		i+=0x1000;
	}
	register_interrupt_handler(14, page_fault); // On enregistre le page_fault 
	// Et la on active le paging
	switch_page_directory(kernel_directory);
}



void switch_page_directory(page_directory_t *dir){
	uint32 cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 &= ~0x80000000; // On desactive le paging
	asm volatile("mov %0, %%cr0":: "r"(cr0));

	current_directory = dir;
	asm volatile("mov %0, %%cr3" :: "r"(&(dir->tablesPhysical)));

	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000001; // On active le paging
	asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(uint32 address, int make, page_directory_t *dir)
{
	address /= 0x1000; // On transforme l'addresse en l'index correspondant

	uint32 table_idx = address /1024;// On cherche la talbe correspondante
	if(dir->tables[table_idx]) // Cette table est assignée
	{
		return &(dir->tables[table_idx]->pages[address%1024]);
	}
	else if(make)
	{
		uint32 tmp;
		dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
		memset(dir->tables[table_idx], 0, 0x1000);
		dir->tablesPhysical[table_idx] = tmp | 0x7;
		return &(dir->tables[table_idx]->pages[address%1024]);
	}
	else
		return 0;
}


void page_fault(registers_t regs)
{
	uint32 faulting_address;
	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

	int present = !(regs.err_code & 0x1);
	int rw = regs.err_code & 0x2;
	int us = regs.err_code & 0x4;
	int reserved = regs.err_code & 0x8;
	int id = regs.err_code & 0x10;

	// print_string("Page fault ! ( ");
	// if(present) print_string("present ");
	// if(rw) print_string("read-only ");
	// if(us) print_string("user-mode ");
	// if(reserved) print_string("reserved ");
	// print_string (") at "); // TODO mettre l'adresse en hexa
	// print_hexa(faulting_address);
	// print_new_line();
	if(current_directory==kernel_directory) PANIC("Page fault");
	alloc_frame(get_page(faulting_address,1,current_directory),1,1,faulting_address,false);
	// print_string("handled !\n");

}
