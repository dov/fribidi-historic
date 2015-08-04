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

typedef guint32 FriBidiChar;

typedef enum
{
  FRIBIDI_TYPE_LTR , /* Strong left to right */
  FRIBIDI_TYPE_RTL , /* Right to left characters */
  FRIBIDI_TYPE_WL  , /* Weak left to right */
  FRIBIDI_TYPE_WR  , /* Weak right to left */
  FRIBIDI_TYPE_EN  , /* European digit */
  FRIBIDI_TYPE_ES  , /* European number separator */
  FRIBIDI_TYPE_ET  , /* European number terminator */
  FRIBIDI_TYPE_AN  , /* Arabic digit */
  FRIBIDI_TYPE_CS  , /* Common Separator */
  FRIBIDI_TYPE_BS  , /* Block separator */
  FRIBIDI_TYPE_SS  , /* Segment separator */
  FRIBIDI_TYPE_WS  , /* Whitespace */
  FRIBIDI_TYPE_AL  , /* Arabic characters */
  FRIBIDI_TYPE_NSM , /* Non spacing mark */
  FRIBIDI_TYPE_LRE , /* Left-To-Right embedding */
  FRIBIDI_TYPE_RLE , /* Right-To-Left embedding */
  FRIBIDI_TYPE_LRO , /* Left-To-Right override */
  FRIBIDI_TYPE_RLO , /* Right-To-Left override */
  FRIBIDI_TYPE_PDF , /* Pop directional override */
  FRIBIDI_TYPE_ON  ,  /* Other Neutral */

  /* The following are only used internally */
  FRIBIDI_TYPE_L = FRIBIDI_TYPE_LTR, 
  FRIBIDI_TYPE_R = FRIBIDI_TYPE_RTL, 
  FRIBIDI_TYPE_BN  = FRIBIDI_TYPE_ON + 2, 
  FRIBIDI_TYPE_CM  , 
  FRIBIDI_TYPE_SOT , 
  FRIBIDI_TYPE_EOT , 
  FRIBIDI_TYPE_N   , 
  FRIBIDI_TYPE_E   , 
  FRIBIDI_TYPE_CTL , /* Control units */
  FRIBIDI_TYPE_EO , /* Control units */
} FriBidiCharType;

/* The following type is used by fribidi_utils */
typedef struct {
  int length;
  void *attribute;
} FriBidiRunType;

/* The following should be configuration parameters, once I can
   figure out how to make configure set them... */
#ifndef FRIBIDI_MAX_STRING_LENGTH
#define FRIBIDI_MAX_STRING_LENGTH 65535
#endif

#endif
