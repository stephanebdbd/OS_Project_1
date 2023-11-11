#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork
#include <sys/wait.h>   // wait
#include <string.h>     // strcmp & strlen
#include <stdlib.h>     // exit
#include <sys/mman.h>   // mmap
#include <signal.h>     // signaux

#define READ 0
#define WRITE 1

struct image{char chemin[1000]; int distance;};

int comparaison(const char* img1, const char* img2){
   int status;
   pid_t pid = fork();
   if (pid == -1){
      perror("fork");
      exit(1);
   }
   if (pid == 0){
      if (execlp("./img-dist/img-dist", "-v", img1, img2, NULL) == -1) {
         perror("execlp");
         exit(1);
      }
   }
   else if (pid > 0){
      wait(&status);
      if (WIFEXITED(status)){
         return WEXITSTATUS(status);
      }
      else{
         perror("wait");
         exit(1);
      }
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

void read_pipe(int pipe1[2], int pipe2[2], const char* imgPath, struct image* best) {
      close(pipe1[WRITE]);
      close(pipe2[READ]);
      close(pipe2[WRITE]);
      char chemin_recu[1000];
      while (read(pipe1[READ], &chemin_recu, sizeof(chemin_recu))){
         int distance = comparaison(imgPath, chemin_recu);
         if (best->distance > distance){
            strcpy(best->chemin, chemin_recu);
            best->distance = distance;
         }
      }
      close(pipe1[READ]);
}

void traitement(struct image* best, const char* imgPath, int *taille) {
   int pipe1[2], pipe2[2];
   pid_t child1, child2;
   if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
      perror("Erreur");
      exit(1);
   }
   child1 = fork();
   if (child1 == -1){
      perror("fork()");
      exit(1);
   }
   if (child1 == 0){
      read_pipe(pipe1, pipe2, imgPath, best);
      exit(0);
   }
   else {
   child2 = fork();
   if (child2 == -1){
      perror("fork()");
      exit(1);
      }
   }
   if (child2 == 0) {
      read_pipe(pipe2, pipe1, imgPath, best);
      exit(0);
   }
   close(pipe1[READ]);
   close(pipe2[READ]);
   char chemin[1000];
   while (fgets(chemin, sizeof(chemin), stdin) != NULL){
      chemin[strlen(chemin)-1] = '\0';
      if (*taille % 2 == 0){
         write(pipe1[WRITE], &chemin, sizeof(chemin));
      }
      else {
         write(pipe2[WRITE], &chemin, sizeof(chemin));
      }
      ++*taille;
   }
   close(pipe1[WRITE]);
   close(pipe2[WRITE]);
   wait(NULL);
   wait(NULL);
}

void *create_shared_memory(size_t size) {
  const int protection = PROT_READ | PROT_WRITE;
  const int visibility = MAP_SHARED | MAP_ANONYMOUS;
  return mmap(NULL, size, protection, visibility, -1, 0);
}

int main(int argc, char* argv[]) {
   const char* imgPath = argv[argc-1];
   struct image *best = create_shared_memory(sizeof(struct image));
   if (best == MAP_FAILED) {
      perror("mmap");
      exit(1);
   } best->distance = 65;
   int taille=0;
   traitement(best, imgPath, &taille);
   if (taille == 0 || best->distance > 64) {
      printf("No similar image found (no comparison could be performed successfully).\n");
      return 1;
   }
   printf("Most similar image found: '%s' with a distance of %d.\n", best->chemin, best->distance);
   if (munmap(best, sizeof(int)) == -1) {
      perror("munmap");
      exit(1);
   }
   signal(SIGPIPE, handler);
   signal(SIGINT, handler);
   return 0;
}