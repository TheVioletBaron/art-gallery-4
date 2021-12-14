#include "geom.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <math.h>

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

point2D calculate_intersect(point2D p0, point2D p1, point2D p2, point2D p3) {

	point2D k; //to return

	// calculate the intersection of the lines
	int al = p1.y - p0.y;
	int bl = p0.x - p1.x;
	int cl = al*p0.x + bl*p0.y;

	int ar = p3.y - p2.y;
	int br = p2.x - p3.x;
	int cr = ar*p2.x + br*p2.y;

	int det = al*br - ar*bl;

	k.x = (br * cl - bl * cr)/det;
	k.y = (al * cr - ar * cl)/det;


	// check whether the intersection falls within the line segment
	float aq = calc_dist(p0, k);
	float bq = calc_dist(p1, k);
	float ab = calc_dist(p0, p1);

	printf("aq + bq = %f, ab = %f\n", aq + bq, ab);

	float qp = calc_dist(k, p2);
	float pg = calc_dist(p2, p3);
	float gq = calc_dist(p3, k);

	int prec = 3;

	if (((ab - prec) <= (aq + bq)) and ((aq + bq) <= (ab + prec)) and
		(((gq - prec) <= (qp + pg)) and ((qp + pg) <= (gq + prec)))) {
		return k;
	}
	k.x = -1;
	return k;
}

float calc_dist(point2D a, point2D b) {
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

point2D ray_extend(vector <point2D> poly, point2D p, point2D guard, int j) {
	vector <point2D> cand;
	for (int i = 1; i < poly.size(); i++) {
		if ((i != j) and (i - 1 != j)) {
			point2D r = calculate_intersect(poly[i - 1], poly[i], p, guard);
			print_point(r);
			if (r.x != -1) {
				cand.push_back(r);
			}
		}
	}
	if ((0 != j) and (poly.size() - 1 != j)) {
		point2D r = calculate_intersect(poly[0], poly[poly.size() - 1], p, guard);
		print_point(r);
		if (r.x != -1) {
			cand.push_back(r);
		}
	}

	float dist = 10000;
	point2D to_return;
	for (int i = 0; i < cand.size(); i++) {
		if (calc_dist(cand[i], guard) > calc_dist(cand[i], p)) {
			if (calc_dist(cand[i], p) < dist) {
				dist = calc_dist(cand[i], p);
				to_return = cand[i];
			}
		}
	}
	if (dist == 10000) {
		to_return.x = -1;
	}
	return to_return;
}

vector <point2D> compute_visible_polygon(vector <point2D> poly, point2D guard) {

	printf("\n\n\nComputing polygon\n");
	vector <point2D> visible;

	int state = 1;
	for (int i = 0; i < poly.size(); i++) {
		printf("\nNew point: ");
		print_point(poly[i]);
		if (is_visible(poly, poly[i], guard, i)) {
			printf("Point is visible\n");
			if (state == 0) {
				printf("Potential cusp located: ");
				point2D r = ray_extend(poly, poly[i], guard, i);
				print_point(r);
				if (r.x != -1) {
					printf("True vertex\n");
					visible.push_back(r);
				} else {
					printf("False vertex\n");
				}
			}
			point2D p;
			p.x = poly[i].x;
			p.y = poly[i].y;
			visible.push_back(p);
			state = 1;
		} else {
			printf("Point is invisible\n");
			if (state == 1) {
				printf("Potential cusp located\n");
				point2D r = ray_extend(poly, poly[i], guard, i);
				print_point(r);
				if (r.x != -1) {
					printf("True vertex\n");
					visible.push_back(r);
				} else {
					printf("False vertex\n");
				}
			}
			state = 0;
		}
	}
	printf("There are %d visible edges\n", visible.size());
	return visible;
}
