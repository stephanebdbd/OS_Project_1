#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork
#include <sys/wait.h>   // wait
#include <string.h>     // strcmp & strlen
#include <stdlib.h>     // exit

#define READ 0
#define WRITE 1

int comparaison(const char* img, const char* imgcompare){
   int status;
   int distance = -1;
   pid_t process = fork();
   if (process == -1){
      perror("fork()");
      exit(1);
   }

   int chem = execlp("img-dist", img, imgcompare, NULL);
   if (chem == -1) {
      perror("execlp");
      exit(1);
   }

   if (wait(&status)){
      distance = WEXITSTATUS(status);
      printf("Distance entre %s et %s : %d", img, imgcompare, distance);
   }
   else{
      perror("wait()");
      exit(1);
   }
   return distance;
}

int main(int argc, char* argv[]) {
   if (argc == 2 && strcmp(argv[0], "-v") != 0 && strlen(argv[1]) < 1000){
      const char* imgPaths[1] = argv[1];
   }
   else {
      printf("No similar image found (no comparison could be performed successfully).\n");
      return 1;
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

   int pipe1[2], pipe2[2];
   pid_t child1, child2;

   if (pipe(pipe1) == -1 || pipe(pipe2) == -1)
   {
      perror("Erreur");
      exit(1);
   }

   child1 = fork();
   if (child1 == -1){
      perror("fork()");
      exit(1);
   }
   

   if (child1 == 0){
      close(pipe1[WRITE]);
      close(pipe2[READ]);
      close(pipe2[WRITE]);
      char chemin_recu[100];
      while (read(pipe1[READ], &chemin_recu, sizeof(chemin_recu))){
//         printf("enfant 1 pid (%d) chemin : %s", getpid(), chemin_recu);
      }
      close(pipe1[READ]);
      exit(0);
   }

   else {
   child2 = fork();
   if (child2 == -1){
      perror("fork()");
      exit(1);
   }
   if (child2 == 0) {
      close(pipe2[WRITE]);
      close(pipe1[READ]);
      close(pipe1[WRITE]);
      char chemin_recu[100];
      while (read(pipe2[READ], &chemin_recu, sizeof(chemin_recu))){
//         printf("enfant 2 pid (%d) chemin : %s", getpid(), chemin_recu);
      }
      close(pipe2[READ]);
      exit(0);
      }
   }
   
   close(pipe1[READ]);
   close(pipe2[READ]);
   

   for (int i=0; i < taille; i++) {
      if (i % 2 == 0){
         write(pipe1[WRITE], &buffer[i], sizeof(buffer[i]));
      }
      else {
         write(pipe2[WRITE], &buffer[i], sizeof(buffer[i]));
      }
   }
   
   close(pipe1[WRITE]);
   close(pipe2[WRITE]);

   wait(NULL);
   wait(NULL);

   return 0;
}
