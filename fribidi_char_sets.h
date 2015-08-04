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
#ifndef FRIBIDI_CHAR_SETS_H
#define FRIBIDI_CHAR_SETS_H

#include "fribidi.h"

FriBidiChar     fribidi_iso8859_8_to_unicode_c     (guchar ch);
void            fribidi_iso8859_8_to_unicode       (guchar *s,
						    /* Output */
						    FriBidiChar *us);
guchar          fribidi_unicode_to_iso8859_8_c     (FriBidiChar uch);
void            fribidi_unicode_to_iso8859_8       (FriBidiChar *us,
						    int length,
						    /* Output */
						    guchar *s);

FriBidiChar     fribidi_iso8859_6_to_unicode_c     (guchar ch);
void            fribidi_iso8859_6_to_unicode       (guchar *s,
						    /* Output */
						    FriBidiChar *us);
guchar          fribidi_unicode_to_iso8859_6_c     (FriBidiChar uch);
void            fribidi_unicode_to_iso8859_6       (FriBidiChar *us,
						    int length,
						    /* Output */
						    guchar *s);


void                  fribidi_unicode_to_utf8                         (FriBidiChar *us,
                                                       int length,
                                                       /* Output */
                                                       guchar *s);
/* warning: the length of output string may exceed the length of the input */

int                           fribidi_utf8_to_unicode                         (guchar *s,
                                                       /* Output */
                                                       FriBidiChar *us);
/* the length of the string is returned */
#endif
