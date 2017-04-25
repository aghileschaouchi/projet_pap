__kernel void transpose_naif (__global unsigned *in, __global unsigned *out)
{
  int x = get_global_id (0);
  int y = get_global_id (1);

  out [x * DIM + y] = in [y * DIM + x];
}



__kernel void transpose (__global unsigned *in, __global unsigned *out)
{
  __local unsigned tile [TILEX][TILEY+1];
  int x = get_global_id (0);
  int y = get_global_id (1);
  int xloc = get_local_id (0);
  int yloc = get_local_id (1);

  tile [xloc][yloc] = in [y * DIM + x];

  barrier (CLK_LOCAL_MEM_FENCE);

  out [(x - xloc + yloc) * DIM + y - yloc + xloc] = tile [yloc][xloc];
}

__kernel void scrollup (__global unsigned *in, __global unsigned *out)
{
  int x = get_global_id (0);
  int y = get_global_id (1);

  int compteur = 0;
  // Si on est pas dans les bords
  if((x != 0) && (y != 0) && (x != DIM - 1) && (y != DIM -1)){

  // On parcours les voisins de la cellule courrante
  for(int i = x - 1; i <= x + 1 && i != x; i++)
    for(int j = y - 1; j <= y + 1 && j != y; j++)
      // On compte le nombre de cellules vivantes
      if(in[j * DIM + i] == 0xFFFF00FF)
        compteur++;

  // Si la cellule courrante est morte et qu'elle a 3 voisines vivantes, elle devient vivante
  if(in[y * DIM + x] == 0x0 && compteur == 3)
    out[y * DIM + x] = 0xFFFF00FF;

  // Si elle est vivante et qu'elle a moins de 2 ou plus de 3 voisines vivantes, elle meurt
  else if(in[y * DIM + x] == 0xFFFF00FF && ((compteur < 3) || (compteur > 2)));
    out[y * DIM + x] = 0x0;
      
  }

 }

// NE PAS MODIFIER
static float4 color_scatter (unsigned c)
{
  uchar4 ci;

  ci.s0123 = (*((uchar4 *) &c)).s3210;
  return convert_float4 (ci) / (float4) 255;
}

// NE PAS MODIFIER: ce noyau est appelé lorsqu'une mise à jour de la
// texture de l'image affichée est requise
__kernel void update_texture (__global unsigned *cur, __write_only image2d_t tex)
{
  int y = get_global_id (1);
  int x = get_global_id (0);
  int2 pos = (int2)(x, y);
  unsigned c;

  c = cur [y * DIM + x];

  write_imagef (tex, pos, color_scatter (c));
}
