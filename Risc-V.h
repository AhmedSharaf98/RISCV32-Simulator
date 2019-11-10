#pragma once
const int MEMORY_SIZE = 64 * 1024; //64 KB
int registers[32] = { 0 };
char memory[MEMORY_SIZE];
unsigned int pc = 0x0;
int cinVariable;
#include "Helper.h"
#include "R_Instructions.h"
#include "I_Instructions.h"
#include "S_Instructions.h"
#include "U_Instructions.h"
#include <map>
#include <fstream>
#ifndef _RiscV
#define _RiscV
class RiscV {
private:
	map<int, Instruction*> instructions;
	void initMap(); //Definition Below
	bool isCompressed(int word) {
		return ((word & 3) != 3);
	}
	int Expand(int word) {
		unsigned long long result = 0;
		int op = Helper::ExtractBits(word, 0, 1);
		switch (op) {
		case 0:
			if (Helper::ExtractBits(word, 13, 15) == 2) { //lw
				op = 0x03; //Opcode of Original Instruction
				result = Helper::shiftAndAdd(Helper::ExtractBits(word, 5, 5), 3, Helper::ExtractBits(word, 10, 12)); // First 4 bits in immediate
				result = Helper::shiftAndAdd(result, 1, Helper::ExtractBits(word, 6, 6));
				result = Helper::shiftAndAdd(result, 7, (8 + Helper::ExtractBits(word, 7, 9)));//Adding rs1 after expanding it
				result = Helper::shiftAndAdd(result, 3, 2);//Adding Funct 3
				result = Helper::shiftAndAdd(result, 5, (8 + Helper::ExtractBits(word, 2, 4)));//Adding rd after expanding it
				result = Helper::shiftAndAdd(result, 7, 3);//Adding opcode
			}
			else if (Helper::ExtractBits(word, 13, 15) == 6) //SW
			{
				op = 0x2; //Opcode of Original Instruction
				result = Helper::shiftAndAdd(Helper::ExtractBits(word, 5, 5), 1, Helper::ExtractBits(word, 12, 12)); // Last 2 bits in immediate
				result = Helper::shiftAndAdd(result, 5, (8 + Helper::ExtractBits(word, 2, 4))); //2 bits in immediate + rs2
				result = Helper::shiftAndAdd(result, 5, (8 + Helper::ExtractBits(word, 7, 9)));//Adding rs1 after expanding it
				result = Helper::shiftAndAdd(result, 3, 2);//Adding Funct 3
				result = Helper::shiftAndAdd(result, 3, Helper::shiftAndAdd(Helper::ExtractBits(word, 10, 11), 1, Helper::ExtractBits(word, 6, 6)));//Adding Rest of immediate
				result = Helper::shiftAndAdd(result, 9, 35);//Adding opcode + shifting the immediate 2 place to left
			}
			else if (Helper::ExtractBits(word, 13, 15) == 0) //ADDI4SPN
			{
				result = Helper::shiftAndAdd(Helper::ExtractBits(word, 7, 10), 2, Helper::ExtractBits(word, 11, 12));
				result = Helper::shiftAndAdd(result, 1, Helper::ExtractBits(word, 5, 5));
				result = Helper::shiftAndAdd(result, 1, Helper::ExtractBits(word, 6, 6));
				result = Helper::shiftAndAdd(result, 7, 2);
				result = Helper::shiftAndAdd(result, 8, 8 + Helper::ExtractBits(word, 2, 4));
				result = Helper::shiftAndAdd(result, 7, 19);
			}
			break;
		case 1:
			if (Helper::ExtractBits(word, 13, 15) == 0) { //addi
				if (Helper::ExtractBits(word, 12, 12) == 1)
					result = Helper::ExtendNegativeSign((Helper::ExtractBits(word, 12, 12)), 7);
				result = Helper::shiftAndAdd(result, 5, Helper::ExtractBits(word, 2, 6));
				int _reg = Helper::ExtractBits(word, 7, 11);
				result = Helper::shiftAndAdd(result, 5, _reg);
				result = Helper::shiftAndAdd(result, 3, 0);
				result = Helper::shiftAndAdd(result, 5, _reg);
				result = Helper::shiftAndAdd(result, 2, 0);
				result = Helper::shiftAndAdd(result, 5, 19);
			}

			else if (Helper::ExtractBits(word, 13, 15) == 1 || Helper::ExtractBits(word, 13, 15) == 5) {  //jal / J
				if (Helper::ExtractBits(word, 8, 8) == 1)
					result = Helper::ExtendNegativeSign(Helper::ExtractBits(word, 8, 8), 2);
				result = Helper::shiftAndAdd(result, 2, Helper::ExtractBits(word, 9, 10));
				result = Helper::shiftAndAdd(result, 1, Helper::ExtractBits(word, 6, 6));
				result = Helper::shiftAndAdd(result, 1, Helper::ExtractBits(word, 7, 7));
				result = Helper::shiftAndAdd(result, 1, Helper::ExtractBits(word, 2, 2));
				result = Helper::shiftAndAdd(result, 1, Helper::ExtractBits(word, 11, 11));
				result = Helper::shiftAndAdd(result, 3, Helper::ExtractBits(word, 3, 5));
				result = Helper::shiftAndAdd(result, 1, Helper::ExtractBits(word, 12, 12));
				if (Helper::ExtractBits(word, 8, 8) == 1)
					result = Helper::shiftAndAdd(result, 8, 255);
				else
					result = Helper::shiftAndAdd(result, 8, 0);
				result = Helper::shiftAndAdd(result, 5, ((Helper::ExtractBits(word, 13, 15) == 1) ? 1 : 0));
				result = Helper::shiftAndAdd(result, 7, 111);
			}
			else if (Helper::ExtractBits(word, 13, 15) == 2) {   //li
				if (Helper::ExtractBits(word, 12, 12) == 1)
					result = Helper::ExtendNegativeSign(Helper::ExtractBits(word, 12, 12), 7);
				result = Helper::shiftAndAdd(result, 5, Helper::ExtractBits(word, 2, 6));
				result = Helper::shiftAndAdd(result, 13, Helper::ExtractBits(word, 7, 11));
				result = Helper::shiftAndAdd(result, 7, 19);
			}

			else if ((Helper::ExtractBits(word, 13, 15) == 3) && (Helper::ExtractBits(word, 7, 11) == 2)) { //addi16sp
				if (Helper::ExtractBits(word, 12, 12) == 1)
					result = Helper::ExtendNegativeSign(Helper::ExtractBits(word, 12, 12), 3);
				result = Helper::shiftAndAdd(result, 2, Helper::ExtractBits(word, 3, 4));
				result = Helper::shiftAndAdd(result, 1, Helper::ExtractBits(word, 5, 5));
				result = Helper::shiftAndAdd(result, 1, Helper::ExtractBits(word, 2, 2));
				result = Helper::shiftAndAdd(result, 1, Helper::ExtractBits(word, 6, 6));
				result = Helper::shiftAndAdd(result, 4, 0);
				result = Helper::shiftAndAdd(result, 5, Helper::ExtractBits(word, 7, 11));
				result = Helper::shiftAndAdd(result, 3, 0);
				result = Helper::shiftAndAdd(result, 5, Helper::ExtractBits(word, 7, 11));
				result = Helper::shiftAndAdd(result, 2, 0);
				result = Helper::shiftAndAdd(result, 5, 19);
			}

			else if (Helper::ExtractBits(word, 13, 15) == 3) { //Lui
				if (Helper::ExtractBits(word, 12, 12) == 1)
					result = Helper::ExtendNegativeSign(Helper::ExtractBits(word, 12, 12), 4);
				result = Helper::shiftAndAdd(result, 5, Helper::ExtractBits(word, 2, 6));
				result = Helper::shiftAndAdd(result, 12, Helper::ExtractBits(word, 7, 11));
				result = Helper::shiftAndAdd(result, 7, 55);
			}
			
			else if (Helper::ExtractBits(word, 13, 15) == 4) {
				if (Helper::ExtractBits(word, 10, 11) == 3) {
					result = Helper::shiftAndAdd(result, 5, 8 + Helper::ExtractBits(word, 2, 4));
					result = Helper::shiftAndAdd(result, 5, 8 + Helper::ExtractBits(word, 7, 9));
					if (Helper::ExtractBits(word, 5, 6) == 0) { //sub
						result = Helper::shiftAndAdd(32, 5, result);
						result = Helper::shiftAndAdd(result, 3, 0);
					}
					else if (Helper::ExtractBits(word, 5, 6) == 1) { //xor
						result = Helper::shiftAndAdd(result, 3, 4);
					}
					else if (Helper::ExtractBits(word, 5, 6) == 2) { //or
						result = Helper::shiftAndAdd(result, 3, 6);
					}
					else if (Helper::ExtractBits(word, 5, 6) == 3) { //and
						result = Helper::shiftAndAdd(result, 3, 7);
					}

					result = Helper::shiftAndAdd(result, 5, 8 + Helper::ExtractBits(word, 7, 9));
					result = Helper::shiftAndAdd(result, 7, 51);
				}

				else if (Helper::ExtractBits(word, 10, 11) == 2) { //andi
					if (Helper::ExtractBits(word, 12, 12) == 1)
						result = Helper::ExtendNegativeSign(Helper::ExtractBits(word, 12, 12), 7);
					result = Helper::shiftAndAdd(result, 5, Helper::ExtractBits(word, 2, 6));
					result = Helper::shiftAndAdd(result, 5, 8 + Helper::ExtractBits(word, 7, 9));
					result = Helper::shiftAndAdd(result, 3, 7);
					result = Helper::shiftAndAdd(result, 5, 8 + Helper::ExtractBits(word, 7, 9));
					result = Helper::shiftAndAdd(result, 7, 19);
				}
				else if (Helper::ExtractBits(word, 10, 11) == 0) { //srli
					result = Helper::ExtractBits(word, 12, 12);
					result = Helper::shiftAndAdd(result, 5, Helper::ExtractBits(word, 2, 6));
					result = Helper::shiftAndAdd(result, 5, 8 + Helper::ExtractBits(word, 7, 9));
					result = Helper::shiftAndAdd(result, 3, 5);
					result = Helper::shiftAndAdd(result, 5, 8 + Helper::ExtractBits(word, 7, 9));
					result = Helper::shiftAndAdd(result, 7, 19);
				}
				else if (Helper::ExtractBits(word, 10, 11) == 1) { //srai
					result = Helper::shiftAndAdd(16, 1, Helper::ExtractBits(word, 12, 12));
					result = Helper::shiftAndAdd(result, 5, Helper::ExtractBits(word, 2, 6));
					result = Helper::shiftAndAdd(result, 5, 8 + Helper::ExtractBits(word, 7, 9));
					result = Helper::shiftAndAdd(result, 3, 5);
					result = Helper::shiftAndAdd(result, 5, 8 + Helper::ExtractBits(word, 7, 9));
					result = Helper::shiftAndAdd(result, 7, 19);
				}
				else if (Helper::ExtractBits(word, 10, 11) == 6) { //BEQZ
					if (Helper::ExtractBits(word, 12, 12) == 1)
						result = Helper::ExtendNegativeSign(Helper::ExtractBits(word, 12, 12), 5);
					result = Helper::shiftAndAdd(result, 2, Helper::ExtractBits(word, 5, 6));
					result = Helper::shiftAndAdd(result, 1, Helper::ExtractBits(word, 2, 2));
					result = Helper::shiftAndAdd(result, 10, 8 + Helper::ExtractBits(word, 7, 9));
					result = Helper::shiftAndAdd(result, 3, 0);
					result = Helper::shiftAndAdd(result, 2, Helper::ExtractBits(word, 10, 11));
					result = Helper::shiftAndAdd(result, 2, Helper::ExtractBits(word, 3, 4));
					result = Helper::shiftAndAdd(result, 8, 99);
				}
				else if (Helper::ExtractBits(word, 10, 11) == 7) { //BNEZ
					if (Helper::ExtractBits(word, 12, 12) == 1)
						result = Helper::ExtendNegativeSign(Helper::ExtractBits(word, 12, 12), 5);
					result = Helper::shiftAndAdd(result, 2, Helper::ExtractBits(word, 5, 6));
					result = Helper::shiftAndAdd(result, 1, Helper::ExtractBits(word, 2, 2));
					result = Helper::shiftAndAdd(result, 10, 8 + Helper::ExtractBits(word, 7, 9));
					result = Helper::shiftAndAdd(result, 3, 1);
					result = Helper::shiftAndAdd(result, 2, Helper::ExtractBits(word, 10, 11));
					result = Helper::shiftAndAdd(result, 2, Helper::ExtractBits(word, 3, 4));
					result = Helper::shiftAndAdd(result, 8, 99);
				}
			}
			break;
		case 2:
			if (Helper::ExtractBits(word, 13, 15) == 0)//slli
			{
				result = Helper::shiftAndAdd(Helper::ExtractBits(word, 2, 6), 5, Helper::ExtractBits(word, 7, 11));
				result = Helper::shiftAndAdd(result, 3, 1);
				result = Helper::shiftAndAdd(result, 5, Helper::ExtractBits(word, 7, 11));
				result = Helper::shiftAndAdd(result, 7, 19);
			}
			else if (Helper::ExtractBits(word, 13, 15) == 2) //lwsp
			{
				result = Helper::shiftAndAdd(Helper::ExtractBits(word, 2, 3), 1, Helper::ExtractBits(word, 12, 12));
				result = Helper::shiftAndAdd(result, 3, Helper::ExtractBits(word, 4, 6));
				result = Helper::shiftAndAdd(result, 2, 0);
				result = Helper::shiftAndAdd(result, 5, 2);
				result = Helper::shiftAndAdd(result, 3, 2);
				result = Helper::shiftAndAdd(result, 5, Helper::ExtractBits(word, 7, 11));
				result = Helper::shiftAndAdd(result, 5, 0);
				result = Helper::shiftAndAdd(result, 2, 3);

			}
			else if ((Helper::ExtractBits(word, 13, 15) == 4) && (Helper::ExtractBits(word, 12, 12) == 0) && (Helper::ExtractBits(word, 2, 6) == 0)) //jr
			{
				result = Helper::shiftAndAdd(Helper::ExtractBits(word, 7, 11), 3, Helper::ExtractBits(word, 12, 14));
				result = Helper::shiftAndAdd(result, 5, 0);
				result = Helper::shiftAndAdd(result, 7, 103);
			}
			else if ((Helper::ExtractBits(word, 13, 15) == 4) && (Helper::ExtractBits(word, 12, 12) == 0)) //move
			{
				result = Helper::shiftAndAdd(Helper::ExtractBits(word, 2, 6), 5, 0);
				result = Helper::shiftAndAdd(result, 3, Helper::ExtractBits(word, 12, 14));
				result = Helper::shiftAndAdd(result, 5, Helper::ExtractBits(word, 7, 11));
				result = Helper::shiftAndAdd(result, 1, 0);
				result = Helper::shiftAndAdd(result, 6, 51);
			}
			else if ((Helper::ExtractBits(word, 13, 15) == 4) && (Helper::ExtractBits(word, 12, 12) == 1) && (Helper::ExtractBits(word, 2, 6) == 0)) //jalr
			{
				result = Helper::shiftAndAdd(Helper::ExtractBits(word, 7, 11), 3, 0);
				result = Helper::shiftAndAdd(result, 5, 1);
				result = Helper::shiftAndAdd(result, 7, 103);
			}
			else if ((Helper::ExtractBits(word, 13, 15) == 4) && (Helper::ExtractBits(word, 12, 12) == 1)) //add
			{
				result = Helper::shiftAndAdd(Helper::ExtractBits(word, 2, 6), 5, Helper::ExtractBits(word, 7, 11));
				result = Helper::shiftAndAdd(result, 3, 0);
				result = Helper::shiftAndAdd(result, 5, Helper::ExtractBits(word, 7, 11));
				result = Helper::shiftAndAdd(result, 1, 0);
				result = Helper::shiftAndAdd(result, 6, 51);
			}

			else if ((Helper::ExtractBits(word, 13, 15) == 6)) //swsp
			{
				result = Helper::shiftAndAdd(Helper::ExtractBits(word, 7, 8), 1, Helper::ExtractBits(word, 12, 12));
				//result = Helper::shiftAndAdd(result, 1, 0);
				result = Helper::shiftAndAdd(result, 5, Helper::ExtractBits(word, 2, 6));
				result = Helper::shiftAndAdd(result, 5, 2);
				result = Helper::shiftAndAdd(result, 3, 2);

				result = Helper::shiftAndAdd(result, 3, Helper::ExtractBits(word, 9, 11));
				result = Helper::shiftAndAdd(result, 3, 0);
				result = Helper::shiftAndAdd(result, 6, 35);
			}
			break;
		default:
			Helper::emitError("Unknown Compressed Instruction");
		}
		return result;
	}
public:
	RiscV(string filename) {
		ifstream inFile;
		inFile.open(filename, ios::in | ios::binary | ios::ate);
		if (inFile.is_open()) {
			int fsize = inFile.tellg();
			inFile.seekg(0, inFile.beg);
			if (!inFile.read((char *)memory, fsize)) cout << ("Cannot read from input file\n");
			initMap();
		}
	}
	void execute(char* delay = "1") {
		int word;
		int word_original;
		bool compression = false;
		while (true) {
			int word_original = (unsigned char)memory[pc] |
				(((unsigned char)memory[pc + 1]) << 8)  |
				(((unsigned char)memory[pc + 2]) << 16) |
				(((unsigned char)memory[pc + 3]) << 24);


			compression = isCompressed(word_original);
			if (compression) {
				word_original = word_original & ((1 << 16) - 1);
				word = Expand(word_original);
			} 
			else word = word_original;
			auto i = instructions.find(Helper::Key(word));
			if (i == instructions.end())
				Helper::emitError("Instruction Undefined");
			else {
				cout << right << "0x" << std::setfill('0') << std::setw(8) << hex << pc << " || "  << "0x"
					<< std::setw(8) << word_original << "|| 0x" << word << " || "	<<((compression) ? "    " : "") <<" || " << dec;
				pc = i->second->execute(word, pc, compression);
				cout << ((compression) ? 'C' : ' ') << i->second->type << " || " << setw(25) << left << setfill(' ') << i->second->name << "|| ";
				i->second->print();
				cout << dec << endl;
			}
			if (pc == MEMORY_SIZE)
				break;

			std::this_thread::sleep_for(std::chrono::milliseconds(atoi(delay)));
		}
		cout << "\n\n\nRegister:\n";
		Helper::printRegisters();
	}
};
#endif // !_RiscV
void RiscV::initMap(){
	//R-Type
	instructions.insert(make_pair(51, new Add));
	instructions.insert(make_pair(32819, new Sub));
	instructions.insert(make_pair(947, new And));
	instructions.insert(make_pair(819, new Or));
	instructions.insert(make_pair(563, new Xor));
	instructions.insert(make_pair(307, new Slt));
	instructions.insert(make_pair(435, new Sltu));
	instructions.insert(make_pair(33459, new Sra));
	instructions.insert(make_pair(691, new Srl));
	instructions.insert(make_pair(179, new Sll));

	//I,S,B-types
	instructions.insert(make_pair(19, new Addi));
	instructions.insert(make_pair(275, new Slti));
	instructions.insert(make_pair(403, new Sltiu));
	instructions.insert(make_pair(531, new Xori));
	instructions.insert(make_pair(787, new Ori));
	instructions.insert(make_pair(915, new Andi));
	instructions.insert(make_pair(147, new Slli));
	instructions.insert(make_pair(659, new Srli));
	instructions.insert(make_pair(259, new Lw));
	instructions.insert(make_pair(131, new Lh));
	instructions.insert(make_pair(643, new Lhu));
	instructions.insert(make_pair(3, new Lb));
	instructions.insert(make_pair(515, new Lbu));
	instructions.insert(make_pair(115, new Ecall));
	instructions.insert(make_pair(103, new Jalr));

	//S Type
	instructions.insert(make_pair(35, new Sb));
	instructions.insert(make_pair(163, new Sh));
	instructions.insert(make_pair(291, new Sw));
	instructions.insert(make_pair(99, new Beq));
	instructions.insert(make_pair(227, new Bne));
	instructions.insert(make_pair(611, new Blt));
	instructions.insert(make_pair(739, new Bge));
	instructions.insert(make_pair(867, new Bltu));
	instructions.insert(make_pair(995, new Bgeu));

	//U type
	instructions.insert(make_pair(55, new Lui));
	instructions.insert(make_pair(23, new Auipc));
	instructions.insert(make_pair(111, new Jal));
}