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
#ifndef FRIBIDI_H
#define FRIBIDI_H

#include <glib.h>
#include "fribidi_types.h"
#include "fribidi_char_sets.h"

FriBidiCharType fribidi_get_type(FriBidiChar uch);
gboolean
fribidi_get_mirror_char(/* Input */
			FriBidiChar ch,
			/* Output */
			FriBidiChar *mirrored_ch);
gboolean
fribidi_is_arabic_strong(/* Input */
			 FriBidiChar ch);

void
fribidi_set_debug(gboolean debug);

void fribidi_log2vis(/* input */
		     FriBidiChar *str,
		     gint len,
		     FriBidiCharType *pbase_dir,
		     /* output */
		     FriBidiChar *visual_str,
		     guint16     *position_L_to_V_list,
		     guint16     *position_V_to_L_list,
		     guint8      *embedding_level_list
		     );

void fribidi_log2vis_get_embedding_levels(
                     /* input */
		     FriBidiChar *str,
		     int len,
		     FriBidiCharType *pbase_dir,
		     /* output */
		     guint8 *embedding_level_list
		     );

/* fribidi_utils.c */

void
fribidi_find_string_changes(/* input */
			    FriBidiChar *old_str,
			    int old_len,
			    FriBidiChar *new_str,
			    int new_len,
			    /* output */
			    int *change_start,
			    int *change_len
			    );


void
fribidi_map_range          (gint span[2],             
			    gboolean is_v2l_map,      
			    guint16 *position_map,
			    guint8 *embedding_level_list,
			    int len,
			    /* output */
			    int *num_mapped_spans,
			    int spans[3][2]);

#endif
