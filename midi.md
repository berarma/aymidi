# MIDI

## Patch programs

| Pgm | Name                  | Buzzer Waveform |
|-----|-----------------------|-----------------|
| 00  | Square                |                 |
| 01  | Buzzer                | Sawtooth        |
| 02  | Buzzer > Square       | Sawtooth        |
| 03  | Square > Buzzer       | Sawtooth        |
| 04  | Buzzer                | Triangle (-)    |
| 05  | Buzzer > Square       | Triangle (-)    |
| 06  | Square > Buzzer       | Triangle (-)    |
| 07  | Buzzer                | Ramp            |
| 08  | Buzzer > Square       | Ramp            |
| 09  | Square > Buzzer       | Ramp            |
| 10  | Buzzer                | Triangle (+)    |
| 11  | Buzzer > Square       | Triangle (+)    |
| 12  | Square > Buzzer       | Triangle (+)    |

| Waveforms    |        |
|--------------|--------|
| Sawtooth     | \|\|\| |
| Ramp         | /|/|/| |
| Triangle (-) | \/\/\/ |
| Triangle (+) | /\/\/\ |

## CCs

| CC  | Function                         | Programs  |
|-----|----------------------------------|-----------|
| 102 | Noise period/off                 |           |
| 103 | Buzzer/Square ratio              | 2,3       |
| 104 | Buzzer/Square detune             | 2,3       |
| 105 | Attack Pitch[^1]                 |           |
| 106 | Attack                           |           |
| 107 | Hold                             |           |
| 108 | Decay                            |           |
| 109 | Sustain                          |           |
| 110 | Release                          |           |
| 111 | Arpeggio speed                   |           |

[^1]: Attack Pitch is how much to raise/lower the tone during the Attack/Hold phases.
