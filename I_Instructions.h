#pragma once
#include<iostream>
using namespace std;
#include "Instruction.h"
#pragma once
#ifndef I_Ins
#define I_Ins
struct I_Instruction : Instruction {
	I_Instruction() { type = "I"; }
	void executeType() {
		rs1 = Helper::ExtractBits(word, 15, 19);
		rd = Helper::ExtractBits(word, 7, 11);
		imm = Helper::ExtractBits(word, 20, 31);
		shift = Helper::ExtractBits(word, 20, 24);
		first7 = Helper::ExtractBits(word, 25, 31);
		perform();
	}
	void virtual perform() = 0;
	void virtual print() {
		cout << operation << " x" << rd << ", x" << rs1 << ", 0x" << hex << imm;
	}
};
#endif // !R_Ins

struct Addi : I_Instruction {
	Addi() { name = "ADD Immediate"; operation = "addi"; }
	void perform() {
		registers[rd] = registers[rs1] + imm;
	}
};

struct Slti : I_Instruction {
	Slti() { name = "SLT Immediate"; operation = "slti"; }
	void perform() {
		registers[rd] = registers[rs1] < imm;
	}
};

struct Sltiu : I_Instruction {
	Sltiu() { name = "SLTU Immediate"; operation = "sltiu"; }
	void perform() {
		registers[rd] = abs(registers[rs1]) < abs(imm);
	}
};

struct Xori : I_Instruction {
	Xori() { name = "XOR Immediate"; operation = "xori"; }
	void perform() {
		registers[rd] = registers[rs1] ^ imm;
	}
};

struct Ori : I_Instruction {
	Ori() { name = "OR Immediate"; operation = "ori"; }
	void perform() {
		registers[rd] = registers[rs1] | imm;
	}
};

struct Andi : I_Instruction {
	Andi() { name = "AND Immediate"; operation = "andi"; }
	void perform() {
		registers[rd] = registers[rs1] & imm;
	}
};
struct Slli : I_Instruction {
	Slli() { name = "SLL Immediate"; operation = "slli"; }
	void perform() {
		registers[rd] = registers[rs1] << shift;
	}
};

struct Srli : I_Instruction {
	Srli() { name = "SRL Immediate"; operation = "srli"; }
	void perform() {
		if (first7 == 0)
			registers[rd] = (unsigned)(registers[rs1] >> shift);
		else {
			operation = "srai";
			registers[rd] = registers[rs1] >> shift;
		}
	}
};

struct Srai : I_Instruction {
	Srai() { name = "SRAI Immediate"; operation = "srai"; }
	void perform() {
		registers[rd] = registers[rs1] >> shift;
	}
};

struct Load_Instructions : I_Instruction {
	int address;
	void perform() {
		address = imm + registers[rs1];
		performLoad();
	}
	//Overridden
	void print() {
		cout << operation << " x" << rd << ", " << imm << "(x" << rs1 << ")";
	}
	void virtual performLoad() = 0;
};

struct Lw : Load_Instructions{
	Lw() { name = "LOAD WORD"; operation = "lw"; }
	void performLoad() {
		if (address % 2 != 0) Helper::emitError("Invalid Allignment");
		registers[rd] = memory[address] & 255;
		registers[rd] |= (memory[address + 1] & 255) << 8;
		registers[rd] |= (memory[address + 2] & 255) << 16;
		registers[rd] |= (memory[address + 3] & 255) << 24;
	}
};

struct Lh : Load_Instructions{
	Lh() { name = "LOAD HALFWORD"; operation = "lh"; }
	void performLoad() {
		if (address % 2 != 0) Helper::emitError("Invalid Allignment");
		registers[rd] = memory[address] & 255;
		registers[rd] |= ((memory[address + 1] & 255) << 8);
	}
};


struct Lhu : Load_Instructions{
	Lhu() { name = "LOAD HALFWORD UNSIGNED"; operation = "lhu"; }
	void performLoad() {
		if (address % 2 != 0) Helper::emitError("Invalid Allignment");
		registers[rd] = (unsigned)memory[address];
		registers[rd] = (unsigned)registers[rd] | (unsigned)(memory[address + 1] << 8);
	}
};

struct Lb : Load_Instructions{
	Lb() { name = "LOAD BYTE"; operation = "lb"; }
	void performLoad() {
		registers[rd] = memory[address];
	}
};

struct Lbu : Load_Instructions{
	Lbu() { name = "LOAD BYTE UNSIGNED"; operation = "lbu"; }
	void performLoad() {
		registers[rd] =  (unsigned)memory[address];
	}
};

struct Ecall : I_Instruction {
	string output = "Output: ";
	Ecall() { name = "System Call"; operation = "ECALL"; }
	void perform() {
		int i = 0;
		string str;
		switch (registers[17])
		{
		case 1: output = "Output: " + to_string(registers[10]);
			break;
		case 4:
			while (memory[registers[10] + i++] != 0)
				output += memory[registers[10] + i];
			break;
		case 10:
			cout << "\n\n\n Register:\n";
			Helper::printRegisters();
			system("pause"); exit(-1);
			break;
		}
	}
	void print() {
		cout << operation << " " << output;
	}
};

struct Jalr : I_Instruction {
	Jalr() { name = "Jump and Link Register"; operation = "jalr"; }
	void perform() {
		registers[rd] = nxt_inst_addr;
		if ((imm & 1) == 1)
			nxt_inst_addr = (registers[rs1] + imm) - 1;
		else
			nxt_inst_addr = (registers[rs1] + imm);
	}
};