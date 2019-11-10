#include "Instruction.h"
#pragma once
#ifndef S_Ins
#define S_Ins
struct S_Instruction : Instruction {
	S_Instruction() { type = "S"; }

	int Bimmediate;
	void executeType() {
		rs1 = Helper::ExtractBits(word, 15, 19);
		rs2 = Helper::ExtractBits(word, 20, 24);
		imm = Helper::shiftAndAdd(Helper::ExtractBits(word, 25, 31), 5, Helper::ExtractBits(word, 7, 11));
		Bimmediate = Helper::shiftAndAdd(Helper::ExtractBits(word, 31, 31), 1, Helper::ExtractBits(word, 7, 7));
		Bimmediate = Helper::shiftAndAdd(Bimmediate, 6, Helper::ExtractBits(word, 25, 30));
		Bimmediate = Helper::shiftAndAdd(Bimmediate, 4, Helper::ExtractBits(word, 8, 11));
		perform();
	}
	void virtual perform() = 0;
	void virtual print() {
		cout << operation << " x" << rs2 << ", " << imm << "(x" << rs1 << ")";
	}
};
#endif // !S_Ins

struct Sb : S_Instruction {
	Sb() { name = "Store Byte"; operation = "sb"; }
	void perform() {
		memory[registers[rs1] + imm] = Helper::ExtractBits(registers[rs2], 0 , 7);
	}
};

struct Sh : S_Instruction {
	Sh() { name = "Store Half Word"; operation = "sh"; }
	void perform() {
		memory[registers[rs1] + imm] = Helper::ExtractBits(registers[rs2], 0, 7);
		memory[registers[rs1] + imm + 1] = Helper::ExtractBits(registers[rs2], 8, 15);
	}
};

struct Sw : S_Instruction {
	Sw() { name = "Store Word"; operation = "sw"; }
	void perform() {
		memory[registers[rs1] + imm] = Helper::ExtractBits(registers[rs2], 0, 7);
		memory[registers[rs1] + imm + 1] = Helper::ExtractBits(registers[rs2], 8, 15);
		memory[registers[rs1] + imm + 2] = Helper::ExtractBits(registers[rs2], 16, 23);
		memory[registers[rs1] + imm + 3] = Helper::ExtractBits(registers[rs2], 24, 31);
	}
};

struct Beq : S_Instruction {
	Beq() { name = "Branch on Equal"; operation = "beq"; }
	void perform() {
		if (registers[rs1] == registers[rs2])
			nxt_inst_addr = pc + (Bimmediate << 1);
	}
	void print() {
		cout << operation << " x" << rs1 << ", x" << rs2 << ", " << Bimmediate;
	}
};

struct Bne : S_Instruction {
	Bne() { name = "Branch on not Equal"; operation = "bne"; }
	void perform() {
		if (registers[rs1] != registers[rs2])
			nxt_inst_addr = pc + (Bimmediate << 1);
	}
	void print() {
		cout << operation << " x" << rs1 << ", x" << rs2 << ", " << Bimmediate;
	}
};

struct Blt : S_Instruction {
	Blt() { name = "Branch on Less than"; operation = "blt"; }
	void perform() {
		if (registers[rs1] < registers[rs2])
			nxt_inst_addr = pc + (Bimmediate << 1);
	}
	void print() {
		cout << operation << " x" << rs1 << ", x" << rs2 << ", " << Bimmediate;
	}
};

struct Bltu : S_Instruction {
	Bltu() { name = "Branch on Less than unsigned"; operation = "blt"; }
	void perform() {
		if (abs(registers[rs1]) < abs(registers[rs2]))
			nxt_inst_addr = pc + (Bimmediate << 1);
	}
	void print() {
		cout << operation << " x" << rs1 << ", x" << rs2 << ", " << Bimmediate;
	}
};

struct Bgeu : S_Instruction {
	Bgeu() { name = "Branch on Greater than or equal Unsigned"; operation = "bgeu"; }
	void perform() {
		if (abs(registers[rs1]) >= abs(registers[rs2]))
			nxt_inst_addr = pc + (Bimmediate << 1);
	}
	void print() {
		cout << operation << " x" << rs1 << ", x" << rs2 << ", " << Bimmediate;
	}
};

struct Bge : S_Instruction {
	Bge() { name = "Branch on Greater than or equal "; operation = "bge"; }
	void perform() {
		if (registers[rs1] >= registers[rs2])
			nxt_inst_addr = pc + (Bimmediate << 1);
	}
	void print() {
		cout << operation << " x" << rs1 << ", x" << rs2 << ", " << Bimmediate;
	}
};