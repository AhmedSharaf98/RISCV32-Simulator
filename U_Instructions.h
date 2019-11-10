#include "Instruction.h"
#pragma once
#ifndef U_Ins
#define U_Ins
struct U_Instruction : Instruction {
	U_Instruction() { type = "U"; }
	void executeType() {	
		rd = Helper::ExtractBits(word, 7, 11);
		imm = Helper::ExtractBits(word, 12, 31);
		perform();
	}
	void virtual perform() = 0;
	void virtual print() {
		cout << operation << " x" << rd << ", 0x" << hex << imm;
	}
};
#endif // !S_Ins

struct Lui : U_Instruction {
	Lui() { name = "LOAD UPPER Immediate"; operation = "lui"; }
	void perform() {
		registers[rd] = imm;
		registers[rd] = registers[rd] << 12;
	}
};

struct Auipc : U_Instruction {
	Auipc() { name = "ADD UPPER IMMEDIATE TO PC"; operation = "auipc"; }
	void perform() {
		registers[rd] = imm;
		registers[rd] = registers[rd] << 12;
		registers[rd] = registers[rd] + pc;
	}
};

struct Jal : U_Instruction {
	Jal() { name = "Jump and Link"; operation = "jal"; }
	void perform() {
		imm = Helper::shiftAndAdd(Helper::ExtractBits(word, 31, 31), 8, Helper::ExtractBits(word, 12, 19));
		imm = Helper::shiftAndAdd(imm, 1, Helper::ExtractBits(word, 20, 20));
		imm = Helper::shiftAndAdd(imm, 10, Helper::ExtractBits(word, 21, 30));
		imm = Helper::shiftAndAdd(imm, 1, 0);
		registers[rd] = nxt_inst_addr;
		nxt_inst_addr = pc + (imm);
	}
};