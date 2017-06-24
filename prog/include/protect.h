#ifndef _PROTECT_H__
#define _PROTECT_H__
#include "type.h"

typedef struct{
    uint16_t    limit_low;
    uint16_t    base_low;
    uint8_t     base_mid;
    uint8_t     attr1;
    uint8_t     limit_high_attr2;
    uint8_t     base_high;
}Descriptor;

typedef struct{
    uint16_t    offset_low;
    uint16_t    selector;
    uint8_t     dcount;
    uint8_t     attr;
    uint16_t    offset_high;
}Gate;

typedef struct{
    uint32_t    backlink;
	uint32_t    esp0;
	uint32_t    ss0;
	uint32_t    esp1;
	uint32_t    ss1;
	uint32_t    esp2;
	uint32_t    ss2;
	uint32_t    cr3;
	uint32_t    eip;
	uint32_t    flags;
	uint32_t    eax;
	uint32_t    ecx;
	uint32_t    edx;
	uint32_t    ebx;
	uint32_t    esp;
	uint32_t    ebp;
	uint32_t    esi;
	uint32_t    edi;
	uint32_t    es;
	uint32_t    cs;
	uint32_t    ss;
	uint32_t    ds;
	uint32_t    fs;
	uint32_t    gs;
	uint32_t    ldt;
	uint16_t    trap;
	uint16_t    iobase;
}TSS;

//GDT Selector
#define INDEX_DUMMY     0
#define INDEX_MEMC      1
#define INDEX_MEMD      2
#define INDEX_VIDEO     3
#define INDEX_TSS       4
#define INDEX_LDT_FIRST 5

/* 描述符类型值说明 */
#define	DA_32			0x4000	/* 32 位段				*/
#define	DA_LIMIT_4K		0x8000	/* 段界限粒度为 4K 字节   */
#define	DA_DPL0			0x00	/* DPL = 0				*/
#define	DA_DPL1			0x20	/* DPL = 1				*/
#define	DA_DPL2			0x40	/* DPL = 2				*/
#define	DA_DPL3			0x60	/* DPL = 3				*/
/* 存储段描述符类型值说明 */
#define	DA_DR			0x90	/* 存在的只读数据段类型值		*/
#define	DA_DRW			0x92	/* 存在的可读写数据段属性值		*/
#define	DA_DRWA			0x93	/* 存在的已访问可读写数据段类型值	*/
#define	DA_C			0x98	/* 存在的只执行代码段属性值		*/
#define	DA_CR			0x9A	/* 存在的可执行可读代码段属性值		*/
#define	DA_CCO			0x9C	/* 存在的只执行一致代码段属性值		*/
#define	DA_CCOR			0x9E	/* 存在的可执行可读一致代码段属性值	*/
/* 系统段描述符类型值说明 */
#define	DA_LDT			0x82	/* 局部描述符表段类型值			*/
#define	DA_TaskGate		0x85	/* 任务门类型值				*/
#define	DA_386TSS		0x89	/* 可用 386 任务状态段类型值		*/
#define	DA_386CGate		0x8C	/* 386 调用门类型值			*/
#define	DA_386IGate		0x8E	/* 386 中断门类型值			*/
#define	DA_386TGate		0x8F	/* 386 陷阱门类型值			*/

/* 选择子类型值说明 */
/* 其中, SA_ : Selector Attribute */
#define	SA_RPL_MASK	0xFFFC
#define	SA_RPL0		0
#define	SA_RPL1		1
#define	SA_RPL2		2
#define	SA_RPL3		3

#define	SA_TI_MASK	0xFFFB
#define	SA_TIG		0
#define	SA_TIL		4

/* 中断向量 */
#define	INT_VECTOR_DIVIDE		0x0
#define	INT_VECTOR_DEBUG		0x1
#define	INT_VECTOR_NMI			0x2
#define	INT_VECTOR_BREAKPOINT	0x3
#define	INT_VECTOR_OVERFLO		0x4
#define	INT_VECTOR_BOUNDS		0x5
#define	INT_VECTOR_INVAL_OP		0x6
#define	INT_VECTOR_COPROC_NOT	0x7
#define	INT_VECTOR_DOUBLE_FAULT	0x8
#define	INT_VECTOR_COPROC_SEG	0x9
#define	INT_VECTOR_INVAL_TSS	0xA
#define	INT_VECTOR_SEG_NOT		0xB
#define	INT_VECTOR_STACK_FAULT	0xC
#define	INT_VECTOR_PROTECTION	0xD
#define	INT_VECTOR_PAGE_FAULT	0xE
#define	INT_VECTOR_COPROC_ERR	0x10

/* 中断向量 */
#define	INT_VECTOR_IRQ0			0x20
#define	INT_VECTOR_IRQ8			0x28

/* 8259A interrupt controller ports. */
#define	INT_M_CTL		0x20	/* I/O port for interrupt controller         <Master> */
#define	INT_M_CTLMASK	0x21	/* setting bits in this port disables ints   <Master> */
#define	INT_S_CTL		0xA0	/* I/O port for second interrupt controller  <Slave>  */
#define	INT_S_CTLMASK	0xA1	/* setting bits in this port disables ints   <Slave>  */

void init_interrupt();

#endif