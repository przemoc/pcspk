#!/usr/bin/gawk -f

#  (C) Copyright 2007-8 Przemyslaw Pawelczyk <przemoc@gmail.com>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License Version 2 as
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#
# Siemens ringtone converter  v. 0.3
#
# Usage:
# siemens.gawk [-v bpm=XXX] [-v oct=Y] siemens_ringtone_file | pcspk -n

BEGIN {
	IGNORECASE = 1
	BPM = 100;	# default BPM
	OCT = 2;	# default octave shift

	_ord_init();
	is = "CDFGA";
	es = "degaB";
	ORS = " ";
}

/^#.*bpm=([0-9]+)/ {
	match($0, /bpm=([0-9]+)/, b);
  if ((b[1] > 0) && !(bpm > 0))
	  bpm = b[1];
}

/^#.*oct=([0-9]+)/ {
	match($0, /oct=([0-9]+)/, b);
  if ((b[1] > 0) && !(oct > 0))
	  oct = b[1];
}

# _ord_init is taken from
# http://www.gnu.org/software/gawk/manual/gawk.html#Ordinal-Functions

function _ord_init( low, high, i, t) {
	low = sprintf("%c", 7);	# BEL is ascii 7
	if (low == "\a") {	# regular ascii
		low = 0;
		high = 127;
	} else if (sprintf("%c", 128 + 7) == "\a") {	# ascii, mark parity
		low = 128;
		high = 255;
	} else {	# ebcdic(!)
		low = 0;
		high = 255;
	}

	for (i = low; i <= high; i++) {
		t = sprintf("%c", i);
		_ord_[t] = i;
	}
}

function parse(input) {
	split(input, array, "[ \t]+");
	for (i = 1; i in array; i++) {
		if (array[i] !~ /^([CDEFGAHBP](is)?)([0-9])?\(1\/([0-9]+)\)/)
			continue;
		match(array[i], /^([CDEFGAHBP])(is)?([0-9])?\(1\/([0-9]+)\)/, s);
		if (s[2] != "")
			s[1] = substr(es, index(is, s[1]), 1);
		else
			s[1] = toupper(s[1]);
		s[3] += oct;
		if (s[3] < 0)
			s[3] = 0;
		s[4] = s[4] - 1;
		if(s[4] >= 10)
			s[4] = sprintf("%c", _ord_["A"] + s[4] - 10);
		print s[1] s[3] s[4]
	}
}

/^[^#]/ {
	if (!(notfirst)) {
		if (!(bpm > 0))
			bpm = BPM;
		if (!(oct > 0))
			oct = OCT;
		printf "%03d%s", bpm, ORS;
		notfirst = 1;
	}
	parse($0); 
}
