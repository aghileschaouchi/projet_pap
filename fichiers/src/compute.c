
#include "compute.h"
#include "graphics.h"
#include "debug.h"
#include "ocl.h"

#include <stdbool.h>

unsigned version = 0;

//Taille de la tuille en séquentielle
#define TILE_SEQ 32
#define TILE_TASK 32

//Tuile pour les task
volatile int tile[TILE_TASK][TILE_TASK+1];



void first_touch_v1 (void);
void first_touch_v1_1 (void);
void first_touch_v1_2 (void);
void first_touch_v2 (void);
void first_touch_v2_1 (void);

unsigned compute_v0 (unsigned nb_iter);
unsigned compute_v0_1 (unsigned nb_iter);
unsigned compute_v0_2 (unsigned nb_iter);

unsigned compute_v1 (unsigned nb_iter);
unsigned compute_v1_1 (unsigned nb_iter);
unsigned compute_v1_2 (unsigned nb_iter);

unsigned compute_v2 (unsigned nb_iter);
unsigned compute_v2_1 (unsigned nb_iter);

unsigned compute_v3 (unsigned nb_iter);

void_func_t first_touch [] = {
  NULL,
  first_touch_v1,
  first_touch_v1_1,
  first_touch_v1_2,
  first_touch_v2,
  first_touch_v2_1,
  NULL,
};

int_func_t compute [] = {
  compute_v0,
  compute_v0_1,
  compute_v0_2,
  compute_v1,
  compute_v1_1,
  compute_v1_2,
  compute_v2,
  compute_v2_1,
  compute_v3,
};

char *version_name [] = {
  "Séquentielle",
  "Séquentielle tuilée",
  "Séquentielle optimisée",
  "OpenMP for base",
  "OpenMP for tuilée",
  "OpenMP for optimisée",
  "OpenMP task tuilée",
  "OpenMP task optimisée",
  "OpenCL",
};

unsigned opencl_used [] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  1,
};

///////////////////////////// Version séquentielle simple


unsigned compute_v0 (unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it ++) {
    for (int i = 0; i < DIM; i++)
      for (int j = 0; j < DIM; j++){  
          int compteur=0;
          //On gére les contours
          //Si le pixel se trouve en haut a gauche
          if(i == 0 && j == 0){
              if(cur_img (0, 1) == 0xFFFF00FF)
                compteur++;
              if(cur_img (1, 0) == 0xFFFF00FF)
                compteur++;
              if(cur_img (1, 1) == 0xFFFF00FF)
                compteur++;
          }
          //En haut a droite
          else if(i == 0 && j == DIM-1){
            if(cur_img (0, DIM-2) == 0xFFFF00FF)
              compteur++;
            if(cur_img (1, DIM-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img (1, DIM-2) == 0xFFFF00FF)
              compteur++;
          }
          //En bas a gauche
          else if(i == DIM-1 && j == 0){
            if(cur_img (DIM-2, 0) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-2, 1) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-1, 1) == 0xFFFF00FF)
              compteur++;
          }
          //En bas a droite
          else if(i == DIM-1 && j == DIM-1){
            if(cur_img (DIM-2, DIM-2) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-2, DIM-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-1, DIM-2) == 0xFFFF00FF)
              compteur++;
          }
          //Ligne du haut
          else if(i == 0){
            for(int k=i; k <= i+1 ; k++)
              for(int h=j-1; h <= j+1; h++)
                if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                 compteur++;
          }
          //Ligne du bas
          else if(i == DIM-1){
            for(int k=i-1; k <= i ; k++)
              for(int h=j-1; h <= j+1; h++)
               if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                 compteur++;
          }
          //Colonne de gauche
          else if(j == 0){
            for(int k=i-1; k <= i+1 ; k++)
              for(int h=j; h <= j+1; h++)
                 if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                   compteur++;
        }
          //Colone de bas
          else if(j == DIM-1){
            for(int k=i-1; k <= i+1 ; k++)
              for(int h=j-1; h <= j; h++)
               if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                 compteur++;
          }
          //Au millieu
          else{
            if(cur_img(i-1, j-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i-1, j) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i-1, j+1) == 0xFFFF00FF)
              compteur++;

            if(cur_img(i, j-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i, j+1) == 0xFFFF00FF)
              compteur++;

            if(cur_img(i+1, j-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i+1, j) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i+1, j+1) == 0xFFFF00FF)
              compteur++;   
            }
          //Régles du jeu de la vie
          //Si la cellule courrante est vivante
          if(cur_img (i, j) == 0xFFFF00FF){
            if(compteur == 0 || compteur == 1)
              next_img (i,j) = 0x0;
            if(compteur == 2 || compteur == 3)
              next_img (i,j) = 0xFFFF00FF;
            if(compteur > 3)
             next_img (i, j) = 0x0;
          }
          //Si elle est morte  
          if(cur_img (i, j) == 0x0 && compteur == 3)
           next_img (i, j) = 0xFFFF00FF;

          if(cur_img (i, j) == 0x0 && compteur != 3)
          next_img (i, j) = 0x0;
      }
    
    swap_images ();
  }
  // retourne le nombre d'étapes nécessaires à la
  // stabilisation du calcul ou bien 0 si le calcul n'est pas
  // stabilisé au bout des nb_iter itérations
  return 0;
}

///////////////////////////// Version séquentielle tuilée
  
unsigned compute_v0_1 (unsigned nb_iter)
{
for (unsigned it = 1; it <= nb_iter; it ++) {
    for(int i_tuile = 0; i_tuile < DIM; i_tuile += TILE_SEQ)
      for(int j_tuile = 0; j_tuile < DIM; j_tuile += TILE_SEQ)
        for (int i = i_tuile; i < i_tuile + TILE_SEQ; i++)
          for (int j = j_tuile; j < j_tuile + TILE_SEQ; j++){    
  int compteur=0;
          //On gére les contours
    //Si le pixel se trouve en haut a gauche
    if(i == 0 && j == 0){
            if(cur_img (0, 1) == 0xFFFF00FF)
        compteur++;
            if(cur_img (1, 0) == 0xFFFF00FF)
              compteur++;
            if(cur_img (1, 1) == 0xFFFF00FF)
              compteur++;
          }
          //En haut a droite
          else if(i == 0 && j == DIM-1){
      if(cur_img (0, DIM-2) == 0xFFFF00FF)
        compteur++;
            if(cur_img (1, DIM-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img (1, DIM-2) == 0xFFFF00FF)
              compteur++;
          }
    //En bas a gauche
          else if(i == DIM-1 && j == 0){
            if(cur_img (DIM-2, 0) == 0xFFFF00FF)
        compteur++;
            if(cur_img (DIM-2, 1) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-1, 1) == 0xFFFF00FF)
              compteur++;
          }
          //En bas a droite
          else if(i == DIM-1 && j == DIM-1){
      if(cur_img (DIM-2, DIM-2) == 0xFFFF00FF)
        compteur++;
            if(cur_img (DIM-2, DIM-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-1, DIM-2) == 0xFFFF00FF)
              compteur++;
          }
    //Ligne du haut
          else if(i == 0){
      for(int k=i; k <= i+1 ; k++)
        for(int h=j-1; h <= j+1; h++)
                 if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                   compteur++;
          }
    //Ligne du bas
    else if(i == DIM-1){
      for(int k=i-1; k <= i ; k++)
        for(int h=j-1; h <= j+1; h++)
                 if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                   compteur++;
          }
    //Colonne de gauche
    else if(j == 0){
      for(int k=i-1; k <= i+1 ; k++)
        for(int h=j; h <= j+1; h++)
                 if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                   compteur++;
          }
    //Colone de bas
    else if(j == DIM-1){
      for(int k=i-1; k <= i+1 ; k++)
        for(int h=j-1; h <= j; h++)
                 if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                   compteur++;
          }
    //Au millieu
    else{
      if(cur_img(i-1, j-1) == 0xFFFF00FF)
        compteur++;
      if(cur_img(i-1, j) == 0xFFFF00FF)
        compteur++;
      if(cur_img(i-1, j+1) == 0xFFFF00FF)
        compteur++;

      if(cur_img(i, j-1) == 0xFFFF00FF)
        compteur++;
      if(cur_img(i, j+1) == 0xFFFF00FF)
        compteur++;

      if(cur_img(i+1, j-1) == 0xFFFF00FF)
        compteur++;
      if(cur_img(i+1, j) == 0xFFFF00FF)
        compteur++;
      if(cur_img(i+1, j+1) == 0xFFFF00FF)
        compteur++;   
            }
    //Régles du jeu de la vie
    //Si la cellule courrante est vivante
          if(cur_img (i, j) == 0xFFFF00FF){
            if(compteur == 0 || compteur == 1)
              next_img (i,j) = 0x0;
            if(compteur == 2 || compteur == 3)
              next_img (i,j) = 0xFFFF00FF;
            if(compteur > 3)
              next_img (i, j) = 0x0;
          }
    //Si elle est morte  
    if(cur_img (i, j) == 0x0 && compteur == 3)
            next_img (i, j) = 0xFFFF00FF;

    if(cur_img (i, j) == 0x0 && compteur != 3)
            next_img (i, j) = 0x0;
        }
    
    swap_images ();
  }
  // retourne le nombre d'étapes nécessaires à la
  // stabilisation du calcul ou bien 0 si le calcul n'est pas
  // stabilisé au bout des nb_iter itérations
  return 0;
}

///////////////////////////// Version séquentielle optimisée

unsigned compute_v0_2 (unsigned nb_iter)
{

  for (unsigned it = 1; it <= nb_iter; it ++) {
    for (int i = 0; i < DIM; i++)
      for (int j = 0; j < DIM; j++)
  next_img (i, j) = cur_img (j, i);
    
    swap_images ();
  }
  // retourne le nombre d'étapes nécessaires à la
  // stabilisation du calcul ou bien 0 si le calcul n'est pas
  // stabilisé au bout des nb_iter itérations
  return 0;
}

///////////////////////////// Version OpenMP for de base

void first_touch_v1 ()
{
  int i,j ;
  int compteur=0;
#pragma omp parallel for reduction(+:compteur)
  for(i=0; i<DIM ; i++) 
    for(j=0; j < DIM ; j += 512){
      compteur=0;
          //On gére les contours
          //Si le pixel se trouve en haut a gauche
          if(i == 0 && j == 0){
              if(cur_img (0, 1) == 0xFFFF00FF)
                compteur++;
              if(cur_img (1, 0) == 0xFFFF00FF)
                compteur++;
              if(cur_img (1, 1) == 0xFFFF00FF)
                compteur++;
          }
          //En haut a droite
          else if(i == 0 && j == DIM-1){
            if(cur_img (0, DIM-2) == 0xFFFF00FF)
              compteur++;
            if(cur_img (1, DIM-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img (1, DIM-2) == 0xFFFF00FF)
              compteur++;
          }
          //En bas a gauche
          else if(i == DIM-1 && j == 0){
            if(cur_img (DIM-2, 0) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-2, 1) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-1, 1) == 0xFFFF00FF)
              compteur++;
          }
          //En bas a droite
          else if(i == DIM-1 && j == DIM-1){
            if(cur_img (DIM-2, DIM-2) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-2, DIM-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-1, DIM-2) == 0xFFFF00FF)
              compteur++;
          }
          //Ligne du haut
          else if(i == 0){
            for(int k=i; k <= i+1 ; k++)
              for(int h=j-1; h <= j+1; h++)
                if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                 compteur++;
          }
          //Ligne du bas
          else if(i == DIM-1){
            for(int k=i-1; k <= i ; k++)
              for(int h=j-1; h <= j+1; h++)
               if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                 compteur++;
          }
          //Colonne de gauche
          else if(j == 0){
            for(int k=i-1; k <= i+1 ; k++)
              for(int h=j; h <= j+1; h++)
                 if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                   compteur++;
        }
          //Colone de bas
          else if(j == DIM-1){
            for(int k=i-1; k <= i+1 ; k++)
              for(int h=j-1; h <= j; h++)
               if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                 compteur++;
          }
          //Au millieu
          else{
            if(cur_img(i-1, j-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i-1, j) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i-1, j+1) == 0xFFFF00FF)
              compteur++;

            if(cur_img(i, j-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i, j+1) == 0xFFFF00FF)
              compteur++;

            if(cur_img(i+1, j-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i+1, j) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i+1, j+1) == 0xFFFF00FF)
              compteur++;   
            }
          //Régles du jeu de la vie
          //Si la cellule courrante est vivante
          if(cur_img (i, j) == 0xFFFF00FF){
            if(compteur == 0 || compteur == 1)
              next_img (i,j) = 0x0;
            if(compteur == 2 || compteur == 3)
              next_img (i,j) = 0xFFFF00FF;
            if(compteur > 3)
             next_img (i, j) = 0x0;
          }
          //Si elle est morte  
          if(cur_img (i, j) == 0x0 && compteur == 3)
           next_img (i, j) = 0xFFFF00FF;

          if(cur_img (i, j) == 0x0 && compteur != 3)
          next_img (i, j) = 0x0;
      }
}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v1(unsigned nb_iter)
{
  for(unsigned it = 1; it <= nb_iter; it++){
    first_touch_v1 ();

    swap_images ();
  }
  return 0;
}

///////////////////////////// Version OpenMP for avec tuiles

void first_touch_v1_1 ()
{
  int i,j ;
  /* A savoir si on aura besoin de ça
  int tuile[32][32];

  memset(tuile, 0, sizeof(tuile));
  */

/* Version fournie
#pragma omp parallel for
  for(i=0; i<DIM ; i++) {
    for(j=0; j < DIM ; j += 512)
      next_img (i, j) = cur_img (i, j) = 0 ;
  }
*/

//Distribution des indices selon un collapse
  //Faire un collapse sur tout ou parcourir les tuiles sequentiellement puis paralleliser les deux autres boucles ?
/*
#pragma omp parallel for collapse(2) schedule(runtime) 
  for(int i_tuile = 0; i_tuile < DIM; i_tuile += TILE_SEQ)
    for(int j_tuile = 0; j_tuile < DIM; j_tuile += TILE_SEQ)
      for (int i = i_tuile; i < i_tuile + TILE_SEQ; i++)
        for (int j = j_tuile; j < j_tuile + TILE_SEQ; j += 512)
          next_img (i,j) = cur_img (j, i);
*/
int compteur=0;
#pragma omp parallel for firstprivate(compteur)
  for(i=0; i<DIM ; i++) 
    for(j=0; j < DIM ; j += 512){
      compteur=0;

          //On gére les contours
          //Si le pixel se trouve en haut a gauche
          if(i == 0 && j == 0){
              if(cur_img (0, 1) == 0xFFFF00FF)
                compteur++;
              if(cur_img (1, 0) == 0xFFFF00FF)
                compteur++;
              if(cur_img (1, 1) == 0xFFFF00FF)
                compteur++;
          }
          //En haut a droite
          else if(i == 0 && j == DIM-1){
            if(cur_img (0, DIM-2) == 0xFFFF00FF)
              compteur++;
            if(cur_img (1, DIM-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img (1, DIM-2) == 0xFFFF00FF)
              compteur++;
          }
          //En bas a gauche
          else if(i == DIM-1 && j == 0){
            if(cur_img (DIM-2, 0) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-2, 1) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-1, 1) == 0xFFFF00FF)
              compteur++;
          }
          //En bas a droite
          else if(i == DIM-1 && j == DIM-1){
            if(cur_img (DIM-2, DIM-2) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-2, DIM-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img (DIM-1, DIM-2) == 0xFFFF00FF)
              compteur++;
          }
          //Ligne du haut
          else if(i == 0){
            for(int k=i; k <= i+1 ; k++)
              for(int h=j-1; h <= j+1; h++)
                if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                 compteur++;
          }
          //Ligne du bas
          else if(i == DIM-1){
            for(int k=i-1; k <= i ; k++)
              for(int h=j-1; h <= j+1; h++)
               if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                 compteur++;
          }
          //Colonne de gauche
          else if(j == 0){
            for(int k=i-1; k <= i+1 ; k++)
              for(int h=j; h <= j+1; h++)
                 if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                   compteur++;
        }
          //Colone de bas
          else if(j == DIM-1){
            for(int k=i-1; k <= i+1 ; k++)
              for(int h=j-1; h <= j; h++)
               if((cur_img (k, h) == 0xFFFF00FF )&& ((k!=i) && (h!=j)))
                 compteur++;
          }
          //Au millieu
          else{
            if(cur_img(i-1, j-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i-1, j) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i-1, j+1) == 0xFFFF00FF)
              compteur++;

            if(cur_img(i, j-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i, j+1) == 0xFFFF00FF)
              compteur++;

            if(cur_img(i+1, j-1) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i+1, j) == 0xFFFF00FF)
              compteur++;
            if(cur_img(i+1, j+1) == 0xFFFF00FF)
              compteur++;   
            }
          //Régles du jeu de la vie
          //Si la cellule courrante est vivante
          if(cur_img (i, j) == 0xFFFF00FF){
            if(compteur == 0 || compteur == 1)
              next_img (i,j) = 0x0;
            if(compteur == 2 || compteur == 3)
              next_img (i,j) = 0xFFFF00FF;
            if(compteur > 3)
             next_img (i, j) = 0x0;
          }
          //Si elle est morte  
          if(cur_img (i, j) == 0x0 && compteur == 3)
           next_img (i, j) = 0xFFFF00FF;

          if(cur_img (i, j) == 0x0 && compteur != 3)
          next_img (i, j) = 0x0;
      }
}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v1_1(unsigned nb_iter)
{
  for(unsigned it = 1; it <= nb_iter; it++){
    first_touch_v1_1 ();

    swap_images ();
  }
  return 0;
}

///////////////////////////// Version OpenMP for optimisée
//TODO!
void first_touch_v1_2 ()
{
  int i,j ;

#pragma omp parallel for
  for(i=0; i<DIM ; i++) {
    for(j=0; j < DIM ; j += 512)
      next_img (i, j) = cur_img (i, j) = 0 ;
  }
}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v1_2(unsigned nb_iter)
{
  for(unsigned it = 1; it <= nb_iter; it++){
    first_touch_v1 ();

    swap_images ();
  }
  return 0;
}

///////////////////////////// Version OpenMP task tuilée
// On incrémente le nombre de colonnes, comme ça ; pas la peine de gérer le cas des bords
void first_touch_v2 ()
{
  #pragma omp parallel 
  {
    #pragma omp single
    {
      for(int i=0; i<DIM ; i++)
        for(int j=0; j < DIM ; j += 512)
          #pragma omp task firstprivate(i,j) depend(out:tile[i][j]) depend(in:tile[i-1][j-1], tile[i-1][j], tile[i-1][j+1], tile[i][j-1], tile[i][j+1], tile[i+1][j-1], tile[i+1][j], tile[i+1][j+1])
          {
          next_img (i, j) = cur_img (j, i);
          }
    } 
  }
}
// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v2(unsigned nb_iter)
{
  for(unsigned it = 1; it <= nb_iter; it++){
    first_touch_v2 ();

    swap_images ();
    return 0; // on ne s'arrête jamais
  }
  
}

///////////////////////////// Version OpenMP task optimisée

void first_touch_v2_1 ()
{

}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v2_1(unsigned nb_iter)
{
  return 0; // on ne s'arrête jamais
}

///////////////////////////// Version OpenCL

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v3 (unsigned nb_iter)
{ 
  return ocl_compute (nb_iter);
}

