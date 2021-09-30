#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// function takes string `path` and parses file name from it
// path: string containing path
// returns: substring of `path`, which is file name
char*
getname(char *path)
{
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  return p++;
}

// function takes string `path` and prints paths to files with same name as argument `fname` (also searches in subdirectories)
// path: string containing path
// fname: file name
void
find(char* path, char* fname)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  switch(st.type){
  case T_FILE:
    if(strcmp(getname(path), fname) == 0){
      printf("%s\n", path);
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      if(st.type == T_FILE && strcmp(de.name, fname) == 0){
        printf("%s/%s\n", path, de.name);
        continue;
      } else if(st.type == T_DIR){
        find(buf, fname);
        continue;
      }
      
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc != 3){
    printf("usage: find <path> <file name>\n");
    exit(-1);
  }
  find(argv[1], argv[2]);
  
  exit(0);
}
