#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork
#include <sys/wait.h>   // wait
#include <string.h>     // strcmp & strlen
bool bon_format(const char *buffer) {
    int longueur = strlen(buffer);

    // Vérifier si la chaîne commence par "./img/"
    if (longueur < 8 || strncmp(buffer, "./img/", 6) != 0) {
        return false;
    }

    // Vérifier si la chaîne se termine par ".bmp" (longueur totale doit être au moins 11)
    if (longueur < 11 || strcmp(buffer + longueur - 4, ".bmp") != 0) {
        return false;
    }

    // Extraire la valeur entre "img/" et ".bmp" et la convertir en entier
    char valeur[4];
    strncpy(valeur, buffer + 6, longueur - 10);
    valeur[longueur - 10] = '\0';

    // Convertir la chaîne en entier de manière personnalisée
    int entier = 0;
    for (int i = 0; valeur[i] != '\0'; i++) {
        if (valeur[i] >= '0' && valeur[i] <= '9') {
            entier = entier * 10 + (valeur[i] - '0');
        } else {
            return false; // Si un caractère n'est pas un chiffre, ce n'est pas un entier
        }
    }

    // Vérifier si la valeur est entre 0 et 100
    if (entier < 0 || entier > 100) {
        return false;
    }

    return true;  // Le format est valide
}

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
   
   printf("\n");

   char buffer[256][100]; int i = 0;
   while (fgets(buffer[i], sizeof(buffer[i]), stdin) != NULL){
      if (bon_format(buffer[i])==true){
         printf("%s", buffer[i]); i++;
      }else{
         printf("mauvais format")
      }
      
   }
   for (int j = 0; j<i; j++) {
      printf("Fichier n°%d : %s", j+1, buffer[j]);
   }

   printf("\n");
   

   // code à exécuter pour le père : répartition des images, comparaison en exécution des processus concurrente
   printf("PID du processus père : %d\n", getppid());

   return 0;
   }
