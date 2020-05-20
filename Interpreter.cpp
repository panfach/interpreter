﻿#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <stack>
using namespace std;


// ============================ ENUM & STRUCT ========================== //
enum type_of_lex {
	LEX_NULL,    // 0
	LEX_BOOL,    // 
	LEX_INT,     // 
	LEX_STRING,  // 
	LEX_AND,     // 
	LEX_OR,      // 5
	LEX_BEGIN,   // 
	LEX_DO,      //
	LEX_END,     //    
	LEX_TRUE,    //
	LEX_FALSE,   // 10
	LEX_IF,      //
	LEX_ELSE,    //
	LEX_FOR,     //
	LEX_WHILE,   //
	LEX_BREAK,   // 15
	LEX_GOTO,    //
	LEX_NOT,     //         
	LEX_PROGRAM, // 
	LEX_READ,    //
	LEX_WRITE,   // 20
	LEX_THEN,    //
	LEX_VAR,     //
	LEX_STRUCT,  //
	LEX_FIN,     //

	LEX_NULL2,     // 25                                                                            
	LEX_SEMICOLON, // 
	LEX_COMMA,     //
	LEX_COLON,     //
	LEX_COLONEQ,   //
	LEX_OBRACKET,  // 30
	LEX_CBRACKET,  //
	LEX_EQ,        //
	LEX_LESS,      //    
	LEX_MORE,      //
	LEX_PLUS,      // 35
	LEX_MINUS,     //
	LEX_STAR,      //
	LEX_SLASH,     //
	LEX_LESSEQ,    //
	LEX_MOREEQ,    // 40
	LEX_NEQ,       //
	LEX_EQEQ,      //
	LEX_OBRACE,    //
	LEX_CBRACE,    //
	LEX_POINT,     // 45
	LEX_UNMINUS,   //
	LEX_QUOT,      //

	LEX_NUM,       //                                                                            
	LEX_ID,        //
	LEX_CSTR,      // 50
	POLIZ_LABEL,   //                                                                             
	POLIZ_ADDRESS, //                                                                                
	POLIZ_GO,      //                                                                                  
	POLIZ_FGO      //                                                                                    
};

enum type_of_err {
	ERR_STD,
	ERR_CHAR,
	ERR_COM,
	ERR_STR,
	ERR_END,
	ERR_LEX,
	ERR_TWDEF,
	ERR_UNDEF,
	ERR_TYPE,
	ERR_NOTSTRUCT,
	ERR_LABEL
};

struct my_exception {
	type_of_err type;
	string text;
	int strC;
	my_exception(type_of_err t, string str, int _strC = 0) {
		type = t;
		text = str;
		strC = _strC;
	}
	my_exception(type_of_err t, char c = '0') {
		type = t;
		text = c;
		strC = 0;
	}
	my_exception(type_of_err t, int str) {
		type = t;
		text = '0';
		strC = str;
	}
};

// ================================ LEX ================================ //
class Lex {
	type_of_lex t_lex;
	int v_lex;
	int s_lex;

public:
	Lex(int s = 0, type_of_lex t = LEX_NULL, int v = 0) { t_lex = t; v_lex = v; s_lex = s; }
	type_of_lex get_type() const { return t_lex; }
	int get_value() const { return v_lex; }
	int get_str() const { return s_lex; }

	/* friend ostream& operator << (ostream& stream, Lex L) */
};

// =================================== IDENT =================================== //
class Ident {
	string name;
	bool declare;
	type_of_lex type;
	bool assign;
	int value;
	int structValue;
	int labelAdress;

public:
	Ident() { declare = false; type = LEX_NULL; assign = false; value = 0; }
	Ident(const string n) { name = n; declare = false; type = LEX_NULL; assign = false; value = 0; }
	bool operator == (const string& s) const { return name == s; }

	string get_name() { return name; }
	bool get_declare() { return declare; }
	type_of_lex get_type() { return type; }
	bool get_assign() { return assign; }
	int get_value() { return value; }
	int get_structValue() { return structValue; }
	int get_labelAdress() { return labelAdress; }

	void put_declare() { declare = true; }
	void put_type(type_of_lex t) { type = t; }
	void put_assign() { assign = true; }
	void put_value(int v) { value = v; }
	void put_structValue(int v) { structValue = v; }
	void put_labelAdress(int v) { labelAdress = v; }
};



// ================================ TABLES === // ========================== //
vector<string> TS;                             // Table of Strings           //
vector<Ident> TID;                             // Table of Identificators    //
vector<Lex> TT;                                // Table of Lex               //
vector<Ident> TL;                              // Table of Labels            //
vector<string> TLN;                            // Table of Label Names       //
vector<vector<Ident>> TStruct;                 // Table of Structures        //
vector<Ident> TSN;                             // Table of Structure Names   //

// ====================================== SCANNER ====================================== //
class Scanner {
	FILE* f;
	char inpCh;
	int strCounter = 1;
	int look(const string& buf, vector<string> list) {
		int i = 0;
		vector<string>::iterator it;
		if ((it = find(list.begin(), list.end(), buf)) != list.end())
			return it - list.begin();
		return 0;
	}
	void gc() { inpCh = fgetc(f); }

public:
	static vector<string> TW, TD;
	Scanner(const char* program) {
		f = fopen(program, "r");
		inpCh = '0';
	}
	Lex GetLex();

	~Scanner() {
		fclose(f);
	}
};

vector<string> Scanner::TW = { "null", "boolean", "int", "string", "and", "or", "begin", "do",
							   "end", "true", "false", "if", "else", "for", "while", "break",
							   "goto", "not", "program", "read", "write", "then", "var", "struct" };
vector<string> Scanner::TD = { "null", ";", ",", ":", ":=", "(", ")", "=", "<", ">",
							   "+", "-", "*", "/", "<=", ">=", "!=", "==", "{", "}", ".", "\"" };

// ================================ PARSER ================================== //
/*
〈программа〉 → program _{ 〈описания〉 〈операторы〉 }_
〈описания〉 → { 〈описание〉; }
〈описание〉 → 〈тип〉 〈переменная〉 { , 〈переменная〉 }
〈тип〉 → int | string
〈переменная〉 → 〈идентификатор〉 | 〈идентификатор〉 = 〈константа〉 
〈константа〉 → 〈целочисленная〉 | 〈строковая〉
〈целочисленная〉 → [〈знак〉] 〈цифра〉 { 〈цифра〉 }
〈знак〉 → + | −
〈строковая〉 → " { 〈литера〉 } "
〈операторы〉 → { 〈оператор〉 }
〈оператор〉 →
	if (<выражение>) <оператор> else <оператор>
	| for ([выражение]; [выражение]; [выражение]) <оператор>
	| while (<выражение>) <оператор>
	| break;
	| goto <идентификатор> ;
	| read (<идентификатор>);
	| write (<выражение> { ,<выражение> } );
	| <составной оператор>
	| <помеченный оператор>
	| <оператор-выражение>
<помеченный оператор> → <идентификатор>: <оператор> ???
〈составной оператор〉 → _{ 〈операторы〉 }_
〈оператор-выражение〉 → 〈выражение〉;
*/
class Parser {
	Lex curr_lex, lastIdent;
	type_of_lex curr_type, var_type, res_type;
	int curr_val, curr_str, curr_struct, loop_counter;
	Scanner scanner;
	stack<int>st_int;
	stack<type_of_lex>st_lex;
	stack<string>st_struct;

	bool structDescrAvailable, structDescribing, descrSection/*!*/, firstIsVariable;

	void P();  // Program 
	void D();  // Descriptions
	void D1(); // Description
	void V1(); // Variable Initialization
	void C0(); // Integer const
	void O();  // Operators
	void O1(); // Operator
	void E0(); // Optional expression
	void E();  // Expression EQ
	void E1(); // Expression OR
	void E2(); // Expression AND
	void E3(); // Expression MORELESS
	void E4(); // Expression PLUSMINUS
	void E5(); // Expression MULTDIV
	void E6(); // Expression BRACKETS 
	void S1(); // Struct Point

	void Dec(type_of_lex type);
	void CheckID();
	void CheckOp();
	void CheckInit();
	int CheckLabel();
	void CheckTLN();
	void CheckNot();
	//void EqType();
	//void EqBool();
	//void CheckID_read();
	unsigned int PutStruct();
	unsigned int PutStructField(int index, Ident& id);
	template<class T, class T_EL> void from_st(T& t, T_EL& x);
	void GetL() {
		curr_lex = scanner.GetLex();
		poliz.push_back(curr_lex);
		curr_type = curr_lex.get_type();
		curr_val = curr_lex.get_value();
	}

public:
	vector<Lex> poliz;
	Parser(const char* program) : scanner(program), loop_counter(0) { }
	void Analyze();
};

// =========================== DESCRIPTIONS I ========================= //
ostream& operator << (ostream& stream, Lex L) {
	string lexName;
	stream.width(3);
	stream << L.get_str() << ": val: ";
	stream.width(8);
	stream << L.get_value();
	if (L.get_type() == LEX_ID)
		stream << " type: IDENT";
	else if (L.get_type() == LEX_NUM)
		stream << " type: NUMBER";
	else if (L.get_type() == LEX_CSTR)
		stream << " type: STRING";
	else if (L.get_type() > LEX_NULL && L.get_type() < LEX_FIN)
		stream << " type: " << Scanner::TW[L.get_type()];
	else if (L.get_type() > LEX_NULL2 && L.get_type() < LEX_NUM)
		stream << " type: " << Scanner::TD[L.get_type() - LEX_NULL2];
	else stream << " type: " << L.get_type();
	return stream;
}

int PutIdent(const string& buf) {
	vector<Ident>::iterator it;
	if ((it = find(TID.begin(), TID.end(), buf)) != TID.end())
		return it - TID.begin();
	TID.push_back(Ident(buf));
	return TID.size() - 1;
}

int PutString(const string& buf) {
	TS.push_back(buf);
	return TS.size() - 1;
}

int PutLex(const Lex& L) {
	TT.push_back(L);
	return TT.size() - 1;
}

int PutLabel(const string& buf) {
	vector<Ident>::iterator it;
	if ((it = find(TL.begin(), TL.end(), buf)) != TL.end()) return -1;
	TL.push_back(Ident(buf));
	return TL.size() - 1;
}

Lex Scanner::GetLex() {
	enum state { H, IDENT, NUMB, COM, ALE, STR, NEQ };
	state CS = H;
	string buf;
	int digit, j;
	while (1) {
		gc();
		switch (CS) {
		case H:
			if (inpCh == ' ' || inpCh == '\r' || inpCh == '\t');
			else
			if (inpCh == '\n') strCounter++;
			else
			if (isalpha(inpCh)) { buf.push_back(inpCh); CS = IDENT; }
			else
			if (isdigit(inpCh)) { digit = inpCh - '0'; CS = NUMB; }
			else
			if (inpCh == '#') { CS = COM; }
			else
			if (inpCh == '=' || inpCh == ':' || inpCh == '<' || inpCh == '>') { buf.push_back(inpCh); CS = ALE; }
			else 
			if (inpCh == '@') return Lex(strCounter, LEX_FIN);
			else
			if (inpCh == '!') { buf.push_back(inpCh); CS = NEQ; }
			else
			if (inpCh == '"') { CS = STR; }
			else 
			if (inpCh == EOF) throw my_exception(ERR_END);
			else {
				buf.push_back(inpCh);
				if (j = look(buf, TD))
					return Lex(strCounter, (type_of_lex)(j + LEX_NULL2), j);
				else
					throw my_exception(ERR_CHAR, inpCh);
			}
			break;
		case IDENT:
			if (isalpha(inpCh) || isdigit(inpCh)) { buf.push_back(inpCh); }
			else if (inpCh == ':') {
				if (j = look(buf, TW)) throw my_exception(ERR_STD, "Error. Incorrect name of label.");
				if ((j = PutLabel(buf)) == -1) throw my_exception(ERR_STD, "Error. Names of labels must be different.\n");
				return Lex(strCounter, POLIZ_LABEL, j);
			}
			else {
				ungetc(inpCh, f);
				if (j = look(buf, TW))
					return Lex(strCounter, (type_of_lex)j, j);
				else {
					j = PutIdent(buf);
					return Lex(strCounter, LEX_ID, j);
				}
			}
			break;
		case NUMB:
			if (isdigit(inpCh)) { digit = digit * 10 + inpCh - '0'; }
			else { ungetc(inpCh, f); return Lex(strCounter, LEX_NUM, digit); }
			break;
		case COM:
			if (inpCh == '\n') strCounter++;
			if (inpCh == '#') { CS = H; }
			else
			if (inpCh == '@') throw my_exception(ERR_COM, '@');
			break;
		case ALE:
			if (inpCh == '=') {
				buf.push_back(inpCh);
				j = look(buf, TD);
				return Lex(strCounter, (type_of_lex)(j + LEX_NULL2), j);
			}
			else {
				ungetc(inpCh, f);
				if (j = look(buf, TD))
					return Lex(strCounter, (type_of_lex)(j + LEX_NULL2), j);
				else throw my_exception(ERR_CHAR, inpCh);
			}

			break;
		case STR:
			if (inpCh == '@' || inpCh == '\n') throw my_exception(ERR_STR, inpCh);
			else
			if (inpCh == '"') {
				j = PutString(buf);
				return Lex(strCounter, LEX_CSTR, j);
			}
			else { buf.push_back(inpCh); }
			break;
		case NEQ:
			if (inpCh == '=') {
				buf.push_back(inpCh);
				j = look(buf, TD);
				return Lex(strCounter, LEX_NEQ, j);
			}
			else throw '!';
			break;
		}
	}
}

// ======================= PARSER FUNCTION DESCRIPTIONS ===================== //
void Parser::Analyze() {
	GetL();
	P();
	if (curr_type != LEX_FIN) throw my_exception(ERR_END);
	if (!TLN.empty()) throw my_exception(ERR_LABEL, TLN[0]); // Проверка на несуществующие метки

	for (Lex l : poliz) cout << l << endl;
	cout << endl;
}

void Parser::P() {  // Program
	if (curr_type == LEX_PROGRAM) GetL();
	else throw curr_lex;
	if (curr_type == LEX_OBRACE) GetL();
	else throw curr_lex;
	structDescrAvailable = true;
	D();
	O();
	if (curr_type == LEX_CBRACE) GetL();
	else throw curr_lex;
}

void Parser::D() {  // Descriptions
	descrSection = true;

	if (curr_type == LEX_INT || curr_type == LEX_STRING || curr_type == LEX_BOOL || curr_type == LEX_STRUCT) {
		type_of_lex var_type = curr_type;
		D1();
		if (curr_type == LEX_SEMICOLON) GetL();
		else throw curr_lex;
		D();
	}

	descrSection = false;
}

void Parser::D1() { // Description
	if (curr_type == LEX_INT || curr_type == LEX_STRING || curr_type == LEX_BOOL) {
		var_type = curr_type;
		structDescrAvailable = false;
		GetL();
		if (curr_type != LEX_ID) throw curr_lex;
		if (structDescribing) {
			st_int.push(PutStructField(curr_struct, TID[curr_val]));
			for (unsigned int i = curr_struct + 1; i < TStruct.size(); i++) {
				PutStructField(i, TID[curr_val]);
			}
		}
		else st_int.push(curr_val);
		GetL();
		V1();
		while (curr_type == LEX_COMMA) {
			GetL();
			if (curr_type != LEX_ID) throw curr_lex;
			if (structDescribing) {
				st_int.push(PutStructField(curr_struct, TID[curr_val]));
				for (unsigned int i = curr_struct + 1; i < TStruct.size(); i++) {
					PutStructField(i, TID[curr_val]);
				}
			}
			else st_int.push(curr_val);
			GetL();
			V1();
		}
		Dec(var_type);
	}
	else if (curr_type == LEX_STRUCT) {
		var_type = curr_type;
		if (structDescribing) throw my_exception(ERR_STD, "Error. Invalid type for structure field.\n");
		if (!structDescrAvailable) throw my_exception(ERR_STD, "Error. Structure must be described before variables.\n");
		curr_struct = -1;
		GetL();
		while (1) {
			if (curr_type != LEX_ID) throw curr_lex;
			st_int.push(curr_val);
			if (curr_struct == -1) {
				curr_struct = PutStruct();           // Запоминание индекса таблицы структур TStruct 
				TID[curr_val].put_structValue(curr_struct);
			}
			else {
				TID[curr_val].put_structValue(PutStruct());
			}
			GetL();
			if (curr_type == LEX_COMMA) {
				GetL();
				continue;
			}
			else break;
		}
		Dec(var_type);
		if (curr_type != LEX_OBRACE) throw curr_lex;
		GetL();
		structDescribing = true;
		D();
		if (curr_type != LEX_CBRACE) throw curr_lex;
		GetL();
		structDescribing = false;
	}
	else throw my_exception(ERR_STD, "Description Error.");
}

void Parser::V1() { // Variable Initialization
	if (curr_type == LEX_EQ) {
		GetL();
		E();
		CheckInit();
	}
}

void Parser::C0() { // Integer const
	if (curr_type == LEX_PLUS || curr_type == LEX_MINUS) {
		GetL();
		if (curr_type == LEX_NUM) GetL();
		else throw curr_lex;
	}
	else if (curr_type == LEX_NUM) GetL();
	else throw curr_lex;
}

void Parser::O() {  // Operators
	if (curr_type == LEX_IF || curr_type == LEX_FOR || curr_type == LEX_WHILE ||
		curr_type == LEX_BREAK || curr_type == LEX_GOTO || curr_type == LEX_READ ||
		curr_type == LEX_WRITE || curr_type == LEX_OBRACE || curr_type == LEX_OBRACKET ||
		curr_type == LEX_ID || curr_type == LEX_PLUS || curr_type == LEX_MINUS ||
		curr_type == LEX_NUM || curr_type == LEX_CSTR || curr_type == LEX_TRUE ||
		curr_type == LEX_FALSE || curr_type == POLIZ_LABEL) {

		O1();
		O();
	}
}

void Parser::O1() { // Operator
	type_of_lex type;
	if (curr_type == LEX_IF) {
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		GetL();
		E0();
		from_st(st_lex, type);
		if (type != LEX_BOOL) throw my_exception(ERR_STD, "Error. IF: Expression must be boolean.", curr_lex.get_str());
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
		O1();
		if (curr_type == LEX_ELSE) {
			GetL();
			O1();
		}
	}
	else if (curr_type == LEX_FOR) {
		loop_counter++;
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		GetL();
		if (curr_type != LEX_SEMICOLON) E();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
		if (curr_type != LEX_SEMICOLON) E0();
		from_st(st_lex, type);
		if (type != LEX_BOOL) throw my_exception(ERR_STD, "Error. FOR: The second expression must be boolean.", curr_lex.get_str());
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
		if (curr_type != LEX_CBRACKET) E();
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
		O1();
		loop_counter--;
	}
	else if (curr_type == LEX_WHILE) {
		loop_counter++;
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		GetL();
		E0();
		from_st(st_lex, type);
		if (type != LEX_BOOL) throw my_exception(ERR_STD, "Error. WHILE: Expression must be boolean.", curr_lex.get_str());
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
		O1();
		loop_counter--;
	}
	else if (curr_type == LEX_BREAK) {
		if (loop_counter == 0) throw my_exception(ERR_STD, "Error. break must be in the body of loop.\n", curr_lex.get_str());
		GetL();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
	}
	else if (curr_type == LEX_GOTO) {
		GetL();
		if (curr_type != LEX_ID) throw curr_lex;
		CheckLabel();
		GetL();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
	}
	else if (curr_type == LEX_READ) {
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		GetL();
		if (curr_type != LEX_ID) throw curr_lex;
		GetL();
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
	}
	else if (curr_type == LEX_WRITE) {
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		GetL();
		E();
		while (curr_type == LEX_COMMA) {
			GetL();
			E();
		}
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
	}
	else if (curr_type == LEX_OBRACE) {
		GetL();
		O();
		if (curr_type != LEX_CBRACE) throw curr_lex;
		GetL();
	}
	else if (curr_type == LEX_OBRACKET || curr_type == LEX_ID || curr_type == LEX_NUM ||
		     curr_type == LEX_CSTR || curr_type == LEX_TRUE || curr_type == LEX_FALSE) {
		firstIsVariable = true;
		E();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
	}
	else if (curr_type == POLIZ_LABEL) {
		CheckTLN();
		GetL();
		if (curr_type == POLIZ_LABEL) throw my_exception(ERR_STD, "Error. Two labels in a row.\n", curr_lex.get_str());
	}
	else throw curr_lex;
}

void Parser::E0() { // Optional expression
	if (curr_type == LEX_OBRACKET || curr_type == LEX_ID || curr_type == LEX_NUM ||
		curr_type == LEX_CSTR || curr_type == LEX_TRUE || curr_type == LEX_FALSE ||
		curr_type == LEX_PLUS || curr_type == LEX_MINUS || curr_type == LEX_NOT) {

		E();
	}
}

void Parser::E() {  // Expression EQ
	E1();
	firstIsVariable = false;
	curr_str = curr_lex.get_str();
	if (curr_type == LEX_EQ) {
		st_lex.push(curr_type);
		GetL();
		E1();
		CheckOp();
	}


}

void Parser::E1() { // Expression OR
	E2();
	while (curr_type == LEX_OR) {
		st_lex.push(curr_type);
		GetL();
		E2();
		CheckOp();
	}
}

void Parser::E2() {  // Expression AND
	E3();
	while (curr_type == LEX_AND) {
		st_lex.push(curr_type);
		GetL();
		E3();
		CheckOp();
	}
}

void Parser::E3() {  // Expression MORELESS
	E4();
	while (curr_type == LEX_LESS || curr_type == LEX_MORE || curr_type == LEX_LESSEQ || curr_type == LEX_MOREEQ ||
		   curr_type == LEX_EQEQ || curr_type == LEX_NEQ) {
		st_lex.push(curr_type);
		GetL();
		E4();
		CheckOp();
	}
}

void Parser::E4() {  // Expression PLUSMINUS
	E5();
	while (curr_type == LEX_PLUS || curr_type == LEX_MINUS) {
		st_lex.push(curr_type);
		GetL();
		E5();
		CheckOp();
	}
}

void Parser::E5() {  // Expression MULTDIV
	E6();
	while (curr_type == LEX_STAR || curr_type == LEX_SLASH) {
		st_lex.push(curr_type);
		GetL();
		E6();
		CheckOp();
	}
}

void Parser::E6() {  // Expression BRACKETS
	if (curr_type == LEX_ID) {
		CheckID();
		lastIdent = curr_lex;
		GetL();
		S1();
	}
	else if (firstIsVariable) throw my_exception(ERR_STD, "Error. Left operand must be variable.\n", curr_str);
	else if (curr_type == LEX_TRUE || curr_type == LEX_FALSE) {
		GetL();
		st_lex.push(LEX_BOOL);
	}
	else if (curr_type == LEX_CSTR) {
		GetL();
		st_lex.push(LEX_STRING);
	}
	else if (curr_type == LEX_NOT) {
		GetL();
		E6();
		CheckNot();
	}
	else if (curr_type == LEX_OBRACKET) {
		GetL();
		E();
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
	}
	else if (curr_type == LEX_NUM) {
		GetL();
		st_lex.push(LEX_INT);
	}
	else if (curr_type == LEX_MINUS) {
		GetL();
		if (curr_type == LEX_NUM) {
			GetL();
			st_lex.push(LEX_INT);
		}
		else if (curr_type == LEX_ID) {
			CheckID();
			lastIdent = curr_lex;
			GetL();
			S1();
		}
		else if (curr_type == LEX_OBRACKET) {
			GetL();
			E();
			if (curr_type != LEX_CBRACKET) throw curr_lex;
			GetL();
		}
	}
	else throw curr_lex;

	/*
	else {
		C0();
		st_lex.push(LEX_INT);
	}
	void Parser::C0() { // Integer const
		if (curr_type == LEX_PLUS || curr_type == LEX_MINUS) {
			GetL();
			if (curr_type == LEX_NUM) GetL();
			else throw curr_lex;
		}
		else if (curr_type == LEX_NUM) GetL();
		else throw curr_lex;
	}*/
}

void Parser::S1() {  // Struct Point
	if (curr_type == LEX_POINT) {
		// Проверка на то, ялвяется ли переменная перед LEX_POINT структурой
		if (TID[lastIdent.get_value()].get_type() != LEX_STRUCT) throw my_exception(ERR_NOTSTRUCT, TID[lastIdent.get_value()].get_name());

		GetL();
		if (curr_type != LEX_ID) throw curr_lex;

		// Проверка на то, есть ли такое поле в структуре
		vector<Ident>::iterator it;
		int structIndex = TID[lastIdent.get_value()].get_structValue();
		if ((it = find(TStruct[structIndex].begin(), TStruct[structIndex].end(), TID[curr_val].get_name())) == TStruct[structIndex].end())
			throw my_exception(ERR_STD, "Error. Incorrect field of a structure.\n");

		st_lex.push(it->get_type()); // Если вызывать просто CheckID, будут проблемы
		GetL();
		S1();
	}
	else if (lastIdent.get_type() == LEX_STRUCT) st_struct.push(TID[lastIdent.get_value()].get_name());
}

void Parser::Dec(type_of_lex type) {
	int i;
	while (!st_int.empty()) {
		from_st(st_int, i);
		if (structDescribing) { // Проверка переменных в структуре
			if (TStruct[curr_struct][i].get_declare()) throw my_exception(ERR_TWDEF, TStruct[curr_struct][i].get_name());
			else {
				for (unsigned int j = curr_struct; j < TStruct.size(); j++) {
					TStruct[j][i].put_declare();
					TStruct[j][i].put_type(type);
				}
			}
		}
		else { // Проверка переменных вне структуры
			if (TID[i].get_declare()) throw my_exception(ERR_TWDEF, TID[i].get_name());
			else {
				TID[i].put_declare();
				TID[i].put_type(type);
			}
		}
	}
}

void Parser::CheckID() {
	if (TID[curr_val].get_declare()) {
		if (TID[curr_val].get_type() != LEX_STRUCT)
			st_lex.push(TID[curr_val].get_type());
	}
	else throw my_exception(ERR_UNDEF, TID[curr_val].get_name());
}

void Parser::CheckOp() {
	type_of_lex t1, t2, op, t, r;
	from_st(st_lex, t2);
	from_st(st_lex, op);
	from_st(st_lex, t1);
	if (op == LEX_EQ) {
		t = t1;
		r = t1;
		if (t1 == LEX_STRUCT) {
			string s1, s2;
			from_st(st_struct, s1);
			from_st(st_struct, s2);
			if (s1 != s2) throw my_exception(ERR_STD, "Error. Equating of different structures.\n", curr_str);
		}
	}
	if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_STAR || op == LEX_SLASH) {
		if (t1 == LEX_INT) {
			t = LEX_INT; r = LEX_INT;
		}
		else if (t1 == LEX_STRING && op == LEX_PLUS) {
			t = LEX_STRING;
			r = LEX_STRING;
		}
		else throw my_exception(ERR_TYPE, curr_str);
	}
	if (op == LEX_OR || op == LEX_AND) {
		t = LEX_BOOL;
		r = LEX_BOOL;
	}
	if (op == LEX_LESS || op == LEX_MORE || op == LEX_LESSEQ || op == LEX_MOREEQ ||
		op == LEX_EQEQ || op == LEX_NEQ) {

		if (t1 == LEX_INT) t = LEX_INT;
		else if (t1 == LEX_STRING) t = LEX_STRING;
		else throw my_exception(ERR_TYPE, curr_str);
		r = LEX_BOOL;
	}
	if (t1 == t2 && t1 == t) st_lex.push(r);
	else throw my_exception(ERR_TYPE, curr_str);
	//poliz.push_back(Lex(op));
}

void Parser::CheckInit() {
	type_of_lex type;
	from_st(st_lex, type);
	if (type != var_type) throw my_exception(ERR_STD, "Error. Incorrect type of initializating constant.\n");
}

int Parser::CheckLabel() {
	vector<Ident>::iterator it = TL.begin();
	while (1) {
		if (it == TL.end()) {
			TLN.push_back(TID[curr_val].get_name()); 
			return 0;
		}
		if (it->get_name() == TID[curr_val].get_name()) return 1;
		it++;
	}
}

void Parser::CheckTLN() {
	vector<string>::iterator it;
	if ((it = find(TLN.begin(), TLN.end(), TL[curr_val].get_name())) != TLN.end()) TLN.erase(it);
}

void Parser::CheckNot() {
	if (st_lex.top() != LEX_BOOL) throw my_exception(ERR_TYPE, curr_str);
	// poliz.push_back(Lex(LEX_NOT)); //////////////////////////////////////////////////////////////////////////
}

unsigned int Parser::PutStruct() {
	vector<Ident> field;
	TStruct.push_back(field);
	return TStruct.size() - 1;
}

unsigned int Parser::PutStructField(int index, Ident& id) {
	vector<Ident>::iterator it;
	const string buf = id.get_name();
	if ((it = find(TStruct[index].begin(), TStruct[index].end(), buf)) != TStruct[index].end())
		return it - TStruct[index].begin();
	TStruct[index].push_back(id);
	return TStruct[index].size() - 1;
}

template<class T, class T_EL> 
void Parser::from_st(T& t, T_EL& x) {
	x = t.top();
	t.pop();
}

// ========================================================================== //
void PrintError(my_exception& exc) {
	cout << endl << exc.strC << ": ";
	switch (exc.type) {
	case ERR_STD:
		cout << exc.text << "\n";
		break;
	case ERR_CHAR:
		cout << "Error. Wrong symbol: " << exc.text << "\n";
		break;
	case ERR_COM:
		cout << "Error. Comment contains '" << exc.text << "' or other forbidden symbol.\n";
		break;
	case ERR_STR:
		cout << "Error. Closing quote of string was not found.\n";
		break;
	case ERR_END:
		cout << "Error. There isn't end symbol.\n";
		break;
	case ERR_TWDEF:
		cout << "Error. Variable " << exc.text << " is defined twice.\n";
		break;
	case ERR_UNDEF:
		cout << "Error. Variable " << exc.text << " is not defined.\n";
		break;
	case ERR_TYPE:
		cout << "Error. " << exc.strC << ": Invalid types.\n";
		break;
	case ERR_NOTSTRUCT:
		cout << "Error. Variable " << exc.text << " is not a structure.\n";
		break;
	case ERR_LABEL:
		cout << "Error. Label " << exc.text << " has not found.\n";
	}
	cout << "\n// ================= ERROR!!! ================ //\n";
}

void PrintIdent() {
	int i = 1;
	vector<Ident>::iterator it = TID.begin();
	cout << "Identifiers: \n";
	while (it != TID.end()) {
		cout.width(3);
		cout << i << ": ";
		cout << it->get_name() << endl;
		it++; i++;
	}
	cout << endl;
}

void PrintString() {
	int i = 1;
	vector<string>::iterator it = TS.begin();
	cout << "Strings: \n";
	while (it != TS.end()) {
		cout.width(3);
		cout << i << ": ";
		cout << *it << endl;
		it++;
	}
	cout << endl;
}

void PrintLex() {
	vector<Lex>::iterator it = TT.begin();
	cout << " Tokens: \n";
	while (it != TT.end()) {
		cout << *it << endl;
		it++;
	}
}

void PrintStruct() {
	int i = 0;
	vector<vector<Ident>>::iterator it = TStruct.begin();
	cout << "Structs: \n";
	while (it != TStruct.end()) {
		cout << i + 1 << ":\n";
		for (unsigned int j = 0; j < TStruct[i].size(); j++) {
			cout << "   " << TStruct[i][j].get_name() << endl;
		}
		it++;
		i++;
	}
	cout << endl;
}

int main(int argc, char* argv[]) {
	Parser parser("TextProg1.txt");
	Lex lex;

	cout << "// ========= Interpreter starts work! ========= //\n";

	try {
		parser.Analyze();
	}
	catch (my_exception exc) {
		PrintError(exc); 
		return 1;
	}
	catch (char c) {
		cout << "Error: character " << c << endl;
		return 1;
	}
	catch (Lex l) {
		cout << "Error. Invalid token:\n" << l << endl;
		return 1;
	}

	//PrintLex();
	PrintIdent();
	PrintString();
	PrintStruct();

	cout << "\n// =========== Interpreter has done! ========== //\n";
}