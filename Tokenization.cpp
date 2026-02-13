#include <iostream>
typedef struct Token_Stream{
    char * name;
} Token_Stream;

struct Graph{
    int *** i; //[0][0][0] 状态,转移字符,状态
    int status_num; // 这个是构造算法动态来的。
}; 

//如果in的优先级大于stack返回真
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

//Regular To NFA
char * RegularToPostfix(char * regular,int size){
    //1、转为后缀表达式
    char * newstr = (char *)malloc(sizeof(char) * size);
    char * str_stack = (char *)malloc(sizeof(char) * size);
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
                    continue;
                }
                if(curr==')'){
                    while(str_stack[top]!='('){
                        newstr[index1++] = str_stack[top];
                        top--;
                    }
                    continue;
                }
                char top_char = str_stack[top];
                //看看优先级如何如果优先级高就入栈。
                //看看优先级低就将栈中的元素都出栈以后再入栈。
                bool is = Periority(curr,top_char);
                if(is){
                    str_stack[++top] = curr;
                }else{
                    while(top!=-1){
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
void graph_bianli(struct Graph *graph, char * ww1 ,int start, int end, int index) {
    if (flag) return; // 已经找到路了，直接收工

    // 成功条件：到达终点且字符串刚好读完 ('\0')
    if (start == end && ww1[index] == '\0') {
        flag = 1;
        return;
    }

    for (int j = 0; j < 1000; j++) {
        if (graph->i[start][126][j] == 1) {
            graph_bianli(graph, ww1 ,j, end, index); 
            if (flag) return;
        }
    }

    char c = ww1[index];
    if (c != '\0') { // 如果字符串没读完
        for (int j = 0; j < 1000; j++) {
            // 检查 start 状态是否有对应字符 c 的边
            if (graph->i[start][(int)c][j] == 1) {
                graph_bianli(graph,ww1 ,j, end, index + 1);
                if (flag) return;
            }
        }
    }
}

int main(){
    //b.a|b
    char * s = RegularToPostfix("s.s.s.s.s",9);
    printf("postfix:%s\n",s);
    //转换为NFA
    struct Graph * o = Graph_init();
    struct Item b = PostfixToNFA(o,s);
    if(b.start!=-1){
        printf("success create NFA\n");
    }else{
        printf("fail to create NFA\n");
    }
    int start = b.start;
    int end = b.end;
    printf("起始状态%d\n",start);
    char input[100];
    while(true){
        if(input == "~"){
            break;
        }else{
            scanf("%s",input);
            graph_bianli(o,input,start,end,0);
            if(flag == 1){
                printf("legal str\n");
                flag = 0;
            }else{
                printf("illegal str\n");
                flag = 0;
            }
        }
    }
    

    return -1;
}