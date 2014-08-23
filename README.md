LED_Cube_5x5x5_emulator
=======================

This is a Qt5/QML software emulator for a microcontroller controlled
5x5x5 LED cube.  The hardware this was based on consists of an
Atmega328P microcontroller with three 3 address to 8 pin decoders to
control 24 of the 25 vertical columns o f LEDs (the last is controlled
by a microcontroller pin without a decoder).

In addition to the current desktop Qt/QML, it was setup with the idea
of supporting the actual hardware microcontroller.  That would involve
having an alternative IO.cc that would call the Arduino IO routines
and an updated CubeControl.cc to run from the Arduino main loop.  No
guarantees it would work, or how easily it would work.
