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

void
fribidi_iso8859_8_to_unicode(guchar *s,
			     FriBidiChar *us)
{
  int i;
  int len = strlen(s);
  
  for (i=0; i<len+1; i++)
    {
      guchar ch = s[i];
      if (ch >= ISO_ALEF && ch <= ISO_TAV)
	us[i] = ch-ISO_ALEF+UNI_ALEF;
      else
	us[i] = s[i];
    }
}

void
fribidi_unicode_to_iso8859_8(FriBidiChar *us,
			     guchar *s)
		     
{
  int i = 0;

  while(us[i])
    {
      FriBidiChar ch = us[i];
      if (ch >= UNI_ALEF && ch <= UNI_TAV)
	s[i] = ch-UNI_ALEF+ISO_ALEF;
      else if (us[i] < 256)
	s[i] = us[i];
      else
	s[i] = '¿';
      i++;
    }
  s[i] = 0;
}
