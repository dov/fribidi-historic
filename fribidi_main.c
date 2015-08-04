/*======================================================================
//  A main program for fribidi.
//
//  Dov Grobgeld
//----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <glib.h>
#include "fribidi.h"

#define CASE(s) if(!strcmp(S_, s))

void die(gchar *fmt, ...)
{
    va_list ap;
    va_start(ap,fmt); 
    
    vfprintf(stderr, fmt, ap);
    exit(-1);
}

#define UNI_ALEF 0x05D0
#define UNI_TAV 0x05EA

void charset_to_unicode(gint char_set,
			guchar *s,
			/* output */
			FriBidiChar *us)
{
  int i;
  int len = strlen(s);
  
  if (char_set == 0)
    {
      /* Convert A-Z into hebrew characters */
      for (i=0; i<len+1; i++)
	{
	  guchar ch = s[i];

	  if (ch >= 'A' && ch <= 'Z')
	    us[i] = ch - 'A' + UNI_ALEF;
	  else
	    us[i] = s[i];
	}
    }
  else if (char_set == 8)
      fribidi_iso8859_8_to_unicode(s, us);
  else
    die("Sorry! Not implemented!\n");
}

void unicode_to_charset(gint char_set,
			FriBidiChar *us,
			/* output */
			gchar *s)
{
  if (char_set == 0)
    {
      /* Convert hebrew characters into A-Z */
      int i = 0;
      
      while(us[i])
	{
	  FriBidiChar ch = us[i];
	  if (ch >= UNI_ALEF && ch <= UNI_TAV)
	    s[i] = ch-UNI_ALEF+'A';
	  else if (us[i] < 256)
	    s[i] = us[i];
	  else
	    s[i] = '¿';
	  i++;
	}
      s[i] = 0;
    }
  else if (char_set == 8)
    fribidi_unicode_to_iso8859_8(us, s);
  else
    die("Sorry! Not implemented!\n");
}

int main(int argc, char *argv[])
{
  int argp=1;
  FILE *IN;
  gboolean text_width = 80;
  gboolean do_pad = TRUE;
  gboolean do_fill = FALSE;
  gint char_set = 0;
  guchar *eol_text = NULL;

  /* Parse the command line */
  while(argp < argc && argv[argp][0] == '-')
    {
      gchar *S_ = argv[argp++];
      CASE("-help")
	{
	  printf("fribidi - A command line interface to the fribidi library\n"
		 "\n"
		 "Syntax:\n"
		 "    fribidi {options} [filename]\n"
		 "\n"
		 "Options:\n"
		 "    -nopad       Don't right adjustify RTL lines\n"
		 "    -fill        Fill lines up to margin. (Not implemented).\n"
		 "    -width w     Specify width of text\n"
		 "    -eol eol     End lines with the string given by eol.\n"
		 "    -charset cs  Specify charset. Default is CapRTL. Available options are:\n"
		 "                     * 8859-8 (Hebrew)\n"
		 "                     * 8859-6 (Arabic)\n"

		 );
	  
	  exit(0);
	}
      CASE("-nopad") { do_pad = FALSE; continue; }
      CASE("-width") { text_width = atoi(argv[argp++]); continue; }
      CASE("-eol")   { eol_text = argv[argp++]; continue; }
      CASE("-fill")  { do_fill = TRUE; continue; };
      CASE("-charset")
	{
	  gchar *S_ = argv[argp++];
	  while(1) {
	    CASE("8859-8") { char_set = 8; break; }
	    CASE("8859-6") { char_set = 6; break; }
	    die("Unknown char set %s!\n", S_);
	  }
	  continue;
	}
      
	

      die("Unknown option %s!\n", S_);
    }

  /* Open the infile for reading */
  if (argp >= argc)
    {
      IN = stdin;
    }
  else
    {
      gchar *fn = argv[argp++];
      IN = fopen(fn, "r");
    }

  /* Read and process input one line at a time */
  {
    gchar S_[2048];
    
    while(fgets(S_, sizeof(S_), IN))
      {
	int len = strlen(S_);
	FriBidiChar us[2048], out_us[2048];
	gint positionLtoV[2048], positionVtoL[2048];
	guchar outstring[2048];
	int base;
	int i;
	
	/* chop */
	if (S_[len-1] == '\n')
	  S_[len-1] = '\0';

	len--;

	charset_to_unicode(char_set, S_, us);
      
	/* Create a bidi string */
	base = FRIBIDI_TYPE_N;
	fribidi_log2vis(us, &base,
			/* output */
			out_us,
			positionLtoV,
			positionVtoL
			);

	/* Convert it to something to print */
	unicode_to_charset(char_set, out_us, outstring);

	if (base == FRIBIDI_TYPE_R && do_pad)
	  for (i=0; i<text_width-len; i++)
	    printf(" ");
	
	printf("%s", outstring);
	if (eol_text)
	  printf("%s", eol_text);
	printf("\n");
      }
  }
}
