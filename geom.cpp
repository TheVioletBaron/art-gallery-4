#include "geom.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;
/* **************************************** */
/* returns the signed area of triangle abc. The area is positive if c
	 is to the left of ab, 0 if a,b,c are collinear; and negative if c
	 is to the right of ab
 */
int signed_area2D(point2D a, point2D b, point2D c) {

	return (b.x - a.x)*(c.y - b.y) - (c.x - b.x)*(b.y - a.y); 
}



/* **************************************** */
/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r) {
	
	return signed_area2D(p, q, r) != 0; 
}



/* **************************************** */
/* return 1 if c is	strictly left of ab; 0 otherwise */
int left_strictly(point2D a, point2D b, point2D c) {
	
	return signed_area2D(a, b, c) > 0; 
}

/* return 1 if c is left of ab or on ab; 0 otherwise */
int left_on(point2D a, point2D b, point2D c) {

	return signed_area2D(a, b, c) >= 0; 
} 

bool ccw(point2D a, point2D b, point2D c) {
	return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
}

bool intersect(point2D a, point2D b, point2D c, point2D d) {
	return (ccw(a, c, d) != ccw(b, c, d)) and (ccw(a, b, c) != ccw(a, b, d));
}

bool is_visible(vector <point2D> poly, point2D p, point2D guard, int j) {
	for (int i = 1; i < poly.size(); i++) {
		if ((i != j) and (i - 1 != j)) {
			if (intersect(poly[i - 1], poly[i], p, guard)) {
				return 0;
			}
		}
	}
	if ((0 != j) and (poly.size() - 1 != j)) {
		if (intersect(poly[0], poly[poly.size() - 1], p, guard)) {
			return 0;
		}
	}
	return 1;
}

vector <point2D> compute_visible_polygon(vector <point2D> poly, point2D guard) {
	vector <point2D> visible;
	int j = 0;
	for (int i = 0; i < poly.size(); i++) {
		if (is_visible(poly, poly[i], guard, i)) {
			j ++;
			point2D p;
			p.x = poly[i].x;
			p.y = poly[i].y;
			visible.push_back(p);
		}
	}
	printf("There are %d visible edges\n", j);
	return visible;
}
