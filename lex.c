/********************************************************************/
/*Projekt:Implementace interpretu imperativního jazyka IFJ16        */
/*Jména řešitelů: Sebastián Kisela, Ondrej Svoreň, Daniel Rudík,    */
/*                  Patrik Roman, Martin Chudý                      */
/*Loginy řešitelů: xkisel02, xsvore01, xrudik00, xroman10, xchudy04 */
/********************************************************************/

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
TStack *token_alloc_stack;
TStack *tokenStack;
int lineNumber = 1;
int ungetTokenIndex = 0;


//klicova slova
char *keywords[] ={"boolean","break","class","continue","do","double","else","false","for","if","int","return",
                   "String","static","true","void","while"};



int isKeyword(TBuffer *buffer)
{
    if(buffer->used < 2)
        return -1;

    for(int i = 0; i < COUNT_OF_KEYWORDS; i++)
    {
        if(!(strcmp(buffer->data,keywords[i])))
        {
            //buffer->data = keywords[i];
            return i;
        }
    }
    return -1;
}

int bufferInit()
{
    buffer = malloc(sizeof(TBuffer));
    if ((buffer->data = (char*) malloc(STR_LEN_INC)) == NULL)
      ret_error(INTERNAL_ERROR);
   buffer->data[0] = '\0';
   buffer->used = 0;
   buffer->capacity = STR_LEN_INC;
   return STR_SUCCESS;
}

void bufferClear(TBuffer *s)
// funkce vymaze obsah retezce
{
   s->data[0] = '\0';
   s->used = 0;
}

int  extendBuffer(TBuffer *s1, char c)
{
    if (s1->used + 1 >= s1->capacity)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->data = (char*) realloc(s1->data, s1->used + STR_LEN_INC)) == NULL)
         return STR_ERROR;
      s1->capacity = s1->used + STR_LEN_INC;
   }
   s1->data[s1->used] = c;
   s1->used++;
   s1->data[s1->used] = '\0';
   return STR_SUCCESS;
}

void unget_token(int y)
{
    //s("****Token wasu nget****\n");
    for(int i = 0; i < y; i++)
    {
        if(ungetTokenIndex >= 1)
            ungetTokenIndex--;
    }
    
}

void pushToken(Ttoken * token)
{
    
    // printf("STACK%d\n",token_alloc_stack->top);
    if(ungetTokenIndex <= 9)
        ungetTokenIndex++;
    
    if(tokenStack->top >= 9)
    {
        for(int i = 0; i <= tokenStack->top - 1; i++)
        {
            tokenStack->data[i] = tokenStack->data[i+1];
            
        }
        tokenStack->data[tokenStack->top] = token;  
        return;
    }
    stackPush(tokenStack, token);
    
}

Ttoken * newToken()
{
    Ttoken * newtoken = malloc(sizeof(Ttoken));
    if(!newtoken)ret_error(INTERNAL_ERROR);
    newtoken->data = malloc(sizeof(char));
    if(!newtoken->data)ret_error(INTERNAL_ERROR);
    newtoken->data[0] = '\0';
    return newtoken;
}

Ttoken * getTokenFromStack()
{
     //printf("Ungetindex:%d\n",ungetTokenIndex);
     //printf("TOKENSTACK\n");
     //for(int i = tokenStack->top; i >= 0; i--)
     //{
     // token = tokenStack->data[i];
     // printf("%s \t%d\n",token->data, i);
        
     //}
     //printf("TOKENSTACK\n");
    
    token = tokenStack->data[ungetTokenIndex];
    ungetTokenIndex++;
    return token;
}

void lexStart()
{
    token_alloc_stack = stackInit();

    if(!tokenStack)
        tokenStack = stackInit();
    if(!token)      
    {
        token = malloc(sizeof(Ttoken));
        if(!token)ret_error(INTERNAL_ERROR);
    }
    if(!buffer)
        bufferInit();
}

void lexFinish()
{
    // for(int i = token_alloc_stack->top; i>=0; i--)
    // {
    //  Ttoken *token = stackPop(token_alloc_stack);
    //  // free(token->data);
    //  if(token)
    //  free(token);
    // }
    
    // free(token_alloc_stack);
    // free(buffer);
    // free(token);
    // free(tokenStack);
    fclose(file);
}

Ttoken *get_token(){

    //printf("som v get_token\n");
    if(ungetTokenIndex <= tokenStack->top && (ungetTokenIndex > -1))
    {
    //  printf("****next token is unget %s*****\n",token->data);
        token = getTokenFromStack();
        // tok;
        return token;
    }
    
    int state = STATE_INIT;
    int c;
    int octal = 0;
    
    if(buffer)
    {
        // char* let = buffer->data;
        free(buffer->data);
        free(buffer);   
    }
    if(token)stackPush(token_alloc_stack, token);
    token = newToken();
    bufferInit(buffer);
    
    
    
    while( 1 )
    {
        token->lineNumber = lineNumber;
        c = fgetc(file);
        // printf("lex:%c\n",c);
        switch( state )
        {
            
            case STATE_INIT:
            {
                if( c == EOF )
                {
                    token->type = TOKEN_EOF;
                    token->data = "END OF FILE";
                    return token;
                }
                if( c == '\n' )
                {
                    lineNumber++;                   
                    break;
                }
                if( c == ' ' )
                    
                    break;
                if( c == '\t' )
                    
                    break;
                if( isdigit(c) )
                {
                    
                    extendBuffer(buffer, c);
                    state       = STATE_INT;
                    token->type = TOKEN_INT;
                    break;
                }
                if( isalpha(c) || c == '$' || c == '_' )
                {
                    extendBuffer(buffer, c);
                    state       = STATE_ID;
                    token->type = TOKEN_ID;
                    break;
                }
                if( c == '{' )
                {
                    extendBuffer(buffer, c);
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    token->type = TOKEN_L_CURLY;
                    
                    pushToken(token);
                    return token;
                }
                if( c == '}' )
                {
                    extendBuffer(buffer, c);
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    token->type = TOKEN_R_CURLY;
                    pushToken(token);
                    return token;
                }
                if( c == '(' )
                {
                    extendBuffer(buffer, c);
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    token->type = TOKEN_L_ROUND;
                    pushToken(token);
                    return token;
                }
                if( c == ')' )
                {
                    extendBuffer(buffer, c);
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    token->type = TOKEN_R_ROUND;
                    pushToken(token);
                    return token;
                }
                if( c == '[' )
                {
                    extendBuffer(buffer, c);
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    token->type = TOKEN_L_SQUARE;
                    pushToken(token);
                    return token;
                }
                if( c == ']' )
                {
                    extendBuffer(buffer, c);
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    token->type = TOKEN_R_SQUARE;
                    pushToken(token);
                    return token;
                }
                if( c == '.' )
                {
                    extendBuffer(buffer, c);
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    token->type = TOKEN_DOT;
                    pushToken(token);
                    return token;
                }
                if( c == '+' )
                {
                    extendBuffer(buffer, c);
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    token->type = TOKEN_PLUS;
                    pushToken(token);
                    return token;
                }
                if( c == '-' )
                {
                    extendBuffer(buffer, c);
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    token->type = TOKEN_MINUS;
                    pushToken(token);
                    return token;
                }
                
                if( c == '*' )
                {
                    extendBuffer(buffer, c);
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    token->type = TOKEN_MUL;
                    pushToken(token);
                    return token;
                }
                if( c == '/' )
                {
                    extendBuffer(buffer, c);
                    state       = STATE_DIV;
                    token->type = TOKEN_DIV;
                    break;
                }
                if( c == ';' )
                {
                    extendBuffer(buffer, c);
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    token->type = TOKEN_SEM_CL;
                    pushToken(token);
                    return token;
                }
                
                if( c == '<' )
                {
                    extendBuffer(buffer, c);
                    state = STATE_LESS;
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
                    state = STATE_STRING_DOUBLE;
                    break;
                }
                // if( c == '\'' )
                // {
                //  state = STATE_STRING_SINGLE;
                //  break;
                // }
                if( c == ',' )
                {
                    extendBuffer(buffer, c);
                    token->type = TOKEN_COLON;
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    pushToken(token);
                    return token;
                }
                //TODO \n \t \" a podobne srandy
                
                line;
                ret_error(LEX_ERROR);
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
                if( c == 'e' || c == 'E')
                {
                    extendBuffer(buffer, c);
                    state = STATE_FUT_E;
                    break;
                }
                if( isalpha(c) )
                {
                    line;
                    ret_error(LEX_ERROR);
                }

                ungetc(c, file);
                token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                if(!token->data)ret_error(INTERNAL_ERROR);
                token->type = TOKEN_INT;
                pushToken(token);
                return token;
        
            }

            case STATE_FUT_DOUBLE_E:
            {
                if(c == '+' || c == '-' || isdigit(c))
                {
                    extendBuffer(buffer, c);
                    state = STATE_DOUBLE_E;
                    break;
                }
                
                ret_error(LEX_ERROR);
            }

            case STATE_FUT_E:
            {
                if(c == '+' || c == '-')
                {
                    extendBuffer(buffer, c);
                    state = STATE_E;
                    break;
                }
                if(isdigit(c))
                {
                    extendBuffer(buffer, c);
                    state = STATE_E;
                    break;
                }
                ret_error(LEX_ERROR);
                break;
            }
            
            case STATE_DOUBLE_E:
            {
                if( isdigit(c) )
                {
                    extendBuffer(buffer, c);
                    break;
                }
                if(buffer->data[buffer->used-1] < 48 || buffer->data[buffer->used-1] > 57)
                {
                    line;
                    ret_error(LEX_ERROR);
                }
                

                ungetc(c, file);
                token->type = TOKEN_DOUBLE_E;
                token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                if(!token->data)ret_error(INTERNAL_ERROR);
                pushToken(token);
                return token;
            }


            
            case STATE_E:
            {
                if( isdigit(c) )
                {
                    extendBuffer(buffer, c);
                    break;
                }
                if( isalpha(c) )
                {
                    line;
                    ret_error(SYNTAX_ERROR);
                }
                if(buffer->data[buffer->used-1] < 48 || buffer->data[buffer->used-1] > 57)
                {
                    line;
                    ret_error(LEX_ERROR);
                }
                ungetc(c, file);
                token->type = TOKEN_E;
                token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                if(!token->data)ret_error(INTERNAL_ERROR);
                pushToken(token);
                return token;
            }
            
            case STATE_FUT_DOUBLE:
            {
                if( isdigit(c) )
                {
                    token->type = TOKEN_DOUBLE;
                    extendBuffer(buffer, c);
                    break;
                }
                if( c == 'e' || c == 'E' )
                {
                    if( token->type != TOKEN_DOUBLE )
                    {
                        line;
                        ret_error(SYNTAX_ERROR);
                    }
                    extendBuffer(buffer, c);
                    state = STATE_FUT_DOUBLE_E;
                    break;
                }
                
                ungetc(c,file);
                token->type = TOKEN_DOUBLE;
                token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                if(!token->data)ret_error(INTERNAL_ERROR);
                pushToken(token);
                return token;
                line;
                ret_error(SYNTAX_ERROR);
                
            }
            
            case STATE_ID:
            {
                token->type = TOKEN_ID;
                if( isalnum(c) || c == '$' || c == '_' )
                {
                    extendBuffer(buffer, c);
                    break;
                }

                 if( isKeyword(buffer) > 0 )
                 {
                    token->type = isKeyword(buffer);
                    switch(token->type)
                    {
                        case KEYWORD_INT:
                        case KEYWORD_DOUBLE:
                        case KEYWORD_STRING:
                            token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                            if(!token->data)ret_error(INTERNAL_ERROR);
                            token->type = TOKEN_TYPE;
                            return token;
                    }
                 }

                if( isKeyword(buffer) > 0 )
                {
                    token->type = isKeyword(buffer);
                    switch(token->type)
                    {
                        case KEYWORD_INT:
                        case KEYWORD_DOUBLE:
                        case KEYWORD_STRING:
                            token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                            if(!token->data)ret_error(INTERNAL_ERROR);
                            token->type = TOKEN_TYPE;
                            pushToken(token);
                            
                            return token;
                    }
                }

                
                token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                if(!token->data)ret_error(INTERNAL_ERROR);
                ungetc(c, file);
                pushToken(token);
                return token;
                
            }

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
                ungetc(c, file);
                token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                if(!token->data)ret_error(INTERNAL_ERROR);
                pushToken(token);
                return token;
                
            }
            
            case STATE_LINE_COM:
            {
                if( c == '\n' || c == EOF )
                {
                    lineNumber++;
                    bufferClear(buffer);
                    state = STATE_INIT;
                    break;
                }
                break;
            }
            
            case STATE_BEGIN_COM:
            {
                if( c == '*' )
                {
                    state = STATE_END_COM;
                    break;
                }
                break;
            }
            
            case STATE_END_COM:
            {
                if( c == '/' )
                {
                    bufferClear(buffer);
                    state = STATE_INIT;
                    break;
                }
                state = STATE_BEGIN_COM;
                break;
            }
            
            case STATE_LESS:
            {
                if( c == '=' )
                {
                    extendBuffer(buffer, c);
                    token->type = TOKEN_LE_EQ;
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    pushToken(token);
                    return token;
                }
                ungetc(c, file);
                token->type = TOKEN_LESS;
                token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                if(!token->data)ret_error(INTERNAL_ERROR);
                pushToken(token);
                return token;
            }
            
            case STATE_GREATER:
            {
                if( c == '=' )
                {
                    extendBuffer(buffer, c);
                    token->type = TOKEN_GR_EQ;
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    pushToken(token);
                    return token;
                }
                ungetc(c, file);
                token->type = TOKEN_GREATER;
                token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                if(!token->data)ret_error(INTERNAL_ERROR);
                pushToken(token);
                return token;
            }
            
            case STATE_ASSIGN:
            {
                if( c == '=' )
                {
                     token->type = TOKEN_EQUALS;
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    pushToken(token);
                    return token;
                }
                ungetc(c, file);
                token->type = TOKEN_ASSIGN;
                token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                if(!token->data)ret_error(INTERNAL_ERROR);
                pushToken(token);
                return token;
            }
            
            case STATE_EXCL_MARK:
            {
                if( c == '=' )
                {
                    extendBuffer(buffer, c);
                    token->type = TOKEN_NOT_EQ;
                    token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                    if(!token->data)ret_error(INTERNAL_ERROR);
                    pushToken(token);
                    return token;
                }
                line;
                ret_error(SYNTAX_ERROR);
                ////////NOT SURE
            }
            
            case STATE_STRING_DOUBLE:
            {
                
                if( c == '\n' )
                {
                    line;
                    ret_error(LEX_ERROR);
                }
                if( c == '\\')
                {
                    state = STATE_ASCII_DOUBLE;
                    break;
                }
                if( c != '"' )
                {
                    extendBuffer(buffer, c);
                    break;
                }
                
                token->type = TOKEN_STRING;
                token->data = realloc(token->data, sizeof(char)*(int)strlen(buffer->data)*8);strcpy(token->data,buffer->data);
                if(!token->data)ret_error(INTERNAL_ERROR);
                pushToken(token);
                return token;
                line;
                ret_error(SYNTAX_ERROR);
                ////////NOT SURE
            }

            
            case STATE_ASCII_DOUBLE:
            {
                switch(c)
                {
                    case 'n':
                        extendBuffer(buffer, '\n');
                        state = STATE_STRING_DOUBLE;
                        break;
                    case 't':
                        extendBuffer(buffer, '\t');
                        state = STATE_STRING_DOUBLE;
                        break;
                    case '\'':
                        extendBuffer(buffer, '\'');
                        state = STATE_STRING_DOUBLE;
                        break;
                    case '"':
                        extendBuffer(buffer, '"');
                        state = STATE_STRING_DOUBLE;
                        break;
                    case '\\':
                        extendBuffer(buffer, '\\');
                        state = STATE_STRING_DOUBLE;
                        break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                        extendBuffer(buffer, c);
                        state = STATE_OCTAL;
                        octal++;
                        break;
                    default:
                        ret_error(LEX_ERROR);
                }
                break;
            }

            case STATE_OCTAL:
                if(octal >= 4)
                    ret_error(LEX_ERROR);
                if(c >= 48 && c <= 55)
                {
                    if(octal == 2)
                    {
                        if(c >= 49 && c <= 55)
                        {
                            extendBuffer(buffer, c);
                            // printf("|%c|%c|%c|%c|\n",buffer->data[buffer->used-3],buffer->data[buffer->used-2],buffer->data[buffer->used-1],buffer->data[buffer->used]);
                            int o = 8*8*(-48+(buffer->data[buffer->used-3])) + 8*(-48+(buffer->data[buffer->used-2])) + (-48+buffer->data[buffer->used-1]);
                            buffer->data[buffer->used-3] = o;
                            buffer->data[buffer->used-2] = '\0';
                            buffer->used = buffer->used - 2;
                            // printf(" c o:%s  \n",buffer->data);
                            octal = 0;
                            state = STATE_STRING_DOUBLE;
                            break;
                        } else {
                            line;
                            ret_error(LEX_ERROR);
                        }
                    }
                    extendBuffer(buffer, c);

                    octal++;
                    break;
                }
                ret_error(LEX_ERROR);
            
            
            default:
            {
                line;
                ret_error(LEX_ERROR);   
            }
            
            
            
            
            
        }
            
        
    }
}