# MIDI

Instructions and cheat sheet for use with MIDI controller. Print using a
monospaced font.

## Patch programs

| Pgm | Name                |
|-----|---------------------|
| 00  | Square              |
| 01  | Sawtooth            |
| 02  | Square + Sawtooth   |
| 03  | Triangle            |
| 04  | Square + Triangle   |

## CCs

| CC  | Function                      | Control     |
|-----|-------------------------------|-------------|
| 102 | Noise (off/freq)              |             |
| 103 | Buzzer detune                 |             |
| 104 | Square detune                 |             |
| 105 | Attack Pitch[^1]              |             |
| 106 | Attack                        |             |
| 107 | Hold                          |             |
| 108 | Decay                         |             |
| 109 | Sustain                       |             |
| 110 | Release                       |             |
| 111 | Arpeggio speed[^2]            |             |

[^1]: Attack Pitch is how much to raise/lower the tone during the Attack/Hold
    phases.
[^2]: Negative for descending, positive for ascending and 0 to disable.
