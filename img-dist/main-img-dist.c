#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"
#include "pHash.h"
#include "verbose.h"

#define DEFAULT_PATH "img/6-1.bmp"

enum { EXIT_ERROR_MISSING_ARGS = 65, EXIT_ERROR_PATH_1, EXIT_ERROR_PATH_2 };

static void ProcessOptions(char* imgPaths[2], int argc, char* argv[]);

int main(int argc, char* argv[]) {
   RgbImage image1, image2;
   char* imgPaths[2];
   
   ProcessOptions(imgPaths, argc, argv);
   
   if (!LoadBmp(&image1, imgPaths[0]))
      return EXIT_ERROR_PATH_1;
   if (!LoadBmp(&image2, imgPaths[1]))
      return EXIT_ERROR_PATH_2;
   
   if (MODE_VERBOSE_ON) {
      printf("Infos img #1 : w=%u h=%u\n", image1.width, image1.height);
      DisplayBMP(&image1);
      printf("Infos img #2 : w=%u h=%u\n", image2.width, image2.height);
      DisplayBMP(&image2);
   }
   
   uint64_t hash1 = pHash(&image1);
   uint64_t hash2 = pHash(&image2);
   
   if (MODE_VERBOSE_ON)
      printf("pHash1 = 0x%" PRIx64 " -- pHash2 = 0x%" PRIx64 "\n", hash1, hash2);
   
   FreeImage(&image1);
   FreeImage(&image2);
   
   return DistancePHash(hash1, hash2);
}

static void ProcessOptions(char* imgPaths[2], int argc, char* argv[]) {
   int imgIndex = 0;
   for (int i = 1; i < argc; ++i) {
      if (imgIndex < 2 && strcmp(argv[i], "-v") != 0) {
         imgPaths[imgIndex] = argv[i];
         ++imgIndex;
      } else if (strcmp(argv[i], "-v") == 0) {
         MODE_VERBOSE_ON = 1;
      }
   }
   
   if (imgIndex < 2) {
      printf("Utilisation :\n\timg-path [-v] path_first_image path_second_image\n");
      exit(EXIT_ERROR_MISSING_ARGS);
   }
}

