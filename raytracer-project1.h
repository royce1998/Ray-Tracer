#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

/* ====================================== */ 
/* ====================================== */ 
/* === STRUCTURE AND TYPE DEFINITIONS === */ 
/* ====================================== */ 
/* ====================================== */ 

/* "uint" is defined for convenience */
typedef unsigned int uint;

typedef struct {
  double x;
  double y;
  double z;
} vector3;

typedef struct {
  /* each value in a color must be on interval [0,1] */
  double r;
  double g;
  double b;
} color;

typedef struct {
  vector3 *origin;
  vector3 *direction; /* note: direction must be a unit vector */
} ray3;

typedef struct {
  vector3 *center;
  double   radius;
  color   *surface_color;
} sphere;

/* a type definition for axis-aligned rectangles */
typedef struct {
  vector3 *upper_left;
  double   w;
  double   h;
  color   *surface_color;
} rectangle;

enum object_tag {
  SPHERE,
  RECTANGLE
};

union object_union {
  sphere    *s;
  rectangle *r;
};

typedef struct {
  enum  object_tag tag;
  union object_union o;
} object;

typedef struct object_list object_list;
struct object_list {
  object first;
  object_list *rest;
};
/* convention: NULL is the empty object list */

typedef struct {
  color *background_color;
  object_list *objects;
} scene;

typedef struct {
  double   t;
  color   *surface_color;
  vector3 *surface_normal;
} hit;

typedef struct {
  double camera_z;
  uint   image_height;
  uint   image_width;
  scene *scene;
} environment;

/* === project 1 operations === */

/* NOTE: you may add function prototypes to this list, but delete or modify none of them. */

vector3 *vector3_new(double x, double y, double z);
vector3 *vector3_add(vector3 *v1, vector3 *v2);
vector3 *vector3_sub(vector3 *v1, vector3 *v2);
vector3 *vector3_negate(vector3 *v);
vector3 *vector3_scale(double scalar, vector3 *v);
double   vector3_dot(vector3 *v1, vector3 *v2);
double   vector3_magnitude(vector3 *v);
vector3 *vector3_normalize(vector3 *v);
void     vector3_normify(vector3 *v); /* normalize vector in place, w/o allocating a new one */
char    *vector3_tos(vector3 *v);
void     vector3_show(FILE *f, vector3 *v);
  
color   *color_new(double r, double g, double b);
char    *color_tos(color *c);
void     color_show(FILE *f, color *c);

ray3    *ray3_new(vector3 *origin, vector3 *direction);
void     ray3_free(ray3 *r);
vector3 *ray3_position(ray3 *r, double t);
char    *ray3_tos(ray3 *r);
void     ray3_show(FILE *f, ray3 *r);

vector3 *logical_coord(uint image_height, uint image_width, uint pixel_row, uint pixel_col);
hit     *intersect(ray3 *r, object *obj); /* return NULL for miss */
color   *trace_ray(ray3 *r, scene *s);
void     render_ppm(FILE *f, environment *e);

#endif /* __RAYTRACER_H__ */
