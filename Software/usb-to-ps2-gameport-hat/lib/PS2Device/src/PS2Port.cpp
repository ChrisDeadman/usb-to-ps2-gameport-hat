#include "PS2Port.h"

uint8_t CLOCK_PIN;
uint8_t DATA_PIN;
uint8_t STATUS_PIN;
PS2PortObserver* ps2PortObserver = NULL;

volatile int8_t platform_sub_clock = 0;
volatile bool platform_clock_enabled = false;
volatile bool platform_clock_inhibited = false;

// use TIMER/COUNTER#5
TcCount16* const TC = (TcCount16*)TC5;
const IRQn TCIRQn = TC5_IRQn;

void TC5_Handler() {
  // wait until host releases the clock
  if (platform_clock_inhibited) {
    if (digitalRead(CLOCK_PIN) == HIGH) {
      platform_clock_inhibited = false;
      digitalWrite(STATUS_PIN, HIGH);  // status pin
      // Host requests to send
      if (digitalRead(DATA_PIN) == LOW) {
        ps2PortObserver->onHostRts();
      }
      // According to http://www.burtonsys.com/ps2_chapweske.htm:
      // "The Clock line must be continuously high for at least 50 microseconds
      // before the device can begin to transmit its data."
      //
      // One sub-clock is 30-50us, so we have to wait an additional cycle.
      // If still not enough for your platform reduce it further.
      platform_sub_clock = -1;
    }
  } else {
    // Sub-Clock:
    // negative values (used for delay): HIGH
    // 0: HIGH
    // 1: HIGH -> ps2Clock() is called here
    // 2: LOW
    // 3: LOW
    uint8_t clk = (platform_sub_clock < 2) ? HIGH : LOW;
    digitalWrite(CLOCK_PIN, clk);

    // check if host inhibits clock
    if ((clk == HIGH) && (digitalRead(CLOCK_PIN) == LOW)) {
      platform_clock_inhibited = true;
      platform_sub_clock = 0;
      ps2PortObserver->onInhibit();
      digitalWrite(CLOCK_PIN, HIGH);   // release clock pin
      digitalWrite(DATA_PIN, HIGH);    // release data pin
      digitalWrite(STATUS_PIN, LOW);  // status pin
    }
    // generate clock
    else if (platform_clock_enabled) {
      if (platform_sub_clock == 1) {
        ps2PortObserver->onClock();
      }
      if (platform_sub_clock < 3) {
        ++platform_sub_clock;
      } else {
        platform_sub_clock = 0;
      }
    }
  }
  // set interrupt flag
  TC->INTFLAG.bit.MC0 = 1;
}

PS2Port::PS2Port(uint8_t clockPin, uint8_t dataPin, uint8_t statusPin) {
  CLOCK_PIN = clockPin;
  DATA_PIN = dataPin;
  STATUS_PIN = statusPin;
}

void PS2Port::init() {
  digitalWrite(CLOCK_PIN, HIGH);
  digitalWrite(DATA_PIN, HIGH);
  digitalWrite(STATUS_PIN, HIGH);

  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(STATUS_PIN, OUTPUT);

  // clock the TC with the core cpu clock (48MHz)
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
                      GCLK_CLKCTRL_ID(GCM_TC4_TC5);
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;  // wait for synchronization

  // reset TC
  TC->CTRLA.reg = TC_CTRLA_SWRST;
  while (TC->CTRLA.bit.SWRST)
    ;  // wait for synchronization

  TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;    // set TC Mode to 16 bits
  TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;    // set TC mode as match frequency
  TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1;  // set prescaler to 1
  TC->CTRLA.reg |= TC_CTRLA_ENABLE;          // enable TC
  // set counter to 15kHz*4 (4 sub-clocks needed)
  TC->CC[0].reg = (uint16_t)((SystemCoreClock / 15000 / 4) - 1);
  while (TC->STATUS.bit.SYNCBUSY)
    ;  // wait for synchronization

  // enable TC interrupt request
  NVIC_ClearPendingIRQ(TCIRQn);
  NVIC_SetPriority(TCIRQn, 0);
  enableClockIrq();
  while (TC->STATUS.bit.SYNCBUSY)
    ;  // wait for synchronization
}

void PS2Port::registerObserver(PS2PortObserver* const observer) {
  ps2PortObserver = observer;
}

void PS2Port::enableClock() {
  platform_sub_clock = 0;
  if (!platform_clock_enabled) {
    platform_clock_enabled = true;
    digitalWrite(STATUS_PIN, LOW);  // status LED
  }
}

void PS2Port::disableClock() {
  if (platform_clock_enabled) {
    platform_clock_enabled = false;
    platform_sub_clock = 0;
    digitalWrite(CLOCK_PIN, HIGH);  // release clock pin
    digitalWrite(DATA_PIN, HIGH);   // release data pin
    digitalWrite(STATUS_PIN, HIGH);  // status LED
  }
}

void PS2Port::enableClockIrq() {
  TC->INTENSET.bit.MC0 = 1;
  NVIC_EnableIRQ(TCIRQn);
  while (TC->STATUS.bit.SYNCBUSY)
    ;  // wait for synchronization
}

void PS2Port::disableClockIrq() {
  NVIC_DisableIRQ(TCIRQn);
  TC->INTENSET.bit.MC0 = 0;
  while (TC->STATUS.bit.SYNCBUSY)
    ;  // wait for synchronization
}

bool PS2Port::read() { return (digitalRead(DATA_PIN) == HIGH) ? true : false; }

void PS2Port::write(bool bit) {
  return digitalWrite(DATA_PIN, bit ? HIGH : LOW);
}
