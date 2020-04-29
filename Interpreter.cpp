#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
using namespace std;


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
	ERR_STR
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

// ================================ IDENT ================================ //
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

// ================================ ÒÀÁËÈÖÛ ================================ //
vector<string> TS;
vector<Ident> TID;
vector<Lex> TT;

// ================================ SCANNER ================================ //
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

vector<string> Scanner::TW = { "null", "bool", "int", "string", "and", "or", "begin", "do",
							   "end", "true", "false", "if", "else", "for", "while", "break",
							   "goto", "not", "program", "read", "write", "then", "var", "struct" };
vector<string> Scanner::TD = { "null", ";", ",", ":", ":=", "(", ")", "=", "<", ">",
							   "+", "-", "*", "/", "<=", ">=", "!=", "==", "{", "}", ".", "\"" };

// ========================================================================== //

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
		cout << "Error. Closing quote of string was not found.";
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
	Scanner scanner("TextProg1.txt");
	Lex lex;

	cout << "// ========= Interpreter starts work! ========= //\n";

	try {
		while ((lex = scanner.GetLex()).get_type() != LEX_FIN) {
			PutLex(lex);
		}
	}
	catch (my_exception exc) {
		PrintError(exc); 
		return 1;
	}
	catch (char c) {
		cout << "Error: character " << c << '\n';
		return 1;
	}

	PrintLex();
	PrintIdent();
	PrintString();

	cout << "\n// =========== Interpreter has done! ========== //\n";
}


// Ïðèâåò