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
	ERR_LEX,
	ERR_TWDEF,
	ERR_UNDEF,
	ERR_TYPE
};

struct my_exception {
	type_of_err type;
	string text;
	int strC;
	my_exception(type_of_err t, string str) {
		type = t;
		text = str;
		strC = 0;
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
	Lex curr_lex;
	type_of_lex curr_type;
	int curr_val, curr_str;
	Scanner scanner;
	stack<int>st_int;
	stack<type_of_lex>st_lex;

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


	/*void P();  // Program 
	void D();  // Descriptions
	void D1(); // Description
	void V0(); // Next Variable 
	void V1(); // Variable Initialization
	void C0(); // Integer const
	void O();  // Operators
	void O1(); // Operator
	void W();  // Write
	void E0(); // Optional expression
	void E();  // Expression
	void E2(); // Next Term
	void E1(); // Term
	void S1(); // Struct Point
	void A();  // Operations
	void I0(); // Optional "else"*/

	void Dec(type_of_lex type);
	void CheckID();
	void CheckOp();
	//void CheckNot();
	//void EqType();
	//void EqBool();
	//void CheckID_read();
	template<class T, class T_EL> void from_st(T& t, T_EL& x);
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
	stream.width(3);
	stream << L.get_str() << ": ";
	if (L.get_type() == LEX_ID)
		stream << "type: IDENT";
	else if (L.get_type() == LEX_NUM)
		stream << "type: NUMBER";
	else if (L.get_type() == LEX_CSTR)
		stream << "type: STRING";
	else if (L.get_type() > LEX_NULL && L.get_type() < LEX_FIN)
		stream << "type: " << Scanner::TW[L.get_type()];
	else if (L.get_type() > LEX_NULL2 && L.get_type() < LEX_NUM)
		stream << "type: " << Scanner::TD[L.get_type() - LEX_NULL2];
	else stream << "type: " << L.get_type();
	stream << "          val: " << L.get_value();
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
	for (Lex l : poliz) cout << l << endl;
	cout << endl << "Structure of program is correct \n" << endl;
}

void Parser::P() {  // Program
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
		type_of_lex var_type = curr_type;
		D1();
		if (curr_type == LEX_SEMICOLON) GetL();
		else throw curr_lex;
		Dec(var_type);
		D();
	}
}

void Parser::D1() { // Description
	if (curr_type == LEX_INT || curr_type == LEX_STRING || curr_type == LEX_BOOL) {
		GetL();
		if (curr_type != LEX_ID) throw curr_lex;
		st_int.push(curr_val);
		GetL();
		V1();
		while (curr_type == LEX_COMMA) {
			GetL();
			if (curr_type != LEX_ID) throw curr_lex;
			st_int.push(curr_val);
			GetL();
			V1();
		}
	}
	else if (curr_type == LEX_STRUCT) {
		GetL();
		if (curr_type != LEX_ID) throw curr_lex;
		GetL();
		if (curr_type != LEX_OBRACE) throw curr_lex;
		GetL();
		D();
		if (curr_type != LEX_CBRACE) throw curr_lex;
		GetL();
	}
	else throw my_exception(ERR_STD, "Description Error.");
}

void Parser::V1() { // Variable Initialization
	if (curr_type == LEX_EQ) {
		GetL();
		E();
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
		curr_type == LEX_FALSE) {

		O1();
		O();
	}
}

void Parser::O1() { // Operator
	if (curr_type == LEX_IF) {
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		GetL();
		E0();
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
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
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
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

		E();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
	}
	else throw curr_lex;
}

void Parser::E0() { // Optional expression
	if (curr_type == LEX_OBRACKET || curr_type == LEX_ID || curr_type == LEX_NUM ||
		curr_type == LEX_CSTR || curr_type == LEX_TRUE || curr_type == LEX_FALSE ||
		curr_type == LEX_PLUS || curr_type == LEX_MINUS) {

		E();
	}
}

void Parser::E() {  // Expression EQ
	E1();
	curr_str = curr_lex.get_str();
	while (curr_type == LEX_EQ) {
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
		GetL();
		S1();
	}
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
		//CheckNot();
	}
	else if (curr_type == LEX_OBRACKET) {
		GetL();
		E();
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
	}
	else {
		C0();
		st_lex.push(LEX_INT);
	}
}

void Parser::S1() {  // Struct Point
	if (curr_type == LEX_POINT) {
		GetL();
		if (curr_type != LEX_ID) throw curr_lex;
		GetL();
		S1();
	}
}

void Parser::Dec(type_of_lex type) {
	int i;
	while (!st_int.empty()) {
		from_st(st_int, i);
		if (TID[i].get_declare()) throw my_exception(ERR_TWDEF, TID[i].get_name());
		else {
			TID[i].put_declare();
			TID[i].put_type(type);
		}
	}
}

void Parser::CheckID() {
	if (TID[curr_val].get_declare())
		st_lex.push(TID[curr_val].get_type());
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
	}
	if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_STAR || op == LEX_SLASH) {
		if (t1 == LEX_INT) {
			t = LEX_INT; r = LEX_INT;
		}
		else if (t1 == LEX_STRING) {
			t = LEX_INT;
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

template<class T, class T_EL> 
void Parser::from_st(T& t, T_EL& x) {
	x = t.top();
	t.pop();
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
	case ERR_TWDEF:
		cout << "Error. Variable " << exc.text << " is defined twice\n";
		break;
	case ERR_UNDEF:
		cout << "Error. Variable " << exc.text << " is not defined\n";
		break;
	case ERR_TYPE:
		cout << "Error. " << exc.strC << ": Invalid types.\n";
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