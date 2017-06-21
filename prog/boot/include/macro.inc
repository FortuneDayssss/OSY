%macro Descriptor 3
    dw      %2 & 0ffffh
    dw      %1 & 0ffffh
    db      (%1 >> 16) & 0ffh
    dw      ((%2 >> 8) & 0f00h) | (%3 & 0f0ffh)
    db      (%1 >> 24) & 0ffh
%endmacro

DA_32       EQU	4000h
DA_LIMIT_4K EQU	8000h

DA_DPL0     EQU	  00h	; DPL = 0
DA_DPL1     EQU	  20h	; DPL = 1
DA_DPL2     EQU	  40h	; DPL = 2
DA_DPL3     EQU	  60h	; DPL = 3


; 存储段描述符类型
DA_DR       EQU	90h	; 存在的只读数据段类型值
DA_DRW      EQU	92h	; 存在的可读写数据段属性值
DA_DRWA     EQU	93h	; 存在的已访问可读写数据段类型值
DA_C        EQU	98h	; 存在的只执行代码段属性值
DA_CR       EQU	9Ah	; 存在的可执行可读代码段属性值
DA_CCO      EQU	9Ch	; 存在的只执行一致代码段属性值
DA_CCOR     EQU	9Eh	; 存在的可执行可读一致代码段属性值

; 系统段描述符类型
DA_LDT		EQU	  82h	; 局部描述符表段类型值
DA_TaskGate	EQU	  85h	; 任务门类型值
DA_386TSS	EQU	  89h	; 可用 386 任务状态段类型值
DA_386CGate	EQU	  8Ch	; 386 调用门类型值
DA_386IGate	EQU	  8Eh	; 386 中断门类型值
DA_386TGate	EQU	  8Fh	; 386 陷阱门类型值

; 分页常量
PG_P		EQU	1	; 页存在属性位
PG_RWR		EQU	0	; R/W 属性位值, 读/执行
PG_RWW		EQU	2	; R/W 属性位值, 读/写/执行
PG_USS		EQU	0	; U/S 属性位值, 系统级
PG_USU		EQU	4	; U/S 属性位值, 用户级