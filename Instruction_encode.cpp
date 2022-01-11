#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Instruction_encode.h"

//------------------------------------------------
using Byte = unsigned char;// 8bits
using Half = unsigned short;//16bits half-word
using Word = unsigned int;//32bits
//------------------------------------------------

Word Load(int imm_12, int RSB, int funct_3, int RSD, int opcode) //instruction construct of load immediate address
{
    Word inst;
    //filtering bits range
//========================================
    imm_12 &= 0xfff;   // signed immediate nunmber, take only 12bits
    RSB &= 0x1f;      // register base, take only 5bits
    funct_3 &= 0x7;  // function 3bits, take only 5bits
    RSD &= 0x1f;      // register base, take only 3bits
    opcode &= 0x7f;   // opcode, take only 7bits
//========================================
    //concatenation to full word instruction to feed to mem
    inst = (imm_12 << 20) + (RSB << 15) + (funct_3 << 12) + (RSD << 7) + opcode;
    return inst;
}
Word Store(int imm_7, int RSS, int RSB, int funct_3, int imm_5, int opcode) //instruction construct of store immediate 
{
    Word inst;
    //filtering bits range
//========================================
    imm_7 &= 0x7f;   // signed immediate nunmber, take only 7bits
    RSS &= 0x1f;      // register base, take only 5bits
    RSB &= 0x1f;      // register base, take only 5bits
    funct_3 &= 0x7;  // function 3bits, take only 3bits
    imm_5 &= 0x1f;  //function 5bits, take only 5bits
    opcode &= 0x7f;   // opcode, take only 7bits
//========================================
    //concatenation to full word instruction to feed to mem
    inst = (imm_7 << 25) + (RSS << 20) + (RSB << 15) + (funct_3 << 12) + (imm_5 << 7) + opcode;
    return inst;
}
Word Reg_imm(int imm_12, int RSC,int funct_3, int RSD,int opcode) 
{
    Word inst; 
//========================================
    imm_12 &= 0xfff;
    RSC &= 0x1f;
    funct_3 &= 0x7;
    RSD &= 0x1f;
    opcode &= 0x7f;
//========================================
    inst = (imm_12 << 20) + (RSC << 15) + (funct_3 << 12) + (RSD << 7) + opcode;
    return inst; 
}
Word Shift_imm(int imm_7, int shamt, int RSC, int funct_3, int RSD, int opcode)
{
    Word inst;
    //========================================
    imm_7 &= 0x7f;
    RSC &= 0x1f;
    funct_3 &= 0x7;
    RSD &= 0x1f;
    opcode &= 0x7f;
    //========================================
    inst = (imm_7 << 25) + (shamt << 20) + (RSC << 15) + (funct_3 << 12) + (RSD << 7) + opcode;
    return inst;
}
Word Reg_reg(int imm_7, int RSC_2, int RSC_1, int funct_3, int RSD, int opcode)
{
    Word inst;
    //========================================
    imm_7 &= 0x7f;
    RSC_2 &= 0x1f;
    RSC_1 &= 0x1f;
    funct_3 &= 0x7;
    RSD &= 0x1f;
    opcode &= 0x7f;
    //========================================
    inst = (imm_7 << 25) + (RSC_2 << 20) + (RSC_1 << 15) + (funct_3 << 12) + (RSD << 7) + opcode;
    return inst;
}
Word Branch(int imm12,int imm_6, int RSC_2, int RSC_1,int funct_3,int imm_4,int imm11,int opcode) 
{
    Word inst;
    //========================================
    imm12 &= 0x1;
    imm_6 &= 0x3f;
    RSC_2 &= 0x1f;
    RSC_1 &= 0x1f;
    funct_3 &= 0x7;
    imm_4 &= 0xf;
    imm11 &= 0x1;
    opcode &= 0x7f;
    //========================================
    inst = (imm12 << 31) + (imm_6 << 25) + (RSC_2 << 20) + (RSC_1 << 15) + (funct_3 << 12) + (imm_4 << 8) + (imm11 << 7) + opcode;
    return inst;
}
Word LUI(int imm_20, int RSD, int opcode)
{
    Word inst;
    //========================================
    imm_20 &= 0xfffff;
    RSD &= 0x1f;
    opcode &= 0x7f;
    //========================================
    inst = (imm_20 << 12) + (RSD << 7) + opcode;
    return inst;
}
Word AUIPC(int imm_20, int RSD, int opcode) 
{
    Word inst;
    //========================================
    imm_20 &= 0xfffff;
    RSD &= 0x1f;
    opcode &= 0x7f;
    //========================================
    inst = (imm_20 << 12) + (RSD << 7) + opcode;
    return inst;
}
Word Branches(int imm12, int imm_6,int RSC_2,int RSC_1,int funct_3,int imm_4,int imm11,int opcode) 
{
    Word inst;
    //========================================
    imm12 &= (imm12 == 1)?0x1:0x0;
    imm_6 = 0x3f;
    RSC_2 &= 0x1f;
    RSC_1 &= 0x1f;
    funct_3 &= 0x7;
    imm_4 &= 0xf;
    imm11 &= (imm11 == 1) ? 0x1 : 0x0 ;
    opcode &= 0x7f;
    //========================================
    inst = (imm12 << 31) + (imm_6 << 25) + (RSC_2 << 20) + (RSC_1 << 15) + (funct_3 << 12) + (imm_4 << 8) + (imm11 << 7) + opcode;
    return inst;
}
//0x1,0x3fe,0x1, 0xff,x_2,INST_unconditional_jump_opcode
Word JAL(int imm20, int imm_10,int imm11,int imm_8,int RSD,int opcode) 
{
    Word inst;
    //========================================
    imm20 &= (imm20 == 1) ? 0x1 : 0x0;
    imm_10 &= 0x3ff;
    imm11 &= (imm11 == 1) ? 0x1 : 0x0;
    imm_8 &= 0xff;
    RSD &= 0x1f;
    opcode &= 0x7f;
    //========================================
    inst = (imm20 << 31) + (imm_10 << 21) + (imm11 << 20) + (imm_8 << 12) + (RSD << 7) + opcode;
    return inst;
}
Word JALR(int imm_12,int RSB,int funct_3,int RSD,int opcode) 
{
    Word inst;
    //========================================
    imm_12 &= 0xfff;
    RSB &= 0x1f;
    funct_3 &= 0x7;
    RSD &= 0x1f;
    opcode &= 0x7f;
    //========================================
    inst = (imm_12 << 20) + (RSB << 15) + (funct_3 << 12) + (RSD << 7) + opcode;
    return inst;
}

