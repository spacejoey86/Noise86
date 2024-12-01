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

Video explaining how I use this module: https://www.youtube.com/watch?v=nMq6vZ9qzbw

## EyeSeq
Weighted sum of a clock divider

Each row has a knob to select the clock division, and then an offset which is added to the output when that row is active. It is active for half the division. Intended to be used as a pitch sequencer.

## MultiLogic
Three of the same logic gates. If you connect the top of the three inputs, it is an XOR gate. If you connect the bottom two inputs, it is an AND gate. Otherwise, it is an OR gate. There is an inverted output. If you connect a single input, the inverted output is a NOT gate and the regular output is a sample delay / mult

## SeqOfChanges
Markov sequencer where the probability of the next trigger is based on the previous three triggers (the 'state').
Each possible state has a knob to set the probability of a trigger, and a reset input that resets the module to start at that state.
There is also a CV output that outputs the probability of the current state. Try using it as a related modulation source to the trigger pattern
The state is ouput as three gates, and shown on LEDS.
Arrows on the module show possible state transitions, with a solid line representing an outputed trigger, and a broken line representing no trigger.

## RepeatBuddy
Counter and comparator utility module capable of adding step repeat to a sequencer, similar to the Metropolis/x and RYK M185.

Designed to be patch programmable, so you can use it for a wide variety of patches when paired with other modules.
### Counter
Count up with the Count trigger input.

The value of the counter is visualised with the green LEDs on the right
When it reaches the maximum value, it will stop increasing.

The output of the counter is available from the Count output, and has an attenuator.

Reset the counter with the reset input (normalised from the comparator output).
### Comparator
Compares the internal counter value (before attenuation) to the Threshold input.

The threshold input is visualised with the red LEDs on the left.

The output of the comparator is a gate, but with the normalisation to the counter reset, this becomes a shorter trigger.
### Divider
The divider outputs triggers according to a division of the main clock.

The divider is reset along with the counter.

There is a knob and cv input to control the division, and it is visualised with brighter and weaker LEDs on the left hand red LEDs.
### Patch ideas
Connect a clock to the Count input, and the comparator output to the clock input on your favourite sequencer with multiple CV lanes/channels. Patch one cv lane to your oscillator, and the other to the threshold input of RepeatBuddy.

Use the division output to trigger a voice, or use it as an accent for a voice triggered by a mult of the clock.

Use the counter output to modulate the clock source coming in to the RepeatBuddy.

Add step repeat to a turing machine, sample and hold, or other generative sequencer.

Use one sequencer to control the threshold, and another to control pitch.

Connect it to a module I have't heard about, have fun, and tell me all about it!
