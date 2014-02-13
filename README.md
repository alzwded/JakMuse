JakMuse
=======

Synthesizes 5 sound signals based on a textual representation of musical notes.

The format is described below.

Input format
============

Input is read from STDIN.

```yacc
input: | samples ;

samples: sample | samples sample ;

sample: CHANNEL INT<fill> INT<multiplier> notes ";" ;

notes: note | notes note;

note: NOTE | PAUSE ; 

NOTE: "\d+[A-G][#b]?\d+" /* <length><note_name><accidental><note_scale> */
PAUSE: "\d+-" /* <length>- */ 
CHANNEL: "0" | "1" | "2" | "3" | "4" ;
INT: "[0-9]*" ;
COMMENT: "#.*$" :
```

The `fill` parameter specifies the fill parameter for the PWM wave.

The `multiplier` multiplies all lengths by that amount.

The same channel can apear in multiple samples; for each subsequent appearance, the notes will be appended to the previous samples for that channel.

Comments are ignored.

You can see examples/samples in the `./samples/` subdirectory.

You can listen to them by running
```sh
./jakmuse <samples/beat2
```

or something like that.

You can exit via `CTRL-C` (a.k.a. SIGINT a.k.a. interrupt signal).

Output
======

Text
----

It outputs a list of the frequencies for all channels and a list for the fill factors for all channels.

This is useful to see what exactly is contained in each of the five buffers. Okay, it's not all that useful, but it's interesting to look at numbers.

Audio
-----

The square waves are loop'd into your soundcard so that you may hear them.

Right now, SDL is used for actual audio output.

For timing, a hacked-up implementation of a of a timer using clock_nanosleepand repeated calls to clock_gettime is used as a pseudo real-time interrupt. This means that while a song sounds consistent on _a_ computer, different computers tend to lag more or less based on their internal clock resolution (sound round to 10ms, some have a better resolution, etc). (fox example, my netbook lags like 5-10% behind my desktop, but it doesn't stutter or jitter or anything) What this means is that until I figure out a way to tap into a real real-time clock that's actually realtime, don't try synchronizing two computers to play a masterwork composition together.

TODO
====

~~Use higher resolution timer because SDL's insists it's at 10ms. Basically import the timer from the JakVM project.~~ DONE

Figure out how one can do real-time signal composition so that one may compose a square wave, a triangle wave and a sine wave of different frequencies without obtaining white noise.

~~THEN, change channel 2 to triangle and channels 3 and 4 to sine.~~ 3 square, two tria/sawtooth

Pink noise generator. One that doesn't take 10ms to compute.

One probable solution would be to take a frequency domain representation of white noise (or some other noise) and then filter it based on desired input, then IFFT it back to the time domain and mix it in with the other signals. Initial estimates say this process might take around 1 ms to perform for one channel, so we're well below the 8ms mark.

Another solution would be to actually implement a noise generator, but meh.

Support doublesharp and doubleflat (quarter steps). Options:
* actually support stuff like `4A##4 4A#b4`
* use `$` and `v` to substitute for 0.25 steps above and 0.25 steps below or 0.75 or whatever

Test files
==========

**Disclaimer**: I have used the word _test_ badly in the following table.

| name                          | description                           |
|-------------------------------|---------------------------------------|
| `test`                        | this unceremoniously named files tests all 5 channels at once |
| `testSolo`                    | tests the two triangle waves acting as either triangle waves or sawteeth |
| `testSolo2`                   | tests a square and a triangle (in triangle or sawtooth mode) together |
| `beat2`                       | a funky beat |
| `testDrumBeat`                | test a drum beat with a square arpeggio |
| `drumSolo`                    | same beat sans square voice |
