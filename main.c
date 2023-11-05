#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork
#include <sys/wait.h>   // wait
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
   pid_t pid = fork();
   if (pid==-1){
      perror("fork()");
      return 1;
   }
   else if (pid == 0){
      execlp("bash", "bash", "list-file", "./img", NULL);
      perror("execlp");
      return 1;
   }
   else{
      int status;
      waitpid(pid, &status, 0);
      }
   char buffer[100]; int i = 0;
   while (fgets(buffer, sizeof(buffer), stdin) != NULL){
      printf("%s test\n", buffer);
   }
   printf("%d\n", i);
   /*int fd[2]; // Stocker deux bouts de pipe
   pipe(fd);

   pid_t child1 = fork();
   if (child1 > 0) {  // Père
      close(fd[READ]); // ferme le descripteur de fichier en lecture
      int x = 5;
      write(fd[WRITE], &x, sizeof(int));// ecriture
    } else if (child1 == 0) {  // Fils
      close(fd[WRITE]); // ferme le descripteur de fichier en ecriture
      int y;
      read(fd[READ], &y, sizeof(int)); // lecture
   } else {             // Erreur
      perror("fork()");  // Affiche sur stderr "fork(): <description de l'erreur>"
      return 1;
   }
   pid_t child2 = fork();
   if (child2 == 0) {  // Fils
      close(fd[WRITE]); // ferme le descripteur de fichier en ecriture
      int y;
      read(fd[READ], &y, sizeof(int)); // lecture
   } else {             // Erreur
      perror("fork()");  // Affiche sur stderr "fork(): <description de l'erreur>"
      return 1;
   }*/
   return 0;
   }