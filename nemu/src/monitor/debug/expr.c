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
bool IsOPERINBRA(int p, int j);
bool IsOPERTRUE(int p, int q);
bool IsPOINT(int p);
uint32_t vaddr_read(paddr_t addr, int len);

enum {
  TK_NOTYPE = 256, 
  //TK_NEG = 99,

  TK_QADDR = 100,
  TK_PLUS = 101,
  TK_REDUCE = 102,
  TK_RIDE = 103,
  TK_EXCEPT = 104,
  TK_MODE = 105,

  TK_DEX = 10,
  TK_HEX = 16,

  TK_REG = 200,

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
  //{"-[0-9]+", TK_NEG},
  {"\\&", TK_QADDR},
  {"\\+", TK_PLUS},         // plus
  {"\\-", TK_REDUCE},         //reduce
  {"\\*", TK_RIDE},         //ride
  {"\\/", TK_EXCEPT},         //except
  {"\\%", TK_MODE},
  {"\\(", '('},         //left brackets
  {"\\)", ')'},         //right brackets
  {"0(x|X)([0-9a-fA-F])+", TK_HEX},
  {"[0-9]+", TK_DEX},
  {"\\$[a-zA-Z]+", TK_REG},

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
        if(rules[i].token_type == TK_NOTYPE){

        }
        else{
          assert(pmatch.rm_eo < 32);
          tokens[nr_token].type = rules[i].token_type;
          memset((void *)(tokens[nr_token].str),0,sizeof(Token)*32);
          memcpy((void *)(tokens[nr_token].str),(void *)(e + position - substr_len),pmatch.rm_eo);

          nr_token++;
        }
        
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
    for(int i = 0; i < nr_token; i++){
      if(tokens[i].type == 103 && (i == 0 || (tokens[i - 1].type >= 100 && tokens[i - 1].type <= 105) || tokens[i - 1].type == 40)){
        tokens[i].type = 80;
      }
      else if(tokens[i].type == 102 && (i == 0 || (tokens[i - 1].type >= 100 && tokens[i - 1].type <= 105) || tokens[i - 1].type == 40)){
        tokens[i].type = 99;
      }
    }
    if(IsOPERTRUE(0,nr_token -1)){
      sum = eval(0,nr_token - 1);
      printf("result = %d(DEX)     %x(HEX)\n", sum, sum);
    }

    else{
      printf("This expression is Bad expression\n");
      return 0;
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


int eval(int p, int q){
  int op;
  int val1, val2;;
  int h_num = 0;
  if(p > q){
    printf("This expression is Bad expression\n");
    return 0;
  }
  else if(p == q){
    if(tokens[p].type != 10 && tokens[p].type != 16 && tokens[p].type != 200){
      //*success = false;
      return 0;
    }
    else{
      if(tokens[p].type == 10){
        return atoi(tokens[p].str);
      }
      else if(tokens[p].type == 16){
        sscanf(tokens[p].str, "%x", &h_num);
        return h_num;
      }
      else if(tokens[p].type == 200){
        if(!strcmp(tokens[p].str, "$eax")){
          return cpu.eax;
        }
        else if(!strcmp(tokens[p].str, "$ecx")){
          return cpu.ecx;
        }
        else if(!strcmp(tokens[p].str, "$edx")){
          return cpu.edx;
        }
        else if(!strcmp(tokens[p].str, "$ebx")){
          return cpu.ebx;
        }
        else if(!strcmp(tokens[p].str, "$esp")){
          return cpu.esp;
        }
        else if(!strcmp(tokens[p].str, "$ebp")){
          return cpu.ebp;
        }
        else if(!strcmp(tokens[p].str, "$eip")){
          return cpu.eip;
        }
        else if(!strcmp(tokens[p].str, "$esi")){
          return cpu.esi;
        }
        else if(!strcmp(tokens[p].str, "$edi")){
          return cpu.edi;
        }
        else{
          return 0;
        }
      }
    }
    return 0;
  }
  else if(check_parentheses(p,q) == true){
    return eval(p + 1, q - 1);
  }
  else{
    if(OP_CET(p, q) == -1){
      printf("This expression is Bad expression\n");
      return 0;
    }
    op = OP_CET(p,q);

    if(tokens[op].type == 80 || tokens[op].type == 99){
      val1 = 0;
      val2 = eval(op + 1, q);
    }
    else{
      val1 = eval(p, op - 1);
      val2 = eval(op + 1, q);
    }
    

    switch(tokens[op].type){
      case 101: return val1 + val2;
      case 102: return val1 - val2;
      case 103: return val1 * val2;
      case 104: return val1 / val2;
      case 105: return val1 % val2;
      case  80: return vaddr_read(val2,4);
      case  99: return val2 * -1;

      default: return 0;
    }
  }
}

int OP_CET(int p, int q){
  int k = 0;
  int oper[nr_token];
  oper[0] = 0;
  for(int i = p; i <= q; i++){
    if((tokens[i].type >= 100 && tokens[i].type <= 105) || tokens[i].type == 80 || tokens[i].type == 99){
      if(IsOPERINBRA(p, i)){
        oper[k] = i;
        k++;
      }
    }
  }
  if(k == 0){
    return -1;
  }
  else if(k == 1){
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
  if(tokens[ope1].type == 80 || tokens[ope1].type == 99){
    if(tokens[ope2].type != 80 || tokens[ope2].type == 99){
      return 1;
    }
    else{
      return 0;
    }

  }
  else if(tokens[ope1].type == 100){
    if(tokens[ope2].type == 80 || tokens[ope2].type == 99){
      return -1;
    }
    else if(tokens[ope2].type == 100){
      return 0;
    }
    else{
      return 1;
    }
  }
  else if(tokens[ope1].type >= 103 && tokens[ope1].type <= 105){
    if(tokens[ope2].type == 80 || tokens[ope2].type == 100 || tokens[ope2].type == 99 ){
      return -1;
    }
    else if(tokens[ope2].type >= 103 && tokens[ope2].type <= 105){
      return 0;
    }
    else{
      return 1;
    }
  }
  else if(tokens[ope1].type == 101 || tokens[ope1].type == 102){
    if(tokens[ope2].type == 101 || tokens[ope2].type == 102){
      return 0;
    }
    else{
      return -1;
    }
  }
  else{
    return 0;
  }
}


bool IsOPERINBRA(int p, int j){
  int flag = 0;
  for(int i = j; i >= p; i--){
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

bool IsOPERTRUE(int p, int q){
  int flag = 0;
  for(int i = p; i <= q; i++){
    if(!strcmp(tokens[i].str,"(")){
      flag++;
    }
    else if(!strcmp(tokens[i].str,")")){
      flag--;
    }
    if(flag < 0){
      return false;
    }
  }
  if(flag > 0)
    return false;
  return true;
}
