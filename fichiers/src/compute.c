
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

///////////////////////////// Le jeu de la vie

void game_of_life(int i, int j){
          int compteur=0;
	
          compteur = (cur_img(i-1, j-1) == 0xFFFF00FF) + (cur_img(i-1, j) == 0xFFFF00FF) + (cur_img(i-1, j+1) == 0xFFFF00FF)
	  + (cur_img(i, j-1) == 0xFFFF00FF) + (cur_img(i, j+1) == 0xFFFF00FF) + (cur_img(i+1, j-1) == 0xFFFF00FF) + 
          (cur_img(i+1, j) == 0xFFFF00FF) + (cur_img(i+1, j+1) == 0xFFFF00FF);     
	  
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

///////////////////////////// Version séquentielle simple


unsigned compute_v0 (unsigned nb_iter)
{
  for (unsigned it = 1; it <= nb_iter; it ++) {
    for (int i = 1; i < DIM-1; i++)
      for (int j = 1; j < DIM-1; j++)
	game_of_life(i,j);
    
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
  int fin_i_tuile;
  int fin_j_tuile;
  int deb_i_tuile;
  int deb_j_tuile;
  for (unsigned it = 1; it <= nb_iter; it ++) {
    for(int i_tuile = 0; i_tuile < DIM; i_tuile += TILE_SEQ)
      for(int j_tuile = 0; j_tuile < DIM; j_tuile += TILE_SEQ){

	fin_i_tuile = i_tuile + TILE_SEQ;
	fin_j_tuile = j_tuile + TILE_SEQ;

	deb_i_tuile = i_tuile;
	deb_j_tuile = j_tuile;
	
	if(i_tuile == DIM-TILE_SEQ)
	  fin_i_tuile--;

	if(j_tuile == DIM-TILE_SEQ)
	  fin_j_tuile--;

	if(i_tuile == 0)
	  deb_i_tuile++;

	if(j_tuile == 0)
	  deb_j_tuile++;

        for (int i = deb_i_tuile; i < fin_i_tuile ; i++)
          for (int j = deb_j_tuile; j < fin_j_tuile ; j++)
	    game_of_life(i,j);
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
  int j;
  int deb_j_tuile=0;
    for (unsigned it = 1; it <= nb_iter; it ++) {
      for(int i_tuile = 0; i_tuile < DIM; i_tuile += TILE_SEQ)
	for(int i = i_tuile; i < i_tuile + TILE_SEQ ; i++){
	  for(j = deb_j_tuile; j < deb_j_tuile + TILE_SEQ; j++){
	    game_of_life(i,j);
	    printf("i:%dj:%d\n");
	  }
	deb_j_tuile += TILE_SEQ;
	}
	
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

  //Initialisation (first touch)
  #pragma omp parallel for
    for(int i=0; i<DIM; i++)
      for(int j=0; j<DIM; j+=512)
        cur_img(i,j) = next_img(i,j) = 0;
  
}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v1(unsigned nb_iter)
{
  for(unsigned it = 1; it <= nb_iter; it++){
  //  first_touch_v1 ();

    
    #pragma omp parallel for 
      for(int i=1; i<DIM-1 ; i++) 
        for(int j=1; j < DIM-1 ; j++)
          game_of_life(i,j);

    swap_images ();
  }
  return 0;
}

///////////////////////////// Version OpenMP for avec tuiles

void first_touch_v1_1 ()
{

  //Initialisation (first touch)
  #pragma omp parallel for 
    for(int i=0; i<DIM ; i++) 
      for(int j=0; j < DIM ; j += 512)
        cur_img(i,j) = next_img(i,j) =0;

}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v1_1(unsigned nb_iter)
{
  int deb_i_tuile, deb_j_tuile, fin_i_tuile, fin_j_tuile;
  for(unsigned it = 1; it <= nb_iter; it++){
 //   first_touch_v1_1 ();

    #pragma omp parallel for collapse(2) schedule(dynamic) reduction(+:deb_i_tuile,deb_j_tuile,fin_i_tuile,fin_j_tuile)
      for(int i_tuile = 0; i_tuile < DIM; i_tuile += TILE_SEQ)
        for(int j_tuile = 0; j_tuile < DIM; j_tuile += TILE_SEQ){

	  fin_i_tuile = i_tuile + TILE_SEQ;
	  fin_j_tuile = j_tuile + TILE_SEQ;

	  deb_i_tuile = i_tuile;
	  deb_j_tuile = j_tuile;
	
	  if(i_tuile == DIM-TILE_SEQ)
	    fin_i_tuile--;

	  if(j_tuile == DIM-TILE_SEQ)
	    fin_j_tuile--;

	  if(i_tuile == 0)
	    deb_i_tuile++;

	  if(j_tuile == 0)
	    deb_j_tuile++;

          for (int i = deb_i_tuile; i < fin_i_tuile ; i++)
            for (int j = deb_j_tuile; j < fin_j_tuile ; j++)
	      game_of_life(i,j);
      }

    swap_images ();
  }
  return 0;
}

///////////////////////////// Version OpenMP for optimisée
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
  //Initialisation (first touch)
  for(int i=0; i<DIM ; i++)
    for(int j=0; j < DIM ; j += 512){
      next_img (i, j) = cur_img (i, j) = 0;
      }

}

// Renvoie le nombre d'itérations effectuées avant stabilisation, ou 0
unsigned compute_v2(unsigned nb_iter)
{
  for(unsigned it = 1; it <= nb_iter; it++){  

    int i,j,deb_i_tuile,deb_j_tuile,fin_i_tuile,fin_j_tuile;

    #pragma omp parallel
    #pragma omp single
    for(int i_tuile = 0; i_tuile < DIM; i_tuile += TILE_SEQ)
        for(int j_tuile = 0; j_tuile < DIM; j_tuile += TILE_SEQ){

	  fin_i_tuile = i_tuile + TILE_SEQ;
	  fin_j_tuile = j_tuile + TILE_SEQ;

	  deb_i_tuile = i_tuile;
	  deb_j_tuile = j_tuile;
	
	  if(i_tuile == DIM-TILE_SEQ)
	    fin_i_tuile--;

	  if(j_tuile == DIM-TILE_SEQ)
	    fin_j_tuile--;

	  if(i_tuile == 0)
	    deb_i_tuile++;

	  if(j_tuile == 0)
	    deb_j_tuile++;

	  #pragma omp task firstprivate(deb_i_tuile,deb_j_tuile,fin_i_tuile,fin_j_tuile)
          for (int i = deb_i_tuile; i < fin_i_tuile ; i++)
            for (int j = deb_j_tuile; j < fin_j_tuile ; j++)
	      game_of_life(i,j);
      }
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

