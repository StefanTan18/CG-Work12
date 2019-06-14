#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "draw.h"
#include "matrix.h"
#include "mesh.h"

/*======== void parse_args() ==========
  Inputs:   char *line
  Parses argument for slashes and removes them
  ====================*/
char * parse_slash(char *line) {
  char * s = calloc (sizeof(char), 1);
  strcpy(s, strsep(&line, "/"));
  return s;
}

/*======== void parse_args() ==========
  Inputs:   char *line
  Parses line into arguments
  ====================*/
char ** parse_args(char *line){
  char ** arr = calloc(sizeof(char*), 10);
  int i = 0;
  while (line) {
    arr[i] = strsep(&line, " ");
    if (strcmp(arr[i], "") != 0) {
      arr[i] = parse_slash(arr[i]);
      i++;
    }
  }
  return arr;
} //end parse_args


void parse(struct matrix *polygons, char *obj) {
  struct matrix *vertices;
  struct matrix *faces;

  FILE *file;
  char line[255];
  char type[3];
  double vals[4];

  char **args;

  vertices = new_matrix(3, 100);
  faces = new_matrix(4, 100);

  file = fopen(obj, "r");
  while (fgets(line, sizeof(line), file) != NULL) {
    line[strlen(line) - 1] = '\0';
    if (strncmp(line, "v", 1) == 0) {
      sscanf(line, "%s %lf %lf %lf", type, vals, vals+1, vals+2);
    if (strncmp(type, "v", strlen(type)) == 0) {
        add_points(vertices, vals, VERTEX);
      }
    }
    else if (strncmp(line, "f", 1) == 0) {
      args = parse_args(line);
      int i = 0;
      while (args[i+1] && (i < 4)) {
        //printf(args[i]);
        vals[i] = atof(args[i+1]);
        i++;
      }
      add_points(faces, vertices, FACE);
    }
  }
  add_mesh(polygons, vertices, faces);
}


void add_points(struct matrix * points, double vals[4], int type) {
  if ( points->lastcol == points->cols ) {
    grow_matrix( points, points->lastcol + 100 );
  }
  points->m[0][ points->lastcol ] = vals[0];
  points->m[1][ points->lastcol ] = vals[1];
  points->m[2][ points->lastcol ] = vals[2];
  if (type == FACE) {
    points->m[3][ points->lastcol ] = vals[3];
  }
  points->lastcol++;
}


void add_mesh(struct matrix *polygons, struct matrix *vertices, struct matrix *faces) {
  int f;
  int v0,v1,v2,v3;
  for (f = 0; f < faces->lastcol; f++) {
    v0 = ((int) faces->m[0][f]) - 1;
    v1 = ((int) faces->m[1][f]) - 1;
    v2 = ((int) faces->m[2][f]) - 1;
    v3 = ((int) faces->m[3][f]) - 1;

    add_polygon(polygons,
      vertices->m[0][v0],
      vertices->m[1][v0],
      vertices->m[2][v0],
      vertices->m[0][v1],
      vertices->m[1][v1],
      vertices->m[2][v1],
      vertices->m[0][v2],
      vertices->m[1][v2],
      vertices->m[2][v2]
    );
    //if 4th vertex exists
    if (v3 > 0) {
      //then it's quadrilateral
      add_polygon(polygons,
        vertices->m[0][v0],
        vertices->m[1][v0],
        vertices->m[2][v0],
        vertices->m[0][v2],
        vertices->m[1][v2],
        vertices->m[2][v2],
        vertices->m[0][v3],
        vertices->m[1][v3],
        vertices->m[2][v3]
      );
    }
  }
  free_matrix(vertices);
  free_matrix(faces);
}
