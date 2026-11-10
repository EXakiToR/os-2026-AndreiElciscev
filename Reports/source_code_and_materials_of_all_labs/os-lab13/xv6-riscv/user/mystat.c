#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char *
typename(int t)
{
  if(t == T_DIR)
    return "directory";
  if(t == T_FILE)
    return "file";
  if(t == T_DEVICE)
    return "device";

  return "?";
}

int
main(int argc, char *argv[])
{
  struct stat st;

  for(int i = 1; i < argc; i++){
    if(stat(argv[i], &st) < 0){
      fprintf(2, "stat: cannot stat %s\n", argv[i]);
      continue;
    }

    printf("%s: %s, inode %d, links %d, size %ld bytes\n",
           argv[i], typename(st.type), st.ino, st.nlink, (long)st.size);
  }

  exit(0);
}
