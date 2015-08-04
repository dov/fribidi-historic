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

#define FRIBIDI_CHARSET_UTF8		   1
#define FRIBIDI_CHARSET_8859_6		   6
#define FRIBIDI_CHARSET_8859_8		   8
#define FRIBIDI_CHARSET_CP1255		1255
#define FRIBIDI_CHARSET_CP1256		1256
#define FRIBIDI_CHARSET_ISIRI_3342	3342

#include "fribidi.h"

FriBidiChar     fribidi_cp1255_to_unicode_c        (guchar ch);
void            fribidi_cp1255_to_unicode          (guchar *s,
						    /* Output */
						    FriBidiChar *us);
guchar          fribidi_unicode_to_cp1255_c        (FriBidiChar uch);
void            fribidi_unicode_to_cp1255          (FriBidiChar *us,
						    int length,
						    /* Output */
						    guchar *s);

FriBidiChar     fribidi_cp1256_to_unicode_c        (guchar ch);
void            fribidi_cp1256_to_unicode          (guchar *s,
						    /* Output */
						    FriBidiChar *us);
guchar          fribidi_unicode_to_cp1256_c        (FriBidiChar uch);
void            fribidi_unicode_to_cp1256          (FriBidiChar *us,
						    int length,
						    /* Output */
						    guchar *s);

FriBidiChar     fribidi_isiri_3342_to_unicode_c    (guchar ch);
void            fribidi_isiri_3342_to_unicode      (guchar *s,
						    /* Output */
						    FriBidiChar *us);
guchar          fribidi_unicode_to_isiri_3342_c    (FriBidiChar uch);
void            fribidi_unicode_to_isiri_3342      (FriBidiChar *us,
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

FriBidiChar     fribidi_iso8859_8_to_unicode_c     (guchar ch);
void            fribidi_iso8859_8_to_unicode       (guchar *s,
						    /* Output */
						    FriBidiChar *us);
guchar          fribidi_unicode_to_iso8859_8_c     (FriBidiChar uch);
void            fribidi_unicode_to_iso8859_8       (FriBidiChar *us,
						    int length,
						    /* Output */
						    guchar *s);


void            fribidi_unicode_to_utf8             (FriBidiChar *us,
                                                     int length,
                                                     /* Output */
                                                     guchar *s);
/* warning: the length of output string may exceed the length of the input */

int             fribidi_utf8_to_unicode             (guchar *s,
                                                     /* Output */
                                                     FriBidiChar *us);
/* the length of the string is returned */

/* the following added by Omer Zak <omerz@actcom.co.il> Sept 2000.
** The following functions do the same thing, but have better-defined
** interfaces.
*/

gboolean  /* Returns TRUE if the outputs are valid, even if the entire
	  ** Unicode string was not converted.
	  */
fribidi_unicode_to_utf8_p(FriBidiChar *in_unicode_str,  /* Unicode string */
			  guint in_unicode_length,      /* Unicode string length in
							** Unicode characters
							*/
			  guchar *utf8_buffer,          /* Buffer for UTF8 translation */
			  guint utf8_buffer_length,     /* Length of UTF8 buffer */
			  /* Outputs */
			  guint *out_uni_consumed_length_p,
			                                /* Actual number of Unicode
							** characters translated
							*/
			  guint *out_actual_utf8_buffer_length_p);
                                                        /* Actual number of bytes
							** used in the UTF8 buffer.
							*/

gboolean  /* Returns TRUE if the entire UTF8 string was converted without errors. */
fribidi_utf8_to_unicode_p(guchar *in_utf8_str,          /* UTF8 string */
			  guint in_utf8_length,         /* Length of UTF8 string in octets */
			  FriBidiChar *unicode_buffer,  /* Buffer for Unicode translation */
			  guint unicode_buffer_length,  /* Length of Unicode buffer in
							** Unicode characters
							*/
			  /* Outputs */
			  guint *out_utf8_consumed_length_p,
			                                /* Actual number of UTF8
							** octets translated
							*/
			  guint *out_actual_unicode_buffer_length_p);
                                                        /* Actual number of Unicode
							** characters used in the
							** Unicode buffer.
							*/


#endif /* FRIBIDI_CHAR_SETS_H */

