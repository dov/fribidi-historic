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
#include "fribidi.h"
#ifndef NO_STDIO
#include <stdio.h>
#endif

#ifndef NO_STDIO
#define DBG(s) if (fribidi_debug) { fprintf(stderr, s); }
#else
#define DBG(s)
#endif

/*======================================================================
// Typedef for the run-length list.
//----------------------------------------------------------------------*/
typedef struct _TypeLink TypeLink;

struct _TypeLink {
  TypeLink *prev;
  TypeLink *next;
  FriBidiCharType type;
  gint pos;
  gint len;
  gint level;
};

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

static gint bidi_string_strlen(FriBidiChar *str)
{
  gint len = 0;

  while(*str++)
    len++;
  
  return len;
}

static void bidi_string_reverse(FriBidiChar *str, gint len)
{
  gint i;
  for (i=0; i<len/2; i++)
    {
      FriBidiChar tmp = str[i];
      str[i] = str[len-1-i];
      str[len-1-i] = tmp;
    }
}

static void
int16_array_reverse(guint16 *arr, gint len)
{
  gint i;
  for (i=0; i<len/2; i++)
    {
      guint16 tmp = arr[i];
      arr[i] = arr[len-1-i];
      arr[len-1-i] = tmp;
    }
}

#ifndef USE_SIMPLE_MALLOC
static TypeLink *free_type_links = NULL;
#endif

static TypeLink *new_type_link(void)
{
  TypeLink *link;
  
#ifdef USE_SIMPLE_MALLOC
  link = g_malloc(sizeof(TypeLink));
#else
  if (free_type_links)
    {
      link = free_type_links;
      free_type_links = free_type_links->next;
    }
  else
    {
      static GMemChunk *mem_chunk = NULL;

      if (!mem_chunk)
       mem_chunk = g_mem_chunk_new ("TypeLinkList",
                                    sizeof (TypeLink),
                                    sizeof (TypeLink) * 128,
                                    G_ALLOC_ONLY);

      link = g_chunk_new (TypeLink, mem_chunk);
    }
#endif
  
  link->len = 0;
  link->pos = 0;
  link->level = 0;
  link->next = NULL;
  link->prev = NULL;
  return link;
}

static void free_type_link(TypeLink *link)
{
#ifdef USE_SIMPLE_MALLOC
  g_free(link);
#else
  link->next = free_type_links;
  free_type_links = link;
#endif
}

static TypeLink *run_length_encode_types(gint *char_type, gint type_len)
{
  TypeLink *list = NULL;
  TypeLink *last;
  TypeLink *link;
  FriBidiCharType type;
  gint len, pos, i;

  /* Add the starting link */
  list = new_type_link();
  list->type = FRIBIDI_TYPE_SOT;
  list->len = 0;
  list->pos = 0;
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
             last->next = link;
             link->prev = last;
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
  last->next = link;
  link->prev = last;

  return list;
}

/* Some convenience macros */
#define RL_TYPE(list) (list)->type
#define RL_LEN(list) (list)->len
#define RL_POS(list) (list)->pos
#define RL_LEVEL(list) (list)->level

static void compact_list(TypeLink *list)
{
  while(list)
    {
      if (list->prev
	  && RL_TYPE(list->prev) == RL_TYPE(list))
	{
          TypeLink *next = list->next;
	  list->prev->next = list->next;
	  list->next->prev = list->prev;
	  RL_LEN(list->prev) = RL_LEN(list->prev) + RL_LEN(list);
          free_type_link(list);
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
#ifndef NO_STDIO
static void print_types_re(TypeLink *pp)
{
  while(pp)
    {
      printf("%d:%c(%d)[%d] ", RL_POS(pp), RL_TYPE(pp), RL_LEN(pp), RL_LEVEL(pp));
      pp = pp->next;
    }
  printf("\n");
}

static void print_resolved_levels(TypeLink *pp)
{
  while(pp)
    {
      gint i;
      for (i=0; i<RL_LEN(pp); i++)
	printf("%d", RL_LEVEL(pp));
      pp = pp->next;
    }
  printf("\n");
}

static void print_resolved_types(TypeLink *pp)
{
  while(pp)
    {
      gint i;
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
      pp = pp->next;
    }
  printf("\n");
}

static void print_bidi_string(FriBidiChar *str)
{
  gint i;
  for (i=0; i<bidi_string_strlen(str); i++)
    printf("%c", str[i]);
  printf("\n");
}
#endif

/*======================================================================
//  search_rl_for strong searches the run length list in the direction
//  indicated by dir for a strong directional. It returns a pointer to
//  the found character or NULL if none is found. */
//----------------------------------------------------------------------*/
static TypeLink *
search_rl_for_strong(TypeLink *pos,
		     gint  dir)
{
  TypeLink *pp = pos;

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
//
//  It is still lacking the support for <RLO> and <LRO>.
//----------------------------------------------------------------------*/
static void
fribidi_analyse_string(/* input */
		       FriBidiChar *str,
		       gint len,
		       FriBidiCharType *pbase_dir,
		       /* output */
                      TypeLink **ptype_rl_list,
		       gint *pmax_level)
{
  gint base_level, base_dir;
  gint max_level;
  gint i;
  gint *char_type;
  gint prev_last_strong, last_strong;
  TypeLink *type_rl_list, *pp;

  /* Determinate character types */
  char_type = g_new(gint, len);
  for (i=0; i<len; i++)
    char_type[i] = fribidi_get_type(str[i]);

  /* Run length encode the character types */
  type_rl_list = run_length_encode_types(char_type, len);
  g_free(char_type);

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
      base_dir = FRIBIDI_TYPE_N;
      for (pp = type_rl_list; pp; pp = pp->next)
	{
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
    
      /* If no strong base_dir was found, resort to the weak direction
       * that was passed on input.
       */
      if (base_dir == FRIBIDI_TYPE_N)
	{
	  if (*pbase_dir == FRIBIDI_TYPE_WR)
	    {
	      base_dir = FRIBIDI_TYPE_RTL;
	      base_level = 1;
	    }
	  else if (*pbase_dir == FRIBIDI_TYPE_WL)
	    {
	      base_dir = FRIBIDI_TYPE_LTR;
	      base_level = 0;
	    }
	}
    }
  
  /* 1. Explicit Levels and Directions. TBD! */
  compact_list(type_rl_list);
  
  /* 2. Explicit Overrides. TBD! */
  compact_list(type_rl_list);
  
  /* 3. Terminating Embeddings and overrides. TBD! */
  compact_list(type_rl_list);
  
  /* 4. Resolving weak types */
  last_strong = base_dir;
  for (pp = type_rl_list->next; pp->next; pp = pp->next)
    {
      gint prev_type = RL_TYPE(pp->prev);
      gint this_type = RL_TYPE(pp);
      gint next_type = RL_TYPE(pp->next);

      /* Remember the last strong character */
      if (prev_type == FRIBIDI_TYPE_AL
	  || prev_type == FRIBIDI_TYPE_R
	  || prev_type == FRIBIDI_TYPE_L)
	  last_strong = prev_type;
      
      /* W1. NSM */
      if (this_type == FRIBIDI_TYPE_NSM)
	{
	  if (prev_type == FRIBIDI_TYPE_SOT)
	    RL_TYPE(pp) = FRIBIDI_TYPE_N;       /* Will be resolved to base dir */
	  else
	    RL_TYPE(pp) = prev_type;
	}

      /* W2: European numbers */
      if (this_type == FRIBIDI_TYPE_N
	  && last_strong == FRIBIDI_TYPE_AL)
	RL_TYPE(pp) = FRIBIDI_TYPE_AN;

      /* W3: Change ALs to R
	 We have to do this for prev character as we would otherwise
	 interfer with the next last_strong which is FRIBIDI_TYPE_AL.
       */
      if (prev_type == FRIBIDI_TYPE_AL)
	RL_TYPE(pp->prev) = FRIBIDI_TYPE_R;

      /* W4. A single european separator changes to a european number.
	 A single common separator between two numbers of the same type
	 changes to that type.
       */
      if (RL_LEN(pp) == 1) 
	{
	  TYPE_RULE_C(EN,ES,EN,   EN);
	  TYPE_RULE_C(EN,CS,EN,   EN);
	  TYPE_RULE_C(AN,CS,AN,   AN);
	}

      /* W5. A sequence of European terminators adjacent to European
	 numbers changes to All European numbers.
       */
      if (this_type == FRIBIDI_TYPE_ET)
	{
	  if (next_type == FRIBIDI_TYPE_EN
	      || prev_type == FRIBIDI_TYPE_EN) {
	    RL_TYPE(pp) = FRIBIDI_TYPE_EN;
	  }
	}

      /* This type may have been overriden */
      this_type = RL_TYPE(pp);
      
      /* W6. Otherwise change separators and terminators to other neutral */
      if (this_type == FRIBIDI_TYPE_ET
	  || this_type == FRIBIDI_TYPE_CS
	  || this_type == FRIBIDI_TYPE_ES)
	RL_TYPE(pp) = FRIBIDI_TYPE_ON;

      /* W7. Change european numbers to L. */
      if (prev_type == FRIBIDI_TYPE_EN
	  && last_strong == FRIBIDI_TYPE_L)
	RL_TYPE(pp->prev) = FRIBIDI_TYPE_L;
    }

  /* Handle the two rules that effect pp->prev for the last element */
  if (RL_TYPE (pp->prev) == FRIBIDI_TYPE_AL) /* W3 */
    RL_TYPE(pp->prev) = FRIBIDI_TYPE_R;
  if (RL_TYPE (pp->prev) == FRIBIDI_TYPE_EN  /* W7 */
      && last_strong == FRIBIDI_TYPE_L)             
    RL_TYPE(pp->prev) = FRIBIDI_TYPE_L;

  compact_list(type_rl_list);
  
  /* 5. Resolving Neutral Types */
  DBG("Resolving neutral types.\n");

  /* We can now collapse all separators and other neutral types to
     plain neutrals */
  for (pp = type_rl_list->next; pp->next; pp = pp->next)
    {
      gint this_type = RL_TYPE(pp);

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
      gint prev_type = RL_TYPE(pp->prev);
      gint this_type = RL_TYPE(pp);
      gint next_type = RL_TYPE(pp->next);

      if (this_type == FRIBIDI_TYPE_N)   /* optimization! */
	{
	  /* "European and arabic numbers are treated
	     as though they were R" */

	  if (prev_type == FRIBIDI_TYPE_EN || prev_type == FRIBIDI_TYPE_AN)
	    prev_type = FRIBIDI_TYPE_R;

	  if (next_type == FRIBIDI_TYPE_EN || next_type == FRIBIDI_TYPE_AN)
	    next_type = FRIBIDI_TYPE_R;

	  /* N1. */
	  TYPE_RULE_C(R,N,R,   R);
	  TYPE_RULE_C(L,N,L,   L);

	  /* N2. Any remaining neutrals takes the embedding direction */
	  if (RL_TYPE(pp) == FRIBIDI_TYPE_N)
	    RL_TYPE(pp) = FRIBIDI_TYPE_E;
	}
    }

  compact_list(type_rl_list);
#ifndef NO_STDIO
  if (fribidi_debug)
      print_types_re(type_rl_list);
#endif
  
  /* 6. Resolving Implicit levels */
  DBG("Resolving implicit levels.\n");
  {
    gint level = base_level;
    max_level = base_level;
    
    for (pp = type_rl_list->next; pp->next; pp = pp->next)
      {
	gint this_type = RL_TYPE(pp);

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

#ifndef NO_STDIO
  if (fribidi_debug)
    {
      print_bidi_string(str);
      print_resolved_levels(type_rl_list);
      print_resolved_types(type_rl_list);
    }
#endif
  
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
  TypeLink *type_rl_list, *pp = NULL;
  gint max_level;
  gboolean private_V_to_L = FALSE;

  if (len == 0)
    return;
  
  /* If l2v is to be calculated we must have l2v as well. If it is not
     given by the caller, we have to make a private instance of it. */
  if (position_L_to_V_list && !position_V_to_L_list)
    {
      private_V_to_L = TRUE;
      position_V_to_L_list = g_new(guint16, len+1);
    }

  if (len > FRIBIDI_MAX_STRING_LENGTH)
    {
#ifndef NO_STDIO
	fprintf(stderr, "Fribidi can't handle strings > 65000 chars!\n");
#endif
      return;
    }
  fribidi_analyse_string(str, len, pbase_dir,
			 /* output */
			 &type_rl_list,
			 &max_level);

  /* 7. Reordering resolved levels */
  DBG("Reordering.\n");

  {
    gint level_idx;
    gint i;

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
	  gint i;
	  gint pos = RL_POS(pp);
	  gint len = RL_LEN(pp);
	  gint level = RL_LEVEL(pp);
	  for (i=0; i<len; i++)
	    embedding_level_list[pos + i] = level;
      }
    
    /* Reorder both the outstring and the order array*/
    if (visual_str || position_V_to_L_list)
      {

	if (visual_str)
	  /* Mirror all characters that are in odd levels and have mirrors */
	  for (pp = type_rl_list->next; pp->next; pp = pp->next)
	    {
	      if (RL_LEVEL(pp) % 2 == 1)
		{
		  gint i;
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
		    gint len = RL_LEN(pp);
		    gint pos = RL_POS(pp);
                    TypeLink *pp1 = pp->next;
		    while(pp1->next && RL_LEVEL(pp1) >= level_idx)
		      {
			len+= RL_LEN(pp1);
			pp1 = pp1->next;
		      }
		    
		    pp = pp1->prev;
		    if (visual_str)
		      bidi_string_reverse(visual_str+pos, len);
		    if (position_V_to_L_list)
		      int16_array_reverse(position_V_to_L_list+pos, len);

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

  /* At this point, pp points to the last link or (rarely) might be NULL
   */
  if (!pp)
    for (pp = type_rl_list->next; pp->next; pp = pp->next)
      /* Nothing */;
  
  pp->next = free_type_links;
  free_type_links = type_rl_list;

  /* Free up V_to_L if we allocated it */
  if (private_V_to_L)
    g_free(position_V_to_L_list);
  
}

/*======================================================================
//  fribidi_embedding_levels() is used in order to just get the
//  embedding levels.
//----------------------------------------------------------------------*/
void fribidi_log2vis_get_embedding_levels(
                     /* input */
		     FriBidiChar *str,
		     gint len,
		     FriBidiCharType *pbase_dir,
		     /* output */
		     guint8 *embedding_level_list
		     )
{
  TypeLink *type_rl_list, *pp;
  gint max_level;

  if (len = 0)
    return;
  
  fribidi_analyse_string(str, len, pbase_dir,
			 /* output */
			 &type_rl_list,
			 &max_level);

  for (pp = type_rl_list->next; pp->next; pp = pp->next)
    {
      gint i;
      gint pos = RL_POS(pp);
      gint len = RL_LEN(pp);
      gint level = RL_LEVEL(pp);
      for (i=0; i<len; i++)
	embedding_level_list[pos + i] = level;
    }
  
  /* Free up the rl_list */
  pp->next = free_type_links;
  free_type_links = type_rl_list;
}

