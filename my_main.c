/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser,
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  push: push a new origin matrix onto the origin stack

  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix
                     based on the provided values, then
                     multiply the current top of the
                     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a
                    temporary matrix, multiply it by the
                    current top of the origins stack, then
                    call draw_polygons.

  line: create a line based on the provided values. Store
        that in a temporary matrix, multiply it by the
        current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the screen
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"
#include "gmath.h"
#include "mesh.h"

void my_main() {

  int i;
  struct matrix *tmp;
  struct matrix *edges;
  struct matrix *polygons;
  struct stack *systems;
  screen t;
  zbuffer zb;
  color g;
  double step_3d = 200;
  double theta;

  //Lighting values here for easy access
  color ambient;
  ambient.red = 50;
  ambient.green = 50;
  ambient.blue = 50;

  double light[2][3];
  light[LOCATION][0] = 0.5;
  light[LOCATION][1] = 0.75;
  light[LOCATION][2] = 1;

  light[COLOR][RED] = 255;
  light[COLOR][GREEN] = 255;
  light[COLOR][BLUE] = 255;

  double view[3];
  view[0] = 0;
  view[1] = 0;
  view[2] = 1;

  //default reflective constants if none are set in script file
  struct constants white;
  white.r[AMBIENT_R] = 0.1;
  white.g[AMBIENT_R] = 0.1;
  white.b[AMBIENT_R] = 0.1;

  white.r[DIFFUSE_R] = 0.5;
  white.g[DIFFUSE_R] = 0.5;
  white.b[DIFFUSE_R] = 0.5;

  white.r[SPECULAR_R] = 0.5;
  white.g[SPECULAR_R] = 0.5;
  white.b[SPECULAR_R] = 0.5;

  //constants are a pointer in symtab, using one here for consistency
  struct constants *reflect;
  reflect = &white;

  systems = new_stack();
  tmp = new_matrix(4, 1000);
  polygons = new_matrix(4, 4);
  edges = new_matrix(4, 4);
  clear_screen( t );
  clear_zbuffer(zb);
  g.red = 0;
  g.green = 0;
  g.blue = 0;

  print_symtab();
  for (i=0;i<lastop;i++) {

    struct matrix *cs = peek(systems);

    printf("%d: ",i);
    switch (op[i].opcode)
      {
        case MESH:
          obj_parser(polygons, op[i].op.mesh.name);
          matrix_mult(cs, polygons);
          reflect = &white;
          if (op[i].op.mesh.constants != NULL) {
            reflect = op[i].op.mesh.constants->s.c;
          }
          draw_polygons(polygons, t, zb, view, light, ambient, reflect);
          polygons->lastcol = 0;
          break;
        case SAVE_COORDS:
          copy_matrix(peek(systems), op[i].op.save_coordinate_system.p->s.m);
          break;
        case PUSH:
          printf("Push");
          push(systems);
          break;
        case POP:
          printf("Pop");
          pop(systems);
          break;
        case MOVE:
          printf("Move");
          tmp = make_translate(op[i].op.move.d[0],op[i].op.move.d[1], op[i].op.move.d[2]);
          matrix_mult(cs, tmp);
          copy_matrix(tmp, cs);
          break;
        case ROTATE:
          printf("Rotate");
          theta = op[i].op.rotate.degrees * (M_PI / 180);
          if (op[i].op.rotate.axis == 0.0)
            tmp = make_rotX(theta);
          else if (op[i].op.rotate.axis == 1.0)
            tmp = make_rotY(theta);
          else
            tmp = make_rotZ(theta);

          matrix_mult(cs, tmp);
          copy_matrix(tmp, cs);
          break;
        case SCALE:
          printf("Scale");
          tmp = make_scale(op[i].op.scale.d[0],op[i].op.scale.d[1], op[i].op.scale.d[2]);
          matrix_mult(cs, tmp);
          copy_matrix(tmp, cs);
          break;
        case BOX:
          printf("Box");
          add_box(polygons, op[i].op.box.d0[0],op[i].op.box.d0[1], op[i].op.box.d0[2], op[i].op.box.d1[0],op[i].op.box.d1[1], op[i].op.box.d1[2]);
          if (op[i].op.box.cs) {
            cs = op[i].op.box.cs->s.m;
          }
          matrix_mult(cs, polygons);
          reflect = &white;
          if (op[i].op.box.constants != NULL) {
            reflect = op[i].op.box.constants->s.c;
          }
          draw_polygons(polygons, t, zb, view, light, ambient, reflect);
          polygons->lastcol = 0;
          break;
        case SPHERE:
          printf("Sphere");
          add_sphere(polygons, op[i].op.sphere.d[0],op[i].op.sphere.d[1], op[i].op.sphere.d[2], op[i].op.sphere.r, step_3d);
          if (op[i].op.sphere.cs) {
            cs = op[i].op.sphere.cs->s.m;
          }
          matrix_mult(cs, polygons);
          reflect = &white;
          if (op[i].op.sphere.constants != NULL) {
            reflect = op[i].op.sphere.constants->s.c;
          }
          draw_polygons(polygons, t, zb, view, light, ambient, reflect);
          polygons->lastcol = 0;
          break;
        case TORUS:
          printf("Torus");
          add_torus(polygons, op[i].op.torus.d[0],op[i].op.torus.d[1], op[i].op.torus.d[2], op[i].op.torus.r0,op[i].op.torus.r1, step_3d);
          if (op[i].op.torus.cs) {
            cs = op[i].op.torus.cs->s.m;
          }
          matrix_mult(cs, polygons);
          reflect = &white;
          if (op[i].op.torus.constants != NULL) {
            reflect = op[i].op.torus.constants->s.c;
          }
          draw_polygons(polygons, t, zb, view, light, ambient, reflect);
          polygons->lastcol = 0;
          break;
        case CONSTANTS:
          printf("Constants");
          break;
        case LINE:
          printf("Line");
          add_edge(edges, op[i].op.line.p0[0],op[i].op.line.p0[1], op[i].op.line.p0[1], op[i].op.line.p1[0],op[i].op.line.p1[1], op[i].op.line.p1[1]);
          matrix_mult(cs, edges);
          draw_lines(edges, t, zb, g);
          edges->lastcol = 0;
          break;
        case SAVE:
          printf("Save");
          save_extension(t, op[i].op.save.p->name);
          break;
        case DISPLAY:
          printf("Display");
          display(t);
          break;
        }
    printf("\n");
  }
}
