

/* Instruction opcode */
//==========================================================================
#define  INST_load_upper_imm_opcode		0x37//LUI
#define  INST_add_upper_imm_opcode		0x17//AUIPC
#define	 INST_load_imm_opcode			0x03
#define	 INST_store_imm_opcode			0x23
#define	 INST_register_imm_opcode		0x13
#define	 INST_register_register_opcode  0x33
#define  INST_conditional_branch_opcode 0x63
#define	 INST_unconditional_jump_opcode 0x6f//JAL
#define	 INST_indirect_jump_opcode		0x67//JALR
#define	 INST_FENCE_opcode				0x0f
#define  INST_ECALL_EBREAK_opcode		0x73
//==========================================================================

/* Instruction funct_3 for "INST_conditional_branch_opcode" */
//==========================================================================
#define BEQ 0x0 
#define BNE 0x1
#define BLT 0x4
#define BGE 0x5
#define BLTU 0x6
#define BGEU 0x7
//==========================================================================



/* Instruction funct_3 for "INST_load_imm_opcode" */
//==========================================================================
#define LB 0x0 //load byte, 8bits, signed extend
#define LH 0x1 //load half word, 16bits, signed extend
#define LW 0x2 //load Word, 32bits
#define LBU 0x4 //load byte, 8bits, unsigned
#define LHU 0x5 //load half word, 16bits, unsigned
//==========================================================================


/* Instruction funct_3 for "INST_store_imm_opcode" */
//==========================================================================
#define SB 0x0 //store byte, 8bits, signed extend
#define SH 0x1 //store half word, 16bits, signed extend
#define SW 0x2 //store Word, 32bits
//==========================================================================


/* Instruction funct_3 for "INST_register_imm_opcode" */
//==========================================================================
#define ADDI	0x0 //add immediate 12bits arithmetic
#define SLTI	0x2 //
#define SLTIU	0x3 //
#define XORI	0x4	//immediate exclusive or 12bits logic
#define ORI		0x6 //immediate or 12bits logic
#define ANDI	0x7 //immediate and 12bits logic
//========================================================================== with "shamt"-shift amount = 5bits
#define SLLI		0x1
#define SRLI_SRAI	0x5
//==========================================================================


/* Instruction funct_3 for "INST_register_register_opcode" */
//==========================================================================
#define ADD_SUB	0x0 
#define SLL		0x1 //
#define SLT		0x2 //
#define SLTU	0x3
#define XOR 	0x4	
#define SRL_SRA	0x5 // use Unsinged_imm_7 and Signed_imm_7
#define OR 		0x6 
#define AND 	0x7 
//==========================================================================


/* Instruction funct_7 for "INST_register_imm_opcode" and "INST_register_register_opcode" */
//==========================================================================
#define Unsigned_imm_7	0x0		// these two signal will command the ALU to use 
#define Signed_imm_7	0x20	// signed mode on arithmetic and logical operation 
//==========================================================================


/* Instruction funct_12 for "INST_ECALL_EBREAK_opcode" */
//==========================================================================
#define ECALL	0x0
#define EBREAK	0x1
//==========================================================================


/* Instruction */
//==========================================================================

//==========================================================================


/* General Register */
//==========================================================================
#define x_0 0x0 //Hard wired zero:zero--
#define x_1 0x1 //return address:ra 
#define x_2 0x2 //stack pointer:sp
#define x_3 0x3 //global pointer:gp
#define x_4 0x4 //thread pointer:tp
#define x_5 0x5 //temporary register:t0
#define x_6 0x6 //temporary register:t1
#define x_7 0x7 //temporary register:t2
#define x_8 0x8 //saved register:s0
#define x_9 0x9 //saved register:s1
#define x_10 0xa //function arguments and return values:a0
#define x_11 0xb //function arguments and return values:a1
#define x_12 0xc //function arguments:a2
#define x_13 0xd //function arguments:a3
#define x_14 0xe //function arguments:a4
#define x_15 0xf //function arguments:a5
#define x_16 0x10 //function arguments:a6
#define x_17 0x11 //function arguments:a7
#define x_18 0x12 //saved register:s2
#define x_19 0x13 //saved register:s3
#define x_20 0x14 //saved register:s4
#define x_21 0x15 //saved register:s5
#define x_22 0x16 //saved register:s6
#define x_23 0x17 //saved register:s7
#define x_24 0x18 //saved register:s8
#define x_25 0x19 //saved register:s9
#define x_26 0x1a //saved register:s10
#define x_27 0x1b //saved register:s11
#define x_28 0x1c //temporary register:t3
#define x_29 0x1d //temporary register:t4
#define x_30 0x1e //temporary register:t5
#define x_31 0x1f //temporary register:t6
//==========================================================================
