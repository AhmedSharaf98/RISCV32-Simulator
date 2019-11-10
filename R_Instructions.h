#include "Instruction.h"
#pragma once
#ifndef R_Ins
#define R_Ins
struct R_Instruction :  Instruction {
	R_Instruction() { type = "R"; }
	void executeType() {
		rs1 = Helper::ExtractBits(word, 15, 19);
		rs2 = Helper::ExtractBits(word, 20, 24);
		rd = Helper::ExtractBits(word, 7, 11);
		perform();
	}
	void virtual perform() = 0;
	void virtual print() {
		cout << operation << " x" << rd << ", x" << rs1 << ", x" << rs2;
	}
};
#endif // !R_Ins

struct Add : R_Instruction {
	Add() { name = "ADD"; operation = "add"; }
	void perform() {
		registers[rd] = registers[rs1] + registers[rs2];
	}
};
struct Sub : R_Instruction {
	Sub() { name = "SUB"; operation = "sub"; }
	void perform() {
		registers[rd] = registers[rs1] - registers[rs2];
	}
};

struct And : R_Instruction {
	And() { name = "AND"; operation = "and"; }
	void perform() {
		registers[rd] = registers[rs1] & registers[rs2];
	}
};

struct Or : R_Instruction {
	Or() { name = "OR"; operation = "or"; }
	void perform() {
		registers[rd] = registers[rs1] | registers[rs2];
	}
};

struct Xor : R_Instruction {
	Xor() { name = "XOR"; operation = "xor"; }
	void perform() {
		registers[rd] = registers[rs1] ^ registers[rs2];
	}
};

struct Slt : R_Instruction {
	Slt() { name = "SLT"; operation = "slt"; }
	void perform() {
		registers[rd] = registers[rs1] < registers[rs2];
	}
};

struct Sltu : R_Instruction {
	Sltu() { name = "SLTU"; operation = "sltu"; }
	void perform() {
		registers[rd] = abs(registers[rs1]) < abs(registers[rs2]);
	}
};

struct Sra : R_Instruction {
	Sra() { name = "SRA"; operation = "sra"; }
	void perform() {
		registers[rd] = registers[rs1] >> (registers[rs2]& 31);
	}
};

struct Srl : R_Instruction {
	Srl() { name = "SRL"; operation = "srl"; }
	void perform() {
		registers[rd] = registers[rs1] >> (registers[rs2] & 31);
	}
};

struct Sll : R_Instruction {
	Sll() { name = "SLL"; operation = "sll"; }
	void perform() {
		registers[rd] = registers[rs1] << (registers[rs2]&31);
	}
};
