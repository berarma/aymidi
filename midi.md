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

| CC  | Function                         | Programs  |
|-----|----------------------------------|-----------|
| 102 | Noise period/off                 |           |
| 103 | Buzzer/Square ratio              | 2,3       |
| 104 | Buzzer/Square detune             | 2,3       |
| 105 | Arpeggio speed                   |           |
| 106 | Attack Pitch                     |           |
| 107 | Attack                           |           |
| 108 | Hold                             |           |
| 109 | Decay                            |           |
| 110 | Sustain                          |           |
| 111 | Release                          |           |
| 112 | SyncSquare period                | 0,2,3     |
| 113 | SyncBuzzer period                | 0,2,3     |
| 114 | SID detune                       | 4         |

Attack Pitch is how much to raise/lower the tone in semitones during the
Attack/Hold phases.
