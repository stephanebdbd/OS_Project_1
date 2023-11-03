#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork
#include <string.h>



int main(int argc, char* argv[]) {
   char* imgPaths[2];
   int c=0;
   for (int i=1; i < argc; i++){
      if (strcmp(argv[i], "-v") != 0 && strlen(argv[i]) < 1000){
         imgPaths[c] = argv[i];
         if (argv[i][-1] != '\0'){
            imgPaths[c][strlen(argv[i])] = '\0';
         }
         c++;
      }
   }
   return 0;
}
