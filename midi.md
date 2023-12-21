# MIDI

## Patch programs

| Pgm | Name                | Tone  | Envelope |
|-----|---------------------|-------|----------|
| X0  | Square              | Note  | Off      |
| X1  | Buzzer              | Off   | Note     |
| X2  | Buzzer > Square     | Note  | Note     |
| X3  | Square > Buzzer     | Note  | Note     |
| X4  | Square (SID)        | Note  | Off      |

| X | Buzzer waveform |        |
|---|-----------------|--------|
| 0 | Sawtooth        | \|\|\| |
| 1 | Triangle (-)    | \/\/\/ |
| 2 | Ramp            | /|/|/| |
| 3 | Triangle (+)    | /\/\/\ |

## CCs

| CC | Function                         | Programs  |
|----|----------------------------------|-----------|
| 70 | Noise period/off                 |           |
| 71 | Buzzer/Square ratio              | 2,3       |
| 72 | Buzzer/Square detune             | 2,3       |
| 73 | Arpeggio speed                   |           |
| 74 | Attack Pitch                     |           |
| 75 | Attack                           |           |
| 76 | Hold                             |           |
| 77 | Decay                            |           |
| 78 | Sustain                          |           |
| 79 | Release                          |           |
| 91 | SyncSquare period                | 0,2,3     |
| 92 | SyncBuzzer period                | 0,2,3     |
| 93 | SID detune                       | 4         |

Pitch Attack is how much to raise/lower the tone in semitones during the
Attack/Hold phases.
