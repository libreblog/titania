#!/usr/bin/perl
# -*- Mode: Perl; coding: utf-8; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*-

use strict;
use warnings;
use v5.10.0;
use utf8;

use Glib;

my $fields = "/home/holger/Projekte/Titania/Library/Prototypes/Fields";

my $keyFile = new Glib::KeyFile ();
$keyFile -> load_from_data (join ("", `titania-info -f 2>/dev/null`), "none");

foreach my $name (`ls -C1 '$fields'`)
{
	print $name;	
	chomp $name;
	
	next if $name eq "Fields.pl";

	my $value = $keyFile -> get_string ($name, "value");
	my $proto = "$fields/$name/$name.x3dv";

	open FILE, ">", $proto;

	say FILE "#X3D V3.3 utf8 Titania

PROTO $name [
  eventIn      SFTime set_triggerTime
  exposedField $name  keyValue $value
  eventOut     $name  value_changed
]
{
  DEF $name Script {
    eventIn       SFTime set_triggerTime IS set_triggerTime
    exposedField  $name  keyValue IS keyValue
    eventOut      $name  value_changed IS value_changed

    url \"vrmlscript:
function set_triggerTime (value, time)
{
	value_changed = keyValue;
}
\"
  }
}
";

	close FILE;

	system "x3dtidy", $proto, $proto;
}

