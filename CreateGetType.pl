#!/usr/local/bin/perl -w

# FriBidi - Library of BiDi algorithm
# Copyright (C) 1999 Dov Grobgeld
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
#

######################################################################
#  This is a Perl program for automatically building the cfunction
#  fribidi_get_type() which returns the Bidi type of a unicode
#  character. To build this function the script parses the
#  PropList-*.txt and the UnicodeData-*.txt files.
######################################################################

use strict;

my $unicode_version = "2.1.8";
my $unicode_data_file = "UnicodeData-$unicode_version.txt";
my $unicode_proplist_file = "PropList-$unicode_version.txt";
my @bidi_entities;
my @arabic_letter_range;
my @mirrors;

my %type_names = ("0x10000090" => ["CTL", "Control units"],
		  "0x01000000" => ["LTR", "Strong left to right"],
		  "0x02000000" => ["RTL", "Strong right to left"],
		  "0x03000000" => ["EN",  "European digit"],
		  "0x04000000" => ["ES",  "European number separator"],
		  "0x05000000" => ["ET",  "European number terminator"],
		  "0x06000000" => ["AN",  "Arabic digit"],
		  "0x07000000" => ["CS",  "Common Separator"],
		  "0x08000000" => ["BS",  "Block separator"],
		  "0x09000000" => ["SS",  "Segment separator"],
		  "0x0A000000" => ["WS",  "Whitespace"],
		  "0x80000009" => ["ON",  "Other Neutral"],
		 );
open(PROP, $unicode_proplist_file)
   or die "Failed opening $unicode_proplist_file!\n";

open(DATA, $unicode_data_file)
   or die "Failed opening $unicode_data_file!\n";

parse_prop_for_bidi_entities();
#print_bidi_entities();
#find_bidi_control();
parse_for_arabic_strong_letters();
print_arabic_letters();
parse_for_mirror_chars();
print_mirrored_chars();
create_c_file();

sub parse_prop_for_bidi_entities {
    my ($type, $type_num, $type_descr);
    while(<PROP>) {
	tr/\r\n//d;
	if (/^Property dump for:\s*(.*bidi.*)/i) {
	    $type = $1;
	    $_=<PROP>;
	    $type=~ / /;
	    ($type_num, $type_descr) = ($`, $');
	    next;
	}
	if ($type && $_ =~ /^\s*$/) {
	    $type = "";
	}
	next unless $type;

	my($range) = split;
	push(@bidi_entities, [$range, $type_num]);
    }

    @bidi_entities = sort { $a->[0] cmp $b->[0] } @bidi_entities;
}

# An arabic letter for the purpose of changing EN to AN is considered
# any unicode character whose name starts with "ARABIC" and is strong
# right.
sub parse_for_arabic_strong_letters {
    my $first = undef;
    my $last;

    seek(DATA,0,0);
    while(<DATA>) {
	my @props = split(";");
	if ($props[1] =~ /^ARABIC/ && $props[4] eq "R") {
	    $first = $props[0] unless $first;
	    $last = $props[0];
	}
	elsif ($first) {
	    push(@arabic_letter_range, [$first, $last]);
	    undef $first;
	}
    }
}

sub print_arabic_letters {
    print STDERR "Arabic letters\n";
    for my $r (@arabic_letter_range) {
	print STDERR join(" ", @$r),"\n";
    }
}

sub parse_for_mirror_chars {
    my @mirrored_chars;
    my %mirror_name_to_num;
    seek(DATA,0,0);
    while(<DATA>) {
	my @props = split(";");
	next unless $props[9] eq "Y";

	# Just save them for now
	push(@mirrored_chars, [@props]);
	$mirror_name_to_num{$props[1]} = $props[0];
    }

    # Now find mirrored characters for a small subset of all those
    # characters defined as mirrorable.
    foreach my $m (@mirrored_chars) {
	my (@props) = @$m;
	my $num = $props[0];
	my $name = $props[1];
	my ($mirror_name, $mirror_num);
	my %opposite = ("UP RIGHT"=>"DOWN RIGHT",
			"DOWN RIGHT"=>"UP RIGHT",
			"LEFT"=>"RIGHT",
			"RIGHT"=>"LEFT",
			"LESS-THAN"=>"GREATER-THAN",
			"GREATER-THAN"=>"LESS-THAN");
	my $pattern = join("|", keys %opposite);
	($mirror_name = $name)
	    =~ s/\b($pattern)\b/$opposite{$&}/e;
	if ($mirror_name_to_num{$mirror_name}) {
	    $mirror_num = $mirror_name_to_num{$mirror_name};
	} else {
	    $mirror_num = $num;
	}

	# Should I include characters that map to themself???
	push(@mirrors, [$num, $mirror_num]) if $num ne $mirror_num;
    }
}

sub print_mirrored_chars {
    print STDERR "Mirrors:\n";
    for my $m (@mirrors) {
	my ($num, $mirror_num) = @$m;
	print STDERR "$num <=> $mirror_num\n";
    }
}

sub print_bidi_entities {
    for my $e (@bidi_entities) {
	print $e->[0], " ", $type_names{$e->[1][0]}, "\n";
    }
}

sub create_c_file {
    # Create the h file

    my $c_file =<<__;
/*======================================================================
//  This file was automatically created from $unicode_proplist_file
//  by the perl script CreateGetType.pl.
//----------------------------------------------------------------------*/

#include "fribidi.h"
__
    $c_file.= <<__;

/* The Bidi property List */
struct {
  FriBidiChar      first, last;
  FriBidiCharType  char_type;
} FriBidiPropertyList[] = {
__

   for my $i (0..@bidi_entities-1) {
       my $e = $bidi_entities[$i];
       my $comment = "";
       my($range, $type) = @$e;
       my($first, $last);
       if ($range =~ /\.\./) {
	   $first = $`; $last = $';
       } else {
	   $first = $last = $range;
       }
       if (hex($first)>=32 && hex($first) < 127) {
	   if ($first eq $last) {
	       $comment = sprintf("/*  '%s'   */", pack("C", hex($first)));
	   } else {
	       $comment = sprintf("/*  '%s'..'%s'   */",
				  pack("C", hex($first)),
				  pack("C", hex($last)),
				 );
	   }
       }
       $c_file.= "  \{0x$first, 0x$last, FRIBIDI_TYPE_$type_names{$type}[0] \}";
       $c_file .= "," unless $i == @bidi_entities-1;
       $c_file .= $comment if $comment;
       $c_file .= "\n";
   }

   $c_file .= "};\n\n"
             . "gint nFriBidiPropertyList = "
             . scalar(@bidi_entities) . ";\n\n";

    ######################################################################
    #   Arabic characters
    ######################################################################
    $c_file .= <<__;

/*======================================================================
//  An arabic letter for the purpose of changing EN to AN is considered
//  any unicode character whose name starts with "ARABIC" and is strong
//  right.
//======================================================================*/
struct {
  FriBidiChar first, last;
} FriBidiArabicStrongLetterRanges[] = {
__

    for my $i (0..@arabic_letter_range-1) {
	my($first, $last) = @{$arabic_letter_range[$i]};
	$c_file .= "  {0x$first, 0x$last}";
	$c_file .= "," unless $i == @arabic_letter_range-1;
	$c_file .= "\n";
    }
    $c_file .= "};\n\n";
    $c_file .= "gint nFriBidiArabicStrongLetterRanges = "
             . scalar(@arabic_letter_range) . ";\n";

    ######################################################################
    #  Mirrored characters.
    ######################################################################

    $c_file .= <<__;
/*======================================================================
//  Mirrored characters include all the characters in the Unicode list
//  that have been declared as being mirrored and that have a mirrored
//  equivalent.
//
//  There are lots of characters that are designed as being mirrored
//  but do not have any mirrored glyph, e.g. the sign for there exist.
//  Are these used in Arabic? That is are all the mathematical signs
//  that are assigned to be mirrorable actually mirrored in Arabic?
//  If that is the case, I'll change the below code to include also
//  characters that mirror to themself. It will then be the responsibility
//  of the display engine to actually mirror these.
//----------------------------------------------------------------------*/

struct {
     FriBidiChar ch, mirrored_ch;
} FriBidiMirroredChars[] = {
__

    for my $i (0..@mirrors-1) {
	my($ch, $mirrored_ch) = @{$mirrors[$i]};
	$c_file .= "  {0x$ch, 0x$mirrored_ch}";
	$c_file .= "," unless $i == @mirrors-1;
	$c_file .= "\n";
    }
    $c_file .= "};\n\n";
    $c_file .= "gint nFriBidiMirroredChars = "
             . scalar(@mirrors) . ";\n";

    print $c_file;
}
