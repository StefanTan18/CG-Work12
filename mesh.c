#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mesh.h"
#include "matrix.h"
#include "draw.h"

//parse helper function
char * parse_helper(char *line) {
  char * s = calloc (sizeof(char), 1);
  strcpy(s, strsep(&line, "/"));
  return s;
}

//parses each line for args
char ** args_parser(char *line){
  char ** arr = calloc(sizeof(char*), 10);
  int i = 0;
  while (line) {
    arr[i] = strsep(&line, " ");
    if (strcmp(arr[i], "") != 0) {
      arr[i] = parse_helper(arr[i]);
      i++;
    }
  }
  return arr;
}

//parses .obj file for v and f
void obj_parser(struct matrix *polygons, char *file) {
  struct matrix * vertices;
  struct matrix * faces;

  FILE *f;
  char line[255];

  char type[3];
  double values[4];

  char ** args;

  vertices = new_matrix(3, 100);
  faces = new_matrix(4, 100);

  f = fopen(file, "r");
  while ( fgets(line, sizeof(line), f) != NULL ) {
    line[strlen(line) - 1] = '\0';

    //vertices
    if (strncmp(line, "v", 1) == 0) {
      sscanf(line, "%s %lf %lf %lf", type, values, values+1, values+2);
      if (strncmp(type, "v", strlen(type)) == 0) {
        add_mesh_point(vertices, values, VERTEX);
      }
    }

    //faces
    else if (strncmp(line, "f", 1) == 0) {
      args = args_parser(line);
      int i = 0;
      while (args[i+1] && (i < 4)) {
        values[i] = atof(args[i+1]);
        i++;
      }
      add_mesh_point(faces, values, FACE);
    }
  }

  add_mesh(polygons, vertices, faces);
}

//adds points or faces
void add_mesh_point(struct matrix * points, double values[4], int type) {

  if ( points->lastcol == points->cols )
    grow_matrix( points, points->lastcol + 100 );

  points->m[0][ points->lastcol ] = values[0];
  points->m[1][ points->lastcol ] = values[1];
  points->m[2][ points->lastcol ] = values[2];
  if (type == FACE) {
    points->m[3][ points->lastcol ] = values[3];
  }
  points->lastcol++;
}

//adds vertices to polygons matrix
void add_mesh(struct matrix *polygons, struct matrix *vertices, struct matrix *faces) {
  int f;
  int v0,v1,v2,v3;
  for (f = 0; f < faces->lastcol; f++) {
    v0 = ((int) faces->m[0][f]) - 1;
    v1 = ((int) faces->m[1][f]) - 1;
    v2 = ((int) faces->m[2][f]) - 1;
    v3 = ((int) faces->m[3][f]) - 1;

    add_polygon(polygons,
      vertices->m[0][v0], vertices->m[1][v0], vertices->m[2][v0],
      vertices->m[0][v1], vertices->m[1][v1], vertices->m[2][v1],
      vertices->m[0][v2], vertices->m[1][v2], vertices->m[2][v2]
    );
    //quadrilateral
    if (v3 > 0) {
      add_polygon(polygons,
        vertices->m[0][v0], vertices->m[1][v0], vertices->m[2][v0],
        vertices->m[0][v2], vertices->m[1][v2], vertices->m[2][v2],
        vertices->m[0][v3], vertices->m[1][v3], vertices->m[2][v3]
      );
    }
  }
  free_matrix(vertices);
  free_matrix(faces);
}
