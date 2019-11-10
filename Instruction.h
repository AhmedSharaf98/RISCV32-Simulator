#include <string>
#pragma once
#ifndef _Instruction
#define _Instruction

struct Instruction {
	std::string type;
	std::string name;
	std::string operation;

	unsigned int rs1;
	unsigned int rs2;
	unsigned int rd;
	int imm;
	int shift;
	unsigned int first7;
	int nxt_inst_addr;
	unsigned int word;

	int execute(int _word, int pc, bool compressed){
		word = _word;
		nxt_inst_addr = pc + ((compressed) ? 2 : 4);
		executeType();
		registers[0] = 0;
		return nxt_inst_addr;
	}
	void virtual executeType() = 0;
	void virtual print() = 0;
};
#endif // !_Instruction