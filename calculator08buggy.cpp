
/*
	calculator08buggy.cpp

	Helpful comments removed.

	We have inserted 3 bugs that the compiler will catch and 3 that it won't.
*/

//NOTE: excersizes 1 and 2 have been added but during implementation some code became more buggy

/*
 *  UCF COP3330 Fall 2021 Assignment 6 Solution
 *  Copyright 2021 Ivan Pavlov
 */

#include "std_lib_facilities.h"


struct Token {		//defines a token
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, string s): kind(ch), name(s){}
	Token(char ch, double val) :kind(ch), value(val) { }
};

class Token_stream {
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { }

	Token get();
	void unget(Token t) { buffer = t; full = true; }

	void ignore(char);
};

double redo(string name);

const char let = 'L';	//predefined tokens
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';

Token Token_stream::get()
{
	if (full) { full = false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '=':
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{	cin.unget();
	double val;
	cin >> val;
	return Token(number, val);
	}
	default:
		if (isalpha(ch)) { 		//if the character is a letter add it to a string, possibly used to build variable names
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) {	//while im getting characters and its either a character or digit keep adding ex1 solved!
				s += ch;}
				cin.unget();
			if (s == "let") {		//s cant possible equal more than one chaacter currently, fixed by adding s+=ch on 3rd line before this one
				return Token(let);
			}
			if (s == "quit") {		//quits the program now
				return Token(quit/*name*/);	//should probably return token quit not name

			}
			return Token(name, s);
		}
		error("Bad token");		//when something like #$ is entered, or when _ is entered
	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch;
	while (cin >> ch)
		if (ch == c) return;
}

struct Variable {
	string name;
	double value;
	Variable(string n, double v) :name(n), value(v) { }
};

vector<Variable> names;

double get_value(string s)		//findsthe value of the variable we are asking for, that is previously entered
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return names[i].value;
	error("get: undefined name ", s);
}

void set_value(string s, double d)		//sets value for a variable, function never gets called
{
	for (int i = 0; i <= names.size(); ++i)
		if (names[i].name == s) {
			names[i].value = d;
			return;
		}
	error("set: undefined name ", s);
}

bool is_declared(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

Token_stream ts;

double expression();

double primary()		//primary retuns a number of a variable 
{
//	cout<<"prim0 ";
	Token t = ts.get();
	switch (t.kind) {		// t.kind is a character char ch
	case '(':
	{	double d = expression();
	t = ts.get();
	if (t.kind != ')') error("')' expected");		//changed ')'
	}
	case '-':
		return -primary();		//turns the term to the right of th '-' negative and retuens it,, correct
	case number:
		return t.value;
	case name:			//this case if equation has a variable		//step 4,if declared and= val is updated
		if(is_declared(t.name) && ts.get().kind == '='){
			return redo(t.name);
		}
		else return get_value(t.name);
	default:
		if(t.value==NULL)error("primary expected");		//error gets thrown when numbers are added?
	}
}

double term()
{
//	cout<<"term0 ";
	double left = primary();  		//step 3 of getting a var
	
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();		//maybe d = /
		if (d == 0) error("divide by zero");
		left /= d;
		break;
		}
		
		default:
			ts.unget(t);
			return left;
		}
	}
}

double expression()		
{
//	cout<<"exp0 ";
	double left = term();		//step 2 of gettigna variable
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		
		default:
			ts.unget(t);
			return left;
		}
	}
}


double redo(string name){
//	cout<<"redo ";
	double left;
	double ter = term();
	set_value(name,ter);
	left = get_value(name);
	return left;
		
}

double declaration()		//ex 1 add underscores to variable names
{
//	cout<<"declare0 ";
	Token t = ts.get();
	if (t.kind != 'a') error("name expected in declaration");
	string name = t.name;
	if (is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", name);
	double d = expression();
	names.push_back(Variable(name, d));
	return d;
} 

double statement()
{
//	cout<<"state0 ";
	Token t = ts.get();
//	cout<<" t val="<<t.value<<endl;
	string name1 = t.name;
//	Token t2 = ts.get();		//if i initialize t2 here it breaks declaration
	switch (t.kind) {
	case let:		//if it starts with let it declares a variable;
		return declaration();
	default:
		ts.unget(t);
		return expression();		//step 1 of getting a var
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()		//sets up formatting
{
	while (true) try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == quit) return;
		ts.unget(t);
		cout << result << statement() << endl;
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()//main function,runs calculate

try {
	calculate();
	return 0;
}
catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';');
	return 1;
}
catch (...) {
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';');
	return 2;
}
