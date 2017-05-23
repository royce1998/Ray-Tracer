#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utils.h"
#include "raytracer-project1.h"

ray3 *ray3_new(vector3 *origin, vector3 *direction){
	ray3 *r = (ray3*)malloc(sizeof(ray3));
	check_malloc("ray3_new",r);
	r->origin = origin;
	r->direction = direction;
	return r;
}

vector3 *ray3_position(ray3 *r, double t)
{
  vector3 *t1 = vector3_scale(t,r->direction);
  vector3 *t2 = vector3_add(r->origin,t1);
  free(t1);
  return t2;
}

void ray3_free(ray3 *r){
	free(r->origin);
	free(r->direction);
	free(r);
}

char *ray3_tos(ray3 *r){
	char buf[256]; /* way more than enough, but chars are cheap */  
	sprintf(buf,"Origin : <%lf,%lf,%lf>, Direction : <%lf,%lf,%lf>",
	r->origin->x, r->origin->y, r->origin->z, r->direction->x, r->direction->y, r->direction->z);
	return strdup(buf);
}

void ray3_show(FILE *f, ray3 *r){
	fprintf(f, "Origin : <%lf,%lf,%lf>, Direction : <%lf,%lf,%lf>",
	r->origin->x, r->origin->y, r->origin->z, r->direction->x, r->direction->y, r->direction->z);
}

vector3 *logical_coord(uint image_height, uint image_width, uint pixel_row, uint pixel_col){
//	printf("height = %d, width = %d, row = %d, col = %d\n", image_height, image_width, pixel_row, pixel_col);
	double max = (image_height > image_width) ? image_height : image_width;
//	printf("max = %f\n", max);
	double x = image_width / max * -1 + 2 / max * pixel_col + 1 / max;
	double y = image_height / max * -1 + 2 / max * pixel_row + 1 / max;
//	printf("x = %f, y = %f\n", x, y);
	return vector3_new(x, y, 0);
}

hit *intersect(ray3 *r, object *obj){
	if (obj->tag == SPHERE){
		vector3 *a = vector3_sub(r->origin, obj->o.s->center);
//		vector3_show(stdout, r->origin);
//		vector3_show(stdout, obj->o.s->center);
//		vector3_show(stdout, a);
//		printf("\n");
		double b = vector3_dot(a, r->direction);
		double c = vector3_dot(a, a) - pow(obj->o.s->radius, 2);
		double d = b * b - c;
		double t = b * -1 - sqrt(d);
//		printf("b = %f, c = %f, d = %f, t = %f\n", b, c, d, t);
		if (d > 0 && t > 0){
//			printf("hello!");
			hit *h = (hit *)malloc(sizeof(hit));
			h->t = t;
			h->surface_color = obj->o.s->surface_color;
			h->surface_normal = vector3_normalize(vector3_sub(vector3_scale(t, r->direction), obj->o.s->center));
//			color_show(stdout, h->surface_color);
			return h;
		}
		else
			return NULL;
	}
	else if (obj->tag == RECTANGLE){
		double t = -1.0 * ((vector3_dot(r->origin, vector3_new(0, 0, -1))) + obj->o.r->upper_left->z);
		t /= vector3_dot(r->direction, vector3_new(0, 0, -1));
//		vector3_show(stdout, r->origin);
//		vector3_show(stdout, r->direction);
//		printf("Dot1 = %f, Dot2 = %f\nz = %f, t = %f\n", vector3_dot(r->origin, vector3_new(0, 0, -1)), vector3_dot(r->direction, vector3_new(0, 0, -1)), obj->o.r->upper_left->z, t);
		if (t > 0){
//			printf("hello2!");
//			exit(1);
			vector3 *intersection = vector3_scale(t, r->direction);
			if (intersection->y >= -1 * obj->o.r->upper_left->y && intersection->y <= -1 * (obj->o.r->upper_left->y - obj->o.r->h)
				&& intersection->x >= obj->o.r->upper_left->x && intersection->x <= obj->o.r->upper_left->x + obj->o.r->w){
//				printf("hello2!");
				hit *h = (hit *)malloc(sizeof(hit));
				h->t = t;
				h->surface_color = obj->o.r->surface_color;
				h->surface_normal = vector3_new(0, 0, -1);
//				color_show(stdout, h->surface_color);
				return h;
			}
		}
		else
			return NULL;
	}
	else{
		fprintf(stderr, "Bad shape tag.");
		exit(1);
	}
	return NULL; // will never get here
}

color *trace_ray(ray3 *r, scene *s){
	hit *h = malloc(sizeof(hit));
	object_list *objlst = s->objects;
	color *c = malloc(sizeof(c));
	double dist = pow(10, 308); // ~ max_double
	int hitsomething = 0;
	while (objlst != NULL){
		h = intersect(r, &objlst->first);
		if (h != NULL && h->t < dist){
//			printf("hi!");
			dist = h->t;
			c = h->surface_color;
			hitsomething = 1;
		}
		objlst = objlst->rest;
	}
	if (hitsomething)
		return c;
	else{
//		color_show(stdout, s->background_color);
		return s->background_color;
	}
}
