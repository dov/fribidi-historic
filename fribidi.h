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

void
fribidi_log2vis(/* input */
		FriBidiChar *str, int *pbase_dir,
		/* output */
		FriBidiChar *visual_str,
		gint *positionLtoV,
		gint *positionVtoL
		);

#endif
