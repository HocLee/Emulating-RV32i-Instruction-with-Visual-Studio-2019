#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "ISA.h"
#include "Instruction_encode.h"

extern int assembler();

#define start_address 0x1000// the allocated first address of the main program
                            //0x0->0x0fff may contain pseudo instruction

//------------------------------------------------
using Byte = unsigned char;// 8bits
using Half = unsigned short;//16bits half-word
using Word = unsigned int;//32bits
//------------------------------------------------


int bit(int bit_num, Word input)// this is to mask a single bit out for wiring sigle bit 
{
    Word temp = 0;
    bool answer = 0;
    temp = (1 << bit_num) & input; //one = 0x00000001
    answer = (temp == (1 << bit_num)) ? 1 : 0;
    return answer;
}
void binary_converter(int input)
{
    for (int i = 31; i > -1; i--) { printf("%d", bit(i, input)); }printf("\n");
}
Word *mem_start;
//======================================================================================
struct MEM { //max is 0xffff memory start from address[mem_start + (0->0xffff)], 
    
    Word *Address= (Word*)malloc(0xffff*4); // memory data with memory allocation
    void initialize() //initialise memory with memory allocation
    {
        mem_start = Address;
        for (int i=0;i<0xffff;i++)
        {
            *(Address + i) = 0;
        }
    }
};
//======================================================================================


struct CPU {
    // program counter 
    Word PC=0x1000;    
    Word PC_count;// for promting purposes not in the design
/*===============================================================
DWord SP;//stack pointer, There is no dedicated stack pointer or
//subroutine return address link register in the Base Integer ISA
===============================================================*/
    Word x[32];     //x[31->1] general register.
                    //x[0] zero register all bits are set to zeros.
//================================================================
//================================================================
    void reset(MEM& memory)
    {
        PC = start_address;
        memory.initialize();
////////// set "0x0" to all general purpose registers
        for (int i = 0; i < 32; i++){x[i] = 0x0;}
    }
    Word fetch(Word& cycles, MEM& memory)
    {
        PC_count = (PC - 0x1000) * 4 + 0x1000 ;//this is the present PC
        Word Data = *(memory.Address + PC);
        PC++; //cycles--;
        return Data;
    }
    void execute(Word cycle, MEM& memory)
    {
        bool halt = true;
        while ((cycle > 0)&&(halt))
        {
            Word inst = fetch(cycle, memory);
//==========================================================================
            printf("========================================================\n");
            binary_converter(inst);
            //printf("inst in hexadecimal representation: 0x%x",inst);
            printf("Present address = 0x%p\n", memory.Address + PC - 1);
            printf("Next address = 0x%p\n", memory.Address + PC);
            printf("present PC count base = 0x%x\n",PC_count);
            printf("Next PC count base = 0x%x\n", PC_count + 4);
            switch (inst & 0x7f)
            {
            case INST_load_upper_imm_opcode://LUI
            {
                Word imm_20 = inst & 0xfffff000,
                    rsd = (inst & 0x00000f80) >> 7;
                x[rsd] = imm_20;
                printf("set register x[%d] to the immediate value = 0x%x",rsd,imm_20);
                break;
            }
            case INST_add_upper_imm_opcode://AUIPC
            {
                Word imm_20 = inst & 0xfffff000,
                    rsd = (inst & 0x00000f80) >> 7;
                x[rsd] = imm_20 + *mem_start + PC -1;
                printf("added instruction address = 0x%x to the immediate 0x%x value and put in the register x[%d] = 0x%x\n",
                       *mem_start + PC -1,imm_20,rsd,x[rsd]);
                break;
            }
            case INST_load_imm_opcode:// load immediate 
            {
                Word imm_12 = inst >> 20,       //immediate 12bits extraction
                     rsb = (inst&0x000f8000) >> 15,          //base register 5bits address
                     funct_3 = (inst & 0x00007000) >> 12,    //function 3 ALU controller
                     rsd = (inst & 0x00000f80) >> 7;         //destination register 5bits address
                
                imm_12 = ((imm_12 & 0x800) == 0x800) ? imm_12 + 0xfffff000 : imm_12;//sign-extension
                
                switch (funct_3) 
                {
                case LB:
                {
                    if (((x[rsd] = *(memory.Address + x[rsb] + imm_12)) & 0x00000080) == 0x00000080)
                    {
                        x[rsd] = (*(memory.Address + x[rsb] + imm_12) & 0x000000ff) + 0xffffff00;
                        printf("loaded 0x%x\n", x[rsd]); break;
                    }
                    else { x[rsd] = *(memory.Address + x[rsb] + imm_12) & 0x000000ff; printf("loaded 0x%x\n", x[rsd]); break;}
                    break;
                }
                case LH:
                {
                    if (((x[rsd] = *(memory.Address + x[rsb] + imm_12)) & 0x00008000) == 0x00008000) 
                    {
                        x[rsd] = (*(memory.Address + x[rsb] + imm_12) & 0x0000ffff) + 0xffff0000;
                        printf("loaded 0x%x\n", x[rsd]); break;
                    }
                    else { x[rsd] = *(memory.Address + x[rsb] + imm_12) & 0x0000ffff; printf("loaded 0x%x\n", x[rsd]); break;}
                    break;
                }
                case LW:
                {x[rsd] = *(memory.Address + x[rsb] + imm_12) & 0xffffffff; printf("loaded 0x%x\n", x[rsd]); break; }
                case LBU:
                {x[rsd] = *(memory.Address + x[rsb] + imm_12 ) & 0x000000ff; printf("loaded 0x%x\n", x[rsd]); break; }
                case LHU:
                {x[rsd] = *(memory.Address + x[rsb] + imm_12) & 0x0000ffff; printf("loaded 0x%x\n", x[rsd]); break; }
                default: {printf("Unknown funct_3\n"); break; }
                }
                break;
            }
            case INST_store_imm_opcode:// store immediate
            {
                Word imm_7 = inst >> 25,       //immediate 12bits extraction
                     rss = (inst & 0x01f00000) >> 20,          //source register 5bits address
                     rsb = (inst & 0x000f8000) >> 15,         //base register 5bits address
                     funct_3 = (inst & 0x00007000) >> 12,    //function 3 ALU controller
                     imm_5 = (inst & 0x00000f80) >> 7,
                     imm_12 = (imm_7 << 5) + imm_5 ;
                
                imm_12 = ((imm_12 & 0x800) == 0x800) ? imm_12 + 0xfffff000 : imm_12;//sign-extension
                switch (funct_3)
                {
                case SB: 
                {
                    *(memory.Address + x[rsb] + imm_12) = x[rss] & 0x000000ff; printf("stored 0x%x\n", *(memory.Address + x[rsb] + imm_12));
                    break;
                }
                case SH:
                {
                    *(memory.Address + x[rsb] + imm_12) = x[rss] & 0x0000ffff; printf("stored 0x%x\n", *(memory.Address + x[rsb] + imm_12));
                    break;
                }
                case SW:
                {
                    *(memory.Address + x[rsb] + imm_12) = x[rss]; printf("stored 0x%x\n", *(memory.Address + x[rsb] + imm_12));
                    break;
                }
                default: {printf("Unknown funct_3\n"); break; }
                }
                break;
            }
            case INST_register_imm_opcode:// register-immediate 
            {
                Word imm_12 = inst >> 20,       //immediate 12bits extraction
                    imm_7 = inst >> 25,
                    imm_5 = (inst & 0x01f00000) >> 20,
                    rsc = (inst & 0x000f8000) >> 15,          //base register 5bits address
                    funct_3 = (inst & 0x00007000) >> 12,    //function 3 ALU controller
                    rsd = (inst & 0x00000f80) >> 7;         //destination register 5bits address

                // At this arithmetic and logical calculation we should sign-extending at each function,
                // not for the whole opcode
                /*============================================================================
                //imm_12 = ((imm_12 & 0x800) == 0x800) ? imm_12 * -1 : imm_12;//sign-extension
                ============================================================================*/
                
                switch (funct_3) 
                {
                case ADDI: 
                {
                    imm_12 = ((imm_12 & 0x800) == 0x800) ? imm_12 + 0xfffff000 : imm_12 ;//sign-extension
                    x[rsd] = x[rsc] + imm_12; //oveflow is ignored !!!
                    printf("added immediate 0x%x to register x[%d]\n", imm_12,rsc); 
                    break;
                }
                case SLTI: 
                {
                    imm_12 = ((imm_12 & 0x800) == 0x800) ? imm_12 + 0xfffff000 : imm_12 ;//sign-extension
                    int a=x[rsc], b=imm_12;// do it because of x[rsc] and imm_12 were unsigned, not signed so we need to cast signed int to the comparison
                    x[rsd] = (a < b); // compare lesser between source register and the immediate in signed.
                    if(x[rsd]==true)printf("x[%d] < %d\n", rsc, imm_12); else printf("x[%d] > or = %d\n", rsc,imm_12);
                    break;
                }
                case SLTIU: 
                {
                    imm_12 = ((imm_12 & 0x800) == 0x800) ? imm_12 + 0xfffff000 : imm_12;//sign-extension
                    x[rsd] = (x[rsc] < imm_12); // compare lesser between source register and the immediate in unsigned.
                    if (x[rsd] == true)printf("x[%d] < %u\n", rsc, imm_12); else printf("x[%d] > or = %u\n", rsc, imm_12);
                    break;
                }
                case XORI: 
                {
                    imm_12 = ((imm_12 & 0x800) == 0x800) ? imm_12 + 0xfffff000 : imm_12;//sign-extension
                    x[rsd] = x[rsc] ^ imm_12;
                    printf("x[%d] Xor-ed imm_12 = 0x%x\n",rsc,x[rsd]);
                    break; 
                }
                case ORI:
                {
                    imm_12 = ((imm_12 & 0x800) == 0x800) ? imm_12 + 0xfffff000 : imm_12;//sign-extension
                    x[rsd] = x[rsc] | imm_12;
                    printf("x[%d] Or-ed imm_12 = 0x%x\n", rsc, x[rsd]);
                    break;
                }
                case ANDI:
                {
                    imm_12 = ((imm_12 & 0x800) == 0x800) ? imm_12 + 0xfffff000 : imm_12;//sign-extension
                    x[rsd] = x[rsc] & imm_12;
                    printf("x[%d] And-ed imm_12 = 0x%x\n", rsc, x[rsd]);
                    break;
                }
                case SLLI: 
                {
                    x[rsd] = x[rsc] << imm_5;
                    printf("logical unsigned left shifted by %d : x[%d] = 0x%x\n", imm_5, rsd, x[rsd]);
                    break; 
                }
                case SRLI_SRAI: 
                {
                    if ( imm_7 == 0x20 )
                    {
                        x[rsd] = (x[rsc] >> imm_5) + ((0xffffffff >> (imm_5))^0xffffffff);
                        printf("arithmetic signed right shifted by %d : x[%d] = 0x%x\n", imm_5, rsd, x[rsd]);
                    }
                    else if( imm_7 == 0x0)
                    {
                        x[rsd] = (x[rsc] >> imm_5);
                        printf("logical unsigned right shifted by %d : x[%d] = 0x%x\n",imm_5,rsd,x[rsd]);
                    }
                    break; 
                }
                
                default: {printf("Unknown funct_3\n"); break; }
                }
                break;
            }
            case INST_register_register_opcode:// register-register
            {
                Word      
                    imm_7 = inst >> 25,
                    rsc_2 = (inst & 0x01f00000) >> 20,
                    rsc_1 = (inst & 0x000f8000) >> 15,          //base register 5bits address
                    funct_3 = (inst & 0x00007000) >> 12,    //function 3 ALU controller
                    rsd = (inst & 0x00000f80) >> 7;         //destination register 5bits address

                // At this arithmetic and logical calculation we should sign-extending at each function,
                // not for the whole opcode
                /*============================================================================
                //imm_12 = ((imm_12 & 0x800) == 0x800) ? imm_12 * -1 : imm_12;//sign-extension
                ============================================================================*/

                switch (funct_3)
                {
                case ADD_SUB:
                {
                    int a = x[rsc_1], b = x[rsc_2];
                    if (imm_7 == Unsigned_imm_7) 
                    {
                        x[rsd] = a + b; //oveflow is ignored !!!
                        printf("added register x[%d]and register x[%d] to register x[%d]= 0x%x\n", rsc_2, rsc_1, rsd,x[rsd]);
                    }
                    else if (imm_7 == Signed_imm_7) 
                    {
                        x[rsd] = a - b; //oveflow is ignored !!!
                        printf("subtracted register x[%d] from register x[%d] to register x[%d]= 0x%x\n", rsc_2, rsc_1, rsd, x[rsd]);
                    }
                    break;
                }
                case SLT:
                {
                    int a = x[rsc_1], b = x[rsc_2];// do it because of x[rsc] and imm_12 were unsigned, not signed so we need to cast signed int to the comparison
                    x[rsd] = (a < b); // compare lesser between source register and the immediate in signed.
                    if (x[rsd] == true)printf("x[%d] < %d\n", rsc_1, x[rsc_2]); else printf("x[%d] > or = %d\n", rsc_1, x[rsc_2]);
                    break;
                }
                case SLTU:
                {
                    x[rsc_2] = ((x[rsc_2] & 0x800) == 0x800) ? x[rsc_2] * -1 : x[rsc_2];//sign-extension
                    x[rsd] = (x[rsc_1] < x[rsc_2]); // compare lesser between source register and the immediate in unsigned.
                    if (x[rsd] == true)printf("x[%d] < %u\n", rsc_1, x[rsc_2]); else printf("x[%d] > or = %u\n", rsc_1, x[rsc_2]);
                    break;
                }
                case XOR:
                {
                    x[rsd] = x[rsc_1] ^ x[rsc_2];
                    printf("x[%d] Xor-ed x[%d] = 0x%x\n", rsc_1, rsc_2, x[rsd]);
                    break;
                }
                case OR:
                {
                    x[rsd] = x[rsc_1] | x[rsc_2];
                    printf("x[%d] Or-ed x[%d] = 0x%x\n", rsc_1, rsc_2, x[rsd]);
                    break;
                }
                case AND:
                {
                    x[rsd] = x[rsc_1] & x[rsc_2];
                    printf("x[%d] And-ed x[%d] = 0x%x\n", rsc_1, rsc_2, x[rsd]);
                    break;
                }
                case SLL:
                {
                    x[rsd] = x[rsc_1] << x[rsc_2];
                    printf("logical unsigned left shifted by %d : x[%d] = 0x%x\n", x[rsc_2], rsd, x[rsd]);
                    break;
                }
                case SRL_SRA:
                {
                    if (imm_7 == 0x20)
                    {
                        x[rsd] = (x[rsc_1] >> x[rsc_2]) + ((0xffffffff >> (x[rsc_2])) ^ 0xffffffff);
                        printf("arithmetic signed right shifted by %d : x[%d] = 0x%x\n", x[rsc_2], rsd, x[rsd]);
                    }
                    else if (imm_7 == 0x0)
                    {
                        x[rsd] = (x[rsc_1] >> x[rsc_2]);
                        printf("logical unsigned right shifted by %d : x[%d] = 0x%x\n", x[rsc_2], rsd, x[rsd]);
                    }
                    break;
                }
                default: {printf("Unknown funct_3\n"); break; }
                }
                break;
            }
            case INST_conditional_branch_opcode:// register-register
            {
                Word imm12 = inst & 0x80000000,
                    imm_6 = (inst & 0x7e000000) >> 25,
                    rsc_2 = (inst & 0x01f00000) >> 20,
                    rsc_1 = (inst & 0x000f8000) >> 15,
                    funct_3 = (inst & 0x00007000) >> 12,
                    imm_4 = (inst & 0x00000f00) >> 8,
                    imm11 = (inst & 0x00000080) >> 7,
                    imm_B;
                //assemblying the imm_B stage
                //========================================
                if (imm12 == 0x80000000) { imm12 = 0xfffff000; }
                else { imm12 = 0x0; };
                imm_B = (imm12 + (imm11 << 11) + (imm_6 << 5) + (imm_4 << 1))&0xfffffffe;
                //========================================
                switch (funct_3)
                {
                case BEQ:// Branch equal 
                {
                    if (x[rsc_1] == x[rsc_2]) 
                    {
                        PC = (imm_B / 4) + PC -1;
                        printf("x[%d] is equal to x[%d]\n", rsc_1, rsc_2);
                        printf("range is %d ,PC = 0x%p\n", imm_B, mem_start + PC);
                    }
                    else { printf("x[%d] is not equal to x[%d]\n", rsc_1, rsc_2); printf("pass branch\n"); }
                    
                    //printf("x[%d] = 0x%x\n",rsc_1,x[rsc_1]);
                    break;
                }
                case BGE:// Branch greater or equal Signed
                {
                    int a = x[rsc_1], b = x[rsc_2];
                    if (a >= b) 
                    { 
                        PC = ((int)imm_B / 4) + PC -1;
                        printf("x[%d] is greater or equal to x[%d]\n", rsc_1, rsc_2);
                        printf("range is %d ,PC = 0x%p\n", imm_B, mem_start + PC);
                    }
                    else { printf("x[%d] is less than x[%d]\n", rsc_1, rsc_2); printf("pass branch\n"); }
                    break;
                }
                case BGEU:// Branch greater or equal Unsinged
                {
                    if (x[rsc_1] >= x[rsc_2]) 
                    { 
                        PC = ((int)imm_B / 4) + PC -1;
                        printf("x[%d] is greater or equal to x[%d]\n", rsc_1, rsc_2);
                        printf("range is %d ,PC = 0x%p\n", imm_B, mem_start + PC);
                    }
                    else { printf("x[%d] is less than x[%d]\n", rsc_1, rsc_2); printf("pass branch\n"); }
                    
                    break;
                }
                case BLT:// Branch less than Signed
                {
                    int a = x[rsc_1], b = x[rsc_2];
                    if (a < b) 
                    { 
                        PC = (imm_B / 4) + PC -1; 
                        printf("x[%d] is less than x[%d]\n", rsc_1, rsc_2);
                        printf("range is %d ,PC = 0x%p\n", imm_B, mem_start + PC);
                    }
                    else { printf("x[%d] is greater or equal to x[%d]\n", rsc_1, rsc_2); printf("pass branch\n"); }
                    break;
                }
                case BLTU:// Branch less than Unsigned
                {
                    if (x[rsc_1] < x[rsc_2]) 
                    { 
                        PC = ((int)imm_B / 4) + PC -1; 
                        printf("x[%d] is less than x[%d]\n", rsc_1, rsc_2);
                        printf("range is %d ,PC = 0x%p\n", imm_B, mem_start + PC);
                    }
                    else { printf("x[%d] is greater or equal to x[%d]\n", rsc_1, rsc_2); printf("pass branch\n"); }
                    break;
                }
                case BNE:// Branch not equal
                {
                    if (x[rsc_1] != x[rsc_2]) 
                    {
                        PC = ((int)imm_B / 4) + PC -1; 
                        printf("x[%d] is not equal to x[%d]\n", rsc_1, rsc_2);
                        printf("range is %d ,PC = 0x%p\n", imm_B, mem_start + PC);
                    }
                    else { printf("x[%d] is equal to x[%d]\n", rsc_1, rsc_2); printf("pass branch\n"); }
                    //printf("x[%d] = 0x%x\n",rsc_1,x[rsc_1]);
                    break;
                }
                default: { printf("Unknown funct_3\n"); break; }
                }
                break;
            }
            case INST_unconditional_jump_opcode:// unconditional jump 
            {
                Word imm20 = inst & 0x80000000,
                imm_10  = (inst & 0x7fe00000) >> 21,
                imm11   = (inst & 0x00100000) >> 20,
                imm_8   = (inst & 0x000ff000) >> 12,
                rsd     = (inst & 0x00000f80) >> 7,
                imm_J;

                if (imm20 == 0x80000000) { imm20 = 0xfff00000; }
                else { imm20 = 0x0; };
                imm_J = (imm20 + (imm_8 << 12) + (imm11 << 11) + (imm_10 << 1)) & 0xfffffffe;
                x[rsd] = PC;// PC + 4 :(PC_count)
                PC = ((int)imm_J / 4) + PC -1;
                printf("jump to 0x%p\n",imm_J );
                //binary_converter(imm_J);
                break;
            }
            case INST_indirect_jump_opcode:// indirect jump
            {
                Word imm_12 = inst  >> 20,
                    rsb = (inst & 0x000f8000) >> 15,
                    funct_3 = ((inst & 0x00007000) & 0x0) >> 12,
                    rsd = (inst & 0x00000f80) >> 7;

                imm_12 = ((imm_12 & 0x800) == 0x800) ? (imm_12 + 0xfffff000)&0xfffffffe : imm_12 & 0xfffffffe;//sign-extension
                x[rsd] = PC ;
                PC = ((int)(imm_12)/4) + PC -1;
                printf("jump to = 0x%p\n", mem_start + PC);
                break;
            }
            case INST_FENCE_opcode:// FENCE
            {
                break;
            }
            case INST_ECALL_EBREAK_opcode:// ECALL or EBREAK
            {
                Word funct_12 = inst >> 20;
                switch (funct_12)
                {
                case ECALL: {   break; }
                case EBREAK: {  printf("program halted\n"); halt = false; break; }
                }
                break;
            }
            default: {printf("Instruction not handled : %x\n", inst); break; }
            }
//======================================================================================
            
            printf("========================================================\n");
        }
    }
};
int main() 
{
    int line=0; //this variable will decide how much number of line of codes will be execute 
    FILE* file_bin; // file reading pointer
    char*buffer=(char*)malloc(32); // initializing buffer pointer for reading out instructions
    //open file_bin
    file_bin = file_bin = fopen("G:\\VLSI_ASIC_IC_designs\\CAP_STONE_PROJECT_2\\RV32i_Version\\v1\\RV32i_V1\\instructions.bin", "r");
        //create the cpu core and RAM
        CPU cpu; MEM mem;
        //reset the memory for initial run
        cpu.reset(mem);
        //set up memory and register to run some example 
        line=assembler();
        /*executation of the cpu*/
        for (int i = 0; i < line;i++) //count the line of asm codes to feed to the execution stage
        {
            *(mem.Address + start_address + i) = atoi(fgets(buffer,32,file_bin));// gets a lines by lines of code as string then convert to integer to put in memory
            printf("0x%x\n", *(mem.Address + start_address + i));// promting out the monitor the machine code 
        }
        //==================
        cpu.execute(line, mem);
        //==================
        //promting register 
        for (int i = 0; i < 32; i++)
        {
            if (i <= 9) {
                printf("x[%d ] = 0x%x | %d | ", i, cpu.x[i], cpu.x[i]); //}
                binary_converter(cpu.x[i]);
            }
            else {
                printf("x[%d] = 0x%x | %d | ", i, cpu.x[i], cpu.x[i]); //}
                binary_converter(cpu.x[i]);
            }
        }
        //showing the stop address
        printf("\nPC = 0x%p\n",mem.Address + cpu.PC);
        free(buffer);
        fclose(file_bin);// close file
    return 0;
}
/* Reuse code */
//===================================================================================================
////binary_converter(*(mem.Data + 0));
   /*Byte    N = 0,      //Negative flag
            Z = 1,      //Zero flag
            C = 0,      //Carry-out flag
            V = 0;      //Overflowe flag*/
/*printf("Negative flag   = %d\n", cpu.N);
        printf("Zero flag       = %d\n", cpu.Z);
        printf("Carry_out flag  = %d\n", cpu.C);
        printf("Overflowe flag  = %d\n", cpu.V);*/
        //*(mem.Address + start_address + 3) = JAL(0x1,0x3fe,0x1, 0xff,x_2,INST_unconditional_jump_opcode);
        //*(mem.Address + start_address + 3) = JALR(0xffe,x_10,0x0,x_11,INST_indirect_jump_opcode);
        //*(mem.Address + start_address + 2) = Reg_imm(0xff, x_7, ADDI, x_8, INST_register_imm_opcode);
        //*(mem.Address + start_address + 3) = Reg_imm(0x123, x_6, SLTI, x_7, INST_register_imm_opcode);
        //*(mem.Address + start_address + 4) = Reg_imm(0xfff, x_6, SLTIU, x_7, INST_register_imm_opcode);
        //*(mem.Address + start_address + 4) = Reg_imm(0xfff, x_6, ANDI, x_7, INST_register_imm_opcode);
        //*(mem.Address + start_address + 5) = Shift_imm(Signed_imm_7, 0x2,x_6, SRLI_SRAI, x_7, INST_register_imm_opcode);
        // //*(mem.Address + start_address) = Load(0x0, x_5, LW, x_5, INST_load_imm_opcode);
        // //*(mem.Address + start_address ) = Reg_imm(0xfff, x_8, ADDI, x_9, INST_register_imm_opcode);
        //*(mem.Address + start_address + 1) = Reg_imm(0xfff, x_8, ADDI, x_9, INST_register_imm_opcode);
        //*(mem.Address + start_address + 2) = Reg_reg(Signed_imm_7, x_8, x_7, ADD_SUB, x_7, INST_register_register_opcode);
        //*(mem.Address + start_address + 3) = Branches(1, 0x3f, x_6, x_7, BEQ, 0xe, 1, INST_conditional_branch_opcode);
        //*(mem.Address + start_address + 4) = AUIPC(0x0,x_30,INST_add_upper_imm_opcode);
//===================================================================================================