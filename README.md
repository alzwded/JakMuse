JakMuse
=======

Sequences 5 square waves based on a textual representation of musical notes.

The format is described below.

Input format
============

```yacc
input: | samples ;

samples: sample | samples sample ;

sample: CHANNEL INT<fill> INT<multiplier> notes ;

notes: note | notes note;

note: "\d+[A-G][#b]?\d+" /* <length><note_name><accidental><note_scale> */
    | "\d+-" /* pause: <length>- */
    ;

CHANNEL: "0" | "1" | "2" | "3" | "4" ;
INT: "[0-9]*" ;
```

The `fill` parameter specifies the fill parameter for the PWM wave.
The `multiplier` multiplies all lengths by that amount.
The same channel can apear in multiple samples; for each subsequent appearance, the notes will be appended to the previous samples for that channel.

Output
======

Text
----

It outputs a list of the frequencies for all channels and a list for the fill factors for all channels.

Audio
-----

The square waves are loop'd into your soundcard so that you may hear them.

TODO
====

Figure out how one can do real-time signal composition so that one may compose a square wave, a triangle wave and a sine wave of different frequencies without obtaining white noise.

THEN, change channel 2 to triangle and channels 3 and 4 to sine.
