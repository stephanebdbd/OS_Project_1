#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork
#include <string.h>


int main(int argc, char* argv[]) {
   char* imgPaths[1];
   imgPaths[0] = argv[0];
   if (argc == 2 && strcmp(argv[0], "-v") != 0 && strlen(argv[1]) < 1000){
      imgPaths[0] = argv[1];
      if (argv[1][-1] != '\0'){
         imgPaths[0][strlen(argv[1])] = '\0';
      }
   } else {
      return 0;
   }
   return 0;
}
