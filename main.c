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

   pid_t child1, child2;

   child1 = fork();
   if (child1 == 0) {      // Fils
      printf("processus fils 1 créé. PID : %d\n", getpid());  //code à exécuter pour le fils
      return 0;
   }
   else if (child1 <0){                  // Erreur
      perror("fork()");    // Affiche sur stderr "fork(): <description de l'erreur>"
      return 1;
   }

   child2 = fork();
   if (child2 == 0) {      // Fils
      printf("processus fils 2 créé. PID : %d\n", getpid());  //code à exécuter pour le fils
      return 0;
   }
   else if (child2 < 0){                  // Erreur
      perror("fork()");    // Affiche sur stderr "fork(): <description de l'erreur>"
      return 1;
   }
   
   char buffer[256][100]; int i = 0;
   while (fgets(buffer[i], sizeof(buffer[i]), stdin) != NULL){
      buffer[i][strlen(buffer[i])-1] = '\0';
      if (access(buffer[i], F_OK) != -1){
         i++;
      }
      else {
         if (strlen(buffer[i]) == 0) {
            break;
         }
         buffer[i][0] = '\0';
      }
   }
   for (int j = 0; j<i; j++) {
      printf("Fichier n°%d : %s\n", j+1, buffer[j]);
   }

   printf("\n");
   

   // code à exécuter pour le père : répartition des images, comparaison en exécution des processus concurrente
   printf("PID du processus père : %d\n", getppid());

   return 0;
   }
