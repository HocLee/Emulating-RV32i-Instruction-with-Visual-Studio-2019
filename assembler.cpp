#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>// for reading working directory
#include <iostream>
#include  "ISA.h"
#include "Instruction_encode.h"

int reg_convert(char* token)
{
	int reg = 0;
	if ((strlen(token) > 1) && (strlen(token) < 3))
	{
		if ((*token == 'X') || (*token == 'x') && (*(token + 1) - 0x30 >= 0) && (*(token + 1) - 0x30 <= 9)) { reg = *(token + 1) - 0x30; }
		else { printf("not a register\n"); }
	}
	else if ((strlen(token) > 2) && (strlen(token) < 4))
	{
		if ((*token == 'X') || (*token == 'x') && (*(token + 1) - 0x30 >= 0) && (*(token + 1) - 0x30 <= 3) && (*(token + 2) - 0x30 >= 0) && (*(token + 2) - 0x30 <= 9)
			&& ((*(token + 2) - 0x30 + (*(token + 1) - 0x30) * 10 - 31) < 0))
		{
			//&&((*(token + 2) - 0x30 + (*(token + 1) - 0x30)*10 -31 )< 0)
			reg = *(token + 2) - 0x30;
			reg = reg + (*(token + 1) - 0x30) * 10;
		}
		else { printf("not a register\n"); }
	}
	else { printf("not a register\n"); }
	return reg;
}
int decoding(char* buffer)
{
	char* token;
	int inst = 0;

	token = strtok(buffer, " ");
	while (token != NULL)
	{
		for (int i = 0; i < strlen(token);i++) {if ((*(token + i)<=90)&&(*(token + i)>=65)) {*(token + i) += 0x20;}}
		printf("%s\n",token);
		if (strcmp(token, "lui") == 0)
		{
			inst += INST_load_upper_imm_opcode;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (atoi(token) & 0xfffff) << 12;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "auipc") == 0)
		{
			inst += INST_add_upper_imm_opcode;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (atoi(token) & 0xfffff) << 12;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "jal") == 0)
		{
			int temp = 0;
			inst += INST_unconditional_jump_opcode;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			temp = atoi(token) & 0xfffff;
			inst += ((temp & 0x80000) << 12)
				+ ((temp & 0x7fe) << 20)
				+ ((temp & 0x800) << 9)
				+ ((temp & 0x0ff000))
				;//imm20,imm_10,imm11,imm_8
			printf("instruction:0x%x\n", inst);
			//-1048576->1048575
		}
		else if (strcmp(token, "jalr") == 0)
		{
			inst += INST_indirect_jump_opcode;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, "(");
			inst += (atoi(token) & 0xfff) << 20;
			token = strtok(NULL, ")");
			inst += (reg_convert(token) & 0x1f) << 15;
			printf("instruction:0x%x\n", inst);//jalr rd,imm(rsc)
		}
		else if (strcmp(token, "beq") == 0)
		{
			int temp = 0;
			inst += INST_conditional_branch_opcode;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 20;
			token = strtok(NULL, ",");
			temp = atoi(token) & 0x1fff;
			inst += ((temp & 0x1e) << 7)
				+ ((temp & 0x7e0) << 20)
				+ ((temp & 0x800) >> 4)
				+ ((temp & 0x1000) << 19);//immediate calculation
			printf("instruction:0x%x\n", inst);


		}
		else if (strcmp(token, "bne") == 0)
		{
			int temp = 0;
			inst += INST_conditional_branch_opcode + 0x1000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 20;
			token = strtok(NULL, ",");
			temp = atoi(token) & 0x1fff;
			inst += ((temp & 0x1e) << 7)
				+ ((temp & 0x7e0) << 20)
				+ ((temp & 0x800) >> 4)
				+ ((temp & 0x1000) << 19);//immediate calculation
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "blt") == 0)
		{
			int temp = 0;
			inst += INST_conditional_branch_opcode + 0x4000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 20;
			token = strtok(NULL, ",");
			temp = atoi(token) & 0x1fff;
			inst += ((temp & 0x1e) << 7)
				+ ((temp & 0x7e0) << 20)
				+ ((temp & 0x800) >> 4)
				+ ((temp & 0x1000) << 19);//immediate calculation
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "bge") == 0)
		{
			int temp = 0;
			inst += INST_conditional_branch_opcode + 0x5000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 20;
			token = strtok(NULL, ",");
			temp = atoi(token) & 0x1fff;
			inst += ((temp & 0x1e) << 7)
				+ ((temp & 0x7e0) << 20)
				+ ((temp & 0x800) >> 4)
				+ ((temp & 0x1000) << 19);//immediate calculation
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "bltu") == 0)
		{
			int temp = 0;
			inst += INST_conditional_branch_opcode + 0x6000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 20;
			token = strtok(NULL, ",");
			temp = atoi(token) & 0x1fff;
			inst += ((temp & 0x1e) << 7)
				+ ((temp & 0x7e0) << 20)
				+ ((temp & 0x800) >> 4)
				+ ((temp & 0x1000) << 19);//immediate calculation
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "bgeu") == 0)
		{
			int temp = 0;
			inst += INST_conditional_branch_opcode + 0x7000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 20;
			token = strtok(NULL, ",");
			temp = atoi(token) & 0x1fff;
			inst += ((temp & 0x1e) << 7)
				+ ((temp & 0x7e0) << 20)
				+ ((temp & 0x800) >> 4)
				+ ((temp & 0x1000) << 19);//immediate calculation
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "lb") == 0)
		{
			inst += INST_load_imm_opcode;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, "(");
			inst += (atoi(token) & 0xfff) << 20;
			token = strtok(NULL, ")");
			inst += (reg_convert(token) & 0x1f) << 15;
			printf("instruction:0x%x\n", inst); //lb rd,imm(rsc)
		}
		else if (strcmp(token, "lh") == 0)
		{
			inst += INST_load_imm_opcode + 0x1000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, "(");
			inst += (atoi(token) & 0xfff) << 20;
			token = strtok(NULL, ")");
			inst += (reg_convert(token) & 0x1f) << 15;
			printf("instruction:0x%x\n", inst); //lh rd,imm(rsc)
		}
		else if (strcmp(token, "lw") == 0)
		{
			inst += INST_load_imm_opcode + 0x2000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, "(");
			inst += (atoi(token) & 0xfff) << 20;
			token = strtok(NULL, ")");
			inst += (reg_convert(token) & 0x1f) << 15;
			printf("instruction:0x%x\n", inst); //lw rd,imm(rsc)
		}
		else if (strcmp(token, "lbu") == 0)
		{
			inst += INST_load_imm_opcode + 0x4000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, "(");
			inst += (atoi(token) & 0xfff) << 20;
			token = strtok(NULL, ")");
			inst += (reg_convert(token) & 0x1f) << 15;
			printf("instruction:0x%x\n", inst); //lbu rd,imm(rsc)
		}
		else if (strcmp(token, "lhu") == 0)
		{
			inst += INST_load_imm_opcode + 0x5000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, "(");
			inst += (atoi(token) & 0xfff) << 20;
			token = strtok(NULL, ")");
			inst += (reg_convert(token) & 0x1f) << 15;
			printf("instruction:0x%x\n", inst); //lhu rd,imm(rsc)
		}
		else if (strcmp(token, "sb") == 0)
		{
			int temp = 0;
			inst += INST_store_imm_opcode + 0x0000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 20;
			token = strtok(NULL, "(");
			temp = (atoi(token) & 0xfff);
			inst += ((temp & 0x1f) << 7)
				+ ((temp & 0xfe0) << 20);
			token = strtok(NULL, ")");
			inst += (reg_convert(token) & 0x1f) << 15;
			printf("instruction:0x%x\n", inst);// sb rss,imm(rsc)
		}
		else if (strcmp(token, "sh") == 0)
		{
			int temp = 0;
			inst += INST_store_imm_opcode + 0x1000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 20;
			token = strtok(NULL, "(");
			temp = (atoi(token) & 0xfff);
			inst += ((temp & 0x1f) << 7)
				+ ((temp & 0xfe0) << 20);
			token = strtok(NULL, ")");
			inst += (reg_convert(token) & 0x1f) << 15;
			printf("instruction:0x%x\n", inst);// sh rss,imm(rsc)
		}
		else if (strcmp(token, "sw") == 0)
		{
			int temp = 0;
			inst += INST_store_imm_opcode + 0x2000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 20;
			token = strtok(NULL, "(");
			temp = (atoi(token) & 0xfff);
			inst += ((temp & 0x1f) << 7)
				+ ((temp & 0xfe0) << 20);
			token = strtok(NULL, ")");
			inst += (reg_convert(token) & 0x1f) << 15;
			printf("instruction:0x%x\n", inst);// sw rss,imm(rsc)
		}
		else if (strcmp(token, "addi") == 0)
		{
			inst += INST_register_imm_opcode + 0x0000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (atoi(token) & 0xfff) << 20;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "slti") == 0)
		{
			inst += INST_register_imm_opcode + 0x2000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (atoi(token) & 0xfff) << 20;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "sltiu") == 0)
		{
			inst += INST_register_imm_opcode + 0x3000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (atoi(token) & 0xfff) << 20;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "xori") == 0)
		{
			inst += INST_register_imm_opcode + 0x4000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (atoi(token) & 0xfff) << 20;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "ori") == 0)
		{
			inst += INST_register_imm_opcode + 0x6000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (atoi(token) & 0xfff) << 20;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "andi") == 0)
		{
			inst += INST_register_imm_opcode + 0x7000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (atoi(token) & 0xfff) << 20;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "slli") == 0)
		{
			inst += INST_register_imm_opcode + 0x1000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (atoi(token) & 0x1f) << 20;
			inst &= 0x01ffffff;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "srli") == 0)
		{
			inst += INST_register_imm_opcode + 0x5000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (atoi(token) & 0x1f) << 20;
			inst &= 0x01ffffff;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "srai") == 0)
		{
			inst += INST_register_imm_opcode + 0x5000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			token = strtok(NULL, ",");
			inst += (atoi(token) & 0x1f) << 20;
			inst = (inst & 0x01ffffff) + 0x40000000;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "add") == 0)
		{
			inst += INST_register_register_opcode + 0x0000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			// when doing the rear-end of the asm code for ex: add r1,r2,r3<- this is where our code line end so when it end it leave "\n" so we have to strtok(,"\n");
			token = strtok(NULL, "\n");
			inst += (reg_convert(token) & 0x1f) << 20;
			inst = (inst & 0x01ffffff) + 0x00000000;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "sub") == 0)
		{
			inst += INST_register_register_opcode + 0x0000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			// when doing the rear-end of the asm code for ex: add r1,r2,r3<- this is where our code line end so when it end it leave "\n" so we have to strtok(,"\n");
			token = strtok(NULL, "\n");
			inst += (reg_convert(token) & 0x1f) << 20;
			inst = (inst & 0x01ffffff) + 0x40000000;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "sll") == 0)
		{
			inst += INST_register_register_opcode + 0x1000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			// when doing the rear-end of the asm code for ex: add r1,r2,r3<- this is where our code line end so when it end it leave "\n" so we have to strtok(,"\n");
			token = strtok(NULL, "\n");
			inst += (reg_convert(token) & 0x1f) << 20;
			inst = (inst & 0x01ffffff) + 0x00000000;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "slt") == 0)
		{
			inst += INST_register_register_opcode + 0x2000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			// when doing the rear-end of the asm code for ex: add r1,r2,r3<- this is where our code line end so when it end it leave "\n" so we have to strtok(,"\n");
			token = strtok(NULL, "\n");
			inst += (reg_convert(token) & 0x1f) << 20;
			inst = (inst & 0x01ffffff) + 0x00000000;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "sltu") == 0)
		{
			inst += INST_register_register_opcode + 0x3000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			// when doing the rear-end of the asm code for ex: add r1,r2,r3<- this is where our code line end so when it end it leave "\n" so we have to strtok(,"\n");
			token = strtok(NULL, "\n");
			inst += (reg_convert(token) & 0x1f) << 20;
			inst = (inst & 0x01ffffff) + 0x00000000;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "xor") == 0)
		{
			inst += INST_register_register_opcode + 0x4000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			// when doing the rear-end of the asm code for ex: add r1,r2,r3<- this is where our code line end so when it end it leave "\n" so we have to strtok(,"\n");
			token = strtok(NULL, "\n");
			inst += (reg_convert(token) & 0x1f) << 20;
			inst = (inst & 0x01ffffff) + 0x00000000;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "srl") == 0)
		{
			inst += INST_register_register_opcode + 0x5000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			// when doing the rear-end of the asm code for ex: add r1,r2,r3<- this is where our code line end so when it end it leave "\n" so we have to strtok(,"\n");
			token = strtok(NULL, "\n");
			inst += (reg_convert(token) & 0x1f) << 20;
			inst = (inst & 0x01ffffff) + 0x00000000;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "sra") == 0)
		{
			inst += INST_register_register_opcode + 0x5000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			// when doing the rear-end of the asm code for ex: add r1,r2,r3<- this is where our code line end so when it end it leave "\n" so we have to strtok(,"\n");
			token = strtok(NULL, "\n");
			inst += (reg_convert(token) & 0x1f) << 20;
			inst = (inst & 0x01ffffff) + 0x40000000;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "or") == 0)
		{
			inst += INST_register_register_opcode + 0x6000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			// when doing the rear-end of the asm code for ex: add r1,r2,r3<- this is where our code line end so when it end it leave "\n" so we have to strtok(,"\n");
			token = strtok(NULL, "\n");
			inst += (reg_convert(token) & 0x1f) << 20;
			inst = (inst & 0x01ffffff) + 0x00000000;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "and") == 0)
		{
			inst += INST_register_register_opcode + 0x7000;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 7;
			token = strtok(NULL, ",");
			inst += (reg_convert(token) & 0x1f) << 15;
			// when doing the rear-end of the asm code for ex: add r1,r2,r3<- this is where our code line end so when it end it leave "\n" so we have to strtok(,"\n");
			token = strtok(NULL, "\n");
			inst += (reg_convert(token) & 0x1f) << 20;
			inst = (inst & 0x01ffffff) + 0x00000000;
			printf("instruction:0x%x\n", inst);
		}
		else if (strcmp(token, "ebreak") == 0) 
		{
			inst += INST_ECALL_EBREAK_opcode;
			inst += 1 << 20;
			printf("instruction:0x%x\n", inst);
		}

		token = strtok(NULL, "\n");// for getting out of the loop
	}
	return inst;
}
int assembler()
{


	char* buffer = (char*)malloc(100);

	FILE* file_asm;
	FILE* file_bin;
	int line = 0;
	int instructions = 0;
	file_asm = fopen("G:\\VLSI_ASIC_IC_designs\\CAP_STONE_PROJECT_2\\RV32i_Version\\v1\\RV32i_V1\\instructions.asm", "r");
	file_bin = fopen("G:\\VLSI_ASIC_IC_designs\\CAP_STONE_PROJECT_2\\RV32i_Version\\v1\\RV32i_V1\\instructions.bin", "rb+");
	while ((fgets(buffer, 32, file_asm) != NULL))
	{
		instructions = decoding(buffer);
		fprintf(file_bin, "%d\t//0x%x\n", instructions, instructions);
		line++;
	}
	fclose(file_bin);
	fclose(file_asm);
	return line;
}