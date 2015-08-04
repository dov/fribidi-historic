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

#define ISO_ALEF 224
#define ISO_TAV 250

/* The following are proposed extensions to iso-8859-8. */
#define ISO_8859_8_LRM 253
#define ISO_8859_8_RLM 254
#define ISO_8859_8_LRE 251
#define ISO_8859_8_RLE 252
#define ISO_8859_8_PDF 221
#define ISO_8859_8_LRO 219
#define ISO_8859_8_RLO 220

#define UNI_ALEF 0x05D0
#define UNI_TAV 0x05EA
#define UNI_LRM 0x200E
#define UNI_RLM 0x200F
#define UNI_LRE 0x202a
#define UNI_RLE 0x202b
#define UNI_PDF 0x202c
#define UNI_LRO 0x202d
#define UNI_RLO 0x202e

#define CP1255_SHEVA 0xC0
#define UNI_SHEVA 0x05B0
#define CP1255_SOF_PASUQ 0xD3
#define UNI_SOF_PASUQ 0x05C3
#define CP1255_DOUBLE_VAV 0xD4
#define UNI_DOUBLE_VAV 0x05F0
#define CP1255_GERSHAYIM 0xD8
#define UNI_GERSHAYIM 0x05F4

#define ISO_HAMZA 0xc1
#define ISO_SUKUN 0xf2
#define UNI_HAMZA 0x0621
#define UNI_SUKUN 0x0652

#define CP1256_DAD 0xD6
#define UNI_DAD 0x0636

FriBidiChar
fribidi_iso8859_8_to_unicode_c(guchar ch)
{
  /* optimization */
  if (ch < ISO_8859_8_LRO)
    return ch;
  else if (ch >= ISO_ALEF && ch <= ISO_TAV)
    return ch-ISO_ALEF+UNI_ALEF;
  switch(ch)
    {
    case ISO_8859_8_RLM: return UNI_RLM;
    case ISO_8859_8_LRM: return UNI_LRM;
    case ISO_8859_8_RLO: return UNI_RLO;
    case ISO_8859_8_LRO: return UNI_LRO;
    case ISO_8859_8_RLE: return UNI_RLE;
    case ISO_8859_8_LRE: return UNI_LRE;
    case ISO_8859_8_PDF: return UNI_PDF;
    default:
      return '?';      /* This shouldn't happen! */
    }
}

guchar
fribidi_unicode_to_iso8859_8_c(FriBidiChar uch)
{
  if (uch<128)
    return uch;
  if (uch >= UNI_ALEF && uch <= UNI_TAV)
    return (guchar)(uch-UNI_ALEF+ISO_ALEF);
  switch (uch) {
    case UNI_RLM: return ISO_8859_8_RLM;
    case UNI_LRM: return ISO_8859_8_LRM;
    case UNI_RLO: return ISO_8859_8_RLO;
    case UNI_LRO: return ISO_8859_8_LRO;
    case UNI_RLE: return ISO_8859_8_RLE;
    case UNI_LRE: return ISO_8859_8_LRE;
    case UNI_PDF: return ISO_8859_8_PDF;
  }
  return '¿';
}

FriBidiChar
fribidi_iso8859_6_to_unicode_c(guchar ch)
{
  if (ch >= ISO_HAMZA && ch <= ISO_SUKUN)
    return ch-ISO_HAMZA+UNI_HAMZA;
  else
    return ch;
}

FriBidiChar
fribidi_cp1255_to_unicode_tab[] = { /* 0x80-0xBF */
	0x20AC,  0x81,0x201A,0x0192,0x201E,0x2026,0x2020,0x2021,
	0x02C6,0x2030,  0x8a,0x2039,  0x8c,  0x8d,  0x8e,  0x8f,
	  0x90,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,
	0x02DC,0x2122,  0x9a,0x203A,  0x9c,  0x9d,  0x9e,  0x9f,
	0x00A0,0x00A1,0x00A2,0x00A3,0x20AA,0x00A5,0x00A6,0x00A7,
	0x00A8,0x00A9,0x00D7,0x00AB,0x00AC,0x00AD,0x00AE,0x00AF,
	0x00B0,0x00B1,0x00B2,0x00B3,0x00B4,0x00B5,0x00B6,0x00B7,
	0x00B8,0x00B9,0x00F7,0x00BB,0x00BC,0x00BD,0x00BE,0x00BF
};

FriBidiChar
fribidi_cp1255_to_unicode_c(guchar ch)
{
  if (ch >= ISO_ALEF && ch <= ISO_TAV)
    return ch-ISO_ALEF+UNI_ALEF;
  else if (ch >= CP1255_SHEVA && ch <= CP1255_SOF_PASUQ)
    return ch-CP1255_SHEVA+UNI_SHEVA;
  else if (ch >= CP1255_DOUBLE_VAV && ch <= CP1255_GERSHAYIM)
    return ch-CP1255_DOUBLE_VAV+UNI_DOUBLE_VAV;
  /* cp1256 specific chars */
  else if (ch >= 0x80 && ch <= 0xbf)
    return fribidi_cp1255_to_unicode_tab[ch-0x80];	 
  else
    return ch;
}

FriBidiChar
fribidi_cp1256_to_unicode_tab[] = { /* 0x80-0xFF */
	0x20AC,0x067E,0x201A,0x0192,0x201E,0x2026,0x2020,0x2021,
	0x02C6,0x2030,0x0679,0x2039,0x0152,0x0686,0x0698,0x0688,
	0x06AF,0x2018,0x2019,0x201C,0x201D,0x2022,0x2013,0x2014,
	0x06A9,0x2122,0x0691,0x203A,0x0153,0x200C,0x200D,0x06BA,
	0x00A0,0x060C,0x00A2,0x00A3,0x00A4,0x00A5,0x00A6,0x00A7,
	0x00A8,0x00A9,0x06BE,0x00AB,0x00AC,0x00AD,0x00AE,0x00AF,
	0x00B0,0x00B1,0x00B2,0x00B3,0x00B4,0x00B5,0x00B6,0x00B7,
	0x00B8,0x00B9,0x061B,0x00BB,0x00BC,0x00BD,0x00BE,0x061F,
	0x06C1,0x0621,0x0622,0x0623,0x0624,0x0625,0x0626,0x0627,
	0x0628,0x0629,0x062A,0x062B,0x062C,0x062D,0x062E,0x062F,
	0x0630,0x0631,0x0632,0x0633,0x0634,0x0635,0x0636,0x00D7,
	0x0637,0x0638,0x0639,0x063A,0x0640,0x0641,0x0642,0x0643,
	0x00E0,0x0644,0x00E2,0x0645,0x0646,0x0647,0x0648,0x00E7,
	0x00E8,0x00E9,0x00EA,0x00EB,0x0649,0x064A,0x00EE,0x00EF,
	0x064B,0x064C,0x064D,0x064E,0x00F4,0x064F,0x0650,0x00F7,
	0x0651,0x00F9,0x0652,0x00FB,0x00FC,0x200E,0x200F,0x00ff
};

FriBidiChar
fribidi_cp1256_to_unicode_c(guchar ch)
{
  if (ch >= 0x80 && ch <= 0xff) 
    return fribidi_cp1256_to_unicode_tab[ch-0x80];
  else
    return ch;
}

FriBidiChar
fribidi_isiri_3342_to_unicode_tab[] = { /* 0xA0-0xFF */
	0x0020,0x200C,0x200D,0x0021,0x00A4,0x066A,0x002E,0x066C,
	0x0029,0x0028,0x00D7,0x002B,0x060C,0x002D,0x066B,0x002F,
	0x06F0,0x06F1,0x06F2,0x06F3,0x06F4,0x06F5,0x06F6,0x06F7,
	0x06F8,0x06F9,0x003A,0x061B,0x003C,0x003D,0x003E,0x061F,
	0x0622,0x0627,0x0621,0x0628,0x067E,0x062A,0x062B,0x062C,
	0x0686,0x062D,0x062E,0x062F,0x0630,0x0631,0x0632,0x0698,
	0x0633,0x0634,0x0635,0x0636,0x0637,0x0638,0x0639,0x063A,
	0x0641,0x0642,0x06A9,0x06AF,0x0644,0x0645,0x0646,0x0648,
	0x0647,0x06CC,0x005D,0x005B,0x007D,0x007B,0x00AB,0x00BB,
	/* 0xec,0xed,0xee,0xef not defined */
	0x002A,0x0640,0x007C,0x005C,0xec  ,0xed  ,0xee  ,0xef  ,
	0x064E,0x0650,0x064F,0x064B,0x064D,0x064C,0x0651,0x0652,
	0x0623,0x0624,0x0625,0x0626,0x0629,0x0643,0x064A,0x007F
};

FriBidiChar
fribidi_isiri_3342_to_unicode_c(guchar ch)
{
  if ((ch >= 0x80 && ch <= 0xa0) || ch == 0xff)
    return ch-0x80; /* FIXME: but they are strong typed RTL ! */
  else if (ch >= 0xa1 && ch <= 0xfe)
    return fribidi_isiri_3342_to_unicode_tab[ch-0xa0]; 
  /* TODO */
  else
    return ch;
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
fribidi_cp1255_to_unicode(guchar *s,
			  FriBidiChar *us)
{
  int i;
  int len = strlen(s);
  
  for (i=0; i<len+1; i++)
    us[i] = fribidi_cp1255_to_unicode_c(s[i]);
}

void
fribidi_cp1256_to_unicode(guchar *s,
			  FriBidiChar *us)
{
  int i;
  int len = strlen(s);
  
  for (i=0; i<len+1; i++)
    us[i] = fribidi_cp1256_to_unicode_c(s[i]);
}

void
fribidi_isiri_3342_to_unicode(guchar *s,
			      FriBidiChar *us)
{
  int i;
  int len = strlen(s);
  
  for (i=0; i<len+1; i++)
    us[i] = fribidi_isiri_3342_to_unicode_c(s[i]);
}

guchar
fribidi_unicode_to_iso8859_6_c(FriBidiChar uch)
{
  if (uch >= UNI_HAMZA && uch <= UNI_SUKUN)
    return (guchar)(uch-UNI_HAMZA+ISO_HAMZA);
  /* TODO: handle pre-composed and presentation chars */
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


guchar
fribidi_unicode_to_cp1255_c(FriBidiChar uch)
{
  if (uch >= UNI_ALEF && uch <= UNI_TAV)
    return (guchar)(uch-UNI_ALEF+ISO_ALEF);
  if (uch >= UNI_SHEVA && uch <= UNI_SOF_PASUQ)
    return (guchar)(uch-UNI_SHEVA+CP1255_SHEVA);
  if (uch >= UNI_DOUBLE_VAV && uch <= UNI_GERSHAYIM)
    return (guchar)(uch-UNI_DOUBLE_VAV+CP1255_DOUBLE_VAV);
  /* TODO: handle pre-composed and presentation chars */
  else if (uch < 256)
    return (guchar)uch;
  else
    return '¿';
}

guchar
fribidi_unicode_to_cp1256_c(FriBidiChar uch)
{
  if (uch < 256)
    return (guchar)uch;
  if (uch >= UNI_HAMZA && uch <= UNI_DAD)
    return (guchar)(uch-UNI_HAMZA+ISO_HAMZA);
  else switch (uch) {
	case 0x0152: return 0x8c;
	case 0x0153: return 0x9c;
	case 0x0192: return 0x83;
	case 0x02C6: return 0x88;
	case 0x060C: return 0xA1;
	case 0x061B: return 0xBA;
	case 0x061F: return 0xBF;
        case 0x0637: return 0xD8;
	case 0x0638: return 0xD9;
        case 0x0639: return 0xDA;
	case 0x063A: return 0xDB; 
	case 0x0640: return 0xDC;
	case 0x0641: return 0xDD;
	case 0x0642: return 0xDE;
	case 0x0643: return 0xDF;
	case 0x0644: return 0xE1;
	case 0x0645: return 0xE3;
	case 0x0646: return 0xE4;
	case 0x0647: return 0xE5;
	case 0x0648: return 0xE6;
	case 0x0649: return 0xEC;
	case 0x064A: return 0xED;
	case 0x064B: return 0xF0;
	case 0x064C: return 0xF1;
	case 0x064D: return 0xF2;
	case 0x064E: return 0xF3;
	case 0x064F: return 0xF5;
	case 0x0650: return 0xF6;
	case 0x0651: return 0xF8;
	case 0x0652: return 0xFA;
	case 0x0679: return 0x8A;
	case 0x067E: return 0x81;
	case 0x0686: return 0x8D;
	case 0x0688: return 0x8F;
	case 0x0691: return 0x9A;
	case 0x0698: return 0x8E;
	case 0x06A9: return 0x98;
	case 0x06AF: return 0x90;
	case 0x06BA: return 0x9F;
	case 0x06BE: return 0xAA;
	case 0x06C1: return 0xC0;
	case 0x200C: return 0x9D;
	case 0x200D: return 0x9E;
	case 0x200E: return 0xFD;
	case 0x200F: return 0xFE;
	case 0x2013: return 0x96;
	case 0x2014: return 0x97;
	case 0x2018: return 0x91;
	case 0x2019: return 0x92;
	case 0x201A: return 0x82;
	case 0x201C: return 0x93;
	case 0x201D: return 0x94;
	case 0x201E: return 0x84;
	case 0x2020: return 0x86;
	case 0x2021: return 0x87;
	case 0x2022: return 0x95;
	case 0x2026: return 0x85;
	case 0x2030: return 0x89;
	case 0x2039: return 0x8B;
	case 0x203A: return 0x9B;
	case 0x20AC: return 0x80;
	case 0x2122: return 0x99;

	default: return '¿';
  }	  
}

guchar
fribidi_unicode_to_isiri_3342_c(FriBidiChar uch)
{
  /* TODO */
  if (uch < 256)
    return (guchar)uch;
  else switch (uch) {
	case 0x060C: return 0xAC;
	case 0x061B: return 0xBB;
	case 0x061F: return 0xBF;
	case 0x0621: return 0xC2;
	case 0x0622: return 0xC0;
	case 0x0623: return 0xF8;
	case 0x0624: return 0xF9;
	case 0x0625: return 0xFA;
	case 0x0626: return 0xFB;
	case 0x0627: return 0xC1;
	case 0x0628: return 0xC3;
	case 0x0629: return 0xFC;
	case 0x062A: return 0xC5;
	case 0x062B: return 0xC6;
	case 0x062C: return 0xC7;
	case 0x062D: return 0xC9;
	case 0x062E: return 0xCA;
	case 0x062F: return 0xCB;
	case 0x0630: return 0xCC;
	case 0x0631: return 0xCD;
	case 0x0632: return 0xCE;
	case 0x0633: return 0xD0;
	case 0x0634: return 0xD1;
	case 0x0635: return 0xD2;
	case 0x0636: return 0xD3;
	case 0x0637: return 0xD4;
	case 0x0638: return 0xD5;
	case 0x0639: return 0xD6;
	case 0x063A: return 0xD7;
	case 0x0640: return 0xE9;
	case 0x0641: return 0xD8;
	case 0x0642: return 0xD9;
	case 0x0643: return 0xFD;
	case 0x0644: return 0xDC;
	case 0x0645: return 0xDD;
	case 0x0646: return 0xDE;
	case 0x0647: return 0xE0;
	case 0x0648: return 0xDF;
	case 0x064A: return 0xFE;
	case 0x064B: return 0xF3;
	case 0x064C: return 0xF5;
	case 0x064D: return 0xF4;
	case 0x064E: return 0xF0;
	case 0x064F: return 0xF2;
	case 0x0650: return 0xF1;
	case 0x0651: return 0xF6;
	case 0x0652: return 0xF7;
	case 0x066A: return 0xA5;
	case 0x066B: return 0xAE;
	case 0x066C: return 0xA7;
	case 0x067E: return 0xC4;
	case 0x0686: return 0xC8;
	case 0x0698: return 0xCF;
	case 0x06A9: return 0xDA;
	case 0x06AF: return 0xDB;
	case 0x06CC: return 0xE1;
	case 0x06F0: return 0xB0;
	case 0x06F1: return 0xB1;
	case 0x06F2: return 0xB2;
	case 0x06F3: return 0xB3;
	case 0x06F4: return 0xB4;
	case 0x06F5: return 0xB5;
	case 0x06F6: return 0xB6;
	case 0x06F7: return 0xB7;
	case 0x06F8: return 0xB8;
	case 0x06F9: return 0xB9;
	case 0x200C: return 0xA1;
	case 0x200D: return 0xA2;
	default: return '¿';
  }
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
fribidi_unicode_to_cp1255(FriBidiChar *us,
			     int length,
			     guchar *s)
		     
{
  int i;
  
  for (i=0; i< length; i++)
    s[i] = fribidi_unicode_to_cp1255_c(us[i]);
  s[i] = 0;
}

void
fribidi_unicode_to_cp1256(FriBidiChar *us,
			     int length,
			     guchar *s)
		     
{
  int i;
  
  for (i=0; i< length; i++)
    s[i] = fribidi_unicode_to_cp1256_c(us[i]);
  s[i] = 0;
}

void
fribidi_unicode_to_isiri_3342(FriBidiChar *us,
			     int length,
			     guchar *s)
		     
{
  int i;
  
  for (i=0; i< length; i++)
    s[i] = fribidi_unicode_to_isiri_3342_c(us[i]);
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

/* the following was added by Omer Zak <omerz@actcom.co.il> Sept 2000.
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
			  guint *out_actual_utf8_buffer_length_p)
                                                        /* Actual number of bytes
							** used in the UTF8 buffer.
							*/
{
  guint l_utf8_buffer_fence;  /* Fence - 6 octets before end */
  guint index;
  guint index_utf8;

  if (NULL == in_unicode_str) return(FALSE);
  if (NULL == utf8_buffer) return(FALSE);
  if (NULL == out_uni_consumed_length_p) return(FALSE);
  if (NULL == out_actual_utf8_buffer_length_p) return(FALSE);
  if (utf8_buffer_length < 6) return(FALSE);

  /* If we are here, all outputs must have some valid values. */
  *out_uni_consumed_length_p = 0;
  *out_actual_utf8_buffer_length_p = 0;

  l_utf8_buffer_fence = utf8_buffer_length-6;

  for (index = 0, index_utf8 = 0; index < in_unicode_length; index++) {
    FriBidiChar ucs4_char;

    if (index_utf8 > l_utf8_buffer_fence) {
      break;  /* No more guaranteed room in UTF8 buffer. */
    }

    ucs4_char = in_unicode_str[index];

#define PUT_UTF8_BYTE(b) utf8_buffer[index_utf8] = (b); index_utf8++

    if (ucs4_char <        0x00000080) {
      PUT_UTF8_BYTE(ucs4_char &  0x0000007F);
    } else if (ucs4_char < 0x00000800) {
      PUT_UTF8_BYTE(((ucs4_char >> 6) & 0x0000001F) | 0x000000C0);
      PUT_UTF8_BYTE((ucs4_char & 0x0000003F) | 0x00000080);
    } else if (ucs4_char < 0x00010000) {
      PUT_UTF8_BYTE(((ucs4_char >> 12) & 0x0000000F) | 0x000000E0);
      PUT_UTF8_BYTE(((ucs4_char >> 6) & 0x0000003F) | 0x00000080);
      PUT_UTF8_BYTE((ucs4_char & 0x0000003F) | 0x00000080);
    } else if (ucs4_char < 0x00200000) {
      PUT_UTF8_BYTE(((ucs4_char >> 18) & 0x00000007) | 0x000000F0);
      PUT_UTF8_BYTE(((ucs4_char >> 12) & 0x0000003F) | 0x00000080);
      PUT_UTF8_BYTE(((ucs4_char >> 6) & 0x0000003F) | 0x00000080);
      PUT_UTF8_BYTE((ucs4_char & 0x0000003F) | 0x00000080);
    } else if (ucs4_char < 0x04000000) {
      PUT_UTF8_BYTE(((ucs4_char >> 24) & 0x00000003) | 0x000000F8);
      PUT_UTF8_BYTE(((ucs4_char >> 18) & 0x0000003F) | 0x00000080);
      PUT_UTF8_BYTE(((ucs4_char >> 12) & 0x0000003F) | 0x00000080);
      PUT_UTF8_BYTE(((ucs4_char >> 6) & 0x0000003F) | 0x00000080);
      PUT_UTF8_BYTE((ucs4_char & 0x0000003F) | 0x00000080);
    } else /* if (ucs4_char < 0x80000000) */ {
      PUT_UTF8_BYTE(((ucs4_char >> 30) & 0x00000001) | 0x000000FC);
      PUT_UTF8_BYTE(((ucs4_char >> 24) & 0x0000003F) | 0x00000080);
      PUT_UTF8_BYTE(((ucs4_char >> 18) & 0x0000003F) | 0x00000080);
      PUT_UTF8_BYTE(((ucs4_char >> 12) & 0x0000003F) | 0x00000080);
      PUT_UTF8_BYTE(((ucs4_char >> 6) & 0x0000003F) | 0x00000080);
      PUT_UTF8_BYTE((ucs4_char & 0x0000003F) | 0x00000080);
    }

#undef PUT_UTF8_BYTE

  }

  *out_uni_consumed_length_p = index;
  *out_actual_utf8_buffer_length_p = index_utf8;

  return(TRUE);
}

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
			  guint *out_actual_unicode_buffer_length_p)
                                                        /* Actual number of Unicode
							** characters used in the
							** Unicode buffer.
							*/
{
  /* !!! To be improved using code from libutf8-0.7.3/src/mbstowcs/mbsrtowcs.c
  ** !!! (the library is LGPLed :-) )
  */

  guint index;
  guint index_uni;

  if (NULL == in_utf8_str) return(FALSE);
  if (NULL == unicode_buffer) return(FALSE);
  if (NULL == out_utf8_consumed_length_p) return(FALSE);
  if (NULL == out_actual_unicode_buffer_length_p) return(FALSE);

  /* If we are here, all outputs must have some valid values. */
  *out_utf8_consumed_length_p = 0;
  *out_actual_unicode_buffer_length_p = 0;

  for (index = 0, index_uni = 0; (index < in_utf8_length) && (index_uni < unicode_buffer_length);) {
    /* NOTE:  there is no protection against UTF8 sequences which overflow the in_utf8_str. */
    if (in_utf8_str[index] <= 0177) /* one byte */ {
      unicode_buffer[index_uni++] = in_utf8_str[index++]; /* expand with 0s */
    }
    else if (in_utf8_str[index] < 0340) /* 2 chars, such as Hebrew */ {
      unicode_buffer[index_uni++] = ((in_utf8_str[index] & 037) << 6)
	+ (in_utf8_str[index+1] & 077);
      index += 2;
    }
    else /* 3 chars */ {
      unicode_buffer[index_uni++] = ((in_utf8_str[index] & 017) << 12)
	+ ((in_utf8_str[index+1] & 077) << 6)
	+ (in_utf8_str[index+2] & 077);
      index += 3;
    }
  }
  *out_utf8_consumed_length_p = index;
  *out_actual_unicode_buffer_length_p = index_uni;

  return(TRUE);
}
