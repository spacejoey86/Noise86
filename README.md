# Noise86
A plugin for VCV rack
## TransQuant
Quantiser with triggers to transpose by a CV controleable interval

Each channel has an input, a trigger output when the note changes, and a quantised output.
The input is first transposed by the sum of the CV input, the transpose knob and the internal transposition value.
  (The octave is preserved by transposing up or down depending which is closer)
It is then quantized to the chosen scale.

The scale input is truncated and then looked up in this table:

| Input | Scale        |
| ----- | ------------ |
| 0     | Chromatic    |
| 1     | Blues        |
| 2     | Major/Ionian |
| 3     | Dorian       |
| 4     | Phrygian     |
| 5     | Lydian       |
| 6     | Mixolydian   |
| 7     | Minor/Aolian |
| 8     | Locrian      |

## MarkovDrummer
Probablistic trigger sequencer based of Markov chains

8 steps. Odd steps always move to the next step.
Even steps have a knob for the probability to move to each of the odd steps. If all of the knobs are turned to zero, it will repeat the current step.

After a reset trigger, the next clock will cause the stage 1 output

## EyeSeq
Weighted sum of a clock divider

Each row has a knob to select the clock division, and then an offset which is added to the output when that row is active. It is active for half the division. Intended to be used as a pitch sequencer.

## MultiLogic
Three of the same logic gates. If you connect the top of the three inputs, it is an XOR gate. If you connect the bottom two inputs, it is an AND gate. Otherwise, it is an OR gate. There is an inverted output. If you connect a single input, the inverted output is a NOT gate and the regular output is a sample delay

## SeqOfChanges (New)
Markov sequencer where the probability of the next trigger is based on the previous three triggers (the 'state').
Each possible state has a knob to set the probability of a trigger, and a reset input that resets the module to start at that state.
There is also a CV output that outputs the probability of the current state. Try using it as a related modulation source to the trigger pattern
The state is ouput as three gates, and shown on LEDS.
Arrows on the module show possible state transitions, with a solid line representing an outputed trigger, and a broken line representing no trigger.
