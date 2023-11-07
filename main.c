#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <stdlib.h>     //exit
#include <unistd.h>     // fork
#include <sys/wait.h>   // wait
#include <string.h>     // strcmp & strlen


#define READ 0
#define WRITE 1

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
   
   char buffer[1024][100]; int i = 0;
   while (fgets(buffer[i], sizeof(buffer[i]), stdin) != NULL){
      i++;
   }
   
   if (i==0){
      printf("\nAucune image n'a été transmise.\nProgramme terminé.\n");
      return 1;
   }

   for (int j = 0; j<i; j++) {
      printf("Fichier n°%d : %s", j+1, buffer[j]);
   }


   int taille = sizeof(buffer) / sizeof(buffer[0]);

   int pipe1[2], pipe2[2];

   pid_t child1, child2;

   if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
      perror("Erreur");
      exit(1); // Échec
   }

   child1 = fork();
   if (child1 == -1) {
      perror("Erreur");
      exit(1); // Échec
   }

   if (child1 == 0) {
      // Code du premier processus enfant
      close(pipe1[WRITE]); // Fermez l'extrémité d'écriture du premier tube
      char chemin_recu[100];
      read(pipe1[READ], chemin_recu, sizeof(chemin_recu));
      printf("enfant 1 pid(%d) chemin : %s", getpid(), chemin_recu);
      close(pipe1[READ]); // Fermez l'extrémité de lecture du premier tube
      exit(0); // Succès
   }

   child2 = fork();
   if (child2 == -1) {
      perror("Erreur");
      exit(1); // Échec
   }

   if (child2 == 0) {
      // Code du deuxième processus enfant
      close(pipe2[WRITE]); // Fermez l'extrémité d'écriture du deuxième tube
      char chemin_recu[100];
      read(pipe2[READ], chemin_recu, sizeof(chemin_recu));
      printf("enfant 2  pid (%d) chemin : %s", getpid(), chemin_recu);
      close(pipe2[READ]); // Fermez l'extrémité de lecture du deuxième tube
      exit(0); // Succès
   }


   for (int i = 0; i < taille / 2; i++) {
      
      char chemin1[] = buffer[i];
      char chemin2[]= buffer[taille - i - 1];
      
      
      // Code du processus parent
      close(pipe1[READ]); // Fermez l'extrémité de lecture du premier tube
      close(pipe2[READ]); // Fermez l'extrémité de lecture du deuxième tube

      write(pipe1[WRITE], chemin1, strlen(chemin1) + 1);
      write(pipe2[WRITE], chemin2, strlen(chemin2) + 1);
      
      close(pipe1[WRITE]); // Fermez l'extrémité d'écriture du premier tube
      close(pipe2[WRITE]); // Fermez l'extrémité d'écriture du deuxième tube

      wait(NULL); // Attendez la fin des processus enfants
      wait(NULL);

      
   }



   return 0;
   }
