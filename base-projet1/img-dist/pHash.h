#ifndef PHASH_H__
#define PHASH_H__

#include <stdint.h>

#include "bmp.h"

/**
 * Calcule le code de hachage perceptif de l'image `image` et le retourne.
 * 
 * @return Code de hachage perceptif de `image`.
 **/
uint64_t pHash(RgbImage* image);

/**
 * Calcule la distance (de Hamming) entre deux codes de hachage perceptif.
 * 
 * @return La distance entre `pHash1` et `pHash2`.
 **/
unsigned int DistancePHash(uint64_t pHash1, uint64_t pHash2);

#endif
