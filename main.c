/********************************************************************/
/*Projekt:Implementace interpretu imperativního jazyka IFJ16        */
/*Jména řešitelů: Sebastián Kisela, Ondrej Svoreň, Daniel Rudík,    */
/*                  Patrik Roman, Martin Chudý                      */
/*Loginy řešitelů: xkisel02, xsvore01, xrudik00, xroman10, xchudy04 */
/********************************************************************/

#include <stdio.h>
//#include "defs.h"
#include "ial.h"
#include "lex.h"
#include "parser.h"
#include "interpret.h"

//#define FILE_ERROR 5
Ttoken *token;
FILE *file;
TBuffer * buffer;

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stderr, "Wrong number of arguments! Use 1\n");
		return EXIT_FAILURE;
	}
	file = fopen(argv[1], "r");

	if(!file)
	{ 
		fprintf(stderr, "nejde otevrit soubor %s\n",argv[1]);
		return EXIT_FAILURE; 
	}

	lexStart();
    parse();
    // TVariable *var = NULL;
    // expression(var);
    // interpret();
    
    lexFinish();
	return 0;
}