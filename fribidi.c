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
#include <glib.h>
#include <stdio.h>
#include "fribidi.h"

/*======================================================================
// Typedef for the run-length list.
//----------------------------------------------------------------------*/
typedef struct {
  FriBidiCharType type;
  int pos;
  int len;
  int level;
} TypeLink;

typedef struct {
  FriBidiChar key;
  FriBidiChar value;
} key_value_t;

/* Global variables */
gboolean fribidi_debug = FALSE;

void fribidi_set_debug(gboolean debug)
{
  fribidi_debug = debug;
}

static int bidi_string_strlen(FriBidiChar *str)
{
  int len = 0;

  while(*str++)
    len++;
  
  return len;
}

static void bidi_string_reverse(FriBidiChar *str, gint len)
{
  int i;
  for (i=0; i<len/2; i++)
    {
      FriBidiChar tmp = str[i];
      str[i] = str[len-1-i];
      str[len-1-i] = tmp;
    }
}

static void
int16_array_reverse(gint16 *arr, gint len)
{
  int i;
  for (i=0; i<len/2; i++)
    {
      gint tmp = arr[i];
      arr[i] = arr[len-1-i];
      arr[len-1-i] = tmp;
    }
}

static TypeLink *new_type_link()
{
  TypeLink *link = g_new(TypeLink, 1);
  link->len = 0;
  link->pos = 0;
  link->level = 0;
  return link;
}

static GList *run_length_encode_types(int *char_type, int type_len)
{
  GList *list = NULL;
  GList *last;
  TypeLink *link;
  FriBidiCharType type;
  int len, pos, i;

  /* Add the starting link */
  link = new_type_link();
  link->type = FRIBIDI_TYPE_SOT;
  link->len = 0;
  link->pos = 0;
  list = g_list_append(NULL, link);
  last = list;

  /* Sweep over the string_types */
  type = -1;
  len = 0;
  pos = -1;
  for (i=0; i<=type_len; i++)
    {
      if (i==type_len || char_type[i] != type)
	{
	  if (pos>=0)
	    {
	      link = new_type_link();
	      link->type = type;
	      link->pos = pos;
	      link->len = len;
	      g_list_append(last, link);
	      last = last->next;
	    }
	  if (i==type_len)
	    break;
	  len = 0;
	  pos = i;
	}
      type =char_type[i];
      len++;
    }

  /* Add the ending link */
  link = new_type_link();
  link->type = FRIBIDI_TYPE_EOT;
  link->len = 0;
  link->pos = type_len;
  g_list_append(last, link);

  return list;
}

/* Some convenience macros */
#define RL_TYPE(list) ((TypeLink*)list->data)->type
#define RL_LEN(list) ((TypeLink*)list->data)->len
#define RL_POS(list) ((TypeLink*)list->data)->pos
#define RL_LEVEL(list) ((TypeLink*)list->data)->level

static void compact_list(GList *list)
{
  while(list)
    {
      if (list->prev
	  && RL_TYPE(list->prev) == RL_TYPE(list))
	{
	  GList *next = list->next;
	  list->prev->next = list->next;
	  list->next->prev = list->prev;
	  RL_LEN(list->prev) = RL_LEN(list->prev) + RL_LEN(list);
	  g_list_free_1(list);
	  list = next;
      }
      else
	list = list->next;
    }
}

/* Define a rule macro */

/* Rules for overriding current type */
#define TYPE_RULE1(old_this,            \
		   new_this)             \
     if (this_type == TYPE_ ## old_this)      \
         RL_TYPE(pp) =       FRIBIDI_TYPE_ ## new_this; \

/* Rules for current and previous type */
#define TYPE_RULE2(old_prev, old_this,            \
		  new_prev, new_this)             \
     if (    prev_type == FRIBIDI_TYPE_ ## old_prev       \
	  && this_type == FRIBIDI_TYPE_ ## old_this)      \
       {                                          \
	   RL_TYPE(pp->prev) = FRIBIDI_TYPE_ ## new_prev; \
	   RL_TYPE(pp) =       FRIBIDI_TYPE_ ## new_this; \
           continue;                              \
       }

/* A full rule that assigns all three types */
#define TYPE_RULE(old_prev, old_this, old_next,   \
		  new_prev, new_this, new_next)   \
     if (    prev_type == FRIBIDI_TYPE_ ## old_prev       \
	  && this_type == FRIBIDI_TYPE_ ## old_this       \
	  && next_type == FRIBIDI_TYPE_ ## old_next)      \
       {                                          \
	   RL_TYPE(pp->prev) = FRIBIDI_TYPE_ ## new_prev; \
	   RL_TYPE(pp) =       FRIBIDI_TYPE_ ## new_this; \
	   RL_TYPE(pp->next) = FRIBIDI_TYPE_ ## new_next; \
           continue;                              \
       }

/* For optimization the following macro only assigns the center type */
#define TYPE_RULE_C(old_prev, old_this, old_next,   \
		    new_this)   \
     if (    prev_type == FRIBIDI_TYPE_ ## old_prev       \
	  && this_type == FRIBIDI_TYPE_ ## old_this       \
	  && next_type == FRIBIDI_TYPE_ ## old_next)      \
       {                                          \
	   RL_TYPE(pp) =       FRIBIDI_TYPE_ ## new_this; \
           continue;                              \
       }

/*======================================================================
//  For debugging, define some macros for printing the types and the
//  levels.
//----------------------------------------------------------------------*/
static void print_types_re(GList *pp)
{
  while(pp)
    {
      printf("%d:%c(%d)[%d] ", RL_POS(pp), RL_TYPE(pp), RL_LEN(pp), RL_LEVEL(pp));
      pp = g_list_next(pp);
    }
  printf("\n");
}

static void print_resolved_levels(GList *pp)
{
  while(pp)
    {
      int i;
      for (i=0; i<RL_LEN(pp); i++)
	printf("%d", RL_LEVEL(pp));
      pp = g_list_next(pp);
    }
  printf("\n");
}

static void print_resolved_types(GList *pp)
{
  while(pp)
    {
      int i;
      for (i=0; i<RL_LEN(pp); i++)
	{
	  gchar ch;
	  FriBidiCharType type = RL_TYPE(pp);

	  /* Convert the type to something readable */
	  if (type == FRIBIDI_TYPE_R)
	    ch = 'R';
	  else if (type == FRIBIDI_TYPE_L)
	    ch = 'L';
	  else if (type == FRIBIDI_TYPE_E)
	    ch = 'E';
	  else if (type == FRIBIDI_TYPE_EN)
	    ch = 'n';
	  else if (type == FRIBIDI_TYPE_N)
	    ch = 'N';
	  else
	    ch = '?';
	  
	  printf("%c", ch);
	}
      pp = g_list_next(pp);
    }
  printf("\n");
}

static void print_bidi_string(FriBidiChar *str)
{
  int i;
  for (i=0; i<bidi_string_strlen(str); i++)
    printf("%c", str[i]);
  printf("\n");
}

/*======================================================================
//  search_rl_for strong searches the run length list in the direction
//  indicated by dir for a strong directional. It returns a pointer to
//  the found character or NULL if none is found.
//----------------------------------------------------------------------*/
static GList *
search_rl_for_strong(GList *pos,
		     gint  dir)
{
  GList *pp = pos;

  if (dir == -1)
    {
      for (pp = pos; pp; pp=pp->prev)
	{
	  FriBidiCharType char_type = RL_TYPE(pp);
	  if (char_type == FRIBIDI_TYPE_R || char_type == FRIBIDI_TYPE_L)
	    return pp;
	}
    }
  else
    {
      for (pp = pos; pp; pp=pp->next)
	{
	  FriBidiCharType char_type = RL_TYPE(pp);
	  if (char_type == FRIBIDI_TYPE_R || char_type == FRIBIDI_TYPE_L)
	    return pp;
	}
    }
  return NULL;
}

/*======================================================================
//  This function should follow the Unicode specification closely!
//----------------------------------------------------------------------*/
static void
fribidi_analyse_string(/* input */
		       FriBidiChar *str,
		       int len,
		       FriBidiCharType *pbase_dir,
		       /* output */
		       GList **ptype_rl_list,
		       gint *pmax_level)
{
  int base_level, base_dir;
  int max_level;
  int i;
  int *char_type;
  GList *type_rl_list, *pp;

  /* Determinate character types */
  char_type = g_new(gint, len);
  for (i=0; i<len; i++)
    char_type[i] = fribidi_get_type(str[i]);

  /* Run length encode the character types */
  type_rl_list = run_length_encode_types(char_type, len);

  /* Find the base level */
  if (*pbase_dir == FRIBIDI_TYPE_L)
    {
      base_dir = FRIBIDI_TYPE_L;
      base_level = 0;
    }
  else if (*pbase_dir == FRIBIDI_TYPE_R)
    {
      base_dir = FRIBIDI_TYPE_R;
      base_level = 1;
    }

  /* Search for first strong character and use its direction as base
     direciton */
  else
    {
      base_level = 0; /* Default */
      base_dir = FRIBIDI_TYPE_L;
      for (pp = type_rl_list; pp; pp = pp->next)
	if (RL_TYPE(pp) == FRIBIDI_TYPE_R)
	  {
	    base_level = 1;
	    base_dir = FRIBIDI_TYPE_R;
	    break;
	  }
	else if (RL_TYPE(pp) == FRIBIDI_TYPE_L)
	  {
	    base_level = 0;
	    base_dir = FRIBIDI_TYPE_L;
	    break;
	  }
    }
  
  /* 1. Explicit Levels and Directions. TBD! */
  compact_list(type_rl_list);
  
  /* 2. Explicit Overrides. TBD! */
  compact_list(type_rl_list);
  
  /* 3. Terminating Embeddings and overrides. TBD! */
  compact_list(type_rl_list);
  
  /* 4. Resolving weak types */
  for (pp = type_rl_list->next; pp->next; pp = pp->next)
    {
      int prev_type = RL_TYPE(pp->prev);
      int this_type = RL_TYPE(pp);
      int next_type = RL_TYPE(pp->next);

      /* C0. */
      TYPE_RULE2(R, CM,      R, R);
      TYPE_RULE2(L, CM,      L, L);
      TYPE_RULE2(SOT, CM,    SOT, N);
      
      /* P0. */
      /* Changing european numbers to Arabic numbers. TODO! */

      /* P0a. */
      TYPE_RULE2(EN,BN,    EN,EN);
      TYPE_RULE2(BN,EN,    EN,EN);
      TYPE_RULE2(AN,BN,    AN,AN);
      TYPE_RULE2(BN,AN,    AN,AN);
      TYPE_RULE_C(AN,BN,EN,    EN);
      TYPE_RULE_C(EN,BN,AN,    EN);

      /* P1. */
      if (RL_LEN(pp) == 1) 
	{
	  TYPE_RULE_C(EN,ES,EN,   EN);
	  TYPE_RULE_C(EN,CS,EN,   EN);
	  TYPE_RULE_C(AN,CS,AN,   AN);
	}

      /* P2. */
      TYPE_RULE2(ET,EN,   EN,EN);
    }

  compact_list(type_rl_list);
  
  /* 5. Resolving Neutral Types */
  if (fribidi_debug)
      fprintf(stderr,"Resolving neutral types.\n");

  /* We can now collapse all separators and other neutral types to
     plain neutrals */
  for (pp = type_rl_list->next; pp->next; pp = pp->next)
    {
      int this_type = RL_TYPE(pp);

      if (   this_type == FRIBIDI_TYPE_WS
	  || this_type == FRIBIDI_TYPE_ON
	  || this_type == FRIBIDI_TYPE_ES
	  || this_type == FRIBIDI_TYPE_ET
	  || this_type == FRIBIDI_TYPE_CS
	  || this_type == FRIBIDI_TYPE_BN)
	RL_TYPE(pp) = FRIBIDI_TYPE_N;
    }
    
  compact_list(type_rl_list);
  
  for (pp = type_rl_list->next; pp->next; pp = pp->next)
    {
      int prev_type = RL_TYPE(pp->prev);
      int this_type = RL_TYPE(pp);
      int next_type = RL_TYPE(pp->next);

      if (this_type == FRIBIDI_TYPE_N)   /* optimization! */
	{
	  /* N1. */
	  TYPE_RULE_C(R,N,R,   R);
	  TYPE_RULE_C(L,N,L,   L);
	  
	  /* N2 */
	  TYPE_RULE_C(L,N,R,     E);
	  TYPE_RULE_C(R,N,L,     E);
	  TYPE_RULE_C(L,N,EOT,   E);
	  TYPE_RULE_C(R,N,EOT,   E);
	  TYPE_RULE_C(SOT,N,L,   E);
	  TYPE_RULE_C(SOT,N,R,   E);
	}

      /* N3a, b

	 For the purpose of resolving neutral European numbers
	 inherit the type of the previous strong character
	 or if there is none found, they get the type of the base direction.
       */
      else if (this_type == FRIBIDI_TYPE_EN)
	{
	  GList *p_bck, *p_fwd, *pp1;
	  FriBidiCharType num_type, fwd_type;

	  /* Search backwards for a strong character */
	  p_bck = search_rl_for_strong(pp, -1);
	  
	  /* The EN is considered to be of this type */
	  if (p_bck)
	    num_type = RL_TYPE(p_bck);
	  /* Or the embedding type */
	  else
	    num_type = FRIBIDI_TYPE_E;

	  /* Override all neutrals between p_bck and this pp */
	  for (pp1 = pp->prev; pp1 != p_bck; pp1=pp1->prev)
	    {
	      if (RL_TYPE(pp1) != FRIBIDI_TYPE_N)
		break;
	      RL_TYPE(pp1) = num_type;
	    }

	  /* Now search forwards for a strong character */
	  p_fwd = search_rl_for_strong(pp, 1);
	  if (p_fwd)
	    fwd_type = RL_TYPE(p_fwd);
	  /* Or the embedding type */
	  else
	    fwd_type = FRIBIDI_TYPE_E;

	  /* Override with the embedding type if fwd_type != num_type */
	  if (fwd_type != num_type)
	    fwd_type = FRIBIDI_TYPE_E;

	  for (pp1 = pp->next; pp1 != p_fwd; pp1=pp1->next)
	    {
	      if (RL_TYPE(pp1) != FRIBIDI_TYPE_N)
		break;
	      RL_TYPE(pp1) = fwd_type;
	    }
	}
    }

  /* Make a separate sweep where Arabic numbers are treated as R. The
     loop is made separate in order not to influence the treatment
     of european numbers above. */
  for (pp = type_rl_list->next; pp->next; pp = pp->next)
    {
      int prev_type, next_type, this_type = RL_TYPE(pp);

      if (this_type != FRIBIDI_TYPE_N)        /* optimization */
	continue;
      
      prev_type = RL_TYPE(pp->prev);
      next_type = RL_TYPE(pp->next);

      /* this_type cannot be AN so there is no need to check. */
      if (prev_type == FRIBIDI_TYPE_AN)
	prev_type = FRIBIDI_TYPE_R;
      if (next_type == FRIBIDI_TYPE_AN)
	next_type = FRIBIDI_TYPE_R;
      
      /* N1. */
      TYPE_RULE_C(R,N,R,   R);
      TYPE_RULE_C(L,N,L,   L);

      /* N2 */
      TYPE_RULE_C(L,N,R,    E);
      TYPE_RULE_C(R,N,L,    E);
      TYPE_RULE_C(L,N,EOT,  E);
      TYPE_RULE_C(R,N,EOT,  E);
      TYPE_RULE_C(SOT,N,L,  E);
      TYPE_RULE_C(SOT,N,R,  E);
    }

  compact_list(type_rl_list);
  if (fribidi_debug)
      print_types_re(type_rl_list);
  
  /* 6. Resolving Implicit levels */
  if (fribidi_debug)
    fprintf(stderr,"Resolving implicit levels.\n");
  {
    int level = base_level;
    max_level = base_level;
    
    for (pp = type_rl_list->next; pp->next; pp = pp->next)
      {
	int this_type = RL_TYPE(pp);

	/* This code should be expanded to handle explicit directions! */

	/* Even */
	if (level % 2 == 0)
	  {
	    if (this_type == FRIBIDI_TYPE_R)
	      RL_LEVEL(pp) = level + 1;
	    else if (this_type == FRIBIDI_TYPE_AN)
	      RL_LEVEL(pp) = level + 2;
	    else if (RL_TYPE(pp->prev) != FRIBIDI_TYPE_L && this_type == FRIBIDI_TYPE_EN)
	      RL_LEVEL(pp) = level + 2;
	    else
	      RL_LEVEL(pp) = level;
	  }
	/* Odd */
	else
	  {
	    if (   this_type == FRIBIDI_TYPE_L
		|| this_type == FRIBIDI_TYPE_AN
		|| this_type == FRIBIDI_TYPE_EN)
	      RL_LEVEL(pp) = level+1;
	    else
	      RL_LEVEL(pp) = level;
	  }

	if (RL_LEVEL(pp) > max_level)
	  max_level = RL_LEVEL(pp);
      }
  }
  
  compact_list(type_rl_list);

  if (fribidi_debug)
    {
      print_bidi_string(str);
      print_resolved_levels(type_rl_list);
      print_resolved_types(type_rl_list);
    }

  *ptype_rl_list = type_rl_list;
  *pmax_level = max_level;
  *pbase_dir = base_dir;
}

/*======================================================================
//  Here starts the exposed front end functions.
//----------------------------------------------------------------------*/

/*======================================================================
//  fribidi_log2vis() calls the function_analyse_string() and then
//  does reordering and fills in the output strings.
//----------------------------------------------------------------------*/
void fribidi_log2vis(/* input */
		     FriBidiChar *str,
		     gint len,
		     FriBidiCharType *pbase_dir,
		     /* output */
		     FriBidiChar *visual_str,
		     guint16     *position_L_to_V_list,
		     guint16     *position_V_to_L_list,
		     guint8      *embedding_level_list
		     )
{
  GList *type_rl_list, *pp;
  int max_level;
  gboolean private_L_to_V = FALSE;

  /* If v2l is to be calculated we must have l2v as well. If it is not
     given by the caller, we have to make a private instance of it. */
  if (position_V_to_L_list && !position_L_to_V_list)
    {
      private_L_to_V++;
      position_L_to_V_list = g_new(guint16, len+1);
    }

  if (len > (2L<<16)-1)
    {
      fprintf(stderr, "Fribidi can't handle strings > 65000 chars!\n");
      return;
    }
  fribidi_analyse_string(str, len, pbase_dir,
			 /* output */
			 &type_rl_list,
			 &max_level);

  /* 7. Reordering resolved levels */
  if (fribidi_debug)
    fprintf(stderr, "Reordering.\n");

  {
    int level_idx;
    int i;

    /* Set up the ordering array to sorted order and copy the logical
       string to the visual */
    if (position_L_to_V_list)
      for (i=0; i<len+1; i++)
	position_L_to_V_list[i]=i;
    
    if (visual_str)
      for (i=0; i<len+1; i++)
	visual_str[i] = str[i];

    /* Assign the embedding level array */
    if (embedding_level_list)
      for (pp = type_rl_list->next; pp->next; pp = pp->next)
	{
	  int i;
	  int pos = RL_POS(pp);
	  int len = RL_LEN(pp);
	  int level = RL_LEVEL(pp);
	  for (i=0; i<len; i++)
	    embedding_level_list[pos + i] = level;
      }
    
    /* Reorder both the outstring and the order array*/
    if (visual_str || position_L_to_V_list)
      {

	if (visual_str)
	  /* Mirror all characters that are in odd levels and have mirrors */
	  for (pp = type_rl_list->next; pp->next; pp = pp->next)
	    {
	      if (RL_LEVEL(pp) % 2 == 1)
		{
		  int i;
		  for (i=RL_POS(pp); i<RL_POS(pp)+RL_LEN(pp); i++)
		    {
		      FriBidiChar mirrored_ch;
		      if (fribidi_get_mirror_char(visual_str[i], &mirrored_ch))
			visual_str[i] = mirrored_ch;
		    }
		}
	    }

	/* Reorder */
	for (level_idx = max_level; level_idx>0; level_idx--)
	  {
	    for (pp = type_rl_list->next; pp->next; pp = pp->next)
	      {
		if (RL_LEVEL(pp) >= level_idx)
		  {
		    /* Find all stretches that are >= level_idx */
		    int len = RL_LEN(pp);
		    int pos = RL_POS(pp);
		    GList *pp1 = pp->next;
		    while(pp1->next && RL_LEVEL(pp1) >= level_idx)
		      {
			len+= RL_LEN(pp1);
			pp1 = pp1->next;
		      }
		    
		    pp = pp1->prev;
		    if (visual_str)
		      bidi_string_reverse(visual_str+pos, len);
		    if (position_L_to_V_list)
		      int16_array_reverse(position_L_to_V_list+pos, len);

		  }
	      }
	  }
      }

    /* Convert the l2v list to v2l */
    if (position_V_to_L_list && position_L_to_V_list)
      for (i=0; i<len; i++)
	position_V_to_L_list[position_L_to_V_list[i]] = i;
  }

  /* Free up the rl_list */
  g_list_free(type_rl_list);

  /* Free up L_to_V if we allocated it */
  if (private_L_to_V)
    g_free(position_L_to_V_list);
  
}

/*======================================================================
//  fribidi_embedding_levels() is used in order to just get the
//  embedding levels.
//----------------------------------------------------------------------*/
void fribidi_log2vis_get_embedding_levels(
                     /* input */
		     FriBidiChar *str,
		     int len,
		     FriBidiCharType *pbase_dir,
		     /* output */
		     guint8 *embedding_level_list
		     )
{
  GList *type_rl_list, *pp;
  int max_level;
  
  fribidi_analyse_string(str, len, pbase_dir,
			 /* output */
			 &type_rl_list,
			 &max_level);

  for (pp = type_rl_list->next; pp->next; pp = pp->next)
    {
      int i;
      int pos = RL_POS(pp);
      int len = RL_LEN(pp);
      int level = RL_LEVEL(pp);
      for (i=0; i<len; i++)
	embedding_level_list[pos + i] = level;
    }

  /* Free up the rl_list */
  g_list_free(type_rl_list);
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
