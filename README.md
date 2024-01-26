## About MD2MSX

MD2MSX is a joystick adapter to connect a Mega Drive / Genesis controller to an MSX computer. It is compatible with the [JoyMega](https://frs.badcoffee.info/hardware/joymega-en.html) adapter.

This is a hardware project by retro computing enthusiasts. We are not experts. **USE AT YOUR OWN RISK!** Please make sure to read the [Usage Notes](#important-usage-notes) first.

![Photo of MD2MSX externals](https://github.com/DigitalSpacemen/MD2MSX/assets/615114/9a9770d4-a4a1-464c-bad5-fd5e394e6db7)

### Why another joystick adapter?

The JoyMega works great with official controllers. However, many 3rd-party controllers deviate quite a bit from the original protocol in various ways. Here's some examples of possible issues:

- 8BitDo wireless receivers working in 3-/6-button mode, but not working as a plain MSX joystick.
- RetroBit wired controllers and receivers not working in 6-button mode.
- Unbranded 3rd party controller switching to a permanent 3-button mode when the Mode button is pressed in-game.

These issues are not caused by the JoyMega adapter; they're a result of incompatibilities between MSX software and the controllers. Please check our [Controller Notes](Controller Notes.md) for more detailed information.

### How does MD2MSX work?

Our proposed solution places an Arduino inbetween the controller and the MSX. This Arduino robustly reads the controller on the input side. On the output side, it implements the controller protocol more accurately than the 3rd-party controllers. We designed a simple PCB for this purpose with two DB-9 (or DE-9) ports and header pins to plug in an Arduino Nano.

![Photo of MD2MSX internals](https://github.com/DigitalSpacemen/MD2MSX/assets/615114/11f8d075-2c05-4a96-969a-653f34b694bf)
*Revision 1.0 of the MD2MSX with a cable soldered directly to the board in place of the 2nd DB-9 port.*

### How do I make one?

Apart from the MD2MSX PCB you need the following components:

- 1x male DB-9 port (matching the footprint on the PCB).
- 1x female DB-9 port (matching the footprint on the PCB) **OR** a cable with a female DB-9 at the end, to solder directly to the board.
- 2x BAT85 DO-35 diodes. Other switching diodes should also work, but have not been tested.
- 2x 15-pin female header pins.
- 1x Arduino Nano (or clone) with male header pins.

For the hardware, the [KiCad](https://www.kicad.org/) design files can be found in the hardware/ directory. KiCad is used to produce Gerber files for upload to your favourite PCB manufacturer.

The firmware resides in firmware/ and is compiled with [PlatformIO](https://platformio.org/).

### Important usage notes

Do not connect the MD2MSX to both an MSX computer and USB at the same time, even if only one of the devices is powered on!

If you're planning to use a wireless receiver, the current draw may exceed the 50mA rating of the MSX joystick port. Make sure your particular MSX computer can handle this.

### Some things to consider if you decide to build one

If you build it without the cable directly attached, you would either need to obtain a straight-through DB-9 to DB-9 cable, or figure out a way to plug the board directly into the MSX computer. The latter may prove to be difficult.

Ensure the cable going to the MSX computer has a sufficient gauge for the current needed by the Arduino and the controller, especially if you're planning to use a wireless receiver. If you do end up using a cable with thin wires, we recommend making the cable as short as possible.
