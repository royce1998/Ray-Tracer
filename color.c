#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "raytracer-project1.h"

color *color_new(double r, double g, double b)
{
	if (r > 1 || r < 0 || g > 1 || g < 0 || b > 1 || b < 0){
		fprintf(stderr, "Color value(s) out of range.");
		exit(1);
	}
	color *c = (color *)malloc(sizeof(color));
	c->r = r;
	c->g = g;
	c->b = b;
	return c;
}

char *color_format = "(r=%lf,g=%lf,b=%lf)";

char *color_tos(color *c)
{
  char buf[256];
  sprintf(buf,color_format,c->r,c->g,c->b);
  return strdup(buf);
}

void color_show(FILE *f, color *c)
{
  fprintf(f,color_format,c->r,c->g,c->b);
}
