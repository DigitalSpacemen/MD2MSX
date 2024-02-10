// Arduino program that toggles a pin to test controllers or the MD2MSX

// Connect 5V, GND to the controller
// Connect one Arduino pin to the controller Select pin
// Connect one controller output pin to an oscilloscope
// Optional: if the oscilloscope is dual-channel, connect
// the Select pin as well

// Requires digitalWriteFast
#include <digitalWriteFast.h>

// Which Arduino pin to use for Select
#define SELECT 2

// Delay between toggles. 35 = 2.9us, 47 = 3.9us
#define DELAY 35

// Enable when testing MD2MSX instead of a controller
//#define INVERT

// Enable to test behaviour for cycle >= 8
// If disabled, it stays in cycle 7 for 5ms to test reset behaviour
//#define LONG

#ifdef INVERT
#define HI LOW
#define LO HIGH
#else
#define HI HIGH
#define LO LOW
#endif

template <int N>
inline void delay_tick() {
  if (N > 1)
	delay_tick<N - 1>();

	__asm__ __volatile__ (
		"nop"
	);
}

template <>
void delay_tick<0>() {
}

void setup() {
	pinMode(SELECT, OUTPUT);
	digitalWriteFast(SELECT, HI);
}

void loop() {
	noInterrupts();
	digitalWriteFast(SELECT, LO);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, HI);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, LO);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, HI);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, LO);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, HI);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, LO);
	delay_tick<DELAY>();
#ifdef LONG
	digitalWriteFast(SELECT, HI);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, LO);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, HI);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, LO);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, HI);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, LO);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, HI);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, LO);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, HI);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, LO);
	delay_tick<DELAY>();
	digitalWriteFast(SELECT, HI);
	interrupts();
	delay(10);
#else
	interrupts();
	delay(5);
	digitalWriteFast(SELECT, HI);
	delay(5);
#endif
}
