JakMuse
=======

Synthesizes a handful of sound signals based on a textual representation of musical notes.

The format is described below.

Signal Types
------------

* pulse-width signal
* two types of noise (á là gameboy)
* triangle (with phase distortion)
* sine (with phase distortion)

All these support ADSR envelopes, glide and RC filtering.

Input format
============

Input is read from STDIN. This means you can either pipe/redirect the input from a file or write it yourself at the terminal.

On POSIX systems, you can end STDIN on a terminal with ^D (Ctrl+D) on a blank line. On Windows you can end STDIN in cmd the CP/M way with ^Z<CR> (Ctrl-Z Enter) on a blank line.

```yacc
input: | sequences ;

sequences: sequence | sequences sequence ;

sequence: CHANNEL
        '/' param_list '/'
        notes
        ";" ;

param_list: | param_list param ;

param: STRING INT ;

notes: note | notes note;

note: NOTE | PAUSE ; 

NOTE: "\d+[A-G][#b]?\d+" /* <length><note_name><accidental><note_scale> */
PAUSE: "\d+-" /* <length>- */ 
CHANNEL: [0-6] ;
INT: "[0-9]*" ;
COMMENT: "#.*$" :
```

Example:

```
# channel
0
# begin parameters
/
  NPS 3                 # all of these will have reasonable
  Fill 128              # default
  MaxVol 128            #
  A 40                  # if one is not specified, its last
  D 47                  # specified value for this channel
  S 200                 # will be kept
  R 10                  #
  Filter 12000          # ...except for NPS which defaults to 1
  LFODepth 60           # if not specified. For now...
  LFOFreq 20
  LFOPhase 20
  Glide 800
  ResetADSR 1
# end parameters
/
# notes
1C4 7D4 ;
```

The `Fill` parameter specifies the fill parameter for the PWM wave. A fill of `128` means you have an actual square wave.

The `NPS` (Notes per second) parameter specifies how many 1-length notes will be played per second. *E.g.* a `1C4` at `NPS 3` will play a single `C4` for a 3rd of a second.

The `MaxVol` controls the master volume of the channel. `A`, `D`, `S`, `R` configure the ADSR envelope. A volume of `MaxVol 255` will result in the maximum amplitude supported by your sound card. If you're using multiple channels, you should scale down the `MaxVol` parameter to avoid distortion. The mixing algorithm tries to prevent clipping, but it still introduces distortion when mixing signals which are too loud. If you want to rely on this distortion, feel free to keep `MaxVol` at high values.

The `S` parameter represents the percentage to which to stabilise the volume of the note after the decay phase. `255` means 100% of `MaxVol`, `128` means ~50%, etc.

`Filter` is the frequency of a low pass filter applied over the signal. The filter is applied individually to each channel, and not to the mixed output.

The `LFO*` parameters control the LFO-based amplitude modulation for each channel. The LFO is a sine.

The same channel can apear in multiple samples; for each subsequent appearance, the notes will be appended to the previous samples for that channel.

The format of a note is `<length><base_note><accidental?><octave>`. The length is a multiple of `NPS`.

The `Glide` parameter controls how much time is taken to shift from the previous note to the next.

The `ResetADSR` parameter configures whether or not to start a new attack/decay phase for consecutive notes without a rest between them. With `ResetADSR 0`, of consecutive notes – i.e. `1A4 1B4` – only the first will have an attack/decay phase and only the last will have a release phase. With `ResetADSR 1` all notes will have ADS phases and only the last will have a release phase.

`# Comments` are ignored.

Notes range from `Cb0` all the way to `B#10`. You probably can't hear these extreme notes, so that means you pretty much have all notes in the diatonic scale available, tuned to `A=440Hz`.

### Units

`NPS` is expressed in notes per second.

`Fill`, `S`, `LFODepth` and `MaxVol` are expressed in values from 0 to 255.

`Filter` and `LFOFreq` are frequencies between 0 and 65535.

`A`, `D`, `R`, `LFOPhase` and `Glide` are durations, and a value of 4096 equals one second.

`ResetADSR` can be 0 (disabled) or 1 (enabled).

Examples
--------

**NOTE**: the samples are not in a stable working order because the parser is going through a re-write.

You can see examples/samples in the `./samples/` subdirectory.

You can listen to them by running
```sh
./jakmuse <samples/beat2
```

or something like that.

Or you can dump a .wav file:
```batch
jakmuse -w beat2.wav <samples/beat2
```

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

You can specify the `-w filename` command line option to jakmuse. The result will be a PCM wave file @44kHz, mono, 16bit.

Audio
-----

The synthesised sound waves are loop'd into your soundcard so that you can hear them.

Right now, SDL is used for actual audio output. The input is rendered up front and the resulting PCM data is streamed is done in realtime.

Technically speaking, SDL is not needed to write the wave file, only for playback.

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

You need libSDL.so version 1.2.x installed on your system.

Roadmap
=======


## pre2.0 ##

Preliminary:

* [x] investigate better mixing technique (preliminary)
* [x] investigate noise generators á là gameboy (preliminary)
* [x] investigate better realtime playback strategy (preliminary)

## v2.0 ##

* [x] rewrite according to _studyV2_ (v2.0)
  + [x] get rid of monotonic clock and be realtime
  + [x] use new divisor for sample tempo
  + [x] implement noise generator
* [x] add simple volume control to samples (alongside fill/phase and divisor) (v2.0)
* [x] add two more phase-shifted sine channels (v2.0)
* [x] low pass filter for generators (v2.0)
* [x] advanced ~~ADSR~~ ~~ADS~~ ADSR volume (v2.0)
* [x] make generators objects because the internal counter leaks across channels (yes, this is a bug caused by static variables in case you were wondering) (v2.0)
* [x] experiment with filtering after ADS envelope (tied to the above) (v2.0)
* [x] support LFO (v2.0)
* [x] output wav file (v2.0)
* [x] improve parser to modify some parameters in an optional way (e.g. I set ADSR once for channel 0 but I still want to modify its fill factor without having to re-specify ADSR) because the amount of parameters is becoming unweildly (8 + 5 more on the way) (v2.0)
* [x] multi-threaded compilation of channels (v2.0)
* [x] support ~~bends/glides~~ glide parameter (v2.0)
* [x] implement simple phase distortion (1 inflection point) + LFOPhase becomes a parameter to a phase-distorsed LFO (v2.0)

## v2.1 ##

* [ ] stereo support
* [ ] high-pass filter in addition to the low pass one; some means of configuring the filter
* [ ] wav output to stdout in order to respect the \*nix philosophy.

## v2.2 ##

* [ ] support pitch modulation
* [ ] more advanced phase distortion (multiple inflection points)

## backlog ##

* [ ] output lillypad document instead of ~~channel dump~~ nothing (v2.5)
* [ ] build interactive library (e.g. to be used in games; this needs a major rewrite (again)) (v3.0)
  + [ ] continuous _music_ channels
  + [ ] one-off jingle/SFX channels as overrides
* [ ] load libSDL only when playing music interactively

FIXME
-----

* [x] NPS value is not kept across sequences
