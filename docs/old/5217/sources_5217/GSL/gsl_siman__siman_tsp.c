/* siman/siman_tsp.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Mark Galassi
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "gsl__config.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "gsl_math.h"
#include "gsl_rng.h"
#include "gsl_siman.h"
#include "gsl_ieee_utils.h"

/* set up parameters for this simulated annealing run */

#define N_TRIES 200             /* how many points do we try before stepping */
#define ITERS_FIXED_T 2000      /* how many iterations for each T? */
#define STEP_SIZE 1.0           /* max step size in random walk */
#define K 1.0                   /* Boltzmann constant */
#define T_INITIAL 5000.0        /* initial temperature */
#define MU_T 1.002              /* damping factor for temperature */
#define T_MIN 5.0e-1

gsl_siman_params_t params = {N_TRIES, ITERS_FIXED_T, STEP_SIZE,
                             K, T_INITIAL, MU_T, T_MIN};

struct s_tsp_city {
  const char * name;
  double lat, longitude;        /* coordinates */
};
typedef struct s_tsp_city Stsp_city;

void prepare_distance_matrix(void);
void exhaustive_search(void);
void print_distance_matrix(void);
double city_distance(Stsp_city c1, Stsp_city c2);
double Etsp(void *xp);
double Mtsp(void *xp, void *yp);
void Stsp(const gsl_rng * r, void *xp, double step_size);
void Ptsp(void *xp);

/* in this table, latitude and longitude are obtained from the US
   Census Bureau, at http://www.census.gov/cgi-bin/gazetteer */

Stsp_city cities[] = {{"Santa Fe",    35.68,   105.95},
                      {"Phoenix",     33.54,   112.07},
                      {"Albuquerque", 35.12,   106.62},
                      {"Clovis",      34.41,   103.20},
                      {"Durango",     37.29,   107.87},
                      {"Dallas",      32.79,    96.77},
                      {"Tesuque",     35.77,   105.92},
                      {"Grants",      35.15,   107.84},
                      {"Los Alamos",  35.89,   106.28},
                      {"Las Cruces",  32.34,   106.76},
                      {"Cortez",      37.35,   108.58},
                      {"Gallup",      35.52,   108.74}};

#define N_CITIES (sizeof(cities)/sizeof(Stsp_city))

double distance_matrix[N_CITIES][N_CITIES];

/* distance between two cities */
double city_distance(Stsp_city c1, Stsp_city c2)
{
  const double earth_radius = 6375.000; /* 6000KM approximately */
  /* sin and cos of lat and long; must convert to radians */
  double sla1 = sin(c1.lat*M_PI/180), cla1 = cos(c1.lat*M_PI/180),
    slo1 = sin(c1.longitude*M_PI/180), clo1 = cos(c1.longitude*M_PI/180);
  double sla2 = sin(c2.lat*M_PI/180), cla2 = cos(c2.lat*M_PI/180),
    slo2 = sin(c2.longitude*M_PI/180), clo2 = cos(c2.longitude*M_PI/180);

  double x1 = cla1*clo1;
  double x2 = cla2*clo2;

  double y1 = cla1*slo1;
  double y2 = cla2*slo2;

  double z1 = sla1;
  double z2 = sla2;

  double dot_product = x1*x2 + y1*y2 + z1*z2;

  double angle = acos(dot_product);

  /* distance is the angle (in radians) times the earth radius */
  return angle*earth_radius;
}

/* energy for the travelling salesman problem */
double Etsp(void *xp)
{
  /* an array of N_CITIES integers describing the order */
  int *route = (int *) xp;
  double E = 0;
  unsigned int i;

  for (i = 0; i < N_CITIES; ++i) {
    /* use the distance_matrix to optimize this calculation; it had
       better be allocated!! */
    E += distance_matrix[route[i]][route[(i + 1) % N_CITIES]];
  }

  return E;
}

double Mtsp(void *xp, void *yp)
{
  int *route1 = (int *) xp, *route2 = (int *) yp;
  double distance = 0;
  unsigned int i;

  for (i = 0; i < N_CITIES; ++i) {
    distance += ((route1[i] == route2[i]) ? 0 : 1);
  }

  return distance;
}

/* take a step through the TSP space */
void Stsp(const gsl_rng * r, void *xp, double step_size)
{
  int x1, x2, dummy;
  int *route = (int *) xp;

  step_size = 0 ; /* prevent warnings about unused parameter */

  /* pick the two cities to swap in the matrix; we leave the first
     city fixed */
  x1 = (gsl_rng_get (r) % (N_CITIES-1)) + 1;
  do {
    x2 = (gsl_rng_get (r) % (N_CITIES-1)) + 1;
  } while (x2 == x1);

  dummy = route[x1];
  route[x1] = route[x2];
  route[x2] = dummy;
}

void Ptsp(void *xp)
{
  unsigned int i;
  int *route = (int *) xp;
  printf("  [");
  for (i = 0; i < N_CITIES; ++i) {
    printf(" %d ", route[i]);
  }
  printf("]  ");
}

int main(void)
{
  int x_initial[N_CITIES];
  unsigned int i;

  const gsl_rng * r = gsl_rng_alloc (gsl_rng_env_setup()) ;

  gsl_ieee_env_setup ();

  prepare_distance_matrix();

  /* set up a trivial initial route */
  printf("# initial order of cities:\n");
  for (i = 0; i < N_CITIES; ++i) {
    printf("# \"%s\"\n", cities[i].name);
    x_initial[i] = i;
  }

  printf("# distance matrix is:\n");
  print_distance_matrix();

  printf("# initial coordinates of cities (longitude and latitude)\n");
  /* this can be plotted with */
  /* ./siman_tsp > hhh ; grep city_coord hhh | awk '{print $2 "   " $3}' | xyplot -ps -d "xy" > c.eps */
  for (i = 0; i < N_CITIES+1; ++i) {
    printf("###initial_city_coord: %g %g \"%s\"\n",
           -cities[x_initial[i % N_CITIES]].longitude,
           cities[x_initial[i % N_CITIES]].lat,
           cities[x_initial[i % N_CITIES]].name);
  }

/*   exhaustive_search(); */

  gsl_siman_solve(r, x_initial, Etsp, Stsp, Mtsp, Ptsp, NULL, NULL, NULL,
                  N_CITIES*sizeof(int), params);

  printf("# final order of cities:\n");
  for (i = 0; i < N_CITIES; ++i) {
    printf("# \"%s\"\n", cities[x_initial[i]].name);
  }

  printf("# final coordinates of cities (longitude and latitude)\n");
  /* this can be plotted with */
  /* ./siman_tsp > hhh ; grep city_coord hhh | awk '{print $2 "   " $3}' | xyplot -ps -d "xy" > c.eps */
  for (i = 0; i < N_CITIES+1; ++i) {
    printf("###final_city_coord: %g %g %s\n",
           -cities[x_initial[i % N_CITIES]].longitude,
           cities[x_initial[i % N_CITIES]].lat,
           cities[x_initial[i % N_CITIES]].name);
  }

  printf("# ");
  fflush(stdout);
#if 0
  system("date");
#endif /* 0 */
  fflush(stdout);

  return 0;
}

void prepare_distance_matrix()
{
  unsigned int i, j;
  double dist;

  for (i = 0; i < N_CITIES; ++i) {
    for (j = 0; j < N_CITIES; ++j) {
      if (i == j) {
        dist = 0;
      } else {
        dist = city_distance(cities[i], cities[j]);
      }
      distance_matrix[i][j] = dist;
    }
  }
}

void print_distance_matrix()
{
  unsigned int i, j;

  for (i = 0; i < N_CITIES; ++i) {
    printf("# ");
    for (j = 0; j < N_CITIES; ++j) {
      printf("%15.8f   ", distance_matrix[i][j]);
    }
    printf("\n");
  }
}

/* [only works for 12] search the entire space for solutions */
static double best_E = 1.0e100, second_E = 1.0e100, third_E = 1.0e100;
static int best_route[N_CITIES];
static int second_route[N_CITIES];
static int third_route[N_CITIES];
static void do_all_perms(int *route, int n);

void exhaustive_search()
{
  static int initial_route[N_CITIES] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  printf("\n# ");
  fflush(stdout);
#if 0
  system("date");
#endif
  fflush(stdout);
  do_all_perms(initial_route, 1);
  printf("\n# ");
  fflush(stdout);
#if 0
  system("date");
#endif /* 0 */
  fflush(stdout);

  printf("# exhaustive best route: ");
  Ptsp(best_route);
  printf("\n# its energy is: %g\n", best_E);

  printf("# exhaustive second_best route: ");
  Ptsp(second_route);
  printf("\n# its energy is: %g\n", second_E);

  printf("# exhaustive third_best route: ");
  Ptsp(third_route);
  printf("\n# its energy is: %g\n", third_E);
}

/* James Theiler's recursive algorithm for generating all routes */
static void do_all_perms(int *route, int n)
{
  if (n == (N_CITIES-1)) {
    /* do it! calculate the energy/cost for that route */
    double E;
    E = Etsp(route);            /* TSP energy function */
    /* now save the best 3 energies and routes */
    if (E < best_E) {
      third_E = second_E;
      memcpy(third_route, second_route, N_CITIES*sizeof(*route));
      second_E = best_E;
      memcpy(second_route, best_route, N_CITIES*sizeof(*route));
      best_E = E;
      memcpy(best_route, route, N_CITIES*sizeof(*route));
    } else if (E < second_E) {
      third_E = second_E;
      memcpy(third_route, second_route, N_CITIES*sizeof(*route));
      second_E = E;
      memcpy(second_route, route, N_CITIES*sizeof(*route));
    } else if (E < third_E) {
      third_E = E;
      memcpy(route, third_route, N_CITIES*sizeof(*route));
    }
  } else {
    int new_route[N_CITIES];
    unsigned int j;
    int swap_tmp;
    memcpy(new_route, route, N_CITIES*sizeof(*route));
    for (j = n; j < N_CITIES; ++j) {
      swap_tmp = new_route[j];
      new_route[j] = new_route[n];
      new_route[n] = swap_tmp;
      do_all_perms(new_route, n+1);
    }
  }
}
