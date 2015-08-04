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
#include "fribidi.h"
#include "fribidi_tables.i"

/*======================================================================
//  fribidi_get_type() returns the bidi type of a character.
//----------------------------------------------------------------------*/
FriBidiCharType fribidi_get_type(FriBidiChar uch)
{
  int char_type = 0;  /* What should the default be */
  int pos, step, i;

  /* Do a binary search of the first character */
  pos = step = (nFriBidiPropertyList+1)/2;

  while(step > 1)
    {
      FriBidiChar first = FriBidiPropertyList[pos].first;
      step = (step+1)/2;
      
      if (first > uch)
	{
	  pos -= step;
	  if (pos<0)
	    pos = 0;
	}
      else
	{
	  pos += step;
	  if (pos> nFriBidiPropertyList-1)
	    pos = nFriBidiPropertyList-1;
	}
    }

  /* Search the neighborhood around the end of the binary search for an
     interval inclusion. */
  {
    int first_idx = pos-1;
    int last_idx = pos+2;

    if (first_idx < 0)
      first_idx = 0;
    if (last_idx > nFriBidiPropertyList-1)
      last_idx = nFriBidiPropertyList-1;

    /* Search the neighborhood. How big a neighborhood do I need to search? */
    for (i=first_idx; i < last_idx; i++)
      {
	FriBidiChar first = FriBidiPropertyList[i].first;
	FriBidiChar last = FriBidiPropertyList[i].last;
	
	if (uch>= first && uch <= last)
	  {
	    char_type = FriBidiPropertyList[i].char_type;
	    break;
	  }
	else if (first > uch)
	  break;
      }
  }

  return char_type;
}

gboolean
fribidi_get_mirror_char(/* Input */
		       FriBidiChar ch,
		       /* Output */
		       FriBidiChar *mirrored_ch)
{
  int pos, step;
  gboolean found = FALSE;

  pos = step = (nFriBidiMirroredChars/2)+1;

  while(step > 1)
    {
      FriBidiChar cmp_ch = FriBidiMirroredChars[pos].ch;
      step = (step+1)/2;
      
      if (cmp_ch < ch)
	{
	  pos += step;
	  if (pos>nFriBidiMirroredChars-1)
	    pos = nFriBidiMirroredChars-1;
	}
      else if (cmp_ch > ch)
	{
	  pos -= step;
	  if (pos<0)
	    pos=0;
	}
      else
	break;
    }
  if (FriBidiMirroredChars[pos].ch == ch)
    {
      *mirrored_ch = FriBidiMirroredChars[pos].mirrored_ch;
      found = TRUE;
    }
  return found;
}

gboolean
fribidi_is_arabic_strong(/* Input */
			 FriBidiChar ch)
{
  int pos, step;
  gboolean found = FALSE;

  pos = step = (nFriBidiArabicStrongLetterRanges/2)+1;

  while(step > 1)
    {
      FriBidiChar first = FriBidiArabicStrongLetterRanges[pos].first;
      step = (step+1)/2;
      
      if (first > ch)
	{ 
	  pos -= step;
	  if (pos<0)
	    pos = 0;
	}
      else
	{
	  pos += step;
	  if (pos > nFriBidiArabicStrongLetterRanges-1)
	    pos = nFriBidiArabicStrongLetterRanges;
	}
    }

  {
    int first_idx = pos-1;
    int last_idx = pos+2;
    int i;

    if (first_idx < 0)
      first_idx = 0;
    if (last_idx > nFriBidiPropertyList-1)
      last_idx = nFriBidiPropertyList-1;
    
    /* Search the neighborhood. How big a neighborhood do I need to search? */
    for (i=first_idx; i < last_idx; i++)
      {
	FriBidiChar first = FriBidiArabicStrongLetterRanges[i].first;
	FriBidiChar last = FriBidiArabicStrongLetterRanges[i].last;
	
	if (ch>= first && ch <= last)
	  {
	    found = TRUE;
	    break;
	  }
	else if (first > ch)
	  break;
      }
  }

  return found;
}

