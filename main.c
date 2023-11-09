#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork
#include <sys/wait.h>   // wait
#include <string.h>     // strcmp & strlen
#include <stdlib.h>     // exit

#define READ 0
#define WRITE 1

int main(int argc, char* argv[]) {
   char* imgPaths[1];
   imgPaths[0] = argv[0];
   if (argc == 2 && strcmp(argv[0], "-v") != 0 && strlen(argv[1]) < 1000){
      imgPaths[0] = argv[1];
   }
   else {
      return 0;
   }
   
   char buffer[1024][100];

   int taille = 0;
   while (fgets(buffer[taille], sizeof(buffer[taille]), stdin) != NULL){
      taille++;
   } 
   
   if (taille==0){
      printf("No similar image found (no comparison could be performed successfully).\n");
      return 1;
   }

   return 0;
}
