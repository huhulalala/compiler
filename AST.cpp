#include "Tokenization.h"

int main(){
    /*
        struct TokenStr {
            enum TokenType type;
            char *str;
        };
        struct result_token {
            struct TokenStr *str_token;
            int size;
        };
    */
    char * text = "int main(int x) {int a;if(x){a = 1 + 2;}}";
    struct result_token * LexicalResult = Tokenization(text);
    for(int i = 0 ; i < LexicalResult->size ; i++){        
        printf("【%s】",LexicalResult->str_token[i].str);
        printf("【%d】\n",LexicalResult->str_token[i].type);
    }
    return -1;
}