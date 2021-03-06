#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdlib.h>
#include "builtin.h"
#include "defs.h"

//porovnanie 2 retazcov
int compare(char* s1,char* s2){
    unsigned long int n = 0;

    if(strlen(s1) < strlen(s2)){
        n = strlen(s1);
    }else{
        n = strlen(s2);
    }
    //porovnavanie jednotlivych prvkov
    for (int i = 0; i < n; ++i) {
        if(s1[i] < s2[i]) {
            return -1;
        }
            if(s1[i] > s2[i]) {
                return 1;
            }
    }

    //v pripade ze sa doteraz zhodovali, vrati kratsi retazec
    if(strlen(s1) < strlen(s2)){
        return -1;
    }
    if(strlen(s1) > strlen(s2)){
        return 1;
    }

    return 0;
}

//podretazec retazca s na indexe i, o dlzke n
char* substr(char* s,int i,int n){
    if(i > length(s) || n > (length(s)-i))
	ret_error(OTHER_RUNTIME_ERROR);

    char* substr;
    substr = (char *) malloc(n);
    for (int j = 0; j < n; ++j) {
        substr[j] = s[i];
        ++i;
    }

return substr;
}

void print(TVariable* var){
    if(var->type == VARTYPE_INTEGER)
        printf("%d",var->value.i);
    if(var->type == VARTYPE_DOUBLE)
        printf("%g",var->value.d);
    if(var->type == VARTYPE_STRING)
        printf("%s",var->value.s);
}

int length(char* str){
    int size;
    size = (int)strlen(str);

    return size;
}

//citanie hodnoty double zo vstupu
double readDouble(){
    int c;
    char *str;
    int i = 0;
    bool bodka = FALSE;
    str = (char *) malloc(sizeof(char)*8);

    while((c = getchar()) != '\n'){
        if(c == EOF)
            break;
        if((!isdigit(c)) && (c != '.'))
            ret_error(10);
        if(c == '.'){
            if(bodka == FALSE)
                bodka = TRUE; //poistenie proti viacerym bodkam vo vstupe
            else
                ret_error(10);
        }

        str = (char *) realloc(str, (sizeof(char))*8*(i+1));
        str[i++] = c;
        str[i] = '\0';
    }
    double cislo = atof(str);

    free(str);

    return cislo;
}

int readInt(){
    int c;
    char *str;
    int i = 0;
    str = (char *) malloc(sizeof(char)*8);

    while((c = getchar()) != '\n'){
        if(c == EOF)
            break;
        if(!isdigit(c))
            ret_error(10);

        str = (char *) realloc(str, (sizeof(char))*8*(i+1));
        str[i++] = c;
        str[i] = '\0';
    }
    int cislo = atoi(str);

    free(str);

    return cislo;
}



char* readString(){
    int c;
    char *str;
    int i = 0;
    str = (char *) malloc(sizeof(char)*8);

    while((c = getchar()) != '\n') {
        if(c == EOF)
            break;
        str = (char *) realloc(str, (sizeof(char))*8*(i+1));

        str[i++] = c;
        str[i] = '\0';
    }

return str;
}
