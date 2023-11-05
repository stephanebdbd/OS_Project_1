#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork
#include <sys/wait.h>   // wait
#include <string.h>     // strcmp & strlen


int main(int argc, char* argv[]) {
   char* imgPaths[1];
   imgPaths[0] = argv[0];
   if (argc == 2 && strcmp(argv[0], "-v") != 0 && strlen(argv[1]) < 1000){
      imgPaths[0] = argv[1];
      if (argv[1][-1] != '\0'){
         imgPaths[0][strlen(argv[1])] = '\0';
      }
   }
   else {
      return 0;
   }


   char buffer[256][100]; int i = 0;
   while (fgets(buffer[i], sizeof(buffer[i]), stdin) != NULL){
      i++;
      }
   for (int j = 0; j<i; j++) {
      printf("Fichier n°%d : %s", j, buffer[j]);
   }


   pid_t child1, child2;

   child1 = fork();
   if (child1 == 0) {      // Fils
      //code à exécuter pour le fils
      return 0;
   }
   else {                  // Erreur
      perror("fork()");    // Affiche sur stderr "fork(): <description de l'erreur>"
      return 1;
   }

   child2 = fork();
   if (child2 == 0) {      // Fils
      // code à exécuter pour le fils
      return 0;
   }
   else {                  // Erreur
      perror("fork()");    // Affiche sur stderr "fork(): <description de l'erreur>"
      return 1;
   }

   // code à exécuter pour le père

   return 0;
   }