JakMuse
=======

Synthesizes 7 sound signals based on a textual representation of musical notes.

The format is described below.

Input format
============

Input is read from STDIN.

```yacc
input: | samples ;

samples: sample | samples sample ;

sample: CHANNEL
        '{' param_list '}'
        notes
        ";" ;

param_list: | param_list param ;

param: STRING INT ;

notes: note | notes note;

note: NOTE | PAUSE ; 

NOTE: "\d+[A-G][#b]?\d+" /* <length><note_name><accidental><note_scale> */
PAUSE: "\d+-" /* <length>- */ 
CHANNEL: "0" | "1" | "2" | "3" | "4" ;
INT: "[0-9]*" ;
COMMENT: "#.*$" :
```

Example:

```
# channel
0
# begin parameters
{
  NPS 3                 # all of these will have reasonable
  Fill 128              # default
  MaxVol 128            #
  A 300                 # if one is not specified, its last
  D 400                 # specified value for this channel
  S 200                 # will be kept
  R 100
  Filter 12000
  LFODepth 60
  LFOFreq 20
  LFOPhase 20
  LFOFMDepth 32
# end parameters
}
# notes
1C4 7D4 ;
```

The `Fill` parameter specifies the fill parameter for the PWM wave.

The `NPS` (Notes per second) parameter specifies how many 1-length notes will be played per second.

The `MaxVol` controls the master volume of the channel. `A`, `D`, `S`, `R` configure the ADSR envelope.

`Filter` is the frequency of a low pass filter applied over the signal.

The `LFO*` parameters control the LFO for each channel. The LFO is a sine.

The same channel can apear in multiple samples; for each subsequent appearance, the notes will be appended to the previous samples for that channel.

The format of a note is `<length><base_note><accidental?><octave>`. The length is a multiple of `NPS`.

`# Comments` are ignored.

Examples
--------

**NOTE**: the samples are not in a stable working order because the parser is going through a re-write.

You can see examples/samples in the `./samples/` subdirectory.

You can listen to them by running
```sh
./jakmuse <samples/beat2
```

or something like that.

You can exit via `CTRL-C` (a.k.a. SIGINT a.k.a. interrupt signal).

Command line options
====================

| option                    | effect                        |
|---------------------------|-------------------------------|
| -w filename               | output a PCM wave file & exit |
| -v                        | prints the version & exits    |

Output
======

Wave
----

You can specify the `-w filename` command line option to jakmuse. The result will be a PCM wave file @11kHz, mono, 16bit.

Audio
-----

The square waves are loop'd into your soundcard so that you may hear them.

Right now, SDL is used for actual audio output.

For timing, a hacked-up implementation of a of a timer using clock_nanosleepand repeated calls to clock_gettime is used as a pseudo real-time interrupt. This means that while a song sounds consistent on _a_ computer, different computers tend to lag more or less based on their internal clock resolution (sound round to 10ms, some have a better resolution, etc). (fox example, my netbook lags like 5-10% behind my desktop, but it doesn't stutter or jitter or anything) What this means is that until I figure out a way to tap into a real real-time clock that's actually realtime, don't try synchronizing two computers to play a masterwork composition together.

Building
========

...on windows
-------------

Works with Visual Studio 2013.

You need to run nmake from the visual studio command line shell (if you don't know what that is, try *Start/Programs/Visual Studio 2013/Tools/Developer Command Prompt for VS2013*.

Then, grab SDL from [here](https://www.libsdl.org/release/SDL-devel-1.2.15-VC.zip) (SDL-devel-1.2.15-VC) and unzip it in the JakMuse\win32 folder.

```batch
cd \path\to\JakMuse\win32
nmake
```

You can use other versions of SDL, but you need to update the SDLROOT var/macro for nmake.

...on linux
-----------

Apart from the regular `g++` & `make`, you need `libSDL-devel` (or whatever your distro calls it). Then, just run `make` and it should compiler.

`g++` needs to be recent enough to support C++11. (e.g. 4.8.x is a good version)

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
* [x] support LFO (v2.0)
* [x] output wav file (v2.0)
* [x] improve parser to modify some parameters in an optional way (e.g. I set ADSR once for channel 0 but I still want to modify its fill factor without having to re-specify ADSR) because the amount of parameters is becoming unweildly (8 + 5 more on the way) (v2.0)
* [ ] support doublesharp and doubleflat (v2.1)
* [ ] support ~~bends/glides~~ glide parameter (v2.2)
* [ ] output lillypad document instead of ~~channel dump~~ nothing (v2.5)
* [ ] build interactive library (e.g. to be used in games) (v3.0)
  + [ ] separate parsing and compilation
  + [ ] multi-threaded compilation of channels
  + [ ] continuous _music_ channels
  + [ ] one-off jingle/SFX channels as overrides

Test files
==========

**NOTE**: the samples are not in a stable working order because the parser is going through a re-write.

**Disclaimer**: I have used the word _test_ badly in the following table.

| name                          | description                           |
|-------------------------------|---------------------------------------|
| `test`                        | this unceremoniously named files tests all 5 channels at once |
| `testSolo`                    | tests the two triangle waves acting as either triangle waves or sawteeth |
| `testSolo2`                   | tests a square and a triangle (in triangle or sawtooth mode) together |
| `beat2`                       | a funky beat |
| `testDrumBeat`                | test a drum beat with a square arpeggio |
| `drumSolo`                    | same beat sans square voice |
