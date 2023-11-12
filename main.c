#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork
#include <sys/wait.h>   // wait
#include <string.h>     // strcmp & strlen
#include <stdlib.h>     // exit
#include <sys/mman.h>   // mmap
#include <signal.h>     // signaux

#define READ 0          //lecture
#define WRITE 1         // ecriture

struct image{char chemin[1000]; int distance;}; // Struct de l'image pour la mémoire partagée qui stockera le chemin d'accès et la meilleure distance trouvée

int comparaison(const char* img, const char* img2){
   // Fonction qui va récupérer la distance des images en faisant appel à img-dist qui recevra les chemins d'accès des 2 images à comparer
   int status;
   pid_t pid = fork(); // Création du processus pour faire le execlp
   if (pid == -1){
      perror("fork");  // Erreur au cas où la création de processus ne fonctionne pas
      exit(1);
   }
   if (pid == 0){ // Appel de execlp pour faire 
      if (execlp("./img-dist/img-dist", "-v", img, img2, NULL) == -1) {
         perror("execlp");
         exit(1);
      }
   }
   else if (pid > 0){ // Si on est revenu au processus père
      wait(&status); // Attente de la fin du processus fils
      if (WIFEXITED(status)){ // On récupère les données de terminaison du processus fils
         return WEXITSTATUS(status);   // On retourne la distance comparée reçue via img-dist
      }
      else{ // Si wait ne fonctionne pas ça génère une erreur
         perror("wait");
         exit(1);
      }
   }
   return 0;
}

void handler(int signal){
   // Fonction qui capturera les signaux lors de l'exécution
   if (signal == SIGINT){  // Si l'utilisateur faire un CTRL+C
      printf("Fermeture des programmes");
      exit(0);
   }
   else if (signal == SIGPIPE){ // Si le pipe ne fonctionne pas
      printf("Fermeture du pipe");
      exit(0);
   }
   else { // Si le signal est inconnu ou pas géré par notre code
      printf("Signal non connnu : %d\n", signal);
   }
   return;
}

void read_pipe(int pipe1[2], int pipe2[2], const char* imgPath, struct image* best) {
   //descripeur de lecture on ferme tous les pipes dont on n'a pas besion et on ouvre le descripteur du pipe du fils
   //qu'on a besoin on appel la fonction comparaison et on stocke dans la memoire partage si la distance est meilleur que la valeur precedemment stockes
   //parmètres:
   //-int pipe1[2]:premier pipe
   //-int pipe2[2]:seconde pipe
   //-const char* imgPath:image a compare donne en parametre dans le main
   //struct image* best : la memoire partage
   close(pipe1[WRITE]);                                          //fermeture du descripteur d'ecriture du premier pipe 
   close(pipe2[READ]);                                           //fermeture du descripteur de lecture du second pipe 
   close(pipe2[WRITE]);                                          //fermeture du descripteur d'ecriture du second pipe 
   char chemin_recu[1000];                                       //fichier qui recevra les chemins d'accès par la lecture
   while (read(pipe1[READ], &chemin_recu, sizeof(chemin_recu))){ //lecture du chemin dans le pipe
      int distance = comparaison(imgPath, chemin_recu);          //appel de la fonction comparaison
      if (best->distance > distance){                            //stocke la valeur dans la memoire partages si elle est plus petite que celle deja stocke precedemment
         strcpy(best->chemin, chemin_recu);
         best->distance = distance;
      }
   }
   close(pipe1[READ]);                                           //fermeture du descripteur d'ecriture du second pipe 
}

void traitement(struct image* best, const char* imgPath, int *taille) {
   //creation des deux processus et utilisation du descripeur d'ecriture du processus pere de maniere concurrente entre chqun de fils
   //parmètres:
   //-struct image* best:la memoire partage
   //-const char* imgPath:image a compare donne en parametre dans le main
   //-int *taille: stocke le nombre d'images à comparer
   int pipe1[2], pipe2[2];                                //declaration des deux pipes pour chacun des fils
   pid_t child1, child2;                                 //declaration des deux pipes pour chacun des fils
   if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
      perror("Erreur");
      exit(1);
   }
   child1 = fork();                                      //creation du premier processus (fils1)
   // Vérification de la réussite de la création du processus fils1
   if (child1 == -1){
      perror("fork()");
      exit(1);                                           // Termine le programme avec un code d'erreur non nul
   }
   if (child1 == 0){
      read_pipe(pipe1, pipe2, imgPath, best);            // Appel d'une fonction pour lire à partir des tubes (pipes)
      exit(0);
   }
   else {
   child2 = fork();                                      //creation du premier processus (fils2)
   if (child2 == -1){                                    // Vérification de la réussite de la création du processus fils2
      perror("fork()");
      exit(1);                                           // Termine le programme avec un code d'erreur non nul
      }
   }
   if (child2 == 0) {
      read_pipe(pipe2, pipe1, imgPath, best);            // Appel d'une fonction pour lire à partir des tubes (pipes)
      exit(0);
   }
   close(pipe1[READ]);                                   //fermeture du descripteur de lecture du premier pipe 
   close(pipe2[READ]);                                   //fermeture du descripteur de lecture du second pipe
   char chemin[1000];                                    //creation d'un tableau pour stocke chaque chemin sui sont des chaines de caractere
   while (fgets(chemin, sizeof(chemin), stdin) != NULL){ //lecture de chaque ligne en entree standard
      chemin[strlen(chemin)-1] = '\0';                   //pour pouvoir fait sizeof et indique la fin de la chaine de caractere
      if (*taille % 2 == 0){
         write(pipe1[WRITE], &chemin, sizeof(chemin));   //fermeture du descripteur de lecture du second pipe 
      }
      else {
         write(pipe2[WRITE], &chemin, sizeof(chemin));   //fermeture du descripteur de lecture du second pipe 
      }
      ++*taille;                                         //incrementation de taille
   }
   close(pipe1[WRITE]);                                  //fermeture du descripteur  d'ecriture du prmier pipe
   close(pipe2[WRITE]);                                  //fermeture du descripteur  d'ecriture du second pipe
   wait(NULL);                                           //attend la fin du premier processus fils1
   wait(NULL);                                           //attend la fin du second processus fils2
}

void *create_shared_memory(size_t size) {
   //creation de la memoire partage
   //parmètres:
   //-size_t size:la taille en octets de la structure struct image qui sera la taille de la mémoire à allouer
   //return : la memoire partage
   const int protection = PROT_READ | PROT_WRITE;        // Définition des protections d'accès à la mémoire (lecture et écriture)
   const int visibility = MAP_SHARED | MAP_ANONYMOUS;    // Définition des options de visibilité et de partage de la mémoire (partagée et sans fichier)
   // Appel à mmap pour allouer une nouvelle région de mémoire partagée
   // - NULL : L'adresse de départ est automatiquement choisie par le système d'exploitation
   // - protection : Les protections d'accès à la mémoire (lecture et écriture)
   // - visibility : Les options de visibilité et de partage de la mémoire (partagée et sans fichier)
   // - -1 : Aucun descripteur de fichier n'est associé à la mémoire partagée
   // - 0 : Décalage dans le fichier à partir duquel mapper la mémoire (pas applicable dans ce cas)
   return mmap(NULL, size, protection, visibility, -1, 0);
}

int main(int argc, char* argv[]) {
   //parmètres:
   //int argc:nombre de parametre donne
   //char* argv[]:image a compare
   const char* imgPath = argv[argc-1];// image a compare
   struct image *best = create_shared_memory(sizeof(struct image)); // creation de la memoire partage
   if (best == MAP_FAILED) {
      // Vérifie si l'appel à mmap a échoué en comparant la valeur de retour à MAP_FAILED
      perror("mmap"); // Affiche un message d'erreur détaillé basé sur le code d'erreur actuel
      exit(1);        // Termine le programme avec un code d'erreur non nul
   } best->distance = 65;
   int taille=0;
   traitement(best, imgPath, &taille); 
   // Tout le traitement de des processus, des pipes, de la comparaison et de mettre le meilleur chemin avec la plus petite distance dans la memoire partage
   if (taille == 0 || best->distance > 64) {  // Vérifie si la taille est nulle ou si la distance est supérieure à 64
      printf("No similar image found (no comparison could be performed successfully).\n");
      return 1;                              // Termine le programme avec un code d'erreur indiquant aucune image similaire trouvée
   } // Si la taille n'est pas nulle et la distance est inférieure ou égale à 64, affiche les détails de l'image la plus similaire 
   printf("Most similar image found: '%s' with a distance of %d.\n", best->chemin, best->distance);
   if (munmap(best, sizeof(int)) == -1) {  // Libère la mémoire utilisée par la structure best en utilisant munmap
      perror("munmap");                    // Affiche un message d'erreur détaillé si munmap échoue
      exit(1);                             // Termine le programme avec un code d'erreur non nul en cas d'échec de munmap
   }
   signal(SIGPIPE, handler);
   signal(SIGINT, handler);
   return 0;
}