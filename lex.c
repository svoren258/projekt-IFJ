#include "lex.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define COUNT_OF_KEYWORDS 17

//globalni promene
Ttoken *token;
FILE *file;
TBuffer *buffer;

char *printToken();

#define line printf("LINE:%d\n",__LINE__);

//klicova slova
char *keywords[] ={"boolean","break","class","continue","do","double","else","false","for","if","int","return",
	"String","static","true","void","while"};

int isKeyword(TBuffer *buffer)
{

	for(int i = 0; i < COUNT_OF_KEYWORDS; i++)
	{
		if(!(strcmp(buffer->data,keywords[i])))
		{
			buffer->data = keywords[i];
			return i;
		}
	}
	return -1;
}

TBuffer *bufferInit(TBuffer *buffer)
{
	buffer = malloc(sizeof(TBuffer));
	buffer->capacity = 1;
	buffer->used = 0;
	return buffer;
}

//funkce rozsiri token
TBuffer * extendBuffer(TBuffer *buffer, char c)
{
	//printf("%c\n",c);
	
		buffer->data = realloc(buffer->data, buffer->capacity);
		if(buffer == NULL)
		{
			return NULL;
		}
		
		buffer->data[buffer->used] = c;
		buffer->capacity++;
		buffer->used++;
		
	return buffer;
	//FIX THIS!!!
}


Ttoken *getToken(){
	int state = STATE_INIT;
	
	int c;
	token = malloc(sizeof(Ttoken));
	if(buffer){
		free(buffer);
	}
	buffer = bufferInit(buffer);
	
	while( 1 )
	{
		c = fgetc(file);
		//printf("%c\n",c);
		switch( state )
		{
			
			case STATE_INIT:
			{
				//printf("%c is to come\n",c);
				if( c == EOF )
				{
					token->type = TOKEN_EOF;
					token->data = "END OF FILE";
					return token;
				}
				if( c == ' ' )
					break;
				if( isdigit(c) )
				{
					extendBuffer(buffer, c);
					state		= STATE_INT;
					token->type = TOKEN_INT;
					break;
				}
				if( isalpha(c) )
				{
					extendBuffer(buffer, c);
					state		= STATE_ID;
					token->type = TOKEN_ID;
					break;
				}
				if( c == '{' )
				{
					extendBuffer(buffer, c);
					token->data = buffer->data;
					token->type = TOKEN_L_CURLY;
					return token;
				}
				if( c == '}' )
				{
					extendBuffer(buffer, c);
					token->data = buffer->data;
					token->type = TOKEN_R_CURLY;
					return token;
				}
				if( c == '(' )
				{
					extendBuffer(buffer, c);
					token->data = buffer->data;
					token->type = TOKEN_L_ROUND;
					return token;
				}
				if( c == ')' )
				{
					extendBuffer(buffer, c);
					token->data = buffer->data;
					token->type = TOKEN_R_ROUND;
					return token;
				}
				if( c == '[' )
				{
					extendBuffer(buffer, c);
					token->data = buffer->data;
					token->type = TOKEN_L_SQUARE;
					return token;
				}
				if( c == ']' )
				{
					extendBuffer(buffer, c);
					token->data = buffer->data;
					token->type = TOKEN_R_SQUARE;
					return token;
				}
				if( c == '.' )
				{
					extendBuffer(buffer, c);
					token->data = buffer->data;
					token->type = TOKEN_DOT;
					return token;
				}
				if( c == '+' )
				{
					extendBuffer(buffer, c);
					token->data = buffer->data;
					token->type = TOKEN_PLUS;
					return token;
				}
				if( c == '-' )
				{
					extendBuffer(buffer, c);
					token->data = buffer->data;
					token->type = TOKEN_MINUS;
					return token;
				}
				
				if( c == '*' )
				{
					extendBuffer(buffer, c);
					token->data = buffer->data;
					token->type = TOKEN_MUL;
					return token;
				}
				if( c == '/' )
				{
					extendBuffer(buffer, c);
					state		= STATE_DIV;
					token->type = TOKEN_DIV;
					break;
				}
				if( c == ';' )
				{
					extendBuffer(buffer, c);
					token->data = buffer->data;
					token->type = TOKEN_SEM_CL;
					return token;
				}
				
				if( c == '<' )
				{
					extendBuffer(buffer, c);
					state = STATE_LESSER;
					break;
				}
				
				if( c == '>' )
				{
					extendBuffer(buffer, c);
					state = STATE_GREATER;
					break;
				}
				if( c == '=' )
				{
					extendBuffer(buffer, c);
					state = STATE_ASSIGN;
					break;
				}
				
				if( c == '!' )
				{
					extendBuffer(buffer, c);
					state = STATE_EXCL_MARK;
					break;
				}
				if( c == '"' )
				{
					//not sure, should the apostrophe be included in the string?
					state = STATE_STRING;
					break;
				}
				break;
				
			}
/********************************END FIRST STATE*****************************/
/********************************BEGIN NEXT STATE***************************/
			case STATE_INT:
			{
				if( isdigit(c) )
				{
					extendBuffer(buffer, c);
					break;
				}
				if( c == '.' )
				{
					extendBuffer(buffer, c);
					state = STATE_FUT_DOUBLE;
					break;
				}
				token->data = buffer->data;
				token->type = TOKEN_INT;
				return token;
				
				line
				ret_error(LEX_ERROR);
			}
			
			case STATE_FUT_DOUBLE:
			{
				if( isdigit(c) )
				{
					token->type = TOKEN_DOUBLE;
					extendBuffer(buffer, c);
					break;
				}
				line
				ret_error(SYNTAX_ERROR);
				
			}
			
			case STATE_ID:
			{
				token->type = TOKEN_ID;
				if( isalnum(c) )
				{
					extendBuffer(buffer, c);
					break;
				}
				
				if( isKeyword(buffer) > 0 )
				{
					token->type = isKeyword(buffer);
				}
				token->data = buffer->data;
				ungetc(c, file);
				return token;
				break;
			}
			
			// case STATE_PLUS:
			// {
			// 	if( c == '+')
			// 	{
			// 		extendBuffer(buffer, c);
			// 		state		= STATE_INC;
			// 		token->type = TOKEN_INC;
			// 		break;
			// 	}
			// 	state = STATE_END;
			// 	break;
				
			// }
			
			// case STATE_MINUS:
			// {
			// 	if( c == '-' )
			// 	{
			// 		extendBuffer(buffer, c);
			// 		state		= STATE_DEC;
			// 		token->type = TOKEN_DEC;
			// 		break;
			// 	}
			// 	state = STATE_END;
			// 	break;
				
			// }
			
			// case STATE_INC:
			// {
			// 	if( c == ';' )
			// 	{
			// 		ungetc(c, file);
			// 		state = STATE_END;
			// 		break;
			// 	}
			// 	line
			// 	ret_error(SYNTAX_ERROR);
			// }
			
			// case STATE_DEC:
			// {
			// 	if( c == ';' )
			// 	{
			// 		ungetc(c, file);
			// 		state = STATE_END;
			// 		break;
			// 	}
			// 	line
			// 	ret_error(SYNTAX_ERROR);
			// }
			
			case STATE_DIV:
			{
				if( c == '/' )
				{
					state = STATE_LINE_COM;
					break;
				}
				if( c == '*' )
				{
					state = STATE_BEGIN_COM;
					break;
				}
				state = STATE_END;
				break;
			}
			
			case STATE_LINE_COM:
			{
				if( c == '\n' )
				{
					state = STATE_INIT;
					break;
				}
			}
			
			case STATE_BEGIN_COM:
			{
				if( c == '/' )
				{
					state = STATE_INIT;
					break;
				}
			}
			
			case STATE_LESSER:
			{
				if( c == '=' )
				{
					extendBuffer(buffer, c);
					token->type = TOKEN_LE_EQ;
					token->data = buffer->data;
					return token;
				}
				ungetc(c, file);
				token->type = TOKEN_LESSER;
				token->data = buffer->data;
				return token;
			}
			
			case STATE_GREATER:
			{
				if( c == '=' )
				{
					extendBuffer(buffer, c);
					token->type = TOKEN_GR_EQ;
					token->data = buffer->data;
					return token;
				}
				ungetc(c, file);
				token->type = TOKEN_GREATER;
				token->data = buffer->data;
				return token;
			}
			
			case STATE_ASSIGN:
			{
				if( c == '=' )
				{
					extendBuffer(buffer, c);
					token->type = TOKEN_EQUALS;
					token->data = buffer->data;
					return token;
				}
				ungetc(c, file);
				token->type = TOKEN_ASSIGN;
				token->data = buffer->data;
				return token;
			}
			
			case STATE_EXCL_MARK:
			{
				if( c == '=' )
				{
					extendBuffer(buffer, c);
					token->type = TOKEN_NOT_EQ;
					token->data = buffer->data;
					return token;
				}
				line
				ret_error(SYNTAX_ERROR);
				////////NOT SURE
			}
			
			case STATE_STRING:
			{
				
				if( c != '"' )
				{
					extendBuffer(buffer, c);
					break;
				}
				token->type = TOKEN_STRING;
				token->data = buffer->data;
				return token;
				line
				ret_error(SYNTAX_ERROR);
				////////NOT SURE
			}
			
			
			case STATE_END:
			{
				// if( buffer->used > 0 )
				// 	token->data = buffer->data;
				// free(buffer);
				return token;
			}
			
			default:
			{
				line
				ret_error(LEX_ERROR);	
			}
			
			
			
			
			
		}
			
		
	}
	printf("%s\n",buffer->data);
	free(buffer);
	free(token);
	fclose(file);
	
	printf("%d the last\n",c);
	printf("The end\n");
	return token;
}