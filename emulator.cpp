/*
EDAPALAPATI HEMANTH GUPTHA
2301CS21, IIT PATNA
CS2102 MINI_PROJECT
EMULATOR
*/
#include<stdio.h> 
#include<string.h> 
#include<stdlib.h>
#include<math.h> 
#include<ctype.h>
#include<time.h>
#include<assert.h> 
#include<locale.h> 
#include<signal.h> 
#include<setjmp.h> 
#include<stdarg.h>
#include<errno.h>
#include<float.h>
#include<limits.h>
#include<stdalign.h> 
#include<wchar.h>
#include <stdbool.h>
#include<iostream>
#include<fstream>
#include <bits/stdc++.h>
using namespace std;

// declaring global variable for memory_size
int Memory_size = 1<<24;

//declaring the 4 registers
int A,B,SP,PC;
int total_no_instructions;

// initializing Memory1,Memory2 
int Memory[1<<24];                  //for saving machine codes before execution
int Memory2[1<<24]; 				//for saving machine codes after execution
//using arrays as hardisk for storage as memory 

// setting operand type
string operand_type_inp(int x) {
	set<int> offset = {2,3,4,5,13,15,16,17};
	set<int> value = {0,1,10};

	if (value.count(x)){
		return "VALUE";
	}else if(offset.count(x)){
		 return "OFFSET";
	}
	return "";
}

//declaring list of the instruction
vector <string> list_of_instructions = {"ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT"};
void show_operands(){

	cout<<"\nop_code | memonic | operand\n";
	int i =0;
	for (auto x:list_of_instructions) {
		cout<<i<<"\t"<<x<<"\t\t"<<operand_type_inp(i)<<"\n";
		i++;
	}
	return;

}

// conversion of number into 8bit hexadecimal
string decimal_to_Hex(int a){
    int n =8;
    stringstream sstream;
    sstream << hex << a;
    string result = sstream.str();
    while(result.size()<n){
        result = "0" +result;
    }
    if(result.size()>n){
        result = result.substr(result.size()-n,n);
    }
    return (result);
}

// binary input to int
int bin_string_to_decimal(string bin) {
	long long number = 0;
	for (int i = 0; i < 32; i++) {
		number = number * 2;
		if (bin[i] != '0'){
			number += 1; 
		}
	}
	return (int)number;
}

//copy the Memory2 from one address to another
void copy_memory(int M_from[],int M_to[]){
	int maxi ;
	maxi = 1<<24;
    for(int i=0;i<maxi;i++)
    {
    	M_to[i] = M_from[i];
    }
	
}

//reset Memory2 to how it was originaly
void reset_hdd2()
{
	long long n = 1<<24 -1;
	while(n--)
	{
		Memory2[n] = Memory[n];
	}
}

//delete the file extension
string del_extension(string st,char enu){
	string out;
	for(int i=0;i<st.length();i++){
		if(st[i]!=enu){
			out+=st[i];
		}else{
			break;
		}
	}

	return out;
}

//emulates line by line
int liveEmulation(int opcode, int operand) {
	fprintf(stderr, " A=%08x, B=%08x, PC=%08x, SP=%08x,%s\t%08x\n",  A, B,PC, SP, list_of_instructions[opcode].c_str(), operand);
	return 1;
}

// All the given operations			
void ldc(int x) {
	B = A;
	A = x;
}

void adc(int x) {
	A = A + x;
}

void ldl(int offset) {
	B = A;
	A = Memory2[SP + offset];
}

void stl (int offset) {
	Memory2[SP + offset] = A;
	A = B;
} 

void ldnl(int offset) {
	A = Memory2[A + offset];
}


void stnl(int offset) {
	Memory2[A + offset] = B;
}

void add() {   
	A = A + B;
}

void sub() {  
	A = B - A;
}

void shl() {
	A = B << A;
}

void shr() {
	A = B >> A;
}

void adj(int x) {
	SP = SP + x;
}

void a2sp() {
	SP = A;
	A = B;
}

void sp2a() {
	B = A;
	A = SP;
}

void call(int offset) {
	B = A;
	A = PC;
	PC = PC + offset;
}

void ret() {
	PC = A;
	A = B;
}
void brz(int offset) {
	if (A == 0) {
		PC = PC + offset;
	}
}

void brlz(int offset) {
	if (A < 0) {
		PC = PC + offset;
	}
}

void br(int offset) {
	PC = PC + offset;
}

//shows the comands available
void show_commands(){
	cout<<"\nenter input\n";
	cout<<"\ncommands : \n";
	cout<<"1) trace : perform emulation in the terminal\n";
	cout<<"2) Memory before execution   : show Memory2 dump before emulation\n";
	cout<<"3) Memory after execution    : show Memory2 dump after emulation\n";
	cout<<"4) ISA             : show the instuction set in terminal\n";
	cout<<"5) export dump          : export to a dump file\n";
	cout<<"6) exit                 : exit the emulator\n";
}


//return the last 8 bits
int extract_opcode_from_harddisk(int x)
{
	int bin = ((1 << 8) - 1);
	return  x & bin;
}

//returns the top 24 bits
int extract_operand_from_harddisk(int x)
{
	long long out = (x>>8);
	return (int)out;
}

//instructions with the help of op_code
int executeInstruction(int opcode, int operand) {
	if (opcode == 0){
		ldc(operand);
	}else if(opcode == 1) {
		adc(operand);
	}else if(opcode == 2) {
		ldl(operand);
	}else if(opcode == 3){
		stl(operand);
	}else if(opcode == 4){
		ldnl(operand);
	}else if(opcode == 5){
		stnl(operand);
	}else if(opcode == 6){
		add();
	}else if(opcode == 7){
		sub();
	}else if(opcode == 8){
		shl();
	}else if(opcode == 9){
		shr();
	}else if(opcode == 10){
		adj(operand);
	}else if(opcode == 11){
		a2sp();
	}else if(opcode == 12){
		sp2a();
	}else if(opcode == 13){
		call(operand);
	}else if(opcode == 14) {
		ret();
	}else if(opcode == 15){
		brz(operand);
	}else if(opcode == 16) {
		brlz(operand);
	}else if(opcode == 17) {
		br(operand);
	}else if(opcode == 18){
		return 0;
	}

	return 1;
}

//----------emulates the give line--------------//
void emulatefunc(bool live)
	{
		int  opcode = 0,operand = 0,instruction_num = 0;
		SP = (1 << 23) -1;
		PC = 0;
		opcode = extract_opcode_from_harddisk(Memory2[PC]);
		while(opcode <= 18){
			
			operand = extract_operand_from_harddisk(Memory2[PC]);

			if(live){
				liveEmulation(opcode,operand);
			}
			// if (trace) printTrace(opcode, operand);
			if (opcode == 18){
				break;
			}
			PC += 1; 
			executeInstruction(opcode, operand); 
			instruction_num += 1;
			opcode = extract_opcode_from_harddisk(Memory2[PC]);
		}
		if(live){
			fprintf(stderr, "\n %d total number of instructions.\n", instruction_num);
		}
	}

int main()
{
	//taking input of file name
	cout<<"\nenter the file name to emulate\n";
	string filename;
	cout<<"file : ";
	cin>>filename;

	//opening file
    fstream objstream;
    objstream.open(filename, ios::in); 

	//checking if file is open
	if(!objstream.is_open()){
		cout<<" no file found\n";
		return 0;
	}
	
	string bin_string;
	int num_instructions = 0;
	// Read data from the file object and put it into a string.
	while (getline(objstream, bin_string)) { 
		// loads the value in Memory2.
		long long number =0;
		for (int i = 0; i < 32; i++) {
			number = number * 2;
			if (bin_string[i] != '0'){
				number += 1; 
		}
		//assign the caluculated number to Memory2
		Memory[num_instructions] = number;
	}
		num_instructions++;
	}
	total_no_instructions = num_instructions;
	
	// Close the file object.
	objstream.close(); 
    
    //coping Memory2
	copy_memory(Memory,Memory2);
    string file_without_extention = del_extension(filename , '.');
	string export_file_name = file_without_extention+"_MEMORY_DUMP.txt";
	string control = "test";

	show_commands();

	while(control!="exit")
	{	
		cout<<"\n::COMMAND::\n";
		cout<<"\n--> ";
		cin>>control;

		if(control =="2")
		{	
			//dumping Memory2
			cout<<"MEMORY DUMP BEFORE EXECUTION (HEX)\n";
			for(int i=0;i<total_no_instructions;i++)
			{
				if(i%4==0){
					printf("\n");
					cout << decimal_to_Hex(i) << "      ";
				}
				fprintf(stderr,"%08x ",Memory[i]);
			}

		}else if(control =="3")
		{
			emulatefunc(0);
			cout<<"Memory2 dump after emulation\n";
			int i=0;
			for(i=0;i<total_no_instructions;i++)
			{
				if(i%4==0){
					printf("\n");
					cout << decimal_to_Hex(i) << "      ";
				}
				fprintf(stderr,"%08x ",Memory2[i]);
				
			}
			reset_hdd2();
		}
		else if(control=="4")
		{
			show_operands();
		}
		else if(control=="5")
		{	
				FILE* dumpfile;
				dumpfile = fopen(export_file_name.c_str(), "w");

				if(dumpfile != NULL){
					emulatefunc(0);
					int i = 0;
					for(i=0;i<total_no_instructions;i++)
					{
						if(i%4==0){
							fprintf(dumpfile, "\n");
						}
						fprintf(dumpfile,"%08x  ",Memory2[i]);
					}
					reset_hdd2();
					fclose(dumpfile);
					cout<<"exported Memory2 file\n";
				}else{
					cout<<"\n--> error openinig file\n";
				}
		}
		else if(control == "1")
		{
			emulatefunc(1);
			reset_hdd2();
		}
		else if(control == "6"||control == "exit"){
			break;
		}else{
			cout<<"command not found\n";
		}
	}
	return 0;
}