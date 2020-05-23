#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

struct Opcode
{
	string type;
	string name;
	string opcode;
	string funct3;
	string funct7;
};

struct labelPos
{
	string name;
	string pos;
};

//global variable
vector<labelPos> label;				//如果指令有label，存行數

string decimal_to_binary(string decimal, int zeroize = 5)
{
	//delete 'x' or other word which is not number
	while (true)
	{
		if (decimal[0] < '0' || decimal[0]>'9')
		{
			for (int i = 0; i < decimal.size() - 1; ++i)
			{
				decimal[i] = decimal[i + 1];
			}

			decimal[decimal.size() - 1] = '\0';
		}
		else
		{
			break;
		}
	}

	int dec = stoi(decimal);
	string returnn;

	while (dec != 0)
	{
		returnn += (dec % 2) + '0';
		dec /= 2;
	}

	//補0
	while (returnn.size() != zeroize)
	{
		returnn += '0';
	}

	//reverse binary
	int i = 0;

	while (i != (returnn.size() / 2))
	{
		swap(returnn[returnn.size() - 1 - i], returnn[0 + i]);

		++i;
	}
	/*
	cout << returnn << endl;
	system("pause");*/

	return returnn;
}

void label_detected(vector<string>& input)
{
	string tmp;
	labelPos can_find;

	for (int i = 0; i < input.size(); ++i)
	{
		if (input[i].find(':') != string::npos)
		{
			tmp.clear();

			//將找到label 的行數儲存
			for (int j = 0; input[i][j] != ':'; ++j)
			{
				tmp += input[i][j];
			}

			can_find.name = tmp;
			can_find.pos = decimal_to_binary(to_string(i + 1), 12);

			label.push_back(can_find);
		}
	}
}

void R_type(Opcode& find, string& rd, string& rs1, string& rs2)
{
	//印出result
	cout << find.funct7 << " " << decimal_to_binary(rs2) << " " << decimal_to_binary(rs1) << " "
		<< find.funct3 << " " << decimal_to_binary(rd) << " " << find.opcode << endl;
}

void I_type(Opcode& find, string& rd, string& rs1, string& offset)
{
	//印出result
	cout << decimal_to_binary(offset, 12) << " " << decimal_to_binary(rs1) << " "
		<< find.funct3 << " " << decimal_to_binary(rd) << " " << find.opcode << endl;
}

void S_type(Opcode& find, string& rs2, string& offset, string& rs1)
{
	//印出result
	string Roffset = decimal_to_binary(offset, 12);

	for (int i = 5; i < 12; ++i)
	{
		cout << Roffset[i];
	}

	cout << " " << decimal_to_binary(rs2) << " "
		<< decimal_to_binary(rs1) << " " << find.funct3 << " ";

	for (int i = 0; i <= 4; ++i)
	{
		cout << Roffset[i];
	}

	cout << " " << find.opcode << endl;
}

void I_type_num(Opcode& find, string& rd, string& rs1, string& imm)
{
	int num = stoi(imm);

	// Determine whether negative or not
	cout << (num < 0) ? 1 : 0;

	/*if (num < 0)
	{
		cout << "1";
	}
	else
	{
		cout << "0";
	}*/

	//印出result
	cout << decimal_to_binary(imm, 12) << " " << decimal_to_binary(rs1) << " "
		<< find.funct3 << " " << decimal_to_binary(rd) << " " << find.opcode << endl;
}

void I_type_M(Opcode& find, string& rd, string& rs1, string& imm)
{
	//印出result
	cout << find.funct7 << " " << decimal_to_binary(imm) << " " << decimal_to_binary(rs1) << " "
		<< find.funct3 << " " << decimal_to_binary(rd) << " " << find.opcode << endl;
}

void B_type(Opcode& findOp, string& rs1, string& rs2, string& offset)
{
	//label 處理
	string simm;

	//避免找不到label，整個程式crash
	bool exist = false;

	for (int i = 0; i < label.size(); ++i)
	{
		if (offset == label[i].name)
		{
			simm = label[i].pos;
			exist = true;
			break;
		}
	}

	if (exist)
	{
		for (int i = 5; i < 10; ++i)
		{
			cout << simm[i];
		}

		cout << simm[11] << " ";
	}
	else
	{
		cout << "==============Error================\n Can't Find Label" << endl;
	}

	cout << decimal_to_binary(rs2) << " " << decimal_to_binary(rs1) << " " << findOp.funct3 << " ";

	if (exist)
	{
		for (int i = 0; i < 5; ++i)
		{
			cout << simm[i];
		}

		cout << simm[10] << " ";
	}

	cout << findOp.opcode << endl;
}

void UJ_type(Opcode& findOp, string& rd, string& offset)
{
	//label 處理
	string simm;

	//避免找不到label，整個程式crash
	bool exist = false;

	for (int i = 0; i < label.size(); ++i)
	{
		if (offset == label[i].name)
		{
			//重新獲取label，因為預設b type label 只需12位
			simm = decimal_to_binary(label[i].pos, 20);
			exist = true;
			break;
		}
	}

	if (exist)
	{


		cout << simm[19] << " ";

		for (int i = 0; i < 10; ++i)
		{
			cout << simm[i] << " ";
		}

		cout << simm[10] << " ";

		for (int i = 11; i < 19; ++i)
		{
			cout << simm[i] << " ";
		}
	}
	else
	{
		cout << "==============Error================\n Can't Find Label" << endl;
	}

	cout << decimal_to_binary(rd);
	cout << findOp.opcode << endl;
}

void toMachineCode(string input, vector<Opcode>& instruction)
{
	//分割input
	string nm, rd, rs1, rs2, imm, offset;

	stringstream ss(input);

	//取得name要判斷type
	getline(ss, nm, ' ');

	//從vector找對應的instruction
	Opcode findOp;

	for (int i = 0; i < instruction.size(); ++i)
	{
		if (nm == instruction[i].name)
		{
			findOp = instruction[i];
			break;
		}
	}

	//判斷型態對應
	if (findOp.type == "R-type")
	{
		//add rd,rs1,rs2
		getline(ss, rd, ',');
		getline(ss, rs1, ',');
		getline(ss, rs2, '\n');

		R_type(findOp, rd, rs1, rs2);
	}
	else if (findOp.type == "I-type")
	{
		//lb rd,offset(rs1)
		getline(ss, rd, ',');
		getline(ss, offset, '(');
		getline(ss, rs1, ')');

		I_type(findOp, rd, rs1, offset);
	}
	else if (findOp.type == "S-type")
	{
		//sb rs2,offset(rs1)
		getline(ss, rs2, ',');
		getline(ss, offset, '(');
		getline(ss, rs1, ')');

		S_type(findOp, rs2, offset, rs1);
	}
	else if (findOp.type == "I-typeNum")
	{
		//addi rd,rs1,imm
		getline(ss, rd, ',');
		getline(ss, rs1, ',');
		getline(ss, imm, '\n');

		I_type_num(findOp, rd, rs1, imm);
	}
	else if (findOp.type == "I-typeM")
	{
		//slli rd,rs1,imm
		getline(ss, rd, ',');
		getline(ss, rs1, ',');
		getline(ss, imm, '\n');

		I_type_M(findOp, rd, rs1, imm);
	}
	else if (findOp.type == "B-type")
	{
		//BEQ rs1, rs2, offset
		getline(ss, rs1, ',');
		getline(ss, rs2, ',');
		getline(ss, offset, '\n');

		B_type(findOp, rs1, rs2, offset);
	}
	else if (findOp.type == "UJ-type")
	{
		//JAL rd, offset
		getline(ss, rd, ',');
		getline(ss, offset, '\n');

		UJ_type(findOp, rd, offset);
	}

}

//rs32i指令增加到vector中
void addinstruction(vector<Opcode>& instruction)
{
	string type;
	ifstream inFile("instruction.csv", ios::in);

	//檢查檔案是否存在
	if (!inFile)
	{
		cerr << "File not found" << endl;
		system("pause");
		exit(0);
	}

	//如果可以讀到檔案
	while (getline(inFile, type, ','))
	{
		Opcode tmp;
		tmp.type = type;

		getline(inFile, tmp.name, ',');

		if (type == "UJ-type")
		{
			getline(inFile, tmp.opcode, '\n');
		}
		else
		{
			getline(inFile, tmp.opcode, ',');

			if (type == "R-type" || type == "I-typeM")
			{
				getline(inFile, tmp.funct3, ',');
				getline(inFile, tmp.funct7, '\n');
			}
			else if (type == "I-type" || type == "S-type" || type == "I-typeNum" || type == "B-type")
			{
				getline(inFile, tmp.funct3, '\n');
			}
			else
			{
				cout << "Why you are here?\n";
				system("pause");
			}
		}



		instruction.push_back(tmp);

	}

	inFile.close();
}

int main()
{
	vector<Opcode> instruction;			//存入各種type 指令
	vector<string> implement;			//儲存使用者輸入的指令
	addinstruction(instruction);			//讀入type 和對應的opcode, etc.
	string input;					//input handler

	while (true)
	{
		getline(cin, input);			//輸入code

		if (input != "exit")
		{
			implement.push_back(input);	//存放輸入的指令
		}
		else
		{
			break;
		}
	}

	//判斷輸入的Label
	label_detected(implement);

	/*for (int i = 0; i < label.size(); ++i)
	{
		cout << label[i].name << "  " << label[i].pos << endl;
	}*/

	for (int imp = 0; imp < implement.size(); ++imp)
	{
		toMachineCode(implement[imp], instruction);
	}


	system("pause");
	return 0;

}
