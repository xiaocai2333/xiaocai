#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

void cpu_exec(uint64_t);

uint32_t paddr_read(paddr_t addr, int len);
//struct CPU_state regis;

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_si(char *args){
  if(args == NULL)
    cpu_exec(1);
  else
  {
    for (int i = 0; i < atoi(args); i++)
    {
      cpu_exec(1);
    }
  }
  return 0;
}

static int cmd_info(char *args){
  if(strcmp(args, "r") == 0){
    printf("eax: %x\n",cpu.eax);
    printf("ecx: %x\n",cpu.ecx);
    printf("edx: %x\n",cpu.edx);
    printf("ebx: %x\n",cpu.ebx);
    printf("esp: %x\n",cpu.esp);
    printf("ebp: %x\n",cpu.ebp);
    printf("esi: %x\n",cpu.esi);
    printf("edi: %x\n",cpu.edi);
    printf("eip: %x\n",cpu.eip);
  }
      
  return 0;
}

static int cmd_p(char *args){
  bool b = true;
  bool *a = &b;
  expr(args,a);
  return 0;
}

static int cmd_x(char *args){
  /*
  int n;
  
  char *p;
  n = atoi(strtok(NULL, " "));
  uint32_t addr;
  p = strtok(NULL, " ");
  sscanf(p,"%x",&addr);
  for(int i = 0; i < n; i++){
    printf("0x%x\t0x%x\n", addr + i*4,paddr_read(addr + i*4,4));
  }
  */
  int n = 1234567;
  printf("%x %x\n", n, paddr_read(7654321,4));
  return 0;
}

static int cmd_q(char *args) {

  return -1;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","Let the program execute N instructions step by step and then suspend execution.When N is not given, the default is 1", cmd_si},
  { "info","info r: Print register status; info w: Print Monitoring Point Information",cmd_info},
  { "p","Find the value of the expression EXPR",cmd_p},
  { "x","Find the value of the expression EXPR and take the result as the starting point.Deposit Address, output N consecutive 4 words in hexadecimal form section", cmd_x},

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))



static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
