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

/* checks whether a point is counterclockwise of another point */
bool ccw(point2D a, point2D b, point2D c) {
	return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
}

/* checks whether an intersect exists between two lines */
bool intersect(point2D a, point2D b, point2D c, point2D d) {
	return (ccw(a, c, d) != ccw(b, c, d)) and (ccw(a, b, c) != ccw(a, b, d));
}

/* checks whether a point is visible from another point given a polygonal obstacle */
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

/* calculates the intersection between a ray p2 -> p3 and a line segment p0 - p1 if one exists */
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

	if (det == 0) { //lines are parallel
		k.x = -1;
		return k;
	}

	k.x = (br * cl - bl * cr)/det;
	k.y = (al * cr - ar * cl)/det;


	// check whether the intersection falls within the line segment
	float aq = calc_dist(p0, k);
	float bq = calc_dist(p1, k);
	float ab = calc_dist(p0, p1);

	float qp = calc_dist(k, p2);
	float pg = calc_dist(p2, p3);
	float gq = calc_dist(p3, k);

	// becuase we're dealing with finite precision floating point numbers,
	// this variable specfies the margin of error for "equality"
	int prec = 3;

	if (((ab - prec) <= (aq + bq)) and ((aq + bq) <= (ab + prec)) and
		(((gq - prec) <= (qp + pg)) and ((qp + pg) <= (gq + prec)))) {
		return k;
	}
	k.x = -1;
	return k;
}

/* calculates the distance between two points */
float calc_dist(point2D a, point2D b) {
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

/* extends a ray from a line segment until it collides with a line segment in the polygon */
point2D ray_extend(vector <point2D> poly, point2D p, point2D guard, int j) {
	vector <point2D> cand;
	for (int i = 1; i < poly.size(); i++) {
		if ((i != j) and (i - 1 != j)) {
			point2D r = calculate_intersect(poly[i - 1], poly[i], p, guard);
			if (r.x != -1) {
				cand.push_back(r);
			}
		}
	}
	if ((0 != j) and (poly.size() - 1 != j)) {
		point2D r = calculate_intersect(poly[0], poly[poly.size() - 1], p, guard);
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

/* computes the visible polygon from a guard g and a polygon p*/
vector <point2D> compute_visible_polygon(vector <point2D> poly, point2D guard) {

	printf("\n\n\nComputing polygon\n");
	vector <point2D> visible;

	char r = 'r';
	char l = 'l';

	/* check whether the polygon is simple. if not, print an error and exit */
	for (int i = 1; i < poly.size(); i++) {
		for (int j = 1; j < poly.size(); j++) {
			if (((j - 1) != i) and (j != i) and (j != (i - 1))) {
				if (intersect(poly[i - 1], poly[i], poly[j - 1], poly[j])) {
					printf("ERROR: Polygon is not simple, please draw a simple polygon\n");
					return visible;
				}
			}
		}
	}

	/* check whether the polygon is counterclockwise. if not, swap left and right */
	if (ccw(poly[0], poly[1], poly[2])) {
		printf("Counterclockwise\n");
	} else {
		r = 'l';
		l = 'r';
		printf("Clockwise\n");
	}

	/* stage 1 of preprocessing is to note whether each vertex is visible to the guard */
	vector <char> status;
	for (int i = 0; i < poly.size(); i++) {
		if (is_visible(poly, poly[i], guard, i)) {
			status.push_back('v');
		} else {
			status.push_back('i');
		}
	}

	/* to prevent issues with looping, add the final element to the start and the
		first element to the end */
	status.insert(status.begin(), status[status.size() - 1]);
	status.push_back(status[1]);

	/* next, for each visible point, note whether it is a right or left cusp */
	for (int i = 1; i < status.size() - 1; i++) {
		if (((status[i - 1] == 'i') and (status[i] == 'v') and (status[i + 1] == 'i')) or
			(((status[i - 1] == 'v') or (status[i - 1] == 'l') or (status[i - 1] == 'r'))
				and (status[i] == 'v') and (status[i + 1] == 'i')) or
			((status[i - 1] == 'i') and (status[i] == 'v') and (status[i + 1] == 'v'))) {
			if ((signed_area2D(guard, poly[i - 1], poly[i - 2]) < 0)
				and (signed_area2D(guard, poly[i - 1], poly[i]) < 0)) {
					status[i] = l;
			} else if ((signed_area2D(guard, poly[i - 1], poly[i - 2]) > 0)
				and (signed_area2D(guard, poly[i - 1], poly[i]) > 0)) {
					status[i] = r;
			}
		}
	}

	/* finally, construct the visible polygon */
	for (int i = 0; i < poly.size(); i++) {
		if (status[i + 1] == 'v') {
			point2D p;
			p.x = poly[i].x;
			p.y = poly[i].y;
			visible.push_back(p);
		} else if (status[i + 1] == 'l') {
			point2D p;
			p.x = poly[i].x;
			p.y = poly[i].y;
			visible.push_back(p);
			point2D r = ray_extend(poly, poly[i], guard, i);
			if (r.x != -1) { //Should never happen
				visible.push_back(r);
			}
		} else if (status[i + 1] == 'r') {
			point2D r = ray_extend(poly, poly[i], guard, i);
			if (r.x != -1) { //Should never happen
				visible.push_back(r);
			}
			point2D p;
			p.x = poly[i].x;
			p.y = poly[i].y;
			visible.push_back(p);
		}
	}

	printf("There are %d visible edges\n", visible.size());
	return visible;
}
