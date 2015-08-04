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
#include "fribidi.h"

#define ISO_ALEF 0xE0
#define ISO_TAV 0xFA
#define UNI_ALEF 0x05D0
#define UNI_TAV 0x05EA

#define ISO_HAMZA 0xc1
#define ISO_SUKUN 0xf2
#define UNI_HAMZA 0x0621
#define UNI_SUKUN 0x0652


FriBidiChar
fribidi_iso8859_8_to_unicode_c(guchar ch)
{
  if (ch >= ISO_ALEF && ch <= ISO_TAV)
    return ch-ISO_ALEF+UNI_ALEF;
  else
    return ch;
}

FriBidiChar
fribidi_iso8859_6_to_unicode_c(guchar ch)
{
  if (ch >= ISO_HAMZA && ch <= ISO_SUKUN)
    return ch-ISO_HAMZA+UNI_HAMZA;
  else
    return ch;
}

void
fribidi_iso8859_8_to_unicode(guchar *s,
			     FriBidiChar *us)
{
  int i;
  int len = strlen(s);
  
  for (i=0; i<len+1; i++)
    us[i] = fribidi_iso8859_8_to_unicode_c(s[i]);
}

void
fribidi_iso8859_6_to_unicode(guchar *s,
			     FriBidiChar *us)
{
  int i;
  int len = strlen(s);
  
  for (i=0; i<len+1; i++)
    us[i] = fribidi_iso8859_6_to_unicode_c(s[i]);
}

guchar
fribidi_unicode_to_iso8859_8_c(FriBidiChar uch)
{
  if (uch >= UNI_ALEF && uch <= UNI_TAV)
    return (guchar)(uch-UNI_ALEF+ISO_ALEF);
  else if (uch < 256)
    return (guchar)uch;
  else
    return '¿';
}

guchar
fribidi_unicode_to_iso8859_6_c(FriBidiChar uch)
{
  if (uch >= UNI_HAMZA && uch <= UNI_SUKUN)
    return (guchar)(uch-UNI_HAMZA+ISO_HAMZA);
  else if (uch < 256)
    return (guchar)uch;
  else if (uch == 0x060c)
    return (guchar)0xac;	
  else if (uch == 0x061b)
    return (guchar)0xbb;
  else if (uch == 0x061f)
    return (guchar)0xbf; 
  else
    return '¿';
}

void
fribidi_unicode_to_iso8859_8(FriBidiChar *us,
			     int length,
			     guchar *s)
		     
{
  int i;
  
  for (i=0; i< length; i++)
    s[i] = fribidi_unicode_to_iso8859_8_c(us[i]);
  s[i] = 0;
}

void
fribidi_unicode_to_iso8859_6(FriBidiChar *us,
			     int length,
			     guchar *s)
		     
{
  int i;
  
  for (i=0; i< length; i++)
    s[i] = fribidi_unicode_to_iso8859_6_c(us[i]);
  s[i] = 0;
}

/* the following added by Raphael Finkel <raphael@cs.uky.edu> 12/1999 */

void
fribidi_unicode_to_utf8(FriBidiChar *us,
                           int length,
                           guchar *s)
/* warning: the length of output string may exceed the length of the input */

{
  int i;

  for (i=0; i< length; i++)
    {
      FriBidiChar mychar = us[i];
      if (mychar <= 0x7F) { /* 7 sig bits; plain 7-bit ascii */
	*s++ = mychar;
      } else if (mychar <= 0x7FF) /* 11 sig bits; Hebrew is in this range */
	{
	  *s++ = 0300 | (guint8) ((mychar >> 6)&037);
	  *s++ = 0200 | (guint8) (mychar & 077); /* lower 6 bits */
	} else if (mychar <= 0xFFFF) { /* 16 sig bits */
	  *s++ = 0340 | (guint8) ((mychar >> 12)&017), /* upper 4 bits */
	  *s++ = 0200 | (guint8) ((mychar >> 6)&077),  /* next 6 bits */
	  *s++ = 0200 | (guint8) (mychar & 077);       /* lowest 6 bits */
	}
    }
  *s = 0;
}


int /* we return the length */
fribidi_utf8_to_unicode(guchar *s,
			FriBidiChar *us)
/* warning: the length of input string may exceed the length of the output */
{
  int length;
  
  length = 0;
  while (*s) {
    if (*s <= 0177) /* one byte */
      {
	*us++ = *s++; /* expand with 0s */
      }
    else if (*s < 0340) /* 2 chars, such as Hebrew */
      {
	*us++ = ((*s & 037) << 6) + (*(s+1) & 077);
	s += 2;
      }
    else /* 3 chars */
      {
	*us++ = ((*s & 017) << 12) + ((*(s+1) & 077) << 6) +
	  (*(s+2) & 077);
	s += 3;
      }
    length += 1;
  }
  *us = 0;
  return(length);
}

