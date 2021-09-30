#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

// function executes combined `argv` and `argvxargs`
// argc: number of original arguments
// argv: original arguments
// argcxargs: number of additional arguments
// argvxargs: additional arguments
void
xargs(int argc, char *argv[], int argcxargs, char *argvxargs[])
{
  int i, j = 0;
  char *fin[argc + argcxargs - 1];
  for(i = 1; i < argc; i++){
    fin[j] = argv[i];
    j++;
  }
  for(i = 0; i < argcxargs; i++){
    fin[j] = argvxargs[i];
    j++;
  }

  int pid = fork();
  if(pid == 0){
    exec(fin[0], fin);
    exit(0);
  } else {
    wait(0);
  }
}

// function splits line on spaces and writes separate strings to `argvxargs`
// line: string, that needs to be splitted
// len: length of line
// argvxargs: pointer to array of strings
// returns: number of strings in `argvxargs`
int
make_argv(char *line, int len, char *argvxargs[])
{
  int i, argcxargs = 0;
  char *begin = line;
  for(i=0; i<len; i++){
    if(line[i] == ' '){
      line[i] = 0;
      argvxargs[argcxargs] = begin;
      argcxargs++;
      begin = line + i + 1;
    }
  }
  argvxargs[argcxargs] = begin;
  argcxargs++;
  argvxargs[argcxargs] = 0;
  argcxargs++;
  return argcxargs;
}

int
main(int argc, char *argv[])
{
  char buf[512], line[512];
  int i = 0, n, index = 0, argcxargs;
  char *argvxargs[MAXARG - argc + 1]; // max additional arguments for exec

  // read from pipe until all is read, save characters to `line`
  while((n = read(0, buf, sizeof(buf))) > 0){
    for(i=0; i < n; i++){
      if(buf[i] == '\n'){
        line[index] = 0;
        argcxargs = make_argv(line, strlen(line), argvxargs);
        xargs(argc, argv, argcxargs, argvxargs);
        index = 0;
        memset(line,0,sizeof(line));
        memset(argvxargs,0,sizeof(argvxargs));
      } else {
        line[index] = buf[i];
        index++;
      }
    }
  }  
  exit(0);
}
