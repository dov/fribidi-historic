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
#include "fribidi.h"

#define CASE(s) if (strcmp(S_, s) == 0)

int main(int argc, char *argv[])
{
  int argp=1;
  gchar *fn;
  guchar S_[255];
  FriBidiChar us[255];
  FILE *IN;
  gint pad_width = 35;
  gboolean do_use_order = FALSE;
  gboolean do_test_vtol = FALSE;
  gboolean do_cap_as_rtl = FALSE;
  gboolean do_output_only = FALSE;
  gboolean do_no_pad = FALSE;

  while(argp< argc && argv[argp][0] == '-')
    {
      gchar *S_ = argv[argp++];

      CASE("-help") {
	printf(
	       "test_fribidi - A program for testing the fribidi library\n"
	       "\n"
	       "Syntax:\n"
	       "    test_fribidi [-debug] [-outputonly] [-test_vtol] [-order] [-capital_rtl]\n"
	       "                 [-nopad]\n"
	       "\n"
	       "Description:\n"
	       "    A program for running the BiDi algorithm on all the lines in\n"
	       "    test file.\n"
	       "\n"
	       "Options:\n"
	       "    -debug      Output debug info about the progress of the algorithm\n"
	       "    -outputonly Don't print the original logical strings.\n"
	       "    -test_vtol  Output string is according to the ltov array.\n"
	       "    -test_ltov  Output string is according to the vtol array.\n"
	       "    -capital_rtl  Treat capital letters as RTL letters.\n"
	       );
	exit(0);
      }

      CASE("-outputonly")  { do_output_only++; pad_width = 80; continue; };
      CASE("-test_vtol") { do_test_vtol++; continue; };
      CASE("-debug") { fribidi_set_debug(TRUE); continue; };
      CASE("-order") { do_use_order++; continue; };
      CASE("-capital_rtl") { do_cap_as_rtl++; continue; }; 
      CASE("-nopad") { do_no_pad++; continue; };

      fprintf(stderr, "Unknown option %s!\n", S_);
      exit(0);
    }
  
  if (argp >= argc) {
    IN = stdin;
  } else {
    fn = argv[argp++];
    IN = fopen(fn, "r");
  }

  while(fgets(S_, sizeof(S_), IN))
    {
      int len = strlen(S_);
      FriBidiChar out_us[255];
      gint positionLtoV[255], positionVtoL[255];
      guchar outstring[255];
      int base;
      int i;

      if (S_[0] == '#')
	continue;
      
      if (len == 1)
	continue;
      
      /* chop */
      if (S_[len-1] == '\n')
	S_[len-1] = '\0';

      len--;

      /* Output before the mapping */
      if (!do_output_only)
	printf("%-35s => ", S_);
      
      if (do_cap_as_rtl)
	for (i=0; i<len; i++)
	  if (S_[i]>='A' && S_[i]<='Z')
	    S_[i]+= 0xE0 - 'A';  /* Map to iso Hebrew */

      fribidi_iso8859_8_to_unicode(S_, us);
      
      /* Create a bidi string */
      base = FRIBIDI_TYPE_N;
      fribidi_log2vis(us, &base,
		      /* output */
		      out_us,
		      positionLtoV,
		      positionVtoL
		      );

      /* Convert it to something to print */
      fribidi_unicode_to_iso8859_8(out_us, outstring);

      if (do_cap_as_rtl)
	for (i=0; i<len; i++)
	  if (outstring[i]>=0xE0 && outstring[i]<=0xFA)
	    outstring[i]-= 0xE0 - 'A';  /* Map to capital letters */
      
      if (base == FRIBIDI_TYPE_R && !do_no_pad)
	for (i=0; i<pad_width-len; i++)
	  printf(" ");
      
      if (do_use_order)
	{
	  int i;
	    
	  for (i=0; i<len; i++)
	    printf("%c", S_[positionLtoV[i]]);
	}
      else if (do_test_vtol)
	{
	  if (base == FRIBIDI_TYPE_R)
	    
	  for (i=0; i<len; i++)
	    printf("%c", outstring[positionVtoL[i]]);
	}
      else
	{
	  printf("%s", outstring);
	}

      if (base == FRIBIDI_TYPE_L && !do_no_pad)
	for (i=0; i<pad_width-len; i++)
	  printf(" ");
      
      printf("\n");
    }
  
  return 0;
}
