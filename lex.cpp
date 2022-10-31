#include <cctype>
#include <map>
#include "lex.h"

using std::map;
using namespace std;
	
string lexeme;

map<Token,string> tokenPrint = {
		{PROGRAM, "PROGRAM"},
		{PRINT, "PRINT"},
		{INT, "INT"},
		{ END, "END" },
		{ FLOAT, "FLOAT"},
		{ BOOL, "BOOL" },
		
		{ ELSE, "ELSE" },
		{ IF, "IF" },
		
		{THEN, "THEN"},
		{ TRUE, "TRUE" },
		{ FALSE, "FALSE" },
		
		{ IDENT, "IDENT" },

		{ ICONST, "ICONST" },
		{ RCONST, "RCONST" },
		{ SCONST, "SCONST" },
		{ BCONST, "BCONST" },
		
		{ PLUS, "PLUS" },
		{ MINUS, "MINUS" },
		{ MULT, "MULT" },
		{ DIV, "DIV" },
		{ ASSOP, "ASSOP" },
		{ EQUAL, "EQUAL" },
		{ GTHAN, "GTHAN" },
		{ LTHAN, "LTHAN" },
		{ AND, "AND" },
		{ OR, "OR" },
		{ NOT, "NOT" },
            
		{ COMMA, "COMMA" },
		{ LPAREN, "LPAREN" },
		{ RPAREN, "RPAREN" },
		
		
		{ SEMICOL, "SEMICOL" },
		
		{ ERR, "ERR" },

		{ DONE, "DONE" },
    };
    map<string, Token> kwmap = {
		{ "PROGRAM", PROGRAM},
		{ "PRINT", PRINT},
		{ "INT", INT},
		{ "END", END },
		{ "FLOAT", FLOAT},
		{ "ELSE", ELSE },
		{ "IF", IF },
		{ "THEN", THEN},
		{ "TRUE", TRUE},
		{ "FALSE", FALSE},
		{ "BOOL", BOOL }
    };

ostream& operator<<(ostream& out, const LexItem& tok) {
    Token tk = tok.GetToken();   
    out << tokenPrint[tk];
    if(tk == IDENT || tk == ICONST || tk == RCONST || tk == BCONST || tk == SCONST){
        out << "(" << lexeme << ")";
    }		
    else if(tk == ERR){
        out << "Error in line " << tok.GetLinenum() + 1 << " (" << tok.GetLexeme() << ")";
    }
    else{
        out << tokenPrint[tk];
    }
    return out;
}
LexItem id_or_kw(const string& lexeme, int linenum){
    Token tk = IDENT;
    
    auto kIt = kwmap.find(lexeme);
    if(kIt != kwmap.end()){
        tk = kIt->second;
    }
    return LexItem(tk, lexeme, linenum);
}
LexItem getNextToken(istream& in, int& linenum){

   enum TokState { START, INID, INSTRING, ININT, INREAL, INCOMMENT } 
    lexstate = START;
	char ch, nextch, nextchar;
	int strtype = 0;

    while(in.get(ch)) {
		switch( lexstate ) {
		case START:
			if( ch == '\n' )
				linenum++;
                
			if( isspace(ch) )
				continue;

			lexeme = ch;

			if( isalpha(ch) ) {
				lexeme = toupper(ch);
				lexstate = INID;
			}
			else if( ch == '"' ) {
				lexstate = INSTRING;
				
			}
			else if( ch == '\'' ) {
				strtype = 1;
				lexstate = INSTRING;
				
			}
			else if( isdigit(ch) ) {
				lexstate = ININT;
			}
			else if( ch == '!' ) {
				lexstate = INCOMMENT;
            }
                else{
                    Token tk = ERR;
                
				switch( ch ) {
				case '+':
					tk = PLUS;
                    break;  
					
				case '-':
					tk = MINUS;
                    break; 
					
				case '*':
					tk = MULT;
					break;
				case '/':
					tk = DIV;
					break;
				case '=':
					nextchar = in.peek();
					if(nextchar == '='){
						in.get(ch);
						tk = EQUAL;
						break;
					}
					tk = ASSOP;
					break;
				case '(':
					tk = LPAREN;
					break;
				case ')':
					tk = RPAREN;
					break;
				case ',':
					tk = COMMA;
					break;
				case '<':
					tk = LTHAN;
					break;
				
				case '.':
					nextch = in.peek();
					if(isdigit(nextch)){
						lexstate = INREAL;
						continue;
					}
					else {
						lexeme += nextch;
						return LexItem(ERR, lexeme, linenum);
                    }
					
                }
                return LexItem(tk, lexeme, linenum);
          
                }
                break;
		case INID:
			if( isalpha(ch) || isdigit(ch) ) {
				ch = toupper(ch);
				lexeme += ch;
			}
			else {
				in.putback(ch);
				return id_or_kw(lexeme, linenum);
			}
			break;

		case INSTRING:
                          
			if( ch == '\n' ) {
				return LexItem(ERR, lexeme, linenum);
			}
			lexeme += ch;
			if( ch == '"' && strtype == 0) {
				lexeme = lexeme.substr(1, lexeme.length()-2);
				return LexItem(SCONST, lexeme, linenum);
			}
			else if (ch == '\'' && strtype == 1){
				lexeme = lexeme.substr(1, lexeme.length()-2);
				return LexItem(SCONST, lexeme, linenum);
			}
			
			break;

		case ININT:
			if( isdigit(ch) ) {
				lexeme += ch;
			}
			else if(ch == '.') {
				lexstate = INREAL;
				in.putback(ch);
				
			}
			else {
				in.putback(ch);
				return LexItem(ICONST, lexeme, linenum);
			}
			break;
		
		case INREAL:
			if( ch == '.' && isdigit(in.peek()) ) {
				lexeme += ch;
				
			}
			else if(isdigit(ch)){
				lexeme += ch;
			}
			else if(ch == '.' && !isdigit(in.peek())){
				return LexItem(ERR, lexeme, linenum);
			}
			else {
				in.putback(ch);
				return LexItem(RCONST, lexeme, linenum);
			}
			break;
		
					
		case INCOMMENT:
			if( ch == '\n' ) {
                linenum++;
				lexstate = START;
			}
			break;
		}

	}

	if( in.eof() ){
		return LexItem(DONE, "", linenum);
    }
    return LexItem();
}
