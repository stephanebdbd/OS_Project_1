#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork
#include <sys/wait.h>   // wait
#include <string.h>     // strcmp & strlen
#include <stdlib.h>     // exit
#include <sys/mman.h>   // mmap
#include <signal.h>     // signaux
#include <sys/types.h>
#include <signal.h>

#define READ 0
#define WRITE 1

struct image{
   char* chemin;
   int distance;
};

int comparaison(const char* img, const char* imgcompare){
    int chem = 65, status;
    pid_t pid = fork();
    if (pid == 0){
        if (execl("./img-dist", "img-dist", "-v", img, imgcompare, NULL) == -1) {
            perror("execl");
            exit(1);
        }
    }
    else if (pid > 0){
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)){
        chem = WEXITSTATUS(status);
        printf("%d\n", chem);
        return chem;
        }
        else{
            perror("wait");
            exit(1);
        }
    }
    else{
        perror("fork");
        exit(1);
    }
    return 0;
}

void handler(int signal){
   if (signal == SIGINT){
      perror("Fermeture des programmes");
      exit(0);
   }
   else if (signal == SIGPIPE){
      perror("Fermeture du pipe");
      exit(0);
   }
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
   
   int n = 1024;
   const int protection = PROT_READ | PROT_WRITE;
   // MAP_SHARED : Partager avec ses enfants
   // MAP_ANONYMOUS : Ne pas utiliser de fichier
   const int visibility = MAP_SHARED | MAP_ANONYMOUS;

    // Mapper la mémoire partagée dans l'espace d'adressage du processus
   
   struct image *partage = (struct image *)mmap(NULL, n * sizeof(struct image), protection, visibility, -1, 0);
   if (partage == MAP_FAILED) {
      perror("mmap");
      exit(1);
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
   
   int nb_img=0;
   if (child1 == 0){
      close(pipe1[WRITE]);
      close(pipe2[READ]);
      close(pipe2[WRITE]);
      char chemin_recu[100];
      while (read(pipe1[READ], &chemin_recu, sizeof(chemin_recu))){
         partage[nb_img].distance = comparaison(imgPath, chemin_recu);
         partage[nb_img].chemin = chemin_recu;
         nb_img++;

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
         partage[nb_img].distance = comparaison(imgPath, chemin_recu);
         partage[nb_img].chemin = chemin_recu;
         nb_img++;
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

   struct image meilleur = {partage[0].chemin, partage[0].distance};
   printf("Valeur à l'index %d : %d au chemin %s\n", 1, partage[0].distance, partage[0].chemin);
   // Afficher les valeurs depuis la mémoire partagée
   for (int i = 1; i < taille; i++) {
      printf("Valeur à l'index %d : %d au chemin %s\n", i+1, partage[i].distance, partage[i].chemin);
      if (meilleur.distance > partage[i].distance){
         meilleur=partage[i];
      }
   }

   // Unmapping et nettoyage
   if (munmap(partage, n * sizeof(int)) == -1) {
      perror("munmap");
      exit(1);
   }

   signal(SIGPIPE, handler);
   signal(SIGINT, handler);

   printf("Most similar image found: %s with a distance of %d\n.", meilleur.chemin,meilleur.distance);
   return 0;
}
