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
#ifndef FRIBIDI_TYPES_H
#define FRIBIDI_TYPES_H

#include <glib.h>

typedef guint16 FriBidiChar;

/* Classifications of the various Bidi properties */
typedef enum
{
  FRIBIDI_TYPE_LTR = 0x01000000, /* Strong left to right */
  FRIBIDI_TYPE_RTL = 0x02000000, /* Strong right to left */
  FRIBIDI_TYPE_EN  = 0x03000000, /* European digit */
  FRIBIDI_TYPE_ES  = 0x04000000, /* European number separator */
  FRIBIDI_TYPE_ET  = 0x05000000, /* European number terminator */
  FRIBIDI_TYPE_AN  = 0x06000000, /* Arabic digit */
  FRIBIDI_TYPE_CS  = 0x07000000, /* Common Separator */
  FRIBIDI_TYPE_BS  = 0x08000000, /* Block separator */
  FRIBIDI_TYPE_SS  = 0x09000000, /* Segment separator */
  FRIBIDI_TYPE_WS  = 0x0A000000, /* Whitespace */
  FRIBIDI_TYPE_CTL = 0x10000090, /* Control units */
  FRIBIDI_TYPE_ON  = 0x80000009,  /* Other Neutral */

  /* The following are only used internally */
  FRIBIDI_TYPE_L   = 0x01000000, 
  FRIBIDI_TYPE_R   = 0x02000000, 
  FRIBIDI_TYPE_BN  = 0xF1000000, 
  FRIBIDI_TYPE_CM  = 0xF2000000, 
  FRIBIDI_TYPE_SOT = 0xF3000000, 
  FRIBIDI_TYPE_EOT = 0xF4000000, 
  FRIBIDI_TYPE_N   = 0xF5000000, 
  FRIBIDI_TYPE_E   = 0xF6000000, 
} FriBidiCharType;

#endif
