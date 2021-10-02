#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"


// function creates child process for primenumber
// parent: file descriptor - read side of the pipe
void
prime(int parent)
{
  int i ,p, num, child[2], pid = 0;

  // close all unnecessary files for process
  for(i = 3; i < NFILE; i++){
    if(i != parent)
      close(i);
  }

  read(parent, &p, sizeof(int));
  printf("prime %d\n", p);
  while(read(parent, &num, sizeof(int))){
    if(num % p != 0){
      if(!child[1]){ // if child process doesn't already exist, create one 
        pipe(child);
        pid = fork();
      }
      if(pid == 0){
        prime(child[0]);
      } else {
        write(child[1], &num, sizeof(int));
      }
    }
  }
  close(child[1]);
  close(child[0]);
  wait(0);
  exit(0);
}


// function starts main process of making pipeline
void
primes()
{
  int i, pid, child[2];
  pipe(child);
  pid = fork();
  if(pid == 0){
    prime(child[0]);
  } else {
    for(i = 2; i < 36; i++){
      write(child[1], &i, sizeof(int));
    }
    close(child[1]);
    close(child[0]);
    wait(0);
  }
}

int
main(int argc, char **argv)
{
  primes();
  exit(0);
}
