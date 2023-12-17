# MIDI

## Patch programs

| Program | Name                                   | Tone  |  Buzzer |
|---------|----------------------------------------|-------|---------|
| X0      | Square                                 | Note  |  Off    |
| X1      | Buzzer                                 | Off   |  Note   |
| X2      | Buzzer > Square                        | Note  |  Note   |
| X3      | Square > Buzzer                        | Note  |  Note   |
| X4      | Square + Fixed Buzzer                  | Note  |  Fixed  |
| X5      | Buzzer + Fixed Square                  | Fixed |  Note   |

| X    | Buzzer waveform |        |
|------|-----------------|--------|
| 0    | Sawtooth        | \|\|\| |
| 1    | Triangle 90º    | \/\/\/ |
| 2    | Ramp            | /|/|/| |
| 3    | Triangle        | /\/\/\ |

## CCs

| CC | Function                         | Programs  |
|----|----------------------------------|-----------|
| 70 | Noise off/period                 |           |
| 71 | Buzzer/Square ratio              | 3-6       |
| 72 | Buzzer/Square detune             | 3-6       |
| 73 | Arpeggio speed                   |           |
| 74 | Attack Pitch                     |           |
| 75 | Attack                           |           |
| 76 | Hold                             |           |
| 77 | Decay                            |           |
| 78 | Sustain                          |           |
| 79 | Release                          |           |
| 91 | Ringmod depth                    | 1,3-6     |
| 92 | Ringmod detune                   | 1,3-6     |
| 93 | Ringmod duty                     | 1,3-6     |
| 94 | SyncBuzzer period                | 3-6       |

Fixed sets the frequency for the fixed square/buzzer/ringmod.
Detune is used with the square/buzzer and square/ringmod combinations.
Ringmod with low detune is better known as SID effect.
Ringmod depth is how much low the volume is set (0-15). A value of 15 disables the effect.
Pitch Attack is how much to raise the tone in semitones during the Attack phase.
