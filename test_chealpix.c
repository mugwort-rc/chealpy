/* -----------------------------------------------------------------------------
 *
 *  Copyright (C) 1997-2012 Krzysztof M. Gorski, Eric Hivon, Martin Reinecke,
 *                          Benjamin D. Wandelt, Anthony J. Banday,
 *                          Matthias Bartelmann,
 *                          Reza Ansari & Kenneth M. Ganga
 *
 *
 *  This file is part of HEALPix.
 *
 *  HEALPix is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  HEALPix is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with HEALPix; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  For more information about HEALPix see http://healpix.jpl.nasa.gov
 *
 *----------------------------------------------------------------------------- */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "chealpix.h"

void test1(void) {

  double theta, phi;
  double vec[3];
  long   nside;
  long  ipix, npix, dpix, ip2, ip1;
  int ix, iy, facenum, ix2, iy2, facenum2;

  printf("Starting C Healpix pixel routines test\n");

  #ifdef HIGH_RESOLUTION

  for(nside = 1; nside <= 1<<29; nside=nside<<1) {
    pix2ang_ring(nside, 1, &theta, &phi);
    printf("%d, %g, %g\n", nside, theta, phi);
  }
  #endif

  #ifdef HIGH_RESOLUTION
  nside = 1<<29;
  dpix = 3 * (1L<<31)- 1;
  #else
  nside = 1<<13;
  dpix = 3 * (1L<<8)- 1;
  #endif
  /* Find the number of pixels in the full map */
  npix = nside2npix(nside);
  printf("Number of pixels in full map: %ld\n", npix);

  printf("dpix: %ld\n", dpix);
  printf("Nest -> ang -> vec -> ang -> Ring -> Nest\n");
  for (ipix = 0; ipix < npix; ipix +=dpix) {
    pix2ang_nest(nside, ipix, &theta, &phi);
    ang2vec(theta, phi, vec);
    vec2ang(vec, &theta, &phi);
    ang2pix_ring(nside, theta, phi, &ip2);
    ring2nest(nside,ip2,&ip1);
    if (ip1 != ipix) {
      printf("Error: %ld %ld %ld %ld\n",nside,ipix,ip2,ip1);
      abort();
    }
  }
  printf("Ring -> ang -> Nest -> Ring\n");
  for (ipix = 0; ipix < npix; ipix +=dpix) {
    pix2ang_ring(nside, ipix, &theta, &phi);
    ang2pix_nest(nside, theta, phi, &ip2);
    nest2ring(nside,ip2,&ip1);
    if (ip1 != ipix) {printf("Error: %ld %ld %ld %ld\n",nside,ipix,ip2,ip1);}
  }

  printf("Nest -> vec -> Ring -> Nest\n");
  for (ipix = 0; ipix < npix; ipix +=dpix) {
    pix2vec_nest(nside, ipix, vec);
    vec2pix_ring(nside, vec, &ip2);
    ring2nest(nside,ip2,&ip1);
    if (ip1 != ipix) {printf("Error: %ld %ld %ld %ld\n",nside,ipix,ip2,ip1);}
  }
  printf("Ring -> vec -> Nest -> Ring\n");
  for (ipix = 0; ipix < npix; ipix +=dpix) {
    pix2vec_ring(nside, ipix, vec);
    vec2pix_nest(nside, vec, &ip2);
    nest2ring(nside,ip2,&ip1);
    if (ip1 != ipix) {printf("Error: %ld %ld %ld %ld\n",nside,ipix,ip2,ip1);}
  }

  printf("%ld\n", nside);
  printf("test completed\n\n");
}

#ifdef ENABLE_FITSIO
void test2 (void) {
  float *map;
  long nside, npix, np, ns;
  int i;
  char file[180] = "test_output.fits" ;
  char fileforce[180] ;
  char order1[10] ;
  char order2[10] ;
  char coord[10] ;


  printf("Starting C Healpix IO test\n");

  nside = 1;
  npix = nside2npix(nside);

  map = (float *)malloc(npix*sizeof(float));

  for (i=0; i<npix; i++){
    map[i] = 2.*i;
  }

  sprintf(fileforce, "!%s",file); // leading ! to allow overwrite
  write_healpix_map( map, nside, fileforce, 1, "C");
  fprintf(stdout,"file written\n");
  free(map);

  np = get_fits_size(file, &ns, order1);
  fprintf(stdout,"%s %ld %ld %s\n", file, ns, np, order1);

  map = read_healpix_map(file, &ns, coord, order2);
  if (strcmp(coord,"C")!=0) {printf("Error: Bad coordinate system\n");}
  if (strcmp(order1,order2)!=0) {printf("Error: Bad ordering\n");}
  for (i=0; i<npix; i++){
    if (map[i]!=2.*i) {printf("Error: Bad pixel value\n");}
  }

  free(map);

  printf("test completed\n\n");
}
#endif

int main(void) {
  test1();
#ifdef ENABLE_FITSIO
  test2();
#endif
  return 0;
}
