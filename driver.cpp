//*****************************************************************************
// purpose: CSE 4713 / 6713 Assignment 1: Lexical Analyzer
//          Driver code
// created: 9-3-2019
//  author: 
//*****************************************************************************
#include "lexer.h"

// Instantiate global variables
FILE *yyin;         // input stream
FILE *yyout;        // output stream
int   yyleng;       // length of current lexeme
char *yytext;       // text of current lexeme

// Do the analysis
int main( int argc, char* argv[] )
{
  int token;   // hold each token code

  // Set the input and output streams
  yyin = stdin;
  yyout = stdout;
 
  // Do the lexical parsing
  while( (token = yylex()) && token != TOK_EOF ) 
  {
    // What did we find?
    if( token != TOK_UNKNOWN )
      printf("lexeme: |%s|, length: %d, token: %d\n", yytext, yyleng, token);
    else
      printf("   ERROR: unknown token\n");
  }
}
