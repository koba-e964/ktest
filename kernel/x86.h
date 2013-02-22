#ifndef X86_H_912be0f7_ebbea67a
#define X86_H_912be0f7_ebbea67a



//About IDT (gate descriptor)


#define ADR_IDT       0x0026f800
#define LIMIT_IDT     0x000007ff

typedef struct gate_descriptor {
    short offset_low, selector;
    char dw_count, access_right;
    short offset_high;
} GATE_DESCRIPTOR, *PGATE_DESCRIPTOR;


enum type_attr{
	//additional attribs of lim_addr(bit 4..7) in byte 6 in
	// segment descriptor
	AR_G         = 0x8000,// Granularity
	AR_DEF32     = 0x4000,// Whether the default operand size==32bit
	AR_CODE64    = 0x2000,// 64-bit code segment in IA-32e mode.
	AR_AVAILABLE = 0x1000,// for use by operating system	

	// type_attr 4..7 
	AR_PRESENT   = 0x80,
#define AR_DPL(x)      ((x)<<6)
	AR_S         = 0x40, //whether this is a data/code segment or a system segment



	//type bits for data segments
	AR_DATA      = 0<<3,

	AR_DATA32_RW = 0x4092,
	AR_CODE32_ER = 0x409a,
	AR_INTGATE32 = 0x008e,
	AR_LDT       = 0x0082,
};


// About GDT ( segment_descriptor )


#define ADR_GDT       0x00270000
#define LIMIT_GDT     0x0000ffff
typedef struct segment_descriptor {
    short limit_low, base_low;
    char base_mid, access_right;
    char limit_high, base_high;
} SEGMENT_DESCRIPTOR, *PSEGMENT_DESCRIPTOR;

// About TSS 

typedef struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
}TSS32;


#define AR_TSS32		0x0089



//eflags flags

enum EFLAGS_FLAG
{
	EFLAGS_DEF=2, //Always on
	EFLAGS_IF=0x200, // interruption


};



#endif
