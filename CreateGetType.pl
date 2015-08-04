#!/usr/bin/perl -w

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

my $unicode_version = "3.0.1";
my $unicode_data_file = "UnicodeData-$unicode_version.txt";
my $unicode_proplist_file = "PropList-$unicode_version.txt";
my @bidi_entities;
my @mirrors;

my %type_translate = (L=>'LTR',R=>'RTL','B'=>'BS','S'=>'SS');

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
		  "0x0B000000" => ["AL",  "Arabic right to left letter"],
		  "0x0C000000" => ["NSM", "Non-spacing mark"],
		  "0x0D000000" => ["BN",  "Boundary Neutral"],
		  "0x0E000000" => ["PDF",  "Pop directional formatting"],
		  "0x0F000000" => ["EO", "Embedding or override"],
		  "0x80000009" => ["ON",  "Other Neutral"],
		  "0x10000091" => ["LRE", "RLE"],		  
		  "0x10000092" => ["RLE", "RLE"],		  
		  "0x10000093" => ["LRO", "RLO"],		  
		  "0x10000094" => ["RLO", "LRO"],		  
		 );

open(PROP, $unicode_proplist_file)
   or die "Failed opening $unicode_proplist_file!\n";

open(DATA, $unicode_data_file)
   or die "Failed opening $unicode_data_file!\n";

#parse_unicode_data_for_bidi_entries();
parse_prop_for_bidi_entities();
find_bidi_controls();
#print_bidi_entities();
parse_for_mirror_chars();
#print_mirrored_chars();
create_c_file();

sub parse_unicode_data_for_bidi_entries {
    seek(DATA,0,0);
    my ($prev_type, $prev_num) = ('',0);
    while(<DATA>) {
	my ($num, $type) = (split(/;/))[0,4];
	my $tt;
	if ($tt = $type_translate{$type}) {
	    $type = $tt;
	}
	$num = hex($num);
	
	if ($prev_type eq $type && $num == $prev_num+1) {
	    $bidi_entities[-1][2]++;
	} else {
	    push(@bidi_entities, [$num, $type, 1]);
	}
	$prev_num = $num;
	$prev_type = $type;
    }
}

sub parse_prop_for_bidi_entities {
    my @property_bidi_entities;
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
	if ($type && /^\s*$/) {
	    $type = "";
	}
	next unless $type;

	my($range) = split;
	my($first,$last,$len);
	if ($range =~ /\./) {
	   ($first,$last) = map(hex, split(/\.\./, $range));
	    $len = $last - $first +1;
	} else {
	    ($first, $len) = (hex($range), 1);
	}
#	print "$first $type_names{$type_num}->[0] $len\n";
	$type = $type_names{$type_num} || die "Undefined num $type_num!\n";

	push(@property_bidi_entities, [$first, $type_names{$type_num}->[0], $len]);
    }

    @bidi_entities = sort { $a->[0] <=> $b->[0] } @property_bidi_entities;
}

######################################################################
#  Manually change the bidi control characters that are not
#  explicitely listed in the PropList file to their corresponding
#  values.
#
#  This should be changed to be more generic, but I don't expect
#  this behaviour to change in the PropList file in the near
#  future.
######################################################################
sub find_bidi_controls {
    # Erase all bidi controls
    @bidi_entities = grep($_->[1] !~ /^(CTL|EO)/o, @bidi_entities);

    # Manually add the bidi controls that are not listed in PropList
    push(@bidi_entities,
	 [0x202a, 'LRE', 1],
	 [0x202b, 'RLE', 1],
	 [0x202d, 'LRO', 1],
	 [0x202e, 'RLO', 1]);

    # resort
    @bidi_entities = sort { $a->[0] <=> $b->[0] } @bidi_entities;
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
	print sprintf("%04x", $e->[0]), " $e->[1] $e->[2]\n";
    }
}

sub split_entity {
    my $e = shift;
    my ($start, $type, $len) = @$e;

    my ($first,$last) = ($start, $start+$len-1);

    return ($first, $last, $type);
};


sub create_block {
    my ($block,$name, $ranges) = @_;
    my($title) = <<__;
guchar ${name}\[256\] = {
__
    my $result;
    my $last_type=$ranges->[0]->[2];
    
    my $ind = 0;
    for my $i ($block*256 .. $block*256 + 255) {
       my $found = 0;
           
       if ($i % 16 == 0) {
           $result .= "  ";
       }
       while ($ind < @$ranges) {
           if ($i > $ranges->[$ind]->[1]) {
               $ind++;
           } elsif ($i < $ranges->[$ind]->[0]) {
               last;
           } else {
               $found = 1;
               $result .= sprintf("%-3s,", $ranges->[$ind]->[2]);
	       $last_type=$ranges->[$ind]->[2];
               last;
           }
       }
       if (!$found) {
	   $result .= sprintf("%-3s,", $last_type);
       }
       if ($i % 16 == 15) {
           $result .= "\n";
       }
    }
    $result .= "};\n\n";
    return($title, $result);
};


sub create_c_file {
    # Create the h file
    my $num_used_blocks = 0;

    my $c_file =<<__;
/*======================================================================
//  This file was automatically created from $unicode_proplist_file
//  by the perl script CreateGetType.pl.
//----------------------------------------------------------------------*/

#include "fribidi.h"

__

    for my $type (keys %type_names) {
       $c_file .= "#define $type_names{$type}->[0] FRIBIDI_TYPE_$type_names{$type}->[0]\n";
    }
    $c_file .= "\n\n";
    
    my $i = 0;
    my $block_array = <<__;
guchar *FriBidiPropertyBlocks[256] = {
__

    my %seen_blocks;
    for my $block (0..255) {
       my @block_ranges;

       while ($i < @bidi_entities) {
           my ($first,$last,$type) = split_entity ($bidi_entities[$i]);

           if ($first > $block * 256 + 255) {
               last;
           } else {
               if ($last >= $block * 256) {
                   push @block_ranges, [ $first > $block * 256 ? $first : $block * 256,
                                         $last < $block * 256 + 255 ? $last : $block * 256 + 255, 
                                         $type ];
                                    
               }
               if ($last < ($block + 1) * 256) {
                   $i++;
               } else {
                   last;
               }
           }
       }

       if (@block_ranges == 0) {
           $block_array .= "  NULL,\n";
           
#       }
#       elsif (@block_ranges == 1
#		&& $block_ranges[0]->[1] - $block_ranges[0]->[0] >= 255
#		&& exists $solid_blocks{$block_ranges[0]->[2]}) {
#           my $name = $solid_blocks{$block_ranges[0]->[2]};
#           $block_array .= "  $name,\n";
           
       } else {
           my $name = sprintf ("FriBidiPropertyBlock%04x", $block * 256);

	   my($title, $block_ctx) = create_block ($block, $name, \@block_ranges);

	   # use the fact that a hash table uses a good hash function. ;-)
	   if (exists $seen_blocks{$block_ctx}) {
	       my $name = $seen_blocks{$block_ctx};
	       $block_array .= "  $name,\n";
	   } else {
	       $num_used_blocks++;
	       $block_array .= "  $name,\n";
	       $c_file .= $title . $block_ctx;
               $seen_blocks{$block_ctx} = $name;
	   }

#           if (@block_ranges == 1
#	       && $block_ranges[0]->[1] - $block_ranges[0]->[0] >= 255
#	      ) {
#               $solid_blocks{$block_ranges[0]->[2]} = $name;
#           }
       }
    }
   $c_file .= "/* $num_used_blocks blocks defined */\n\n";

   $c_file .= $block_array
             . "};\n\n";

    for my $type (keys %type_names) {
       $c_file .= "#undef $type_names{$type}->[0]\n";
    }

   $c_file .= "\n\n";

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
