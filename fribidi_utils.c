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

/*======================================================================
//  This file contains various utility functions that are commonly
//  needed by programs that implement BiDi functionality. The more
//  code that may be put here, the easier it is for the application
//  writers.
//----------------------------------------------------------------------*/

#include "fribidi.h"

/*======================================================================
//  The find_visual_ranges() function is used to convert between a
//  continous span in either logical or visual space to a one, two or
//  three discontinous spans in the other space. The function outputs
//  the number of ranges needed to display the logical selection as
//  well as the resolved visual ranges.
//----------------------------------------------------------------------*/
void
fribidi_map_range(gint in_span[2],          /* Start and end span */
		  gboolean is_v2l_map,      /* Needed for embedding_level */
		  guint16 *position_map,
		  guint8 *embedding_level_list,
		  int len,
		  /* output */
		  int *num_mapped_spans,
		  int mapped_spans[3][2])
{
  gint ch_idx;
  gboolean in_range = FALSE;
  gint start_idx = in_span[0];
  gint end_idx = in_span[1];

  if (start_idx == -1)
    start_idx = 0;
  
  if (end_idx == -1)
    end_idx = len;
  
  *num_mapped_spans = 0;

  for (ch_idx = 0; ch_idx <= len; ch_idx++)
    {
      int mapped_pos;
      gboolean is_char_rtl;
      
      if (ch_idx < len)
	{
	  mapped_pos = position_map[ch_idx];
	  if (is_v2l_map)
	    is_char_rtl = embedding_level_list[mapped_pos];
	  else
	    is_char_rtl = embedding_level_list[ch_idx];
	}
      else
	mapped_pos = -1; /* Will cause log_pos < start_idx to trigger below */
      
      if (!in_range && mapped_pos >= start_idx && mapped_pos < end_idx)
	{
	  in_range = TRUE;
	  (*num_mapped_spans)++;
	  mapped_spans[(*num_mapped_spans)-1][0] = ch_idx;
	}
      else if (in_range && (mapped_pos < start_idx || mapped_pos >= end_idx))
	{
	  mapped_spans[(*num_mapped_spans)-1][1] = ch_idx;
	  in_range = FALSE;
	}
    }
}

/*======================================================================
//  fribidi_find_string_changes() finds the bounding box of the section
//  of characters that need redrawing. It returns the start and the
//  length of the section in the new string that needs redrawing.
//----------------------------------------------------------------------*/
void
fribidi_find_string_changes(/* input */
			    FriBidiChar *old_str,
			    int old_len,
			    FriBidiChar *new_str,
			    int new_len,
			    /* output */
			    int *change_start,
			    int *change_len
			    )
{
  int i;
  int num_bol, num_eol;

  /* Search forwards */
  i = 0;
  while(    i < old_len
	 && i < new_len
	 && old_str[i] == new_str[i])
    i++;
  num_bol = i;

  /* Search backwards */
  i = 0;
  while(    i <old_len
	 && i <new_len
	 && old_str[old_len-1-i] == new_str[new_len-1-i])
    i++;
  num_eol = i;

  /* Assign output */
  *change_start = num_bol;
  *change_len = new_len - num_eol - num_bol;
}
