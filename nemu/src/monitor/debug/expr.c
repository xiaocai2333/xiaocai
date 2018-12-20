#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

bool check_parentheses(int p, int q);
int eval(int p, int q);
int OP_CET(int p,int q);
int COMPARE_OPERATOR(int ope1, int ope2);
bool IsOPerINBRA(int p);

enum {
  TK_NOTYPE = 256, 
  TK_QADDR = 100,
  TK_PLUS = 101,
  TK_REDUCE = 102,
  TK_RIDE = 103,
  TK_EXCEPT = 104,
  TK_MODE = 105,

  TK_CONSTANT = 666,

   TK_EQ,

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\&", TK_QADDR},
  {"\\+", TK_PLUS},         // plus
  {"\\-", TK_REDUCE},         //reduce
  {"\\*", TK_RIDE},         //ride
  {"\\/", TK_EXCEPT},         //except
  {"\\%", TK_MODE},
  {"\\(", '('},         //left brackets
  {"\\)", ')'},         //right brackets
  {"[0-9]+", TK_CONSTANT},
  {"==", TK_EQ}         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        assert(pmatch.rm_eo < 32);
        tokens[nr_token].type = rules[i].token_type;

        memset((void *)(tokens[nr_token].str),0,sizeof(Token)*32);
        memcpy((void *)(tokens[nr_token].str),(void *)(e + position - substr_len),pmatch.rm_eo);

        nr_token++;
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        /*switch (rules[i].token_type) {
          default: TODO();
        }*/

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  int sum = 0;
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  else{
    printf("%d\n", nr_token);
    for(int i = 0; i < nr_token; i++){
      printf("%s\n", tokens[i].str);
    }
    sum = eval(0,nr_token - 1);
    printf("result = %d\n", sum);
  }
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();

  return 0;
}



bool check_parentheses(int p, int q){

  int BRA_NUM = 0;

  if(strcmp(tokens[p].str,"(")){
      return false;
  }
  BRA_NUM = 1;

  for(int i = p + 1; i <= q; i++){
    
    if(!strcmp(tokens[i].str,"(")){
      BRA_NUM++;
    }

    else if(!strcmp(tokens[i].str,")")){
      BRA_NUM--;
      if((BRA_NUM < 1) && (i != q)){
        return false;
      }
    }

    

  }
  return true;
}


int eval(int p, int q){
  int op;
  int val1, val2;;
  if(p > q){
    printf("This expression is Bad expression\n");
    return 0;
  }
  else if(p == q){
    if(tokens[p].type != 666){
      //*success = false;
      return 0;
    }
    else{
      return atoi(tokens[p].str);
    }
  }
  else if(check_parentheses(p,q) == true){
    
    p = p + 1;
    q = q - 1;
    printf("p = %d, q = %d\n", p, q);
    //return 0;
    return eval(p, q);
  }
  else{
    printf("p = %d , q = %d\n", p, q);
    op = OP_CET(p,q);
    printf("str =  %s\n", tokens[op].str);
    printf("op = %d\n", op);
    if(p == 1){
      return 0;
    }
    val1 = eval(p, op - 1);
    val2 = eval(op + 1, q);

    printf("val1 = %d, val2 = %d\n", val1, val2);
    switch(tokens[op].type){
      case 101: return val1 + val2;
      case 102: return val1 - val2;
      case 103: return val1 * val2;
      case 104: return val1 / val2;
      case 105: return val1 % val2;

      default: return 0;
    }
  }
}
int x = 0;
int OP_CET(int p, int q){
  int k = 0;
  int oper[nr_token];
  x++;
  printf("%d\n", x);
  if(x >= 3){

    return 0;
  }
  for(int i = p; i <= q; i++){
    if(tokens[i].type >= 100 && tokens[i].type <= 105){
      if(IsOPerINBRA(i)){
        oper[k] = i;
        k++;
      }
    }
  }
  printf("oper = %d\n", oper[0]);
  if(k == 1){
    return oper[0];
  }
  for(int j = 1; j < k; j++){
    if(COMPARE_OPERATOR(oper[0],oper[j]) != -1){
      oper[0] = oper[j];
    }
  }
  return oper[0];
}



int COMPARE_OPERATOR(int ope1, int ope2){
  if(!strcmp(tokens[ope1].str,"&")){
    if(strcmp(tokens[ope2].str,"&"))
      return 1;
    return 0;
  }

  else if(!(strcmp(tokens[ope1].str,"/")) || !(strcmp(tokens[ope1].str, "*")) || !(strcmp(tokens[ope1].str, "%"))){
    if(!(strcmp(tokens[ope2].str, "&"))){
      return -1;
    }
   else if(!(strcmp(tokens[ope2].str,"/")) || !(strcmp(tokens[ope2].str, "*")) || !(strcmp(tokens[ope2].str, "%"))){
    return 0;
   }
   return 1;
  } 

  else if(!(strcmp(tokens[ope1].str,"+")) || !(strcmp(tokens[ope1].str, "-"))){
    if(!(strcmp(tokens[ope2].str,"/")) || !(strcmp(tokens[ope2].str, "*")) || !(strcmp(tokens[ope2].str, "%")) || !(strcmp(tokens[ope2].str,"&"))){
      return -1;
    }
    else if(!(strcmp(tokens[ope2].str,"+")) || !(strcmp(tokens[ope2].str, "-"))){
      return 0;
    }
    return 1;
  }
  return 1;
}


bool IsOPerINBRA(int p){
  int flag = 0;
  for(int i = p; i >= 0; i--){
    if(!strcmp(tokens[i].str,"(")){
      flag++;
    }
    else if(!strcmp(tokens[i].str,")")){
      flag--;
    }
    if(flag >= 1){
      return false;
    }
  }
  return true;
}