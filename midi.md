# MIDI [^1]

## Patch programs

| Pgm | Name    |
|-----|---------|
| 00  | Square  |
| 01  | Buzzer  |

## CCs [^2]

| CC  | Function                      | Control     |
|-----|-------------------------------|-------------|
| 102 | Buzzer waveform (0-7)[^3]     |             |
| 103 | Mix buzzer/square (on/off)    |             |
| 104 | Buzzer/square ratio           |             |
| 105 | Buzzer/square detune          |             |
| 106 | Noise (off/period)            |             |
| 107 | Attack Pitch[^4]              |             |
| 108 | Attack                        |             |
| 109 | Hold                          |             |
| 110 | Decay                         |             |
| 111 | Sustain                       |             |
| 112 | Release                       |             |
| 113 | Arpeggio speed                |             |

## Available buzzer waveforms [^5]

|   | Waveforms |
|---|-----------|
| 0 | `\|\|\|`  |
| 1 | `\_____`  |
| 2 | `\/\/\/`  |
| 3 | `\|────`  |
| 4 | `/|/|/|`  |
| 5 | `/─────`  |
| 6 | `/\/\/\`  |
| 7 | `/|____`  |

[^1]: This document is intended as instructions and as a cheat sheet when using
    the synth. Print using a monospaced font from a raw text viewer to avoid
    issues with display renderers.
[^2]: You can fill the Control column in the printed sheet with the name of the
    assigned control in your MIDI device.
[^3]: MIDI values will be scaled down from 0-127 to 0-7 to match the values in
    in the waveforms table.
[^4]: Attack Pitch is how much to raise/lower the tone during the Attack/Hold
    phases.
[^5]: Display renderers might not correctly show the waveform representation
    this table.
