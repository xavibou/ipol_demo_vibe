/*----------------------------------------------------------------------------

  Copyright (c) 2018-2022 Xavier Bou Hernandez <xavibouhae@gmail.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  ----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include "iio.h"
#include "vibe-background-sequential.h"

/*----------------------------------------------------------------------------*/
#ifndef FALSE
#define FALSE 0
#endif /* !FALSE */

#ifndef TRUE
#define TRUE 1
#endif /* !TRUE */

/*----------------------------------------------------------------------------*/
/* PI */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif /* !M_PI */

/*----------------------------------------------------------------------------*/
/* Label for pixels with undefined gradient. */
#define NOTDEF -2.0

/*----------------------------------------------------------------------------*/
/* Fatal error, print a message to standard error and exit
 */
void error(char * msg)
{
  fprintf(stderr,"error: %s\n", msg);
  exit(EXIT_FAILURE);
}

/*----------------------------------------------------------------------------*/
/* Memory allocation, print an error and exit if fail
 */
void * xmalloc(size_t size)
{
  void * p;
  if( size == 0 ) error("xmalloc input: zero size");
  p = malloc(size);
  if( p == NULL ) error("out of memory");
  return p;
}

/*----------------------------------------------------------------------------*/
/*  Uniform random number generator in [1,2147483562].

    This algorithm is described and analyzed on:

      "Efficient and Portable Combined Random Number Generators"
      by Pierre L'Ecuyer, Communications of the ACM, vol. 31, num. 6,
      pp. 742-749 and 774, June 1988.

    This code is a modification from the code available at
      http://cg.scs.carleton.ca/~luc/lecuyer.c
    on February 5, 2011.

    The algorithm is based on two combined Multiplicative Linear
    Congruential Generators (MLCGs). The generator produces double
    numbers in the range (0,1), (it will never return 0.0 or 1.0).
    The period of the generator is about 2.30584 x 10^18,
    in the order of 2^61.

    A normal call is in the form 'uniform_rand(NULL)'.
    But it can also be called giving a pointer to long with a seed:
      long seed = 263287632;
      uniform_rand(&seed);
    This is usually done only once. The function 'rand_time_seed'
    performs this initilization with the current time.

    The state of the generator is determined by the static variables
    s1 and s2, that must take values in the following intervals:

      s1 in [1,2147483562]
      s2 in [1,2147483398]
 */
long random_number(long * seed)
{
  static long s1 = 55555;
  static long s2 = 99999;
  long k,z;

  /* Initialization of the seed, if demanded. */
  if( seed != NULL ) s1 = 1 + (*seed % 2147483561); /* s1 in [1,2147483562] */

  /* First MLCG: s1 = (40014 * s1) mod 2147483563, efficient implementation */
  k = s1 / 53668;
  s1 = 40014 * ( s1 % 53668 ) - k * 12211;
  if( s1 < 0 ) s1 += 2147483563;

  /* Second MLCG: s2 = (40692 * s2) mod 2147483399, efficient implementation */
  k = s2 / 52774;
  s2 = 40692 * ( s2 % 52774 ) - k * 3791;
  if( s2 < 0 ) s2 += 2147483399;

  /* Combination of both MLCGs */
  z = ( s1 - 2147483563 ) + s2;
  if(z < 1) z += 2147483562;

  return z;
}


/*----------------------------------------------------------------------------*/
/* Initialize seed with the current time.                                     */
void rand_time_seed(void)
{
  long s;
  s = time(NULL);
  random_number(&s);
}


/*----------------------------------------------------------------------------*/
/* get an optional parameter from arguments

   if found, the value is returned and it is removed from the list of arguments.
   adapted from pick_option by Enric Meinhardt-Llopis.

   example: if arguments are "command -p 123 input.txt",
            char * p = get_option(&argc,&argv,"-p","0");
            will give "123" in p and leave arguments as "command input.txt"
 */
char * get_option_arg(int * argc, char *** argv, char * opt, char * def)
{
  int i,j;

  for(i=0; i<(*argc-1); i++) /* last argument cannot have an optional value */
    if( strcmp( (*argv)[i], opt ) == 0 )  /* option opt found */
      {
        char * r = (*argv)[i+1];     /* save the optional value to return   */
        for(j=i; j < (*argc-2); j++) /* shift arguments to remove opt+value */
          (*argv)[j] = (*argv)[j+2];
        *argc -= 2;     /* decrease the number of arguments in 2, opt+value */
        return r;  /* return the value found for option opt */
      }
  return def; /* option not found, return the default value */
}

/*----------------------------------------------------------------------------*/
/* get an option from arguments

   if found, return true and it is removed from the list of arguments.
   adapted from pick_option by Enric Meinhardt-Llopis.

   example: if arguments are "command -opt input.txt",
            int = get_option(&argc,&argv,"-opt");
            will return true and leave arguments as "command input.txt"
 */
int get_option(int * argc, char *** argv, char * opt)
{
  int i,j;

  for(i=0; i<*argc; i++)                  /* loop over arguments */
    if( strcmp( (*argv)[i], opt ) == 0 )  /* option opt found */
      {
        for(j=i; j < (*argc-1); j++)      /* shift arguments to remove opt */
          (*argv)[j] = (*argv)[j+1];
        *argc -= 1;                       /* decrease the number of arguments */
        return TRUE;                      /* return option found! */
      }
  return FALSE;                           /* option not found */
}

/*----------------------------------------------------------------------------*/
void usage()
{
  fprintf(stderr,"----------------------------------------");
  fprintf(stderr,"----------------------------------------\n");
  fprintf(stderr,"Adaptation of ViBe source code for IPOL Demo\n");
  fprintf(stderr,"Background Subtraction algorithm\n");
  fprintf(stderr,"Copyright (c) 2018-2022 ");
  fprintf(stderr,"Xavier Bou Hernandez <xavibouhae@gmail.com>\n");
  fprintf(stderr,"----------------------------------------");
  fprintf(stderr,"----------------------------------------\n");
  fprintf(stderr,"\n");
  fprintf(stderr,"usage: vibe [options] image1 image2 [image3 ... imageN]\n");
  fprintf(stderr,"\n");
  fprintf(stderr,"\n");
  fprintf(stderr,"options:\n");
  fprintf(stderr," -s numberOfSamples   sets the number of samples in the background model\n");
  fprintf(stderr," -r matchingThreshold   sets the radius R (refer to article) or matching threshold\n");
  fprintf(stderr," -c matchingNumber   sets the minimum cardinality (refer to article) or number of matches\n");
  fprintf(stderr," -uf updateFactor   sets the update factor for the update mechanism\n");
  fprintf(stderr,"\n");
  fprintf(stderr,"The output images are stored with the same name and path as");
  fprintf(stderr," the\ninput images but adding the suffix '_mask.png'.");
  fprintf(stderr,"\n");
  fprintf(stderr,"examples: vibe img1.jpg img2.jpg img3.jpg img4.jpg\n");
  fprintf(stderr,"          vibe -s 40 -r 15 -c 4 -uf 12 data/*.jpg\n");

  exit(EXIT_FAILURE);
}

/*----------------------------------------------------------------------------*/
/*                                    main                                    */
/*----------------------------------------------------------------------------*/
int main(int argc, char ** argv)
{
  int X, Y, C, F, n;
  vibeModel_Sequential_t *model = NULL;
  uint8_t *segmentation_map = NULL;
  int numberOfSamples = atoi(get_option_arg(&argc,&argv,"-s","20"));
  int matchingThreshold = atoi(get_option_arg(&argc,&argv,"-r","20"));
  int matchingNumber = atoi(get_option_arg(&argc,&argv,"-c","2"));
  int updateFactor = atoi(get_option_arg(&argc,&argv,"-uf","16"));

  /* usage */
  if( argc < 3 ) usage();

  /* read input */
  if( numberOfSamples < matchingNumber) error("Number of samples must be greater or equal than the matching number");
  if( numberOfSamples <= 0 ) error("Number of samples must be greater than 0");
  if( matchingThreshold <= 0 ) error("Matching threshold must be greater than 0");
  if( matchingNumber <= 0 ) error("Matching number must be greater than 0");
  if( updateFactor <= 0 ) error("Update factor must be greater than 0");
  F = argc - 1;

  /* Start execution time tracking */
  clock_t begin = clock();
  
  /* Iterate through sequence */
  for(n=0; n<F; n++) 
  {

    uint8_t * image;

    /* read new image */
    image = iio_read_image_uint8_vec(argv[n + 1], &X, &Y, &C);

    if( !n )
    {
      /* init ViBe model and set the parameters */
      model = (vibeModel_Sequential_t *)libvibeModel_Sequential_New();
      libvibeModel_Sequential_SetNumberOfSamples(model, numberOfSamples);
      libvibeModel_Sequential_SetMatchingThreshold(model, matchingThreshold);
      libvibeModel_Sequential_SetMatchingNumber(model, matchingNumber);

      /* Allocates the model and initialize it with the first image. */
      libvibeModel_Sequential_AllocInit_8u_C3R(model, image, X, Y);
      libvibeModel_Sequential_SetUpdateFactor(model, updateFactor);
    }

    segmentation_map = (uint8_t*)malloc(X * Y * sizeof(uint8_t));

    /* Segmentation step: produces the output mask. */
    libvibeModel_Sequential_Segmentation_8u_C3R(model, image, segmentation_map);

    /* Next, we update the model. This step is optional. */
    libvibeModel_Sequential_Update_8u_C3R(model, image, segmentation_map);

    char filename[512];
    FILE *fp;
    sprintf(filename, "%s_mask.png", argv[n + 1]);
    fp = fopen(filename, "w+");
    iio_write_image_uint8_vec(filename, segmentation_map, X, Y, 1);
    fclose(fp);

    /* free memory. */
    free( (void *) image );
  }

  /* Cleanup allocated memory. */
  libvibeModel_Sequential_Free(model);

  /* Start execution time tracking */
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  int fps = F / time_spent;
  printf("\nExecution time: %f seconds  |  %d fps\n\n", time_spent, fps);
}

/*----------------------------------------------------------------------------*/
