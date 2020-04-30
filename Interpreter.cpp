#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <stack>
using namespace std;


// ============================ ENUM & STRUCT ========================== //
enum type_of_lex {
	LEX_NULL,    // 0
	LEX_BOOL,    // 1
	LEX_INT,     // 2
	LEX_STRING,  // 3
	LEX_AND,     // 4
	LEX_OR,      // 5
	LEX_BEGIN,   // 6
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
	LEX_QUOT,      //

	LEX_NUM,       //                                                                            
	LEX_ID,        //
	LEX_CSTR,      //
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
	ERR_LEX
};

struct my_exception {
	type_of_err type;
	string text;
	my_exception(type_of_err t, string str) {
		type = t;
		text = str;
	}
	my_exception(type_of_err t, char c = '0') {
		type = t;
		text = c;
	}
};

// ================================ LEX ================================ //
class Lex {
	type_of_lex t_lex;
	int v_lex;

public:
	Lex(type_of_lex t = LEX_NULL, int v = 0) { t_lex = t; v_lex = v; }
	type_of_lex get_type() const { return t_lex; }
	int get_value() const { return v_lex; }

	/* friend ostream& operator << (ostream& stream, Lex L) */
};

// =================================== IDENT =================================== //
class Ident {

	string name;
	bool declare;
	type_of_lex type;
	bool assign;
	int value;

public:
	Ident() { declare = false; type = LEX_NULL; assign = false; value = 0; }
	Ident(const string n) { name = n; declare = false; type = LEX_NULL; assign = false; value = 0; }
	bool operator == (const string& s) const { return name == s; }

	string get_name() { return name; }
	bool get_declare() { return declare; }
	type_of_lex get_type() { return type; }
	bool get_assign() { return assign; }
	int get_value() { return value; }

	void put_declare() { declare = true; }
	void put_type(type_of_lex t) { type = t; }
	void put_assign() { assign = true; }
	void put_value(int v) { value = v; }
};

// ================================ TABLES ================================= //
vector<string> TS;
vector<Ident> TID;
vector<Lex> TT;

// =================================== SCANNER =================================== //
class Scanner {
	FILE* f;
	char inpCh;
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
	Lex curr_lex;
	type_of_lex curr_type;
	int curr_val;
	Scanner scanner;
	stack<int>st_int;
	stack<type_of_lex>st_lex;

	void P();  // Program 
	void D();  // Descriptions
	void D1(); // Description
	void V();  // Variables
	void V1(); // Variable
	void C0(); // Integer const
	void S();  // Struct
	void O();  // Operators
	void O1(); // Operator
	void E0(); // Boolean expression
	void E();  // Expression
	void E1(); // Term
	void A0(); // Boolean operations
	void A();  // Operations
	void O2(); // Compound operator

	//void Dec(type_of_lex type);
	//void CheckID();
	//void CheckOP();
	//void CheckNot();
	//void EqType();
	//void EqBool();
	//void CheckID_read();
	void GetL() {
		curr_lex = scanner.GetLex();
		poliz.push_back(curr_lex);
		curr_type = curr_lex.get_type();
		curr_val = curr_lex.get_value();
	}

public:
	vector<Lex> poliz;
	Parser(const char* program) : scanner(program) { }
	void Analyze();
};

// =========================== DESCRIPTIONS I ========================= //
ostream& operator << (ostream& stream, Lex L) {
	string lexName;
	if (L.get_type() > LEX_NULL && L.get_type() < LEX_FIN)
		stream << "type_of_lex: " << Scanner::TW[L.get_type()] << ", ";
	else if (L.get_type() > LEX_NULL2 && L.get_type() < LEX_NUM)
		stream << "type_of_lex: " << Scanner::TD[L.get_type() - LEX_NULL2] << ", ";
	else stream << "type_of_lex: " << L.get_type() << ", ";
	stream << "value: " << L.get_value();
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

Lex Scanner::GetLex() {
	enum state { H, IDENT, NUMB, COM, ALE, STR, NEQ };
	state CS = H;
	string buf;
	int digit, j;
	while (1) {
		gc();
		switch (CS) {
		case H:
			if (inpCh == ' ' || inpCh == '\n' || inpCh == '\r' || inpCh == '\t');
			else
			if (isalpha(inpCh)) { buf.push_back(inpCh); CS = IDENT; }
			else
			if (isdigit(inpCh)) { digit = inpCh - '0'; CS = NUMB; }
			else
			if (inpCh == '#') { CS = COM; }
			else
			if (inpCh == '=' || inpCh == ':' || inpCh == '<' || inpCh == '>') { buf.push_back(inpCh); CS = ALE; }
			else 
			if (inpCh == '@') return Lex(LEX_FIN);
			else
			if (inpCh == '!') { buf.push_back(inpCh); CS = NEQ; }
			else
			if (inpCh == '"') { CS = STR; }
			else {
				buf.push_back(inpCh);
				if (j = look(buf, TD))
					return Lex((type_of_lex)(j + LEX_NULL2), j);
				else
					throw my_exception(ERR_CHAR, inpCh);
			}
			break;
		case IDENT:
			if (isalpha(inpCh) || isdigit(inpCh)) { buf.push_back(inpCh); }
			else {
				ungetc(inpCh, f);
				if (j = look(buf, TW))
					return Lex((type_of_lex)j, j);
				else {
					j = PutIdent(buf);
					return Lex(LEX_ID, j);
				}
			}
			break;
		case NUMB:
			if (isdigit(inpCh)) { digit = digit * 10 + inpCh - '0'; }
			else { ungetc(inpCh, f); return Lex(LEX_NUM, digit); }
			break;
		case COM:
			if (inpCh == '#') { CS = H; }
			else
			if (inpCh == '@') throw my_exception(ERR_COM, '@');
			break;
		case ALE:
			if (inpCh == '=') {
				buf.push_back(inpCh);
				j = look(buf, TD);
				return Lex((type_of_lex)(j + LEX_NULL2), j);
			}
			else {
				ungetc(inpCh, f);
				if (j = look(buf, TD))
					return Lex((type_of_lex)(j + LEX_NULL2), j);
				else throw my_exception(ERR_CHAR, inpCh);
			}

			break;
		case STR:
			if (inpCh == '@' || inpCh == '\n') throw my_exception(ERR_STR, inpCh);
			else
			if (inpCh == '"') {
				j = PutString(buf);
				return Lex(LEX_CSTR, j);
			}
			else { buf.push_back(inpCh); }
			break;
		case NEQ:
			if (inpCh == '=') {
				buf.push_back(inpCh);
				j = look(buf, TD);
				return Lex(LEX_NEQ, j);
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
	for (Lex l : poliz) cout << l << endl;
	cout << endl << "Structure of program is correct \n" << endl;
}

void Parser::P() {
	if (curr_type == LEX_PROGRAM) GetL();
	else throw curr_lex;
	if (curr_type == LEX_OBRACE) GetL();
	else throw curr_lex;
	D();
	O();
	if (curr_type == LEX_CBRACE) GetL();
	else throw curr_lex;
}

void Parser::D() {  // Descriptions
	if (curr_type == LEX_INT || curr_type == LEX_STRING || curr_type == LEX_BOOL || curr_type == LEX_STRUCT) {
		D1();
		if (curr_type == LEX_SEMICOLON) GetL();
		else throw curr_lex;
		D();
	}
}

void Parser::D1() { // Description
	if (curr_type == LEX_INT || curr_type == LEX_STRING || curr_type == LEX_BOOL) {
		GetL();
		V();
	}
	else if (curr_type == LEX_STRUCT) {
		GetL();
		S();
	}
	else throw my_exception(ERR_STD, "Error.");
}

void Parser::V() {  // Variables
	V1();
	if (curr_type == LEX_COMMA) {
		GetL();
		V();
	}
}

void Parser::V1() { // Variable
	if (curr_type != LEX_ID) throw curr_lex;
	GetL();
	if (curr_type == LEX_EQ) {
		GetL();
		if (curr_type == LEX_CSTR) GetL();
		else C0();
	}
}

void Parser::C0() { // Integer const
	if (curr_type == LEX_PLUS || curr_type == LEX_MINUS) GetL();
	if (curr_type == LEX_NUM) GetL();
	else throw curr_lex;
}

void Parser::S() {  // Struct
	if (curr_type != LEX_OBRACE) throw curr_lex;
	GetL();
	D();
	if (curr_type != LEX_CBRACE) throw curr_lex;
	GetL();
}

void Parser::O() {  // Operators
	if (curr_type != LEX_CBRACE) {
		O1();
		O();
	}
}

void Parser::O1() { // Operator
	if (curr_type == LEX_IF) {
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		E0();
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		O1();
		if (curr_type == LEX_ELSE) {
			GetL();
			O1();
		}
	}
	else if (curr_type == LEX_FOR) {
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		GetL();
		if (curr_type != LEX_SEMICOLON) E();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
		if (curr_type != LEX_SEMICOLON) E0();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
		if (curr_type != LEX_CBRACKET) E();
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
		O1();
	}
	else if (curr_type == LEX_WHILE) {
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		GetL();
		E0();
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
		O1();
	}
	else if (curr_type == LEX_BREAK) {
		GetL();
	}
	else if (curr_type == LEX_GOTO) {
		GetL();
		if (curr_type != LEX_ID) throw curr_lex;
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
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
	}
	else if (curr_type == LEX_OBRACE) {
		O2();
	}
	else if (curr_type == LEX_OBRACKET || curr_type == LEX_ID || curr_type == LEX_NUM || curr_type == LEX_CSTR) {
		E();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
	}
	else throw curr_lex;
}

void Parser::E0() { // Boolean expression
	if (curr_type == LEX_NOT) {
		GetL();
		if (curr_type != LEX_ID) throw curr_lex;
		GetL();
	}
	if (curr_type != LEX_ID) throw curr_lex;
	GetL();
	if (curr_type != LEX_CBRACKET && curr_type != LEX_SEMICOLON) {
		A0();
		if (curr_type == LEX_ID) GetL();
		else C0();
	}
}

void Parser::E() {  // Expression
	if (curr_type == LEX_OBRACKET) {
		GetL();
		E1();
		if (curr_type == LEX_CBRACKET);
		else {
			A();
			E();
		}
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
	}
	else {
		E1();
		if (curr_type != LEX_EQ && curr_type != LEX_PLUS && curr_type != LEX_MINUS && curr_type != LEX_STAR &&
			curr_type != LEX_SLASH && curr_type != LEX_POINT);
		else {
			A();
			E();
		}
	}
}

void Parser::E1() { // Term
	if (curr_type == LEX_ID || curr_type == LEX_CSTR) GetL();
	else C0();
}

void Parser::A0() { // Boolean operations
	if (curr_type == LEX_LESS || curr_type == LEX_MORE || curr_type == LEX_LESSEQ || curr_type == LEX_MOREEQ ||
		curr_type == LEX_EQEQ || curr_type == LEX_NEQ) GetL();
	else throw curr_lex;
}

void Parser::A() {  // Operations
	if (curr_type == LEX_EQ || curr_type == LEX_PLUS || curr_type == LEX_MINUS || curr_type == LEX_STAR ||
		curr_type == LEX_SLASH || curr_type == LEX_POINT) GetL();
	else throw curr_lex;
}

void Parser::O2() { // Compound operator
	if (curr_type != LEX_OBRACE) throw curr_lex;
	GetL();
	O();
	if (curr_type != LEX_CBRACE) throw curr_lex;
	GetL();
}

// ========================================================================== //
void PrintError(my_exception& exc) {
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
	}
	cout << "\n// ================= ERROR!!! ================ //\n";
}

void PrintIdent() {
	vector<Ident>::iterator it = TID.begin();
	cout << " Identifiers: \n";
	while (it != TID.end()) {
		cout << it->get_name() << "\n";
		it++;
	}
}

void PrintString() {
	vector<string>::iterator it = TS.begin();
	cout << " Strings: \n";
	while (it != TS.end()) {
		cout << *it << endl;
		it++;
	}
}

void PrintLex() {
	vector<Lex>::iterator it = TT.begin();
	cout << " Tokens: \n";
	while (it != TT.end()) {
		cout << *it << endl;
		it++;
	}
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
		cout << "Error. Invalid token: " << l << endl;
		return 1;
	}

	//PrintLex();
	PrintIdent();
	PrintString();

	cout << "\n// =========== Interpreter has done! ========== //\n";
}