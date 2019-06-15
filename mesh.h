#ifndef MESH_H
#define MESH_H

#define VERTEX 0
#define FACE 1

#include "matrix.h"
#include "draw.h"

char * parse_helper(char *line);
char ** args_parser(char *line);
void obj_parser(struct matrix *polygons, char *file);
void add_mesh_point( struct matrix *points, double values[4], int type);
void add_mesh(struct matrix *polygons, struct matrix *vertices, struct matrix *faces);

#endif
