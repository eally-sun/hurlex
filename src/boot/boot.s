; ----------------------------------------------------------------
;
; 	boot.s -- 内核从这里开始
;
;                 这里还有根据 GRUB Multiboot 规范的一些定义
;
; ----------------------------------------------------------------

MBOOT_HEADER_MAGIC 	equ 	0x1BADB002 	; Multiboot 魔数，由规范决定的

MBOOT_PAGE_ALIGN 	equ 	1 << 0    	; 0 号位表示所有的引导模块将按页(4KB)边界对齐
MBOOT_MEM_INFO 		equ 	1 << 1    	; 1 号位通过 Multiboot 信息结构的 mem_* 域包括可用内存的信息(一般的ELF映像这样就可以)
MBOOT_AOUT_KLUDGE 	equ 	1 << 16

; 注意，我们并没有加上第16位(MBOOT_AOUT_KLUDGE)，这意味着 GRUB 不会使用我们的符号表
MBOOT_HEADER_FLAGS 	equ 	MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO

; 域checksum是一个32位的无符号值，当与其他的magic域(也就是magic和flags)相加时，
; 结果必须是32位的无符号值0(即magic + flags + checksum = 0)
MBOOT_CHECKSUM 		equ 	- (MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

; 符合Multiboot规范的 OS 映象需要这样一个 magic Multiboot 头

; Multiboot 头的分布必须如下表所示：
; ----------------------------------------------------------
; 偏移量 类型 域名 备注
;
; 0  u32  magic  		必需
; 4  u32  flags  		必需 
; 8  u32  checksum  		必需 
; 12  u32  header_addr 	 	如果flags[16]被置位 
; 16  u32  load_addr  		如果flags[16]被置位 
; 20  u32  load_end_addr  	如果flags[16]被置位 
; 24  u32  bss_end_addr 	如果flags[16]被置位 
; 28  u32  entry_addr  		如果flags[16]被置位 
; 32  u32  mode_type  		如果flags[2]被置位 
; 36  u32  width  		如果flags[2]被置位 
; 40  u32  height 		如果flags[2]被置位 
; 44  u32  depth  		如果flags[2]被置位
;
; 未完，详细说明请参阅 GNU 相关文档
;-----------------------------------------------------------

[BITS 32]  ; 所有代码以 32-bit 的方式编译

; 声明一些符号
[GLOBAL mboot]
[EXTERN _code]
[EXTERN _bss]
[EXTERN _end]

mboot:
	dd MBOOT_HEADER_MAGIC 		; GRUB 会通过这个魔数判断该映像是否支持
	dd MBOOT_HEADER_FLAGS        	; GRUB 的一些加载时选项，其详细注释在定义处
	dd MBOOT_CHECKSUM            	; 检测数值，其含义在定义处

[GLOBAL start] 		; 内核代码入口
[EXTERN hx_main] 	; 内核 C 代码的入口

start:
	cli  		; 我们关闭中断，然后启动内核
	push ebx 	; 调用内核 main 函数的参数，struct multiboot *mboot_ptr
	call hx_main
	jmp $ 		; 到这里结束，其实 hlt 是不是更好点呢？关机什么的后面再说
.end:

