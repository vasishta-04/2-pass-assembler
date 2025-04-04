/*
EDAPALAPATI HEMANTH GUPTHA
2301CS21, IIT PATNA
CS2102 MINI_PROJECT
ASSEMBLER
*/

#include <bits/stdc++.h>
using namespace std;


vector<pair <int,string>> Errors;	 
//for storing all the errors identified and the line of the respective error

vector<pair <int,string>> Warnings;	 
//for storing all the warning identified and the line of the respective warning

vector< tuple<int,string,string,string,string> > Lines;	  
//prog. counter,label, mnemonic, operand, prevOperand	

vector<tuple<string,string,string>> List;	 
//address, machine Code, statement(for printing in .lst file)

vector<string> MachineCodes;				
// Machines codes in 8 bit hex form

void pushErrors(int line_no, string err) {
	Errors.push_back({line_no, err});
}
//function to add the identified errors into the defined vector

void pushWarnings(int line_number, string err) {
	Warnings.push_back({line_number, err});
}
//function to add the identified warnings into the defined vector

map<string, string > op_code;		
// for giving the operands with respective opcodes

map<string, int> operand_type;      
/*type of operands 
 	type 0 : nothing required
 	type 1 : value required
 	type 2 : offset required
*/

map<string, pair<int, int> > labelss;		
//for storing labels there program counter and line number

map<string, vector<int> > locLabels; 
// location of labels where they are used

map<string, string> setVars; 		
// variables(label) and value associated


vector<string> readLines; 		
// stores each line 

void fill_op_code() {
	op_code = {{"data", ""}, {"ldc", "00"}, {"adc", "01"}, {"ldl", "02"},
			{"stl", "03"}, {"ldnl", "04"}, {"stnl", "05"}, {"add", "06"},
			{"sub", "07"}, {"shl", "08"}, {"shr", "09"}, {"adj", "0A"},
			{"a2sp", "0B"}, {"sp2a", "0C" }, {"call", "0D"}, {"return", "0E"},
			{"brz", "0F"}, {"brlz", "10"}, {"br", "11"}, {"HALT", "12"},
			{"SET", ""}};

}
void fill_operand_type(){
    operand_type = {{"data",  1}, {"ldc",  1}, {"adc", 1}, {"ldl", 2},
			{"stl", 2}, {"ldnl",  2}, {"stnl", 2}, {"add",  0},
			{"sub", 0}, {"shl", 0}, {"shr", 0}, {"adj", 1},
			{"a2sp", 0}, {"sp2a", 0}, {"call", 2}, {"return", 0},
			{"brz", 2}, {"brlz", 2}, {"br", 2}, {"HALT", 0},
			{"SET", 1}};
}

vector<string> extract_words(string present_line) {	// extract information from each line
	if (present_line.empty()) return {};
	stringstream present(present_line);
	string word;
    vector<string> res;
	while (present >> word) {                           
		if (word.length()==0){
            continue;
        }
		if (word[0] == ';'){
            break;
        } 
		auto i = word.find(':');
		if (i != -1 and i!= word.length()-1) {		// case when there is no separation between ':' and the statement
			string temp = word.substr(0, i + 1);
            word = word.substr(i + 1);
            res.push_back(temp);
		}
		if (word.back() == ';') {				// case when there is no seperation between ';' and previous word
			word.pop_back();
			res.push_back(word);
			break;
		}
        auto j = word.find(';');                //case when there is no space between ';', previous word, nxt word
        if(j != -1 and j!= word.length()-1){
            word=word.substr(0,j);
            res.push_back(word);
            break;
        }
        res.push_back(word);
	}
	string comment = "";             
    int i=0;
	while ( i < (int)present_line.size()) {
		if (present_line[i] == ';') {
			int j = i + 1;
			for (;j < (int)present_line.size() and present_line[j] == ' ';) j++;
			for ( ;j < (int)present_line.size();j++) {
				comment += present_line[j];
			}
			break;
		}
        i++;
	}
	return res;
}
//checking weather a char is a number or no
bool it_is_Digit(char c) {
    if(c >= '0' and c <= '9'){
        return 1;
    }else{
        return 0;
    }
}

bool it_is_Alphabet(char c) {
	c = tolower(c);             //conversion of uppercase to lower case for verifying
    if(c >= 'a' and c <= 'z'){
        return 1;
    }else{
        return 0;
    }
}
//checking weather a lable is valid or not
bool Valid_Label(string labelName) {
	bool ok = true;
    if(!it_is_Alphabet(labelName[0])){
        ok = false;
    }
	for (char c : labelName) {
        if(! (it_is_Digit(c) or it_is_Alphabet(c) or (c == '_'))){
            ok = false;
            break;
        }
		
	}
	return ok;
}
//checking weather a string is a number or not
bool is_decimal(string num) {
	bool ok = true;
	if(num[0]=='0'){
		return false;
	}
	for (char c : num) {
        if(!it_is_Digit(c)){
            ok=false;
            break;
        }
	}
	return ok;
}
//checking weather a string is a octal or not
bool is_Octal(string num) {
	bool ok = true;
    if(num[0] != '0'){
        ok=false;
    }
	for (char c : num) {
        if(!(c >= '0' and c <= '7')){
            ok=false;
            break;
        }
	}
	return ok;
}
//checking weather a string is a hexa decimal or not
bool is_Hexadecimal(string num) {
	bool ok = true;
    if(!((int)num.size() >= 3 and num[0] == '0' and tolower(num[1]) == 'x')){
        ok =false;
    }
	for (int i = 2; i < (int)num.size(); ++i) {
        if(!(it_is_Digit(num[i]) or (tolower(num[i]) >= 'a' and tolower(num[i]) <= 'f'))){
            ok=false;
            break;
        }
	}
	return ok;
}
// Conversions...
string Octal_to_Decimal(string num) {
	int res = 0;
    int i = (int)num.size() - 1, pwr = 1; 
	while (i >= 0) {
		res += pwr * (num[i] - '0');
        --i;
        pwr *= 8;
	}
	return to_string(res);
}

string Hex_to_Decimal(string num) {
	int res = 0;
    int i = (int)num.size() - 1, pwr = 1;
	while ( i >= 0) {
        if(it_is_Digit(num[i])){
            res = res +pwr*(num[i] - '0');
        }else{
            res = res +pwr*((tolower(num[i]) - 'a') + 10);
        }
        --i;
        pwr *= 16;
	}
	return to_string(res);
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

// Checking errors in labels if present
void verifying_label(string label, int line_number, int prog_counter) {
	if (label.length()==0) {
        return;
    }
	bool valid_label = Valid_Label(label);
	if (valid_label == false) {
		pushErrors(line_number, "not a valid Label name");  
	} else {
		if (labelss.count(label) !=0 and labelss[label].first != -1 ) {
			pushErrors(line_number, "Duplicate label definition");
		} else {
			labelss[label].first= prog_counter;
            labelss[label].second= line_number;
		}
	}
}

// Checking errors associated with operands
string processed_Operand(string operand, int line_number) {
	string temp = "";
	if (Valid_Label(operand)) {
		if (labelss.count(operand)==0) {
			labelss[operand] = {-1, line_number};		// label used but wasn't declared so far...
		}
		locLabels[operand].push_back(line_number);
		return operand;                  
	}
	string op = operand;
	if (op[0] == '-' or op[0] == '+') {
		temp = temp+op[0];
		op = op.substr(1);
	}
	if (is_Octal(op) == true) {
		temp = temp + Octal_to_Decimal(op.substr(1));
	} else if (is_Hexadecimal(op) == true) {
		temp = temp + Hex_to_Decimal(op.substr(2)); 
	} else if (is_decimal(op) == true) {
		temp = temp + op;
	} else {
		temp = "";
	}

	return temp;
}

// Checking errors associated with Mnemonics
void processed_Mnemonic(string instName, string &operand, int line_number, int prog_counter, int reminder, bool &flag) {
	flag =0;
    if (instName.length()==0) return;
	if (op_code.count(instName)==0) {
		pushErrors(line_number, "Bogus Mnemonic(mnemonic not found)");
	} else {
		int type = operand_type[instName];
		if (type > 0) {
			if (operand.length()==0) {
				pushErrors(line_number, "Missing operand");
			} else if (reminder > 0) {
				pushErrors(line_number, "extra on end of line");
			} else {
				string replaceOP = processed_Operand(operand, line_number);
				if (replaceOP.length()==0) {
					pushErrors(line_number, "Invalid format: label or the given number is not valid");
				} else {
					operand = replaceOP;
					flag = 1;
				}
			}
		} else if (type == 0 and operand.length()!=0) {
			pushErrors(line_number, "unexpected operand");
		} else {
			flag = 1;
		}
	}
}

// Reading from the input file
void read(const string& filename) {
	ifstream input_file;
    input_file.open(filename);
    if (input_file.fail()) {
        cout << "Input file doesn't exist" << endl;
        exit(0);
    }
	string present_line;
	while (getline(input_file, present_line)) {
		readLines.push_back(present_line);
	}
	input_file.close();
}

// Function to find all the errors and also saving each line 
void pass1(const vector<string> &readLines) {
	int line_number = 0, prog_counter = 0;
	for (string present_line : readLines) {
		line_number++;
		auto current_sentence = extract_words(present_line);
		if (current_sentence.size()==0){
            continue;
        }
		
		string label = "", instName = "", operand = "";
		int i = 0, size = current_sentence.size();
		if (current_sentence[i].back() == ':') {
			label = current_sentence[i];
			label.pop_back();
			i++;
		}
		if (i < size) {
			instName = current_sentence[i];
			i++;
		}
		if (i < size) {
			operand = current_sentence[i];
			i++;
		}
		
		verifying_label(label, line_number, prog_counter);	
		string prevOperand = operand;
        bool flag;
		processed_Mnemonic(instName, operand, line_number, prog_counter, size - i, flag);
		Lines.push_back({prog_counter, label, instName, operand, prevOperand});
		
		prog_counter = prog_counter + flag;
		if (flag == true and instName == "SET") {		
			if (label.length()==0) {
				pushErrors(line_number, "label(or variable) name missing");
			} else {
				setVars[label] = operand;		// Storing SET instruction information
			}
		}
	}
	// Checking for some errors...
	for (auto label : labelss) {
		if (label.second.first == -1) {
			for (auto lineNum : locLabels[label.first]) {
				pushErrors(lineNum, "no such label");
			} 
		} else if (locLabels.count(label.first)==0) {
			pushWarnings(label.second.second, "declared label is not used");
		}
	}
}

// Inserting into List vector
void pushInList(int prog_counter, string macCode, string label, string mnemonic, string operand) {
	if (label.length()!=0) label += ": ";
	if (mnemonic.length()!=0) mnemonic += " ";
	string statement = label + mnemonic + operand;
	List.push_back({decimal_to_Hex(prog_counter), macCode, statement});
}


// Generating machine codes and building list vector
void pass2() {
	for (auto present_line : Lines) {
		string label = get<1>(present_line), mnemonic = get<2>(present_line), operand = get<3>(present_line);
		string prevOperand = get<4>(present_line);
		int prog_counter = get<0>(present_line), type = -1;
		if (mnemonic.length()!=0) {
			type = operand_type[mnemonic];
		}
		string Mcode = "        ";
		if (type == 2) { 		// offset is required
			int offset;
			if(operand != "0" and labelss.count(operand) != 0){
				offset = labelss[operand].first - (prog_counter )-1;
			}else{
				offset = (int)stoi(operand);
			}
			Mcode = decimal_to_Hex(offset).substr(2) + op_code[mnemonic];
			
		} else if (type == 1 and mnemonic != "data" and mnemonic != "SET") {	// value is required	
			int value;
            if(labelss.count(operand) != 0 ){
				value = labelss[operand].first;
			}else{
                value = (int)stoi(operand);
            }
			Mcode = decimal_to_Hex(value).substr(2) + op_code[mnemonic];
		} else if (type == 1 and (mnemonic == "data" || mnemonic == "SET")) {	 // specical case for data  and SET mnemonic
			Mcode = decimal_to_Hex(stoi(operand));
			
		} else if (type == 0) {													// nothing is required
			Mcode = "000000" + op_code[mnemonic];
			
		}
		MachineCodes.push_back(Mcode);
		
		pushInList(prog_counter, Mcode, label, mnemonic, prevOperand);
	}
}

// Writing errors and warnings into .log file
void ErrorsAndWarnings(string input_file) {
	ofstream coutErrors(input_file+".log");
	sort(Warnings.begin(), Warnings.end());
    sort(Errors.begin(), Errors.end());
	cout << "Errors(.log) file generated" << endl;
	if (Errors.size()==0) {
		coutErrors << "No errors!" << endl;
        if(Warnings.size()==0){
            coutErrors<<"no warnings!!"<<endl;
        }
		for (auto warning : Warnings) {
			coutErrors << "In Line" << warning.first << " WARNING: " << warning.second << endl;
		}
		coutErrors.close();
		return;
	}
	for (auto error : Errors) {
		coutErrors << "In Line" << error.first << " ERROR: " << error.second << endl;
	}
	coutErrors.close();
}


// Writing into listing file(.lst) and machine codes object file(.o)
void write(string input_file) {
	ofstream coutList(input_file+".lst");
	
	for (auto current_sentence : List) {
		coutList << get<0>(current_sentence) << " " << get<1>(current_sentence) << " " << get<2>(current_sentence) << endl;
	}
	coutList.close();
	cout << "Listing(.lst) file generated" << endl;
	ofstream coutMCode;
	coutMCode.open(input_file+".o", ios::binary | ios::out);
	for (auto code : MachineCodes) {
		if (code.length() == 0 or code == "        ") continue;
		unsigned int curr = (unsigned int)stoi(Hex_to_Decimal(code));
		for (int i=31;i>=0;i--){
			int bit = (curr>>i)&1;
			coutMCode<<bit;
		}
		coutMCode<<endl;
	}
	coutMCode.close();
	cout << "Machine code object(.o) file generated" << endl;
}

int main() {
    cout<<"enter filename(ex: test1):\n";
    string filename;
    cin>>filename;             // giving filename(example: bubblesort)
    read(filename+".txt");	   // reading the given file		
	fill_op_code();				// intialise Opcode table
    fill_operand_type();
	pass1(readLines);							// first pass
	ErrorsAndWarnings(filename);			// if found errors, write and terminate 
	if (Errors.empty()) {					 
		pass2();							//if no errors go for second pass
		write(filename);					// write machine code and listing file	
	}
	return 0;
}