#!/usr/bin/gawk -f

#  (C) Copyright 2007 Przemys³aw Pawe³czyk <przemoc@gmail.com>
#
# Siemens ringtone converter  v. 0.1
#
# Usage:
# siemens.gawk [-v bpm=XXX] [-v oct=Y] siemens_ringtone_file | pcspk -n

BEGIN {
	BPM = 100;	# default BPM
	OCT = 2;	# default octave shift

	_ord_init();
	if (!(bpm > 0))
		bpm = BPM;
	if (oct == "")
		oct = OCT;
	is = "CDFGA";
	es = "degaB";
	ORS = " ";
	printf "%03d%s", bpm, ORS;
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
	parse($0); 
}
