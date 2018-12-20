#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

bool check_parentheses(int p, int q);


enum {
  TK_NOTYPE = 256, 
 

  TK_CONSTANT,

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
  {"\\+", '+'},         // plus
  {"\\-", '-'},         //reduce
  {"\\*", '*'},         //ride
  {"\\/", '/'},         //except
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
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  else{

    if(check_parentheses(0,nr_token-1)){
      printf("choumianyang\n");
    }
    for(int i = 0; i < nr_token; i++){
      printf("%d     %s\n", tokens[i].type,tokens[i].str);
    }
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

/*
int eval(int p, int q){
  if(p > q){
    *success = false;
    printf("This expression is Bad expression");
  }
  else if(p == q){
    if(tokens[p].type != 257){
      *success = false;
    }
    else{
      return tokens[p].str;
    }
  }
  else if(check_parentheses(p,q) == true){

    return eval(p + 1, q - 1);
  }
  else{
    //op = "the position of zhuyunsuanfu in the token expression;"
    val1 = eval(p, op - 1);
    val2 = eval(op + 1, q);

    switch(op_type){
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
    }
  }

}

int OP_CET(int p){
  int q;
  for(int i = p; i < nr_token; i++){
    if(strcmp(tokens[i].str, "(")){
      q = i;
      break;
    }  
  }
}



int COMPARE_OPERATOR(int ope1, int ope2){
  if(!strcmp(tokens[ope1].str,"&")){
    if(strcmp(tokens[ope2].str,"&"))
      retrun 1;
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
}

*/