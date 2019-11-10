#include <string>
#include <vector>
#pragma once
#ifndef HelperClass
#define HelperClass
struct Helper {
	static int Decimal(std::string sub) {
		int decimalNumber = 0;
		int remainder;
		int j = 0;
		for (int i = sub.size() - 1; i >= 0; i--) {
			remainder = sub[i];
			remainder = remainder - 48;
			decimalNumber += (remainder*pow(2, j));
			j++;
		}
		return decimalNumber;
	}
	
	static int ExtractBits(int src, int from, int to) {
		int result;
		result = (1 << (to - from + 1)) - 1; //N Ones at the begginning
		result = result << from; //Shiffting the ones to desired location to extract
		result = result & src; //Extracting
		return (result >> from); //Moving the extracted data to the begginning
	}

	static int shiftAndAdd(int src, int shift, int toadd) {
		int result = (src << shift);
		return (result |= toadd);
	}

	static int Key(int word) {
		unsigned int op = ExtractBits(word, 0, 6);
		if (op == 51) //0110011
			return (unsigned)shiftAndAdd(shiftAndAdd(ExtractBits(word, 25, 31), 3, ExtractBits(word, 12, 14)), 7, op); //op+fun3+fun7
		else if (op == 55 || op == 111 || op == 23) //0110111 || 1101111
			return op;
		return (shiftAndAdd(ExtractBits(word, 12, 14), 7, op)); //func3 + op
	}

	static int InvertBits(int src, int from, int to) {
		int msk = (src >> from);
		int result = 0;
		for (int i = 0; i <= to - from; i++) {
			result = result << 1;
			result += (msk & 1);
			msk = msk >> 1;
		}
		return result;
	}

	static bool isNegative(int word, int size) {
		return (((long long)((1 << (size - 1)))) & word);
	}

	static int ExtendNegativeSign(int word, int size) {
		int result = (((long long)((1 << size) - 1)) & (word * -1));
		return  result;
	}
	static void printRegisters() {
		cout << setfill(' ') << left << dec;
		for (int i = 0; i < 32; i++) {
			cout << "x" << setw(7) << i << setw(7) << registers[i] << endl;
		}
		cout << "------------------------------------------" << endl;
	}
	static void emitError(string error) {
		cout << error << endl;
		system("pause");
		exit(-1);
	}
};
#endif