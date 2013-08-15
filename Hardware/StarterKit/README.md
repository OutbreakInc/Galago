# Starter Kit

The Starter Kit is a bundle exclusive to the Kickstarter campaign that contains components required to build simple circuits around Galago.

[![Photo of the StarterKit](https://github.com/OutbreakInc/Galago/blob/master/Hardware/StarterKit/photos/StarterKit-components-small.jpg?raw=true)](https://github.com/OutbreakInc/Galago/blob/master/Hardware/StarterKit/photos/StarterKit-components.jpg?raw=true)

## Contents

- One solderless breadboard (30-row, two rail pairs)
- One [audio amplifier break-out board](#audio-amplifier-breakout)
- A serial-in, parallel-out shift register ([74HC595](http://www.nxp.com/documents/data_sheet/74HC_HCT595.pdf‎))
- Light sensor ([Vishay TEPT5600](http://www.vishay.com/docs/84768/tept5600.pdf‎))
- [Temperature sensor](#temperature-sensor) (Microchip MCP9700A)
- Two buttons
- A dozen jumper wires
- 8 LEDs, 10 resistors and 10 capacitors:
- 2 red 3mm LEDs
- 2 yellow 3mm LEDs
- 2 blue 3mm LEDs
- 2 white 3mm LEDs
- 4 100-ohm resistors
- 2 47-ohm resistors
- 2 1.5k-ohm resistors
- 2 1nF ceramic capacitors
- 4 100nF ceramic capacitors
- 2 47uF 16V electrolytic capacitors
- 2 10uF 50V electrolytic capacitor


## Audio Amplifier Breakout

The Audio Amplifier Breakout is a product unique to the Kickstarter campaign which contains a miniature 1.5W class-D audio amplifier with a differential analog input and built-in gain of 100x (40dB).

[![Photo of the Audio Amplifier Breakout](https://github.com/OutbreakInc/Galago/blob/master/Hardware/StarterKit/photos/AudioAmplifierBreakout-small.jpg?raw=true)](https://github.com/OutbreakInc/Galago/blob/master/Hardware/StarterKit/photos/AudioAmplifierBreakout-small.jpg?raw=true)
[![Photo of the Audio Amplifier Breakout](https://github.com/OutbreakInc/Galago/blob/master/Hardware/StarterKit/photos/AudioAmplifierBreakout-top-small.jpg?raw=true)](https://github.com/OutbreakInc/Galago/blob/master/Hardware/StarterKit/photos/AudioAmplifierBreakout-top-small.jpg?raw=true)

### Connecting

Connect **Vdd** to a 2.7V to 5V source (such as a battery that's also powering Galago) and **GND** to ground.  **O-** and **O+** are outputs - connect them directly to an 8-ohm speaker.  **ON** is a logic-level enable control - when logic '1', the speaker output is enabled, and when '0', it's off.  Finally, connect **IN+** and **IN-** to your input source either as a differential or single-ended signal connection (*see below*).

### Differential Input

To minimize noise in signal lines, the TPA2006D1 chip takes a differential input.  What this means is that it amplifies the **difference** between the **IN+** and **IN-** pins.  At any given time, the voltages of **IN+** and **IN-** must be between **GND** (0V) and **Vdd** (2.7V to 5V).

### Single-ended Input

If you don't have a differential signal to input, don't worry!  It's easy to convert *single-ended* signals (non-differential signals) by conencting the amplifier's **IN-** pin to **GND** with the shortest wire possible.  **IN+** is then your signal input,and it must be between **GND** and **Vdd** at all times.

### Impedance-matching

This is really important, and if you mismatch the input impedance, the sound will either be far too loud, garbled or distored (known as *clipping*) or too quiet.  The characteristic impedance of the amplifier is 150kOhm per differential input.  There's a 1.5kOhm 1% resistor on both **IN+** and **IN-**, giving the module a 100x gain (40dB), which makes it compatible out of the box with headphones, *'2Vpp'* line-out and other common audio sources with minimum input attenuation.  If you have a normalized audio signal centered at Vdd/2 and reaching to within say, 10% of the rails - such as from a DAC - then you can remove the built-in 100x gain by placing series 150kOhm 1% resistors on both **IN+** and **IN-**.  For single-ended signals, connect that 150kOhm resistor between **IN-** and **GND**.

## Light Sensor

We include a [Vishay TEPT5600](http://www.vishay.com/docs/84768/tept5600.pdf‎) ambient light sensor.  It looks like a large LED - a clear bullet-shaped blob of plastic with two legs protruding from the bottom.  In essence this component is a phototransistor, but optimized for the human-visible light spectrum.  If you look inside the clear plastic blob, there's a curved piece of metal and a straight one.  The straight portion is the collector pin and the curved one is the emitter pin.  Like a normal transistor, current flows from the collector to the emitter as a function of the base current - except in this device, the base is not a pin but instead the photons of light striking the semiconductor element.

[![Youtube video](https://github.com/OutbreakInc/Galago/blob/master/Hardware/StarterKit/photos/AmbientLightSensorYouTube.jpg?raw=true)](http://www.youtube.com/watch?v=RS-aaMn-FSI)

Please see the [instructional video on Youtube](http://www.youtube.com/watch?v=RS-aaMn-FSI) for further details on this component.

## Temperature Sensor

The temperature sensor we include is the [Microchip MCP9700A](ww1.microchip.com/downloads/en/DeviceDoc/21942e.pdf‎) in TO-92 packaging.  It looks like a little black plastic pellet with three legs.  By supplying the Vdd pin with 2.7V to 5V, you may sample a variable voltage on the Vout pin, which can be converted to a temperature as follows:

Temp (ºC) = (Vout - 500mV) / 10mV

By example:

- A Vout of 546mV represents (546mV - 500mV) / 10mV = 4.6ºC, close to the ideal temperature for serving beer.
- A Vout of 760mV represents (702mV - 500mV) / 10mV = 20.2ºC, a typical indoor room temperature.
- A Vout of 760mV represents (765mV - 500mV) / 10mV = 26.5ºC, a warm day's temperature. About the temperature at which dark chocolate melts.
