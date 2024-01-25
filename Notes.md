## Notes on specific 6-button Mega Drive / Genesis controllers

These are some quick notes on various controllers I tested.

Note: cycle numbers in this document are 0-based.

### SEGA Arcade Power Stick II

Response time: ~65ns

Reset time: ~1.4ms

### Unbranded "16-BIT" controller

Response time: ~125ns

Reset time: ~2.3ms

Deviations:
- Under certain conditions pressing Mode in-game makes it enter 3-button mode. I haven't gotten to the bottom of this yet.

### Retro-Bit Bluetooth Receiver

Response time: ~200ns

Reset time: ~0.1ms

### Retro-Bit Big6

Response time: ~290ns

Reset time: ~0.1ms

### 8BitDo Bluetooth Receiver

Reponse time: ~490ns

Reset time: ~1.6ms

Deviations:
- 3.3V outputs instead of 5V
- After cycle 7 it repeats all cycles instead of just cycles 0 and 1.
- After a reset with the select pin low (i.e. cycle 1 is entered), a select toggle will then make it enter cycle 0 instead of cycle 2.
- Buttons are latched on select toggle and are not updated otherwise.
- One additional button: Home (in cycle 7)
