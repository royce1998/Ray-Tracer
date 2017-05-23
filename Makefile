.PHONY : clean

raytracer : raytracer-project1.h vector3.c ray3.c main.c utils.[hc] color.c
	clang -g -Wall -lm -o raytracer utils.c vector3.c ray3.c color.c main.c

clean :
	rm -rf raytracer raytracer.dSYM

