/*
* ws2811.c - Simple 800kHz WS2811 output for the ESP8266
*
* This is taken from the ESP8266 support of Adafruit's NeoPixel library, 
* http://github.com/adafruit/Adafruit_NeoPixel. Nothing fancy, just 
* pump pixel data from our pre-existing buffer.
*
*/

#ifdef ESP8266

#include <Arduino.h>
#include <eagle_soc.h>

#define CYCLES_800_T0H  (F_CPU / 2500000) // 0.4us
#define CYCLES_800_T1H  (F_CPU / 1250000) // 0.8us
#define CYCLES_800      (F_CPU /  800000) // 1.25us per bit

static uint32_t _getCycleCount(void) __attribute__((always_inline));
static inline uint32_t _getCycleCount(void) {
    uint32_t ccount;
    __asm__ __volatile__("rsr %0,ccount":"=a" (ccount));
    return ccount;
}

/* Outpint pin, pointer to pixel buffer, and size of the buffer */
void ICACHE_RAM_ATTR do2811(uint8_t pin, uint8_t *pixels, uint32_t numBytes) {
  uint8_t *p, *end, pix, mask;
  uint32_t t, time0, time1, period, c, startTime, pinMask;

  pinMask   = _BV(pin);
  p         =  pixels;
  end       =  p + numBytes;
  pix       = *p++;
  mask      = 0x80;
  startTime = 0;

  time0  = CYCLES_800_T0H;
  time1  = CYCLES_800_T1H;
  period = CYCLES_800;

  for(t = time0;; t = time0) {
    if(pix & mask) t = time1;                             // Bit high duration
    while(((c = _getCycleCount()) - startTime) < period); // Wait for bit start
    GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, pinMask);       // Set high
    startTime = c;                                        // Save start time
    while(((c = _getCycleCount()) - startTime) < t);      // Wait high duration
    GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, pinMask);       // Set low
    if(!(mask >>= 1)) {                                   // Next bit/byte
      if(p >= end) break;
      pix  = *p++;
      mask = 0x80;
    }
  }
  while((_getCycleCount() - startTime) < period); // Wait for last bit
}

#endif
