#ifndef __geom_h
#define __geom_h

#include <stdio.h>

typedef struct _point2d {
  int x,y; 
} point2D;



/*  If you use plain C you will need to implement a linked list, which
    will store the result returned by the convex hullx */ 

//a list of points 
typedef struct _pointNode pointNode; 
struct _pointNode  {
  point2D p;
  pointNode* next;
}; 



/* returns 2 times the signed area of triangle abc. The area is
   positive if c is to the left of ab, 0 if a,b,c are collinear and
   negative if c is to the right of ab
 */
int signed_area2D(point2D a, point2D b, point2D c); 


/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r);


/* return 1 if c is  strictly left of ab; 0 otherwise */
int left_strictly (point2D a, point2D b, point2D c); 


/* return 1 if c is left of ab or on ab; 0 otherwise */
int left_on(point2D a, point2D b, point2D c); 

void compute_visible_polygon();

/* compute the convex hull of the points in p; the points on the CH
   are returned as a list or vector.
*/
pointNode* graham_scan(point2D* p, int n);

void print_point(point2D p) {
        printf("%d, %d\n", p.x, p.y);
}

#endif
