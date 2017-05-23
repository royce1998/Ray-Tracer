#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "utils.h"
#include "raytracer-project2.h"

/* some convenience constructors for objects, etc. */

surface surf_const(double r, double g, double b)
{
  surface s;
  s.tag = CONSTANT;
  s.c.k = color_new(r,g,b);
  return s;
}

surface surf_fn(color*(*f)(vector3*,vector3*))
{
  surface s;
  s.tag = FUNCTION;
  s.c.f = f;
  return s;
}

/* create a container object for a sphere */
object *obj_sph(sphere *s)
{
  if (!s) {
    fprintf(stderr,"obj_sph given NULL\n");
    exit(1);
  }
  object *o = (object*)malloc(sizeof(object));
  check_malloc("obj_sph",o);
  o->tag = SPHERE;
  o->o.s = s;
  return o;
}

/* create a container object for a rectangle */
object *obj_rect(rectangle *r)
{
  if (!r) {
    fprintf(stderr,"obj_rect given NULL\n");
    exit(1);
  }
  object *o = (object*)malloc(sizeof(object));
  check_malloc("obj_rect",o);
  o->tag = RECTANGLE;
  o->o.r = r;
  return o;
}

/* private internal sphere constructor that leaves color slot uninitialized */
sphere *sph(double cx, double cy, double cz, double r, double sr, double sg, double \
	    sb)
{
  sphere *s = (sphere*)malloc(sizeof(sphere));
  check_malloc("sph",s);
  s->center = vector3_new(cx,cy,cz);
  if (r<0) {
    fprintf(stderr,"sph: r<0 (r=%lf)\n",r);
    exit(1);
  }
  s->radius = r;
  s->shine = color_new(sr,sg,sb);
  return s;
}

/* solid-color sphere constructor */
object *sphere_new(double cx, double cy, double cz,
                   double r,
                   double cr, double cg, double cb,
                   double sr, double sg, double sb)
{
  sphere *s = sph(cx,cy,cz,r,sr,sg,sb);
  s->surf   = surf_const(cr,cg,cb);
  return obj_sph(s);
}

/* private internal rectangle constructor that leaves color slot uninitialized */
rectangle *rect(double ulx, double uly, double ulz,
                double w, double h,
                double sr, double sg, double sb)
{
  rectangle *r = (rectangle*)malloc(sizeof(rectangle));
  check_malloc("rect",r);
  r->upper_left = vector3_new(ulx,uly,ulz);
  if (w<0) {
    fprintf(stderr,"rectangle_new: negative width (%lf)\n",w);
    exit(1);
  }
  r->w = w;
  if (h<0) {
    fprintf(stderr,"rectangle_new: negative height (%lf)\n",h);
    exit(1);
  }
  r->h = h;
  r->shine = color_new(sr,sg,sb);
  return r;
}

/* solid-color rectangle constructor */
object *rectangle_new(double ulx, double uly, double ulz,
                      double w, double h,
                      double cr, double cg, double cb,
                      double sr, double sg, double sb)
{
  rectangle *r = rect(ulx,uly,ulz,w,h,sr,sg,sb);
  r->surf = surf_const(cr,cg,cb);
  return obj_rect(r);
}

/* shallow-copy object list cons */
object_list *cons(object *o, object_list *os)
{
  object_list *l = (object_list*)malloc(sizeof(object_list));
  check_malloc("cons",l);
  l->first = *o;
  l->rest  = os;
  return l;
}

/* (mostly) shallow-copy scene constructor */
scene *scene_new(color *bg, color *amb, light *dl, object_list *objs)
{
  if (!bg || !amb || !dl) {
    fprintf(stderr,"scene_new: unexpected NULL\n");
    exit(1);
  }
  scene *sc = (scene*)malloc(sizeof(scene));
  check_malloc("scene_new",sc);
  sc->bg.tag = CONSTANT;
  sc->bg.c.k = color_new(bg->r,bg->g,bg->b);
  sc->amb_light = amb;
  sc->dir_light = dl;
  sc->objects = objs;
  return sc;
}

/* dl_new: new directional light */
/* note: direction vector need not be a unit vector, it is normalized here */
light *dl_new(double x, double y, double z, double r, double g, double b)
{
  light *dl = (light*)malloc(sizeof(light));
  check_malloc("dl_new",dl);
  dl->direction = vector3_new(x,y,z);  
  vector3_normify(dl->direction);
  dl->color = color_new(r,g,b);
  return dl;
}

/* shallow copy environment constructor */
environment *environment_new(double z, uint w, uint h, scene *sc)
{
  environment *e = (environment*)malloc(sizeof(environment));
  check_malloc("environment_new",e);
  e->camera_z = z;
  e->image_width = w;
  e->image_height = h;
  e->scene = sc;
  return e;					
}

/* *** stubs *** */

unsigned char byte(double x)
{
  if (x<0.0 || x>1.0) {
    fprintf(stderr,"byte: out of range %lf\n",x);
    exit(1);
  }
  return (unsigned char)(x*255.0);
}

void color_ppm(FILE *f, color *c)
{
  fprintf(f,"%d %d %d\n",byte(c->r),byte(c->g),byte(c->b));
}

void render_ppm(FILE *f, environment *e)
{
  vector3 *cam = vector3_new(0,0,e->camera_z);
  uint n_rows = e->image_height;
  uint n_cols = e->image_width;
  fprintf(f,"P3\n%d %d\n255\n",n_cols,n_rows);
  for (int row=0; row<n_rows; row++) {
    for (int col=0; col<n_cols; col++) {
      vector3 *curr = logical_coord(e->image_height,e->image_width,row,col);
      vector3 *cam_to_curr = vector3_sub(curr,cam);
      vector3_normify(cam_to_curr);
      ray3 *r = ray3_new(cam,cam_to_curr);
      color *k = trace_ray(r,e->scene);
	  if (!k){
		  if (e->scene->bg.tag == CONSTANT)
			  k = e->scene->bg.c.k;
		  else
			  k = e->scene->bg.c.f(vector3_scale(e->camera_z, r->direction), curr); // is this correct location of camera?
	  }
      color_ppm(f,k);
      free(curr);
      free(cam_to_curr);
      free(r);
      if (k!=e->scene->bg.c.k) {
	/* note: this test is checking for object inequality,  */
	/* i.e., it's not checking for "different color" as in */
	/* different hue */
	free(k);
      }
    }
  }
  free(cam);
}

int is_pre(char *str, char *buf){
	for (int i = 0; i < strlen(str); i++){
		if (str[i] != buf[i])
			return 0;
	}
	return 1;
}

environment *read_env()
{
	scene *s = malloc(sizeof(scene));
	environment *e = malloc(sizeof(environment));
	if (!s || !e){
		fprintf(stderr, "Malloc error.");
		exit(1);
	}
	s->objects = NULL;
	char buf[512];
	double r, g, b;
	while (fgets(buf, 512, stdin) != NULL) {
		if (is_pre("BG",buf)) {
			sscanf(buf,"BG %lf %lf %lf",&r,&g,&b);
			s->bg.tag = CONSTANT;
			s->bg.c.k = color_new(r,g,b);
		} else if (is_pre("ENV",buf)) {
			uint w = 0;
			uint h = 0;
			double z = 0;
			sscanf(buf,"ENV %lf %d %d",&z,&w,&h);
			e->camera_z = z;
			e->image_height = h;
			e->image_width = w;
		} else if (is_pre("AMB",buf)) {
			sscanf(buf,"AMB %lf %lf %lf",&r,&g,&b);
			s->amb_light = color_new(r, g, b);
		} else if (is_pre("DL",buf)) {
			double x, y, z;
			sscanf(buf,"DL %lf %lf %lf %lf %lf %lf",&x, &y, &z, &r, &g, &b);
			s->dir_light = malloc(sizeof(light));
			s->dir_light->direction = vector3_new(x, y, z);
			s->dir_light->color = color_new(r, g, b);
		} else if (is_pre("SPHERE",buf)) {
			double a,b,c,d,e,f,g,h,i,j;
			sscanf(buf,"SPHERE %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
			&a,&b,&c,&d,&e,&f,&g,&h,&i,&j);
			s->objects = cons(sphere_new(a,b,c,d,e,f,g,h,i,j), s->objects);
		} else if (is_pre("RECTANGLE",buf)) {
			double a,b,c,d,e,f,g,h,i,j,k;
			sscanf(buf,"RECTANGLE %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
			&a,&b,&c,&d,&e,&f,&g,&h,&i,&j,&k);
			s->objects = cons(rectangle_new(a,b,c,d,e,f,g,h,i,j,k), s->objects);
		} else {
			fprintf(stderr,"skipping \"%s\"\n",buf);
		}
	}
	e->scene = s;
	return e;
}

void env_free(environment *e){
	if (!e)
		return;
	free(e->scene->bg.c.k);
	free(e->scene->amb_light);
	free(e->scene->dir_light->direction);
	free(e->scene->dir_light->color);
	free(e->scene->dir_light);
	object_list *ls = e->scene->objects;
	while (!ls){
		if (ls->first.tag == SPHERE)
			free(ls->first.o.s);
		else
			free(ls->first.o.r);
		ls = ls->rest;
	}
	free(ls);
	free(e);
}

/* *** main program *** */

int main(int argc, char *argv[]) 
{
  if (argc==2 && !strcmp(argv[1],"1")) {
    /* n.b. WHITE sphere (so you can tell this apart from other similar scenes) */
    object *sphere0    = sphere_new(1,0,3,0.6,1,1,1,0,0,0);
    object *rectangle0 = rectangle_new(1,1.3,4,1,2.5,0,0,1,0,0,0);
    object_list *objs0 = cons(sphere0,cons(rectangle0,NULL));
    scene *scene0      = scene_new(color_new(0.8,0.8,0.8),
				   color_new(0.2,0.2,0.2),
				   dl_new(-1,1,-1,1,1,1),
				   objs0);
    environment *env0  = environment_new(-4.0,300,300,scene0);
    render_ppm(stdout,env0);
    /* env_free(env0); */
  } else if (argc==1) {
    environment *e = read_env();
    render_ppm(stdout,e);
    /* env_free(e); */
  }
  return 0;
}
