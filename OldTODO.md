TODO
====

~~Use higher resolution timer because SDL's insists it's at 10ms. Basically import the timer from the JakVM project.~~ DONE

~~Figure out how one can do real-time signal composition so that one may compose a square wave, a triangle wave and a sine wave of different frequencies without obtaining white noise.~~ maybe done? See roadmap

~~THEN, change channel 2 to triangle and channels 3 and 4 to sine.~~ 3 square, two tria/sawtooth

~~Pink noise generator. One that doesn't take 10ms to compute.~~ pseudo random number generator

~~One probable solution would be to take a frequency domain representation of white noise (or some other noise) and then filter it based on desired input, then IFFT it back to the time domain and mix it in with the other signals. Initial estimates say this process might take around 1 ms to perform for one channel, so we're well below the 8ms mark.~~ no.

~~Another solution would be to actually implement a noise generator, but meh.~~ see roadmap

Support doublesharp and doubleflat (quarter steps). Options:
* actually support stuff like `4A##4 4A#b4`
* use `$` and `v` to substitute for 0.25 steps above and 0.25 steps below or 0.75 or whatever
