#include "geom.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

/* **************************************** */
/* returns the signed area of triangle abc. The area is positive if c
   is to the left of ab, 0 if a,b,c are collinear; and negative if c
   is to the right of ab
 */
int signed_area2D(point2D a, point2D b, point2D c) {

  return (b.x - a.x)(c.y - b.y) - (c.x - b.x)(b.y - a.y); 
}



/* **************************************** */
/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r) {
  
  return signed_area2D(p, q, r) != 0; 
}



/* **************************************** */
/* return 1 if c is  strictly left of ab; 0 otherwise */
int left_strictly(point2D a, point2D b, point2D c) {
  
  return signed_area2D(a, b, c) > 0; 
}


/* return 1 if c is left of ab or on ab; 0 otherwise */
int left_on(point2D a, point2D b, point2D c) {

  return signed_area2D(a, b, c) >= 0; 
} 
