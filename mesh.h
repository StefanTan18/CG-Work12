#ifndef MESH_H
#define MESH_H

#include "draw.h"
#include "matrix.h"

#define VERTEX 0
#define FACE 1

void parse(struct matrix *polygons, char *obj);
void add_points(struct matrix *points, double vals[4], int t);
void add_mesh(struct matrix *polygons, struct matrix *vertices, struct matrix *faces);

#endif
