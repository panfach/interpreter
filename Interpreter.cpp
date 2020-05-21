#define _CRT_SECURE_NO_WARNINGS
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
	POLIZ_FGO,     //    
	LEX_ENDWRITE,   //
	LEX_STRINDEX   //
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
	ERR_LABEL,
	ERR_ASSIGN
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

	void put_type(type_of_lex t) { t_lex = t; }

	/* friend ostream& operator << (ostream& stream, Lex L) */
};

// =================================== IDENT =================================== //
class Ident {
	string name;
	bool declare;
	type_of_lex type;
	bool assign, structDescription, structField;
	int valueTID;
	int structValue;
	int labelAddress;

public:
	int intValue;
	string stringValue;
	bool boolValue;

	Ident() { declare = false; type = LEX_NULL; assign = false; structField = false; }
	Ident(const string n) { name = n; declare = false; type = LEX_NULL; assign = false; structField = false; }
	Ident(type_of_lex t, int val) { type = LEX_INT, intValue = val; assign = true; structField = false; }
	Ident(type_of_lex t, string val) { type = LEX_STRING, stringValue = val; assign = true; structField = false; }
	Ident(type_of_lex t, bool val) { type = LEX_BOOL, boolValue = val; assign = true; structField = false; }
	bool operator == (const string& s) const { return name == s; }

	string get_name() { return name; }
	bool get_declare() { return declare; }
	type_of_lex get_type() { return type; }
	bool get_assign() { return assign; }
	bool get_structDescription() { return structDescription; }
	//int get_value() { return value; }
	int get_valueTID() { return valueTID; }
	int get_structValue() { return structValue; }
	int get_labelAddress() { return labelAddress; }
	bool get_structField() { return structField; }

	void put_declare() { declare = true; }
	void put_type(type_of_lex t) { type = t; }
	void put_assign() { assign = true; }
	void put_structDescription() { structDescription = true; }
	//void put_value(int v) { value = v; }
	void put_valueTID(int v) { valueTID = v; }
	void put_structValue(int v) { structValue = v; }
	void put_labelAddress(int v) { labelAddress = v; }
	void put_structField() { structField = true; }
};

struct Structure {
	string name;
	int value;
	vector<Ident> var;
	Structure(string n = "0", int v = 0) : name(n), value(v) {}
};

// ================================ TABLES === // ========================== //
vector<string> TS;                             // Table of Strings           //
vector<Ident> TID;                             // Table of Identificators    //
vector<Lex> TT;                                // Table of Lex               //
vector<Ident> TL;                              // Table of Labels            //
vector<Ident> TLN;                             // Table of Label Names       //
vector<Structure> TStruct;                     // Table of Structures        //
vector<Structure> TSV;                         // Table of Structure Variables //

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
	stack<int>st_struct;
	stack<Lex>st_op;
	stack<int>st_breaks;

	bool structDescrAvailable, structDescribing, descrSection/*!*/, firstIsVariable;

	void P();  // Program 
	void D();  // Descriptions
	void D1(); // Description
	void V1(); // Variable Initialization
	//void C0(); // Integer const
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
	void CheckID_read();
	unsigned int PutStruct();
	unsigned int PutStructVariable();
	unsigned int PutStructField(int index, Ident& id);
	template<class T, class T_EL> void from_st(T& t, T_EL& x);
	template<class T> void st_to_poliz(T& t);
	void GetL() {
		curr_lex = scanner.GetLex();
		curr_type = curr_lex.get_type();
		curr_val = curr_lex.get_value();
	}

public:
	static int counter;
	vector<Lex> poliz;
	Parser(const char* program) : scanner(program), loop_counter(0) { }
	void Analyze();
};

int Parser::counter = 0;

// =========================== EXECUTER AND INTERPRETER ============================== //
class Executer {
public:
	void Execute(vector <Lex>& poliz);
	template<class T, class T_EL> void from_st(T& t, T_EL& x);
};

class Interpreter {
	Parser parser;
	Executer executer;
public:
	Interpreter(const char* program) : parser(program) { };
	void Interpret();
};

// =========================== DESCRIPTIONS I ========================= //
ostream& operator << (ostream& stream, Lex& L) {
	string lexName;
	stream.width(3); stream << Parser::counter << ": ";
	stream << "["; stream.width(3); stream << L.get_str() << "] val:";
	stream.width(8); stream << L.get_value() << " type: ";

	switch (L.get_type()) {
	case LEX_ID:
		stream << TID[L.get_value()].get_name() << " (ID) ";
		break;
	case LEX_NUM:
		stream << L.get_value() << " (INT) ";
		break;
	case LEX_CSTR:
		stream << TS[L.get_value()] << " (STR) ";
		break;
	case POLIZ_LABEL:
		stream << "LABEL ";
		break;
	case POLIZ_ADDRESS:
		stream << "ADDRESS ";
		break;
	case POLIZ_GO:
		stream << "! ";
		break;
	case POLIZ_FGO:
		stream << "!F ";
		break;
	case LEX_ENDWRITE:
		stream << "endwrite ";
		break;
	case LEX_STRINDEX:
		stream << "STRUCT_INDEX ";
	}
	if (L.get_type() > LEX_NULL && L.get_type() < LEX_FIN)
		stream << Scanner::TW[L.get_type()];
	else if (L.get_type() > LEX_NULL2 && L.get_type() < LEX_NUM)
		stream << Scanner::TD[L.get_type() - LEX_NULL2];
	//else stream << L.get_type();
	return stream;
}

ostream& operator << (ostream& stream, Ident& id) {
	switch (id.get_type()) {
	case LEX_INT:
		stream << id.intValue;
		break;
	case LEX_STRING:
		stream << id.stringValue;
		break;
	case LEX_BOOL:
		stream << id.boolValue;
	}
	return stream;
}

int PutIdent(const string& buf) {
	vector<Ident>::iterator it;
	if ((it = find(TID.begin(), TID.end(), buf)) != TID.end())
		return it - TID.begin();
	TID.push_back(Ident(buf));
	TID[TID.size() - 1].put_valueTID(TID.size() - 1);
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
			if (inpCh == '@') throw my_exception(ERR_STR, inpCh);
			else
			if (inpCh == '"') {
				j = PutString(buf);
				return Lex(strCounter, LEX_CSTR, j);
			}
			else if (inpCh != '\n'){ buf.push_back(inpCh); }
			if (inpCh == '\n') strCounter++;
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
	if (!TLN.empty()) throw my_exception(ERR_LABEL, TLN[0].get_name()); // Проверка на несуществующие метки

	counter = 0;
	for (Lex l : poliz) { cout << l << endl; counter++; }
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
	if (!structDescribing) descrSection = true;

	if (curr_type == LEX_INT || curr_type == LEX_STRING || curr_type == LEX_BOOL || curr_type == LEX_STRUCT) {
		type_of_lex var_type = curr_type;
		D1();
		if (curr_type == LEX_SEMICOLON) GetL();
		else throw curr_lex;
		D();
	}

	if (!structDescribing) descrSection = false;
}

void Parser::D1() { // Description
	if (curr_type == LEX_INT || curr_type == LEX_STRING || curr_type == LEX_BOOL) {
		var_type = curr_type;
		if (!structDescribing) structDescrAvailable = false;
		GetL();
		if (curr_type != LEX_ID) throw curr_lex;
		//lastIdent = curr_lex;
		st_op.push(curr_lex);
		if (structDescribing) {
			st_int.push(PutStructField(curr_struct, TID[curr_val]));  
			/*for (unsigned int i = curr_struct + 1; i < TStruct.size(); i++) {
				PutStructField(i, TID[curr_val]);
			}*/
		}
		else st_int.push(curr_val);
		GetL();
		V1();
		while (curr_type == LEX_COMMA) {
			GetL();
			if (curr_type != LEX_ID) throw curr_lex;
			//lastIdent = curr_lex;
			st_op.push(curr_lex);
			if (structDescribing) {
				st_int.push(PutStructField(curr_struct, TID[curr_val]));
				/*for (unsigned int i = curr_struct + 1; i < TStruct.size(); i++) {
					PutStructField(i, TID[curr_val]);
				}*/
			}
			else st_int.push(curr_val);
			GetL();
			V1();
		}
		Dec(var_type);
	}
	else if (curr_type == LEX_STRUCT) {
		if (structDescribing) throw my_exception(ERR_STD, "Error. Invalid type for structure field.\n");
		if (!structDescrAvailable) throw my_exception(ERR_STD, "Error. Structure must be described before variables.\n");

		var_type = curr_type;
		curr_struct = -1;
		GetL();

		// Заполнение TStruct (Таблица описаний структур)
		if (curr_type != LEX_ID) throw curr_lex;
		st_int.push(curr_val);
		if (curr_struct == -1) {
			curr_struct = PutStruct();                  // Запоминание индекса таблицы структур TStruct 
			TID[curr_val].put_structValue(curr_struct);
		}
		else {
			TID[curr_val].put_structValue(PutStruct());
		}
		TID[curr_val].put_structDescription();
		GetL();

		Dec(var_type);
		if (curr_type != LEX_OBRACE) throw curr_lex;
		GetL();
		structDescribing = true;
		D();
		structDescribing = false;
		if (curr_type != LEX_CBRACE) throw curr_lex;
		GetL();
	}
	else throw my_exception(ERR_STD, "Description Error.");
	while (curr_type == LEX_ID) { // Заполнение TSV (Таблица структурных переменных)
		st_int.push(curr_val);
		PutStructVariable();
		GetL();
		if (curr_type == LEX_COMMA) {
			GetL();
			continue;
		}
		else break;
	}
	Dec(LEX_STRUCT);
}

void Parser::V1() { // Variable Initialization
	if (curr_type == LEX_EQ) {
		if (structDescribing) throw curr_lex;
		st_to_poliz(st_op);
		st_op.push(curr_lex);
		GetL();
		E();
		CheckInit();
		st_to_poliz(st_op);
		//TID[lastIdent.get_value()].put_assign();
	}
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
	int p1, p2, p3, p4;
	if (curr_type == LEX_IF) {
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		GetL();
		E0();
		from_st(st_lex, type);
		if (type != LEX_BOOL) throw my_exception(ERR_STD, "Error. IF: Expression must be boolean.", curr_lex.get_str());

		p2 = poliz.size();
		poliz.push_back(Lex(curr_lex.get_str()));
		poliz.push_back(Lex(curr_lex.get_str(), POLIZ_FGO));

		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
		O1();

		p3 = poliz.size();
		poliz.push_back(Lex(curr_lex.get_str()));
		poliz.push_back(Lex(curr_lex.get_str(), POLIZ_GO));

		poliz[p2] = Lex(poliz[p2].get_str(), POLIZ_LABEL, poliz.size());
		if (curr_type == LEX_ELSE) {
			GetL();
			O1();
		}
		poliz[p3] = Lex(poliz[p3].get_str(), POLIZ_LABEL, poliz.size());
	}
	else if (curr_type == LEX_FOR) {
		loop_counter++;
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		GetL();
		if (curr_type != LEX_SEMICOLON) E0();                // 1
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();

		p4 = poliz.size();

		if (curr_type != LEX_SEMICOLON) E0();               // 2
		from_st(st_lex, type);
		if (type != LEX_BOOL) throw my_exception(ERR_STD, "Error. FOR: The second expression must be boolean.", curr_lex.get_str());

		p1 = poliz.size();
		poliz.push_back(Lex(curr_lex.get_str()));
		poliz.push_back(Lex(curr_lex.get_str(), POLIZ_FGO));

		p2 = poliz.size();
		poliz.push_back(Lex(curr_lex.get_str()));
		poliz.push_back(Lex(curr_lex.get_str(), POLIZ_GO));

		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();

		p3 = poliz.size();

		if (curr_type != LEX_CBRACKET) E0();                 // 3

		poliz.push_back(Lex(curr_lex.get_str(), POLIZ_LABEL, p4));
		poliz.push_back(Lex(curr_lex.get_str(), POLIZ_GO));

		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();

		poliz[p2] = Lex(poliz[p2].get_str(), POLIZ_LABEL, poliz.size());

		O1();

		poliz.push_back(Lex(curr_lex.get_str(), POLIZ_LABEL, p3));
		poliz.push_back(Lex(curr_lex.get_str(), POLIZ_GO));

		loop_counter--;
		if (!st_breaks.empty()) {
			int i;
			from_st(st_breaks, i);
			poliz[i] = Lex(poliz[i].get_str(), POLIZ_LABEL, poliz.size());
		}

		poliz[p1] = Lex(poliz[p1].get_str(), POLIZ_LABEL, poliz.size());
	}
	else if (curr_type == LEX_WHILE) {
		loop_counter++;
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		GetL();

		p1 = poliz.size();

		E0();
		from_st(st_lex, type);
		if (type != LEX_BOOL) throw my_exception(ERR_STD, "Error. WHILE: Expression must be boolean.", curr_lex.get_str());

		p2 = poliz.size();
		poliz.push_back(Lex(curr_lex.get_str()));
		poliz.push_back(Lex(curr_lex.get_str(), POLIZ_FGO));

		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
		O1();

		poliz.push_back(Lex(curr_lex.get_str(), POLIZ_LABEL, p1));
		poliz.push_back(Lex(curr_lex.get_str(), POLIZ_GO));

		loop_counter--;
		if (!st_breaks.empty()) {
			int i;
			from_st(st_breaks, i);
			poliz[i] = Lex(poliz[i].get_str(), POLIZ_LABEL, poliz.size());
		}

		poliz[p2] = Lex(poliz[p2].get_str(), POLIZ_LABEL, poliz.size());
	}
	else if (curr_type == LEX_BREAK) {
		if (loop_counter == 0) throw my_exception(ERR_STD, "Error. break must be in the body of loop.\n", curr_lex.get_str());

		st_breaks.push(poliz.size());
		poliz.push_back(Lex(curr_lex.get_str()));
		poliz.push_back(Lex(curr_lex.get_str(), POLIZ_GO));

		GetL();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
	}
	else if (curr_type == LEX_GOTO) {
		GetL();
		if (curr_type != LEX_ID) throw curr_lex;
		int i;
		if ((i = CheckLabel()) != -1) {
			poliz.push_back(Lex(curr_lex.get_str(), POLIZ_LABEL, TL[i].get_labelAddress()));
		}
		else {
			poliz.push_back(Lex(curr_lex.get_str()));
		}
		poliz.push_back(Lex(curr_lex.get_str(), POLIZ_GO));
		GetL();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
	}
	else if (curr_type == LEX_READ) {
		st_op.push(curr_lex);
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		GetL();
		if (curr_type != LEX_ID) throw curr_lex;
		CheckID_read();
		poliz.push_back(curr_lex);
		GetL();
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
		st_to_poliz(st_op);
	}
	else if (curr_type == LEX_WRITE) {
		poliz.push_back(curr_lex);  // Начало печати
		GetL();
		if (curr_type != LEX_OBRACKET) throw curr_lex;
		GetL();
		E();
		while (curr_type == LEX_COMMA) {
			poliz.push_back(curr_lex);
			GetL();
			E();
		}
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
		if (curr_type != LEX_SEMICOLON) throw curr_lex;
		GetL();
		poliz.push_back(Lex(curr_lex.get_str(), LEX_ENDWRITE));
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
		TL[curr_lex.get_value()].put_labelAddress(poliz.size());
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
		if (descrSection) throw curr_lex;
		st_op.push(curr_lex);
		st_lex.push(curr_type);
		GetL();
		E1();
		CheckOp();
		st_to_poliz(st_op);
	}
}

void Parser::E1() { // Expression OR
	E2();
	while (curr_type == LEX_OR) {
		st_op.push(curr_lex);
		st_lex.push(curr_type);
		GetL();
		E2();
		CheckOp();
		st_to_poliz(st_op);
	}
}

void Parser::E2() {  // Expression AND
	E3();
	while (curr_type == LEX_AND) {
		st_op.push(curr_lex);
		st_lex.push(curr_type);
		GetL();
		E3();
		CheckOp();
		st_to_poliz(st_op);
	}
}

void Parser::E3() {  // Expression MORELESS
	E4();
	while (curr_type == LEX_LESS || curr_type == LEX_MORE || curr_type == LEX_LESSEQ || curr_type == LEX_MOREEQ ||
		   curr_type == LEX_EQEQ || curr_type == LEX_NEQ) {
		st_op.push(curr_lex);
		st_lex.push(curr_type);
		GetL();
		E4();
		CheckOp();
		st_to_poliz(st_op);
	}
}

void Parser::E4() {  // Expression PLUSMINUS
	E5();
	while (curr_type == LEX_PLUS || curr_type == LEX_MINUS) {
		st_op.push(curr_lex);
		st_lex.push(curr_type);
		GetL();
		E5();
		CheckOp();
		st_to_poliz(st_op);
	}
}

void Parser::E5() {  // Expression MULTDIV
	E6();
	while (curr_type == LEX_STAR || curr_type == LEX_SLASH) {
		st_op.push(curr_lex);
		st_lex.push(curr_type);
		GetL();
		E6();
		CheckOp();
		st_to_poliz(st_op);
	}
}

void Parser::E6() {  // Expression BRACKETS
	if (curr_type == LEX_ID) {
		CheckID();
		poliz.push_back(curr_lex);
		lastIdent = curr_lex;
		GetL();
		S1();
	}
	else if (firstIsVariable) throw my_exception(ERR_STD, "Error. Left operand must be variable.\n", curr_str);
	else if (curr_type == LEX_TRUE || curr_type == LEX_FALSE) {
		poliz.push_back(curr_lex);
		GetL();
		st_lex.push(LEX_BOOL);
	}
	else if (curr_type == LEX_CSTR) {
		poliz.push_back(curr_lex);
		GetL();
		st_lex.push(LEX_STRING);
	}
	else if (curr_type == LEX_NOT) {
		st_op.push(curr_lex);
		GetL();
		E6();
		CheckNot();
		st_to_poliz(st_op);
	}
	else if (curr_type == LEX_OBRACKET) {
		GetL();
		E();
		if (curr_type != LEX_CBRACKET) throw curr_lex;
		GetL();
	}
	else if (curr_type == LEX_NUM) {
		poliz.push_back(curr_lex);
		GetL();
		st_lex.push(LEX_INT);
	}
	else if (curr_type == LEX_MINUS) {
		Lex l1 = curr_lex;
		l1.put_type(LEX_UNMINUS);
		st_op.push(l1);

		GetL();
		if (curr_type == LEX_NUM) {
			poliz.push_back(curr_lex);
			GetL();
			st_lex.push(LEX_INT);
		}
		else if (curr_type == LEX_ID) {
			CheckID();
			poliz.push_back(curr_lex);
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
		st_to_poliz(st_op);
	}
	else throw curr_lex;
}

void Parser::S1() {  // Struct Point
	if (curr_type == LEX_POINT) {
		// Проверка на то, ялвяется ли переменная перед LEX_POINT структурой
		if (TID[lastIdent.get_value()].get_type() != LEX_STRUCT) throw my_exception(ERR_NOTSTRUCT, TID[lastIdent.get_value()].get_name());

		st_op.push(curr_lex);
		GetL();
		if (curr_type != LEX_ID) throw curr_lex;

		// Проверка на то, есть ли такое поле в структуре
		int structIndex = TID[lastIdent.get_value()].get_structValue(), k = 0;
		vector<Ident>::iterator it = TSV[structIndex].var.begin();
		while (1) {
			if (TID[curr_val].get_name() == it->get_name()) break;
			if (it == TSV[structIndex].var.end()) throw my_exception(ERR_STD, "Error. Incorrect field of a structure.\n");
			it++; k++;
		}
		poliz.push_back(Lex(curr_str, LEX_STRINDEX, k));
		st_lex.push(it->get_type()); // Если вызывать просто CheckID, будут проблемы
		GetL();

		st_to_poliz(st_op);
	}
	else if (TID[lastIdent.get_value()].get_type() == LEX_STRUCT) {
		st_struct.push(TSV[TID[lastIdent.get_value()].get_structValue()].value);
		st_lex.push(LEX_STRUCT);
	}
}

void Parser::Dec(type_of_lex type) {
	int i;
	while (!st_int.empty()) {
		from_st(st_int, i);
		if (structDescribing) { // Проверка переменных в структуре
			if (TStruct[curr_struct].var[i].get_declare()) throw my_exception(ERR_TWDEF, TStruct[curr_struct].var[i].get_name());
			else {
				TStruct[curr_struct].var[i].put_declare();
				TStruct[curr_struct].var[i].put_type(type);
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
			int i1, i2;
			from_st(st_struct, i1);
			from_st(st_struct, i2);
			if (i1 != i2) throw my_exception(ERR_STD, "Error. Equating of different structures.\n", curr_str);
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
			Ident ident(TID[curr_val].get_name());
			ident.put_labelAddress(poliz.size());
			TLN.push_back(ident); 
			return -1;
		}
		if (it->get_name() == TID[curr_val].get_name()) return it - TL.begin();
		it++;
	}
}

void Parser::CheckTLN() {
	vector<Ident>::iterator it = TLN.begin();
	while (1) {
		if (it == TLN.end())
			break;
		if (it->get_name() == TL[curr_val].get_name()) {
			poliz[it->get_labelAddress()] = Lex(poliz[it->get_labelAddress()].get_str(), POLIZ_LABEL, poliz.size());
			it = TLN.erase(it);
		}
		else it++;
	}
}

void Parser::CheckNot() {
	if (st_lex.top() != LEX_BOOL) throw my_exception(ERR_TYPE, curr_str);
	// poliz.push_back(Lex(LEX_NOT)); //////////////////////////////////////////////////////////////////////////
}

void Parser::CheckID_read() {
	if (!TID[curr_val].get_declare()) throw my_exception(ERR_UNDEF, TID[curr_val].get_name());
}

unsigned int Parser::PutStruct() {
	TStruct.push_back(Structure(TID[curr_val].get_name()));
	return TStruct.size() - 1;
}

unsigned int Parser::PutStructVariable() {
	TSV.push_back(TStruct[curr_struct]);
	TID[curr_val].put_type(LEX_STRUCT);
	int curr_index = TSV.size() - 1, k = 0;
	TSV[curr_index].value = curr_struct;
	vector<Ident>::iterator it = TSV[curr_index].var.begin();
	while (it != TSV[curr_index].var.end()) {
		it->put_structValue(curr_index);
		it->put_structField();
		it->put_valueTID(k);
		it++; k++;
	}
	TID[curr_val].put_structValue(curr_index);
	return curr_index;
}

unsigned int Parser::PutStructField(int index, Ident& id) {
	vector<Ident>::iterator it;
	const string buf = id.get_name();
	if ((it = find(TStruct[index].var.begin(), TStruct[index].var.end(), buf)) != TStruct[index].var.end())
		return it - TStruct[index].var.begin();
	TStruct[index].var.push_back(id);
	return TStruct[index].var.size() - 1;
}

template<class T, class T_EL> 
void Parser::from_st(T& t, T_EL& x) {
	x = t.top();
	t.pop();
}

template<class T>
void Parser::st_to_poliz(T& t) {
	poliz.push_back(t.top());
	t.pop();
}

// =============================== EXECUTER AND INTERPRETER =============================== //
void Executer::Execute(vector<Lex>& poliz) {
	Lex lex;
	stack<Ident> args;
	Ident i, j;
	int index = 0, size = poliz.size();
	bool writingState = false, readingState = false;

	while (index < size) {
		lex = poliz[index];
		switch (lex.get_type()) {
		case LEX_TRUE:
			args.push(Ident(LEX_BOOL, true));
			args.top().put_assign(); // ????????????????????????????????????????????????
			break;
		case LEX_FALSE:
			args.push(Ident(LEX_BOOL, false));
			args.top().put_assign();
			break;
		case LEX_NUM:
		case POLIZ_LABEL:
			args.push(Ident(LEX_INT, lex.get_value()));
			args.top().put_assign();
			break;
		case LEX_CSTR:
			args.push(Ident(LEX_STRING, TS[lex.get_value()]));
			args.top().put_assign();
			break;
		case LEX_ID:
			args.push(TID[lex.get_value()]);
			break;
		case LEX_STRINDEX:
			args.push(Ident(LEX_INT, lex.get_value()));
			break;


		case LEX_NOT:
			from_st(args, i);
			if (!i.get_assign()) throw my_exception(ERR_ASSIGN, i.get_name());
			i.boolValue = !i.boolValue;
			args.push(i);
			break;
		case LEX_OR:
			from_st(args, i);
			from_st(args, j);
			if (!i.get_assign()) throw my_exception(ERR_ASSIGN, i.get_name());
			if (!j.get_assign()) throw my_exception(ERR_ASSIGN, j.get_name());
			args.push(Ident(LEX_BOOL, i.boolValue || j.boolValue));
			break;
		case LEX_AND:
			if (!i.get_assign()) throw my_exception(ERR_ASSIGN, i.get_name());
			if (!j.get_assign()) throw my_exception(ERR_ASSIGN, j.get_name());
			from_st(args, i);
			from_st(args, j);
			args.push(Ident(LEX_BOOL, i.boolValue && j.boolValue));
			break;
		case POLIZ_GO:
			from_st(args, i);
			index = i.intValue - 1;
			break;
		case POLIZ_FGO:
			from_st(args, i);
			from_st(args, j);
			if (!j.boolValue) index = i.intValue - 1;
			break;
		case LEX_WRITE:
			writingState = true;
			break;
		case LEX_ENDWRITE:
			from_st(args, i);
			cout << i;
			writingState = false;
			break;
		case LEX_COMMA:
			from_st(args, i);
			cout << i;
			break;
		case LEX_READ:
			from_st(args, i);
			//Ident& readId = TID[poliz[index - 1].get_value()];
			TID[poliz[index - 1].get_value()].put_assign();
			if (i.get_type() == LEX_INT) {
				cin >> TID[poliz[index - 1].get_value()].intValue;
				break;
			}
			else if (i.get_type() == LEX_STRING) {
				cin >> TID[poliz[index - 1].get_value()].stringValue;
				break;
			}
			else if (i.get_type() == LEX_BOOL) {
				string readStr;
				cin >> readStr;
				TID[poliz[index - 1].get_value()].boolValue = (readStr == "0") ? false : true;
				break;
			}
		case LEX_EQ:
			from_st(args, i);
			from_st(args, j);
			if (!i.get_assign() && j.get_type() != LEX_STRUCT) throw my_exception(ERR_ASSIGN, i.get_name());
			if (!j.get_structField() && j.get_type() != LEX_STRUCT) {
				if (j.get_type() == LEX_INT) TID[j.get_valueTID()].intValue = i.intValue;
				else if (j.get_type() == LEX_STRING) TID[j.get_valueTID()].stringValue = i.stringValue;
				else if (j.get_type() == LEX_BOOL) TID[j.get_valueTID()].boolValue = i.boolValue;
			}
			else if (j.get_structField()) {
				if (j.get_type() == LEX_INT) TSV[j.get_structValue()].var[j.get_valueTID()].intValue = i.intValue;
				else if (j.get_type() == LEX_STRING) TSV[j.get_structValue()].var[j.get_valueTID()].stringValue = i.stringValue;
				else if (j.get_type() == LEX_BOOL) TSV[j.get_structValue()].var[j.get_valueTID()].boolValue = i.boolValue;
			}
			else if (j.get_type() == LEX_STRUCT) {
				int k = 0;
				vector<Ident>::iterator it1 = TSV[j.get_structValue()].var.begin();
				vector<Ident>::iterator it2 = TSV[i.get_structValue()].var.begin();
				while (1) {
					if (it1 == TSV[j.get_structValue()].var.end() || it2 == TSV[i.get_structValue()].var.end()) break;
					if (it1->get_type() == LEX_INT) it1->intValue = it2->intValue;
					else if (it1->get_type() == LEX_STRING) it1->stringValue = it2->stringValue;
					else if (it1->get_type() == LEX_BOOL) it1->boolValue = it2->boolValue;
					it1++; it2++;
				}
			}

			if (!j.get_structField()) TID[j.get_valueTID()].put_assign();
			else TSV[j.get_structValue()].var[j.get_valueTID()].put_assign();
			break;
		case LEX_PLUS:
			from_st(args, i);
			from_st(args, j);
			if (!i.get_assign()) throw my_exception(ERR_ASSIGN, i.get_name());
			if (!j.get_assign()) throw my_exception(ERR_ASSIGN, j.get_name());
			if (i.get_type() == LEX_INT) args.push(Ident(LEX_INT, i.intValue + j.intValue));
			if (i.get_type() == LEX_STRING) args.push(Ident(LEX_STRING, j.stringValue + i.stringValue));
			break;
		case LEX_MINUS:
			from_st(args, i);
			from_st(args, j);
			if (!i.get_assign()) throw my_exception(ERR_ASSIGN, i.get_name());
			if (!j.get_assign()) throw my_exception(ERR_ASSIGN, j.get_name());
			args.push(Ident(LEX_INT, j.intValue - i.intValue));
			break;
		case LEX_STAR:
			from_st(args, i);
			from_st(args, j);
			if (!i.get_assign()) throw my_exception(ERR_ASSIGN, i.get_name());
			if (!j.get_assign()) throw my_exception(ERR_ASSIGN, j.get_name());
			args.push(Ident(LEX_INT, j.intValue * i.intValue));
			break;
		case LEX_SLASH:
			from_st(args, i);
			from_st(args, j);
			if (!i.get_assign()) throw my_exception(ERR_ASSIGN, i.get_name());
			if (!j.get_assign()) throw my_exception(ERR_ASSIGN, j.get_name());
			args.push(Ident(LEX_INT, j.intValue / i.intValue));
			break;
		case LEX_EQEQ:
			from_st(args, i); /////////////////////////////////////////////// можно ли сравнивать строки?
			from_st(args, j);
			if (!i.get_assign()) throw my_exception(ERR_ASSIGN, i.get_name());
			if (!j.get_assign()) throw my_exception(ERR_ASSIGN, j.get_name());
			if (i.get_type() == LEX_INT) args.push(Ident(LEX_BOOL, i.intValue == j.intValue));
			if (i.get_type() == LEX_STRING) args.push(Ident(LEX_STRING, i.stringValue == j.stringValue));
			break;
		case LEX_POINT:
			from_st(args, i);
			from_st(args, j);
			args.push(TSV[j.get_structValue()].var[i.intValue]);
			break;
		case LEX_UNMINUS:
			from_st(args, i);
		case LEX_MOREEQ:
		case LEX_LESSEQ:
		case LEX_MORE:
		case LEX_LESS:
			from_st(args, i); 
			from_st(args, j);
			if (!i.get_assign()) throw my_exception(ERR_ASSIGN, i.get_name());
			if (!j.get_assign()) throw my_exception(ERR_ASSIGN, j.get_name());
			if (lex.get_type() == LEX_MOREEQ) args.push(Ident(LEX_BOOL, j.intValue >= i.intValue));
			if (lex.get_type() == LEX_LESSEQ) args.push(Ident(LEX_BOOL, j.intValue <= i.intValue));
			if (lex.get_type() == LEX_MORE) args.push(Ident(LEX_BOOL, j.intValue > i.intValue));
			if (lex.get_type() == LEX_LESS) args.push(Ident(LEX_BOOL, j.intValue < i.intValue));
			break;
		default:
			throw my_exception(ERR_STD, "POLIZ: error ???\n");
		}
		index++;
	}
}

void Interpreter::Interpret() {
	parser.Analyze();
	executer.Execute(parser.poliz);
}

template<class T, class T_EL>
void Executer::from_st(T& t, T_EL& x) {
	x = t.top();
	t.pop();
}

// ======================================================================================== //
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
		break;
	case ERR_ASSIGN:
		cout << "Error. Variable " << exc.text << " has not assigned.\n";
	}
	cout << "\n// ================= ERROR!!! ================ //\n";

}

void PrintIdent() {
	int i = 0;
	vector<Ident>::iterator it = TID.begin();
	cout << "\nIdentifiers: \n";
	while (it != TID.end()) {
		cout.width(3);
		cout << i << ": ";
		cout << it->get_name() << endl;
		it++; i++;
	}
	cout << endl;
}

void PrintString() {
	int i = 0;
	vector<string>::iterator it = TS.begin();
	cout << "\nStrings: \n";
	while (it != TS.end()) {
		cout.width(3);
		cout << i << ": ";
		cout << *it << endl;
		it++; i++;
	}
	cout << endl;
}

int main(int argc, char* argv[]) {

	cout << "// ========= Interpreter starts work! ========= //\n";

	try {
		Interpreter I("TextProg1.txt");
		I.Interpret();

		//PrintIdent();
		//PrintString();

		cout << "\n// =========== Interpreter has done! ========== //\n";
		return 0;
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
	return 1;
}