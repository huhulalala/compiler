#ifndef Tokenization_guard
#define Tokenization_gurad

#include <iostream>


enum TokenType {
    INT, CHAR, VOID, IF, ELSE, WHILE, RETURN,
    ID, NUM, 
    ASSIGN, PLUS, MINUS, STAR, DIV, 
    EQ, NE, LT, GT, AND, OR,
    LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA,
    END_OF_FILE
};

struct TokenStr {
    enum TokenType type;
    char *str;
};

struct result_token {
    struct TokenStr *str_token;
    int size;
};

struct Graph {
    int ***i; 
    int status_num;
};

struct Item {
    int start;
    int end;
};

// --- 函数声明 ---

// 正则处理
char * RegularToPostfix(char * regular, int size);
bool Periority(char in, char stack);

// NFA 核心
struct Graph * Graph_init();
struct Item PostfixToNFA(struct Graph * graph, char *str);
int graph_bianli(struct Graph *graph, char * ww1, int start, int end, int index);

// 词法分析主接口
struct result_token * Tokenization(char *sourcetext);
char* extract_substring(const char* str, int start, int end);

#endif