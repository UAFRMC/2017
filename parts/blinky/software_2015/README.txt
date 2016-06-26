Simple infrared blinking beacon.  Blink rate is fairly slow, 4 or 5 milliseconds per cycle, to allow software signal processing.

Signal recovery is using a complex-valued "phasor" system, where we scale the incoming amplitude by a time-varying complex helix.  Frequencies that don't match the helix spin rate have low resulting amplitudes.

