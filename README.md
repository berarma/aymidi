# AyMidi

MIDI synthesizer plugin built around the programmable sound generators
AY-3-8910 and YM2149.

Based on [Ayumi](https://github.com/true-grue/ayumi), an emulator for the
AY-3-8910 and YM2149.

## Goals

- Sound as close to the original chips as possible.
- Similar limitations to the original machines using these chips (mainly ZX
  Spectrum, Amstrad CPC, MSX and Atari ST).
- Implement similar techniques to the ones used in those computers in a way
  that makes it easy to use them to create music in a MIDI environment.
- Support popular audio plugin formats.

## Features

General:

- AY-3-8910 and YM2149 modes.
- Configurable clock rate from 1 to 2 Mhz.
- Configurable update rate from 50 to 300Hz.
- Jack standalone.
- LV2 plugin.
- VST2 plugin.
- VST3 plugin.
- CLAP plugin.

MIDI:

- Omni on/off.
- Mono/Poly modes.
- Poly mode with up to 3 simultaneous voices.
- Mono mode with MIDI channels grouped for each voice.
- Arpeggios in mono mode with variable speed and direction.
- Software controlled amplitude envelope (AHDSR).
- Software controlled pitch envelope for the attack/hold.
- Volume.
- Velocity.
- Pitch bend.
- Voice pan.

## Build

Install dependencies:

- cmake
- make
- GCC
- lv2-dev

Clone this repository with submodules:

`git clone --recurse-submodules https://github.com/berarma/aymidi`

Build the project:

```
cmake build
cd build
make
```

The plugins will be in the `build/bin/` directory.

## How to use

Load the plugin into your plugins host and connect the MIDI input and audio
outputs.

The output is unfiltered unlike it was in the old computers. The amplification
circuits which accompanied these chips together with the computer or TV
speakers filtered the audio output significantly.

**WARNING: Exposing your ears to the high harmonics for extended periods of time
or at high volumes can damage them.**

It's recommended to put at least a low-pass filter in the audio outputs to take
care of your ears but also to get it to sound more like the original hardware.

The [MIDI implementation table](midi.md) can help you when playing.

## Related projects

Other similar projects which might better fulfill your needs and also served as
inspiration:

- [Ayumi](https://github.com/true-grue/ayumi)
- [ayumi-lv2](https://github.com/atsushieno/ayumi-lv2)
- [Zynayumi](https://github.com/eriser/zynayumi)
- [Ym2149Synth](https://github.com/trash80/Ym2149Synth)
- [Catskull Electronics](https://catskullelectronics.com/products/ym2149-synth)

# Learning resources

Thanks to the authors of these resources which helped in the learning process:

- [Summary of MIDI 1.0 Messages](https://midi.org/summary-of-midi-1-0-messages)
- [MIDI 1.0 Control Change Messages](https://midi.org/midi-1-0-control-change-messages)
- [Philip Rees article on MIDI channels, voices, timbres and modes](http://www.philrees.co.uk/articles/midimode.htm)
- [gwEm's maxYMiser Tutorial](https://www.youtube.com/watch?v=OjPPUdwIAC0&list=PL1NhyQvCufQnczcLMUHOkSHtCt2dHi9_Z)

## Attributions

This project uses code from the following projects:

- [Ayumi](https://github.com/true-grue/ayumi)
- [DPF](https://github.com/DISTRHO/DPF)

# Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
