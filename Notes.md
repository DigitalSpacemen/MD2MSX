## Notes on specific 6-button Mega Drive / Genesis controllers

These are some quick notes on various controllers we tested.

- Cycle numbers in this document are 0-based.
- Reponse time: the time it takes the controller to update the output pins after the select pin is toggled.
- Reset time: the time the controller remains in cycle 2+ before falling back to cycle 0 or 1.

### SEGA Arcade Power Stick II

Response time: ~65ns

Reset time: ~1.4ms

### Unbranded "16-BIT" controller

Response time: ~125ns

Reset time: ~2.3ms

Deviations:
- Under certain conditions pressing Mode in-game makes it enter 3-button mode, while this should only happen on startup. We haven't gotten to the bottom of this yet.

### Retro-Bit Bluetooth Receiver

Response time: ~200ns

Reset time: ~0.1ms

Deviations:
- Extremely short reset time.

### Retro-Bit Big6

Response time: ~290ns

Reset time: ~0.1ms

Deviations:
- Extremely short reset time.

### 8BitDo Bluetooth Receiver

Reponse time: ~490ns, but only properly handles select toggles that are at least ~1.4µs apart.

Reset time: ~1.6ms

Deviations:
- 3.3V outputs instead of 5V.
- After cycle 7 it repeats all cycles instead of just cycles 0 and 1.
- After a reset with the select pin low (i.e. cycle 1 is entered), a select toggle will then make it enter cycle 0 instead of cycle 2.
- Buttons are latched on select toggle and are not updated otherwise.
- One additional button: Home (in cycle 7).

