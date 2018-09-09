#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/desc.h>
#include <asm/pgtable.h>
#include <asm/current.h>
#include <linux/sched.h>
#include <linux/highmem.h>



#define store_gdt(ptr) asm volatile( "sgdt %0":"=m" (*ptr) )
#define load_gdt(ptr) asm volatile( "lgdt %0":"=m" (*ptr) )



struct _TSS_32
{
	unsigned short prev_task_link, res0;
	unsigned esp0;
	unsigned short ss0, res8;
	unsigned esp1;
	unsigned short ss1,res16;
	unsigned esp2;
	unsigned short ss2, res24;
	unsigned cr3;
	unsigned eip;
	unsigned eflags;
	unsigned eax;
	unsigned ecx;
	unsigned edx;
	unsigned ebx;
	unsigned esp;
	unsigned ebp;
	unsigned esi;
	unsigned edi;
	unsigned short es,res72;
	unsigned short cs,res76;
	unsigned short ss,res80;
	unsigned short ds,res84;
	unsigned short fs,res88;
	unsigned short gs,res92;
	unsigned short ldt_set_selector, res96;
	unsigned short t_flag, io_map_base_address;
};



struct GDTINFO
{
   unsigned short limit1;           // The lower 16 bits of the limit.
   unsigned short base1;            // The lower 16 bits of the base.
   unsigned char  base2;         // The next 8 bits of the base.
   unsigned char  access;              // Access flags, determine what ring 
   unsigned char  limit2:4,granularity:4;
   unsigned char  base3;
}__attribute__((packed));



struct _TSS_32 *pTss,oldTSS;
unsigned originalEIP=0;


static void SwitchedRoutine(void)
{	
	printk("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	printk("switched\n");	
	printk("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");			
	*pTss = oldTSS;
	asm __volatile__ (".byte 0xEA");
	asm __volatile__ (".byte 0x00");
	asm __volatile__ (".byte 0x00");
	asm __volatile__ (".byte 0x00");
	asm __volatile__ (".byte 0x00");
	asm __volatile__ (".byte 0xf8");
	asm __volatile__ (".byte 0x00");
	
}




static int hello_init(void)
{
	struct GDTINFO *GDT;
	struct desc_ptr gdtptr;
	
	pgd_t *pgd;
	pte_t *ptep;
	pud_t *pud;
	pmd_t *pmd;
	struct mm_struct *mm = current->mm;



	char *p,buf[16]={0};
	int i=0,j=0;
	unsigned char v;
	unsigned Limit=0,Base=0,mask=0;


	store_gdt(&gdtptr);	
	
	printk("\n\n\n");
	printk(" gdt size %d \n",gdtptr.size);
	printk(" gdt address %x \n",gdtptr.address);
	printk(" driver init address %x\n\n",hello_init);
	

	GDT = (struct desc_struct *)gdtptr.address;
	Limit = GDT[31].limit2<<16 | GDT[31].limit1;		
	Base = GDT[31].base3<<24 | GDT[31].base2<<16 | GDT[31].base3;

	//GDT[31].access = 0x8b;	// access bit clear

	
	pTss = (struct TSS_32 *	)Base;
		
	printk("original tss base : %x\n",Base);
	if(Base > 0xf0000000)
		return 0;	



	pgd = pgd_offset(mm,Base);
	if(pgd_none(*pgd) || pgd_bad(*pgd) )
		return 0;
	printk("Valid pgd\n");
	
	pud = pud_offset(pgd,Base);
	if(pud_none(*pud) || pud_bad(*pud) )
		return 0;
	printk("Valid pud\n");
	
	pmd = pmd_offset(pud,Base);
	if(pmd_none(*pmd) || pmd_bad(*pmd) )
		return 0;
	printk("Valid pmd\n");
	
	ptep = pte_offset_map(pmd,Base);
	if( !ptep )
		return 0;
	printk("Valid pte\n");
	printk("pte : %x\n",*ptep);	

	*ptep = (pte_t)pte_mkwrite(*ptep);








	printk("our routine address : %x\n",(int)SwitchedRoutine);	
	printk("current tss eflags : %x\n",pTss->eflags);
	printk("current tss eip : %x\n",pTss->eip);
	oldTSS = *pTss;
	oldTSS.eip =&&out; 
	pTss->eip = (unsigned int)SwitchedRoutine;
	




	printk("modified tss eip : %x\n",pTss->eip);	
	printk("backuped tss eip : %x\n",oldTSS.eip);	



	printk("far jumping~\n");

		/*
	asm __volatile__ (".byte 0xEA");
	asm __volatile__ (".byte 0x00");
	asm __volatile__ (".byte 0x00");
	asm __volatile__ (".byte 0x00");
	asm __volatile__ (".byte 0x00");
	asm __volatile__ (".byte 0xf8");
	asm __volatile__ (".byte 0x00");
	

	
	printk("==================================================\n");
	for(i = 0;i<32;i++)
	{
		
		printk("gdt No%d.\n",i);
		
		Limit = GDT[i].limit2<<16 | GDT[i].limit1;		
		Base = GDT[i].base3<<24 | GDT[i].base2<<16 | GDT[i].base3;

		printk("limit : %x\n",Limit);
		printk("base : %x\n",Base);
		printk("access : %hhx\n",GDT[i].access);
		printk("granularity : %hhx\n",GDT[i].granularity);	

		printk("==================================================\n");
	}
*/
out:
	printk("returned ...!!!\n"); 
	return 0;
}

static void hello_exit(void)
{

	printk("end module\n=========================================================\n");

}


module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");



