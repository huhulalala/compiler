#include <iostream>
typedef struct Token_Stream{
    char * name;
} Token_Stream;

struct Graph{
    int *** i; //[0][0][0] 状态,转移字符,状态
    int status_num; // 这个是构造算法动态来的。
}; 

//如果in的优先级大于stack返回真
//* > . > | 【优先级】
bool Periority(char in , char stack){
    // * . | A_{3}{2}
    // * . 
    // * |
    if(in=='*'){
        if(stack=='*'){
            return false;
        }else{
            return true;
        }
    }
    // . *
    // . |
    if(in=='.'){
        if(stack=='*'){
            return false;
        }else if(stack=='|'){
            return true;
        }else if(stack=='.'){
            return false;
        }
    }
    // | *
    // | .
    if(in=='|'){
        if(stack=='|'){
            return false;
        }else{
            return false;
        }
    }
}

char * RegularToPostfix(char * regular,int size){
    //1、转为后缀表达式
    char * newstr = (char *)malloc(sizeof(char) * (size+1));
    char * str_stack = (char *)malloc(sizeof(char) * (size+1));
    int top = -1;
    int index = 0; //旧字符串下标
    int index1 = 0; //新字符串下标
    while(regular[index]!='\0'){
        char curr = regular[index];
        if(curr == '*' || curr == '.' || curr == '|' || curr =='(' || curr ==')'){
            if(top==-1){
                //空的直接入符号栈
                str_stack[++top] = curr;
            }else{
                if(curr=='('){
                    str_stack[++top] = curr;
                    index++;
                    continue;
                }
                if(curr==')'){
                    while(str_stack[top]!='('){
                        newstr[index1++] = str_stack[top];
                        top--;
                    }
                    if(str_stack[top]=='('){
                        top--;
                    }
                    index++;
                    continue;
                }
                char top_char = str_stack[top];
                //看看优先级如何如果优先级高就入栈。
                //看看优先级低就将栈中的元素都出栈以后再入栈。
                bool is = Periority(curr,top_char);
                if(is){
                    str_stack[++top] = curr;
                }else{
                    while(top!=-1 && str_stack[top]!='(' && !Periority(curr, str_stack[top])){
                        newstr[index1++] = str_stack[top--];
                    }
                    str_stack[++top] = curr;
                }

            }
        }else{
            newstr[index1++] = curr;
        }
        index++;
    }
    //最后处理...
    while(top!=-1){
        newstr[index1++] = str_stack[top--];
    }
    newstr[index1] = '\0';
    //注意要释放栈。
    free(str_stack);
    return newstr;
}


struct Graph * Graph_init(){
    struct Graph * graph = (struct Graph *) malloc(sizeof(struct Graph));
    graph->status_num = -1;
    graph->i = nullptr;
    return graph;
}

struct Item{
    int start;
    int end;
};


//Postfix To NFA
struct Item PostfixToNFA(struct Graph * graph,char *str){
    // 汤普森算法
    struct Item * items = (struct Item *)malloc(sizeof(struct Item) * 1000);
    int top = -1;
    int index = 0; 
    while(str[index]!='\0'){
        if(str[index]=='*' || str[index] == '.' || str[index]=='|'){
            if(str[index]=='*'){
                //出栈一个
                struct Item b = items[top--];
                graph->status_num++;
                graph->status_num++;
                //第一个新增的状态射出的两条边。
                graph->i[graph->status_num-1][126][b.start] = 1;
                graph->i[graph->status_num-1][126][graph->status_num] = 1;
                //第二个
                graph->i[b.end][126][graph->status_num] = 1;
                graph->i[b.end][126][b.start] = 1;
                struct Item * aa = (struct Item *)malloc(sizeof(struct Item));
                aa->start = graph->status_num - 1;
                aa->end = graph->status_num;
                items[++top] = *aa; //free to do
            }else if(str[index]=='.'){
                struct Item follow = items[top--];
                struct Item first = items[top--];
                graph->i[first.end][126][follow.start] = 1;
                struct Item * aa = (struct Item *)malloc(sizeof(struct Item));
                aa->start = first.start;
                aa->end = follow.end;
                items[++top] = *aa;
            }else if(str[index]=='|'){
                struct Item follow = items[top--];
                struct Item first = items[top--];
                graph->status_num++;
                graph->status_num++;
                graph->i[first.end][126][graph->status_num] = 1;
                graph->i[follow.end][126][graph->status_num] = 1;
                graph->i[graph->status_num - 1][126][first.start] = 1 ;
                graph->i[graph->status_num - 1][126][follow.start] = 1;
                struct Item * aa = (struct Item *)malloc(sizeof(struct Item));
                aa->start = graph->status_num - 1;
                aa->end = graph->status_num;
                items[++top] = *aa;
            }
        }else{
            // 创建两个状态，中间通过这个字符进行转移。
            if(graph->i == nullptr){
                //第一次 相当于去初始化 这个1000状态数先写死，128是可能转移的字母编号ascii表，其中第127，也就是数组下标126当作epsilon转移。
                int ***a = (int ***) malloc(sizeof(int**) * 1000);
                for(int i=0; i<1000; i++) {
                    *(a + i) = (int **) malloc(sizeof(int*) * 128);
                    for(int j=0; j<128; j++) {
                        *(*(a + i) + j) = (int *) malloc(sizeof(int) * 1000);
                    }
                }
                //进行初始化。
                for(int i = 0 ; i < 1000 ; i++){
                    for(int j = 0 ; j < 128 ; j++){
                        for(int z = 0 ; z < 1000 ; z++){
                            a[i][j][z] = -1;
                        }
                    }
                }
                graph->i = a;
                // x状态 通过 (int)str[index]字符 转移到 x状态
                graph->i[0][(int)str[index]][1] = 1;
                graph->status_num++;
                graph->status_num++;
                struct Item * ii = (struct Item *)malloc(sizeof(struct Item)); 
                ii->start = 0;
                ii->end = 1;
                items[++top] = *ii; 
            }else{
                //其他次数
                graph->status_num++;
                graph->status_num++;
                graph->i[graph->status_num - 1][(int)str[index]][graph->status_num] = 1;
                struct Item * ii = (struct Item *)malloc(sizeof(struct Item)); 
                ii->start = graph->status_num - 1;
                ii->end = graph->status_num;
                items[++top] = *ii;
            }
        }
        index++;
    }

    //暂时不返回start和end，只判断是否成功构建NFA，并且存储在栈中。
    if(top == 0){
        return items[top--];
    }else{
        struct Item aa;
        aa.start = -1;
        aa.end = -1;
        return aa;
    }
}



Token_Stream * Tokenzation(char * Context){

    return nullptr;
}


int flag = 0;
//传入参数为NFA的起始状态和终止状态的编号。
// index 表示当前处理到字符串的第几个字符
// 返回值：1 表示匹配成功，0 表示匹配失败
int graph_bianli(struct Graph *graph, char * ww1, int start, int end, int index) {
    // 成功条件：到达终点且字符串刚好读完
    if (start == end && ww1[index] == '\0') {
        return 1;
    }

    // 1. 处理 空转换 (epsilon transitions)
    // 假设你用 126 代表 epsilon
    for (int j = 0; j < 1000; j++) {
        if (graph->i[start][126][j] == 1) {
            if (graph_bianli(graph, ww1, j, end, index)) {
                return 1; // 只要有一条路通了，就向上返回成功
            }
        }
    }

    // 2. 处理 字符转换
    char c = ww1[index];
    if (c != '\0') { 
        for (int j = 0; j < 1000; j++) {
            if (graph->i[start][(int)c][j] == 1) {
                if (graph_bianli(graph, ww1, j, end, index + 1)) {
                    return 1; // 只要有一条路通了，就向上返回成功
                }
            }
        }
    }

    // 如果尝试了所有可能的路径都走不通
    return 0;
}

char* extract_substring(const char* str, int start, int end) {
    // 计算长度（闭区间）
    int len = end - start + 1;
    
    // 分配内存（+1 给结束符'\0'）
    char* result = (char*)malloc(len + 1);
    if (result == NULL) {
        return NULL;  // 内存分配失败
    }
    
    // 复制子字符串
    strncpy(result, str + start, len);
    result[len] = '\0';  // 手动添加结束符
    
    return result;
}

int main(){
    printf("全局准备..（正则表达式转后缀）\n");
    char * postfix_int    = RegularToPostfix("i.n.t", 5);
    char * postfix_char   = RegularToPostfix("c.h.a.r", 7);
    char * postfix_void   = RegularToPostfix("v.o.i.d", 7);
    char * postfix_if     = RegularToPostfix("i.f", 3);
    char * postfix_else   = RegularToPostfix("e.l.s.e", 7);
    char * postfix_while  = RegularToPostfix("w.h.i.l.e", 9);
    char * postfix_return = RegularToPostfix("r.e.t.u.r.n", 11);
    
    char * postfix_plus  = RegularToPostfix("+", 1);
    char * postfix_minus = RegularToPostfix("-", 1);
    char * postfix_star  = RegularToPostfix("&", 1);
    char * postfix_div   = RegularToPostfix("/", 1);

    char * postfix_assign = RegularToPostfix("=", 1);
    char * postfix_eq     = RegularToPostfix("=.=", 3);
    char * postfix_ne     = RegularToPostfix("!.=", 3);
    char * postfix_lt     = RegularToPostfix("<", 1);
    char * postfix_gt     = RegularToPostfix(">", 1);
    char * postfix_and    = RegularToPostfix("&.&", 3);
    char * postfix_or     = RegularToPostfix("]", 1);


    char * postfix_lparen    = RegularToPostfix("(", 1);
    char * postfix_rparen    = RegularToPostfix(")", 1);
    char * postfix_lbrace    = RegularToPostfix("{", 1);
    char * postfix_rbrace    = RegularToPostfix("}", 1);
    char * postfix_semicolon = RegularToPostfix(";", 1);
    char * postfix_comma     = RegularToPostfix(",", 1);

    char * raw_num = "(0|1|2|3|4|5|6|7|8|9).(0|1|2|3|4|5|6|7|8|9)*";
    char * postfix_num = RegularToPostfix(raw_num, 45);

    char * raw_id = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|_).(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|_|0|1|2|3|4|5|6|7|8|9)*";
    char * postfix_id = RegularToPostfix(raw_id, strlen(raw_id)+1);
    enum TokenType {
        // 关键字
        INT, CHAR, VOID, IF, ELSE, WHILE, RETURN,
        // 标识符与常量
        ID, NUM, 
        // 运算符
        ASSIGN, PLUS, MINUS, STAR, DIV, 
        EQ, NE, LT, GT, AND, OR,
        // 分隔符
        LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA,
        // 特殊
        END_OF_FILE
    }; 

    struct TokenStr
    {
        enum TokenType type;
        char *str;
    };
    
    
    printf("全局准备..（构建NFA）\n");
    struct Graph * g_int = Graph_init();   struct Item b_int = PostfixToNFA(g_int, postfix_int);
    struct Graph * g_char = Graph_init();  struct Item b_char = PostfixToNFA(g_char, postfix_char);
    struct Graph * g_void = Graph_init();  struct Item b_void = PostfixToNFA(g_void, postfix_void);
    struct Graph * g_if = Graph_init();    struct Item b_if = PostfixToNFA(g_if, postfix_if);
    struct Graph * g_else = Graph_init();  struct Item b_else = PostfixToNFA(g_else, postfix_else);
    struct Graph * g_while = Graph_init(); struct Item b_while = PostfixToNFA(g_while, postfix_while);
    struct Graph * g_return = Graph_init();struct Item b_return = PostfixToNFA(g_return, postfix_return);
    struct Graph * g_plus = Graph_init();  struct Item b_plus = PostfixToNFA(g_plus, postfix_plus);
    struct Graph * g_minus = Graph_init(); struct Item b_minus = PostfixToNFA(g_minus, postfix_minus);
    struct Graph * g_star = Graph_init();  struct Item b_star = PostfixToNFA(g_star, postfix_star);
    struct Graph * g_div = Graph_init();   struct Item b_div = PostfixToNFA(g_div, postfix_div);
    struct Graph * g_assign = Graph_init();struct Item b_assign = PostfixToNFA(g_assign, postfix_assign);
    struct Graph * g_eq = Graph_init();    struct Item b_eq = PostfixToNFA(g_eq, postfix_eq);
    struct Graph * g_ne = Graph_init();    struct Item b_ne = PostfixToNFA(g_ne, postfix_ne);
    struct Graph * g_lt = Graph_init();    struct Item b_lt = PostfixToNFA(g_lt, postfix_lt);
    struct Graph * g_gt = Graph_init();    struct Item b_gt = PostfixToNFA(g_gt, postfix_gt);
    struct Graph * g_and = Graph_init();   struct Item b_and = PostfixToNFA(g_and, postfix_and);
    struct Graph * g_or = Graph_init();    struct Item b_or = PostfixToNFA(g_or, postfix_or);
    struct Graph * g_lpar = Graph_init();  struct Item b_lpar = PostfixToNFA(g_lpar, postfix_lparen);
    struct Graph * g_rpar = Graph_init();  struct Item b_rpar = PostfixToNFA(g_rpar, postfix_rparen);
    struct Graph * g_lbr = Graph_init();   struct Item b_lbr = PostfixToNFA(g_lbr, postfix_lbrace);
    struct Graph * g_rbr = Graph_init();   struct Item b_rbr = PostfixToNFA(g_rbr, postfix_rbrace);
    struct Graph * g_semi = Graph_init();  struct Item b_semi = PostfixToNFA(g_semi, postfix_semicolon);
    struct Graph * g_comma = Graph_init(); struct Item b_comma = PostfixToNFA(g_comma, postfix_comma);
    struct Graph * g_num = Graph_init();   struct Item b_num = PostfixToNFA(g_num, postfix_num);
    struct Graph * g_id = Graph_init();    struct Item b_id = PostfixToNFA(g_id, postfix_id);
    
    char *sourcetext = "int main(int x) {int a;if(x){a = 1 + 2;}}";
    printf("开始转换为token流..\n");

    struct TokenStr * Tokenstr = (struct TokenStr *) malloc(sizeof(struct TokenStr)*1000);
    int Tokenstr_index = -1;
    enum TokenType * sourcetext_token = nullptr;
    int perior = 0;
    int last = 0;
    while(perior<strlen(sourcetext)){
        //将[last,perior]变为一个新的临时字符串，传入进判断。
        char* sub = extract_substring(sourcetext, last, perior);
        int f_int    = graph_bianli(g_int, sub, b_int.start, b_int.end, 0);
        int f_char   = graph_bianli(g_char, sub, b_char.start, b_char.end, 0);
        int f_void   = graph_bianli(g_void, sub, b_void.start, b_void.end, 0);
        int f_if     = graph_bianli(g_if, sub, b_if.start, b_if.end, 0);
        int f_else   = graph_bianli(g_else, sub, b_else.start, b_else.end, 0);
        int f_while  = graph_bianli(g_while, sub, b_while.start, b_while.end, 0);
        int f_return = graph_bianli(g_return, sub, b_return.start, b_return.end, 0);
        
        int f_plus   = graph_bianli(g_plus, sub, b_plus.start, b_plus.end, 0);
        int f_minus  = graph_bianli(g_minus, sub, b_minus.start, b_minus.end, 0);
        int f_star   = graph_bianli(g_star, sub, b_star.start, b_star.end, 0);
        int f_div    = graph_bianli(g_div, sub, b_div.start, b_div.end, 0);
        
        int f_eq     = graph_bianli(g_eq, sub, b_eq.start, b_eq.end, 0);
        int f_ne     = graph_bianli(g_ne, sub, b_ne.start, b_ne.end, 0);
        int f_assign = graph_bianli(g_assign, sub, b_assign.start, b_assign.end, 0);
        int f_lt     = graph_bianli(g_lt, sub, b_lt.start, b_lt.end, 0);
        int f_gt     = graph_bianli(g_gt, sub, b_gt.start, b_gt.end, 0);
        int f_and    = graph_bianli(g_and, sub, b_and.start, b_and.end, 0);
        int f_or     = graph_bianli(g_or, sub, b_or.start, b_or.end, 0);

        int f_lpar   = graph_bianli(g_lpar, sub, b_lpar.start, b_lpar.end, 0);
        int f_rpar   = graph_bianli(g_rpar, sub, b_rpar.start, b_rpar.end, 0);
        int f_lbr    = graph_bianli(g_lbr, sub, b_lbr.start, b_lbr.end, 0);
        int f_rbr    = graph_bianli(g_rbr, sub, b_rbr.start, b_rbr.end, 0);
        int f_semi   = graph_bianli(g_semi, sub, b_semi.start, b_semi.end, 0);
        int f_comma  = graph_bianli(g_comma, sub, b_comma.start, b_comma.end, 0);

        int f_num    = graph_bianli(g_num, sub, b_num.start, b_num.end, 0);
        int f_id     = graph_bianli(g_id, sub, b_id.start, b_id.end, 0);
        if(f_int || f_char || f_void || f_if || f_else || f_while || f_return ||f_plus || f_minus || f_star || f_div || f_eq || f_ne || f_assign || f_lt || f_gt || f_and || f_or || f_lpar || f_rpar || f_lbr || f_rbr || f_semi || f_comma || f_num || f_id) {
            //成功匹配继续贪心匹配...
            //设置：sourcetext_token
            sourcetext_token = (enum TokenType *) malloc(sizeof(enum TokenType));
            if(f_int) *sourcetext_token = INT;
            else if(f_char)   *sourcetext_token = CHAR;
            else if(f_void)   *sourcetext_token = VOID;
            else if(f_if)     *sourcetext_token = IF;
            else if(f_else)   *sourcetext_token = ELSE;
            else if(f_while)  *sourcetext_token = WHILE;
            else if(f_return) *sourcetext_token = RETURN;
            else if(f_eq)     *sourcetext_token = EQ;
            else if(f_ne)     *sourcetext_token = NE;
            else if(f_assign) *sourcetext_token = ASSIGN;
            else if(f_plus)   *sourcetext_token = PLUS;
            else if(f_minus)  *sourcetext_token = MINUS;
            else if(f_star)   *sourcetext_token = STAR;
            else if(f_div)    *sourcetext_token = DIV;
            else if(f_lt)     *sourcetext_token = LT;
            else if(f_gt)     *sourcetext_token = GT;
            else if(f_and)    *sourcetext_token = AND;
            else if(f_or)     *sourcetext_token = OR;
            else if(f_lpar)   *sourcetext_token = LPAREN;
            else if(f_rpar)   *sourcetext_token = RPAREN;
            else if(f_lbr)    *sourcetext_token = LBRACE;
            else if(f_rbr)    *sourcetext_token = RBRACE;
            else if(f_semi)   *sourcetext_token = SEMICOLON;
            else if(f_comma)  *sourcetext_token = COMMA;
            else if(f_num)    *sourcetext_token = NUM;
            else if(f_id)     *sourcetext_token = ID;
            perior++;
        }else{
            //没有任何一个符合
            if(sourcetext_token==nullptr){
                if(sourcetext[perior]==' '){
                    perior++;
                    last = perior;
                }else{
                    printf("%s","error");
                    break;
                }
            }else{
                //前面那个有，但本次不匹配，则加入到token流。
                char* sub1 = extract_substring(sourcetext, last, perior-1);
                struct TokenStr * TokenStr_temp = (struct TokenStr *)malloc(sizeof(struct TokenStr));
                TokenStr_temp->str = sub1;
                TokenStr_temp->type = *sourcetext_token;
                Tokenstr[++Tokenstr_index] = *TokenStr_temp;
                free(sourcetext_token);
                sourcetext_token = nullptr;
                
                if(sourcetext[perior]==' '){
                    //如果是空格跳过空格
                    perior++;
                    last = perior;
                }else{
                    //如果是普通
                    last = perior;
                }
            }
        }
        free(sub);
    }
    printf("token流数量：%d\n",Tokenstr_index);
    for(int i = 0 ; i <= Tokenstr_index ; i++){        
        printf("【%s】",Tokenstr[i].str);
        printf("【%d】\n",Tokenstr[i].type);
    }
    

    return -1;
}