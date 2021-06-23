//*****************************************************************************
// purpose: CSE 4713 / 6713 Assignment 1: Lexical Analyzer
// created: 9-3-2019
//  author: 
//*****************************************************************************
#include "lexer.h"
#include <ctype.h>
#include <assert.h>
#include <string.h>

//*****************************************************************************
// Do the lexical parsing
char lexeme[MAX_LEXEME_LEN];  // Character buffer for lexeme

// short alias to compare two strings
#define equal(a,b) strcmp(a,b) == 0

// check if the lexeme is a word reserved
int is_reserved() {
  // keywords
  if (equal(lexeme, "if"))  return TOK_IF;
  if (equal(lexeme, "else")) return TOK_ELSE;
  if (equal(lexeme, "for"))  return TOK_FOR;
  if (equal(lexeme, "while"))  return TOK_WHILE;
  if (equal(lexeme, "print"))  return TOK_PRINT;
  if (equal(lexeme, "return"))  return TOK_RETURN;
  if (equal(lexeme, "continue"))  return TOK_CONTINUE;
  if (equal(lexeme, "break"))  return TOK_BREAK;
  if (equal(lexeme, "debug"))  return TOK_DEBUG;
  if (equal(lexeme, "read"))  return TOK_READ;
  if (equal(lexeme, "let"))  return TOK_LET;
  // datatype specifiers
  if (equal(lexeme, "int"))  return TOK_INT;
  if (equal(lexeme, "float"))  return TOK_FLOAT;
  if (equal(lexeme, "string"))  return TOK_STRING;
  // logical operators
  if (equal(lexeme, "and"))  return TOK_AND;
  if (equal(lexeme, "or"))  return TOK_OR;
  if (equal(lexeme, "not"))  return TOK_NOT;
  if (equal(lexeme, "len"))  return TOK_LENGTH;
  return 0;
}

// check if the current char is an operator
int is_operator(char c) {
  char nxt;
  switch(c) {
    case '+': return TOK_PLUS;
    case '-': return TOK_MINUS;
    case '*': return TOK_MULTIPLY;
    case '/': return TOK_DIVIDE;
    case ':': return TOK_ASSIGN;
    case '=': return TOK_EQUALTO;
    case '<': return TOK_LESSTHAN;
    case '>': return TOK_GREATERTHAN;
    default: return 0;
  }
}

// check if the current char is a punctuation
int is_punctuation(char c) {
  switch (c) {
  	case ';': return TOK_SEMICOLON;
    case '(': return TOK_OPENPAREN;
    case ')': return TOK_CLOSEPAREN;
    case '[': return TOK_OPENBRACKET;
    case ']': return TOK_CLOSEBRACKET;
    case '{': return TOK_OPENBRACE;
    case '}': return TOK_CLOSEBRACE;
    case ',': return TOK_COMMA;
    default : return 0;
  }
}


int yylex()
{
  char c;
  int token = 0;
  // initialize current lexeme
  yyleng = 0;  
  yytext = lexeme;
  
  while ((scanf("%c", &c) != EOF)) {  //process lexeme
    
    if (isblank(c) || c == '\n')
      continue;
    
    // base case: if the lexeme don't start with some of these thus is unknown
    if (!isalpha(c) && !isdigit(c) && !is_operator(c) && !is_punctuation(c) && c != '"' && c != '.')
      return TOK_UNKNOWN;
    
    // test if is a punctuation lexeme
    token = is_punctuation(c);
    
    if (token) {  // if is then return it 
      lexeme[yyleng++] = c;
      lexeme[yyleng] = '\0';
      return token;
    }
    
    // test if is an operator lexeme
    token = is_operator(c);

    if (token) {  // if is then check if it need another character
      lexeme[yyleng++] = c;
      if (token == TOK_ASSIGN) {
        c = getchar();
        if (c != '=')  // if don't match then is unknown
          token = TOK_UNKNOWN;
        else 
          lexeme[yyleng++] = c;
      } 
      else if (token == TOK_EQUALTO) {
        c = getchar();
        if (c != '=')
          token = TOK_UNKNOWN;
        else 
          lexeme[yyleng++] = c;
      }
      else if (token == TOK_LESSTHAN) {  // it may need another character
        c = getchar();
        if (c == '>') {  // if match we update the token
          token = TOK_NOTEQUALTO;
          lexeme[yyleng++] = c;
        }
        else 
          ungetc(c, yyin);  // else return the character back to the standard input flow (stdin)
      }
      lexeme[yyleng] = '\0';
      return token;
    }
    
    // test if is a integer literal or floating point literal lexeme
    if (isdigit(c)) {
      lexeme[yyleng++] = c;
      
      while ((c = getchar()) && isdigit(c)) // read characters until no more digits are found
        lexeme[yyleng++] = c;
      
      if (c != '.') {  // if the next char is not a dot, then is an integer literal
        ungetc(c, yyin);
        token = TOK_INTLIT;
      }
      else {  // else is an floating point literal
        lexeme[yyleng++] = c;
        
        while ((c = getchar()) && isdigit(c))  // same process
          lexeme[yyleng++] = c;
        
        ungetc(c, yyin);  // return the next character back to stdin, because we don't need it here
        token = TOK_FLOATLIT;
      }
      lexeme[yyleng] = '\0';
      return token;
    }
    
    // support for floating point literal lexemes who starts with a dot
    if (c == '.') {
      lexeme[yyleng++] = c;
      
      while ((c = getchar()) && isdigit(c)) // read characters until no more digits are found
        lexeme[yyleng++] = c;
      
      ungetc(c, yyin);  // return the next character back to stdin, because we don't need it here
      token = TOK_FLOATLIT;

      if (yyleng < 2)  // if is only a dot without digits then is unknown
        token = TOK_UNKNOWN;
      
      lexeme[yyleng] = '\0';
      return token;
    }
    
    // test if is a string literal
    if (c == '"') {
      lexeme[yyleng++] = c;  // first quotation mark
      
      while ((c = getchar()) && c != '"') // read characters until quotation mark are found
        lexeme[yyleng++] = c;
      
      lexeme[yyleng++] = c;  // second quotation mark
      
      token = TOK_STRINGLIT;
      return token;
    }
    
    // at this point the lexeme starts with a letter
    lexeme[yyleng++] = c;
    
    while ((c = getchar()) && (!isblank(c) && c != '\n')) // read characters until a separator is found
      lexeme[yyleng++] = c;
    
    ungetc(c, yyin);  // return the separator back to stdin
    
    lexeme[yyleng] = '\0';
    
    // test if is a word reserved (keyword, data type, logical operator) lexeme
    token = is_reserved();
    
    if (token)  // if is then return it 
      return token;
    
    // else then it be an identifier lexeme
    int i = 0;
    
    while(isalpha(lexeme[i]) || isdigit(lexeme[i]) || lexeme[i] == '_') // found the index where the identifier ends 
      i++;
    
    for (int k = yyleng - 1; k >= i; k--, yyleng--) // return the remaining characters back to stdin (note this has to be in reverse order)
      ungetc(lexeme[k], yyin);
    
    lexeme[i] = '\0'; 
  
    // uncomment in case valid identifiers are of length >= 2
    /* if (yyleng < 2)
     token = TOK_UNKNOWN;
    else */ 
      token = TOK_IDENTIFER;
    
    return token;
  }
  return(TOK_EOF); 
}
