/* FriBidi - Library of BiDi algorithm
 * Copyright (C) 1999 Dov Grobgeld
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include "fribidi.h"

#define TEST_STRING "a THE QUICK BROWN 123,456 (FOX JUMPS OVER) THE LAZY DOG the quick brown fox jumps over the lazy dog THE QUICK BROWN FOX JUMPS 123,456 OVER THE LAZY DOG"

#define CASE(s) if (strcmp(S_, s) == 0)

double utime ()
{
  struct tms tb;
  times(&tb);
  return 0.01 * tb.tms_utime;
}

int main(int argc, char *argv[])
{
  int argp=1;
  guchar S_[255];
  FriBidiChar us[255];
  int len;
  FriBidiChar out_us[255];
  guint16 positionLtoV[255], positionVtoL[255];
  guint8 embedding_list[255];
  FriBidiCharType base;
  int i;
  int niter = 1000;
  double time0, time1;
  
  while(argp< argc && argv[argp][0] == '-')
    {
      gchar *S_ = argv[argp++];

      CASE("-help") {
	printf(
	       "fribidi_benchmark - A program for benchmarking the fribid library\n"
	       "\n"
	       "Syntax:\n"
	       "    fribidi_benchmark -niter niter\n"
	       "\n"
	       "Description:\n"
	       "    A program for benchmarking the speed of the BiDi algorithm.\n"
	       "\n"
	       "Options:\n"
	       "    -niter niter  Number of iterations. Default is 1000.\n"
	       );
	exit(0);
      }

      CASE("-niter")  { niter = atoi(argv[argp++]); continue; };
      fprintf(stderr, "Unknown option %s!\n", S_);
      exit(0);
    }
  

  strcpy(S_, TEST_STRING);
  len= strlen(S_);

  /* Map capital to iso Hebrew */
  for (i=0; i<len; i++)
    if (S_[i]>='A' && S_[i]<='Z')
      S_[i]+= 0xE0 - 'A';  /* Map to iso Hebrew */

  fribidi_iso8859_8_to_unicode(S_, us);

  /* Start timer */
  time0 = utime();

  for (i=0; i<niter; i++) {
    /* Create a bidi string */
    base = FRIBIDI_TYPE_N;
    fribidi_log2vis(us, len, &base, 
		    /* output */
		    out_us,
		    positionVtoL,
		    positionLtoV,
		    embedding_list
		    );
  }

  /* stop timer */
  time1 = utime();
  
  /* output result */
  printf("len = %d\n", len);
  printf("%d iterations in %f seconds\n", niter, time1-time0);
  printf("= %f iterations/second\n", 1.0 * niter/(time1-time0));
  
  return 0;
}
