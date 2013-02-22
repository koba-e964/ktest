 
	__asm__
	(
	".text\n"
	".globl _memtest_sub_iasm\n"
	".def	_memtest_sub_iasm;	.scl 3;	.type 32;	.endef\n"
	"_memtest_sub_iasm:\n\t"
		"pushl %edi\n\t"
		"pushl %esi\n\t"
		"pushl %ebx\n\t"
		"movl $0xaa55aa55,%esi\n\t"
		"movl $0x55aa55aa,%edi\n\t"
		"movl 0x10(%esp),%eax\n\t"//i=start
	"l77_mts_loop:\n\t"
		//"movl %eax,%ebx\n\t"
		"leal 0xffc(%eax),%ebx\n\t"
		//"addl $0xffc,%ebx\n\t"
		"movl (%ebx),%edx\n\t"
		"movl %esi,(%ebx)\n\t"
		"xorl $-1,(%ebx)\n\t"
		"cmpl (%ebx),%edi\n\t"
		"jne l77_mts_fin\n\t"
		"xorl $-1,(%ebx)\n\t"
		"cmpl (%ebx),%esi\n\t"
		"jne l77_mts_fin\n\t"
		"movl %edx,(%ebx)\n\t"
		"addl $0x1000,%eax\n\t"
		"cmpl 0x14(%esp),%eax\n\t"
		"jbe l77_mts_loop\n\t"
		"popl %ebx;\n""popl %esi;\n""popl %edi\n\t"
		"ret\n\t"


	"l77_mts_fin:\n\t"
		"movl %edx,(%ebx)\n\t"
		"popl %ebx;\n""popl %esi;\n""popl %edi\n\t"
		"ret\n\t"
	);
