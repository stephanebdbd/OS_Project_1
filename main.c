#include <stdio.h>      // printf, perror
#include <sys/types.h>  // pid_t
#include <unistd.h>     // fork

#define MAX_IMAGES 100      


void ListeimageBank(FILE searim, char* images[], int* numImages) {
   char imagePath[MAX_PATH_LENGTH];
   int i = 0;
   while (i < MAX_IMAGES && fgets(imagePath, MAX_PATH_LENGTH, searim) != NULL) {//fgets pour lire sur linput le fichier
      size_t len = strlen(imagePath);
      if (imagePath[len - 1] == '\n') {//enleve saut de ligne  en /0
         imagePath[len - 1] = '\0';
      }
      images[i] = imagePath;
      i++;
   }
   *numImages = i;
}


int main(int argc, char* argv[]) {
   FILE searim = argv[1];
   char* images[MAX_IMAGES];
   int numImages = 0;
   ListeimageBank( searim,images, &numImages);

   int mempartage ;
   mempartage = shmget(IPC_PRIVATE, numImages, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR );//creer la memeoire partagees
   if (mempartage == -1) {
      perror("shmget");
      return 1;
   }



   pid_t child1 = fork();
   if (child1 > 0) {  // Père
    printf("Je suis le père.\n");
    } else if (child1 == 0) {  // Fils
    printf("Je suis le fils.\n");
   } else {             // Erreur
    perror("fork()");  // Affiche sur stderr "fork(): <description de l'erreur>"
    return 1;
   }
   pid_t child2 = fork();
   if (child2 > 0) {  // Père
    printf("Je suis le père.\n");
   } else if (child2 == 0) {  // Fils
    printf("Je suis le fils.\n");
   } else {             // Erreur
    perror("fork()");  // Affiche sur stderr "fork(): <description de l'erreur>"
    return 1;
   }
}
