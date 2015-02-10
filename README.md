JakMuse
=======

Synthesizes 7 sound signals based on a textual representation of musical notes.

The format is described below.

_(note: the documentation is a bit out of date on account of the re-write)_

Input format
============

Input is read from STDIN.

```yacc
input: | samples ;

samples: sample | samples sample ;

sample: CHANNEL
        INT<fill> INT<notesPerSecond> 
        INT<volume>
        INT<attack_length> INT<decay_length>
        INT<sustain>
        INT<lowpass_filter_frequency>
        notes
        ";" ;

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

But that will change
--------------------

~~For v2.0 the sample format will be changed to:~~ no it won't.

```yacc
sample: CHANNEL INT<fill> INT<divisor> INT<volume> notes ";" ;
```

~~The `divisor` specifies by what amount to divide a second. E.g. if the divisor is `64` and your notes are `32C4 32E4`, a C4 and an E4 of equal length will be played over the span of a second (give or take; the wav output will be closer to a second than realtime playback).~~

~~The `volume` option specifies the amplitude of the signal. If you're using only one channel, 255 will be a good option. If you're using 7 you may consider using 32. (around `2^(8-log(Nchannels))`)~~

*TODO* develop a nice little media tracking document format. It's a bit described in the Roadmap.

Examples
--------

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

Roadmap
=======

Preliminary:

* [x] investigate better mixing technique (preliminary)
* [x] investigate noise generators á là gameboy (preliminary)
* [x] investigate better realtime playback strategy (preliminary)

TODO:

* [x] rewrite according to _studyV2_ (v2.0)
  + [x] get rid of monotonic clock and be realtime
  + [x] use new divisor for sample tempo
  + [x] implement noise generator
* [x] add simple volume control to samples (alongside fill/phase and divisor) (v2.0)
* [x] add two more phase-shifted sine channels (v2.0)
* [x] low pass filter for generators (v2.0)
* [x] advanced ~~ADSR~~ ADS volume (v2.0)
* [x] make generators objects because the internal counter leaks across channels (yes, this is a bug caused by static variables in case you were wondering) (v2.0)
* [x] experiment with filtering after ADS envelope (tied to the above) (v2.0)
* [ ] support LFO (v2.0) _(the support is there, just need to actually teach the parser about it)_
* [ ] output wav file (v2.0)
* [ ] improve parser to modify some parameters in an optional way (e.g. I set ADSR once for channel 0 but I still want to modify its fill factor without having to re-specify ADSR) because the amount of parameters is becoming unweildly (8 + 5 more on the way) (v2.0)
* [ ] support doublesharp and doubleflat (v2.1)
* [ ] support ~~bends/glides~~ glide parameter (v2.2)
* [ ] output lillypad document instead of ~~channel dump~~ nothing (v2.5)
* [ ] build interactive library (e.g. to be used in games) (v3.0)
  + [ ] separate parsing and compilation
  + [ ] multi-threaded compilation of channels
  + [ ] continuous _music_ channels
  + [ ] one-off jingle/SFX channels as overrides

JakMuse v2.0 will be released when there are no more TODO items for it.

Tentative syntax supporting optional parameters:
```
# channel
0
# begin parameters
{
  NPS = 3                 # all of these will have reasonable
  Fill = 128              # default
  MaxVol = 128            #
  A = 300                 # if one is not specified, its last
  D = 400                 # specified value for this channel
  S = 200                 # will be kept
  R = 100
  Filter = 12000
  LFODepth = 60
  LFOFreq = 20
  LFOPhase = 20 # maybe
# end parameters
}
# notes
1C4 7D4 ;
```

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
