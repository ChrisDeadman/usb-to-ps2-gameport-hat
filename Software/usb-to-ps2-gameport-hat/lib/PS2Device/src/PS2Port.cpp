#include "PS2Port.h"

#include <assert.h>

#define MAX_PORTS 2

PS2Port* ports[MAX_PORTS];
uint8_t num_ports = 0;

// use TIMER/COUNTER#5
TcCount16* const TC = (TcCount16*)TC5;
const IRQn TCIRQn = TC5_IRQn;

void TC5_Handler() {
  for (uint8_t i = 0; i < num_ports; i++) {
    PS2Port* port = ports[i];

    // Ensure proper idle state
    if (!port->clock_enabled || port->clock_inhibited) {
      port->sub_clock = 0;
      digitalWrite(port->clock_pin, HIGH);
      digitalWrite(port->data_pin, HIGH);
    }

    // wait until host releases the clock
    if (port->clock_inhibited) {
      if (digitalRead(port->clock_pin) == HIGH) {
        port->clock_inhibited = false;
        // Host requests to send
        if (digitalRead(port->data_pin) == LOW) {
          port->on_host_rts();
        } else {
          // According to http://www.burtonsys.com/ps2_chapweske.htm:
          // "The Clock line must be continuously high for at least 50
          // microseconds before the device can begin to transmit its data."
          //
          // One sub-clock is 16.67us, so we have to wait an additional cycle.
          // Note that sub-clocks 0 and 1 are also HIGH.
          port->sub_clock = -1;
        }
      }
    } else {
      // Sub-Clock:
      // negative values (used for delay): HIGH
      // 0: HIGH
      // 1: HIGH -> on_clock() is called here
      // 2: LOW
      // 3: LOW
      uint8_t clk = (port->sub_clock < 2) ? HIGH : LOW;
      digitalWrite(port->clock_pin, clk);

      // check if host inhibits clock
      if ((clk == HIGH) && (digitalRead(port->clock_pin) == LOW)) {
        port->clock_inhibited = true;
        port->on_inhibit();
      }
      // generate clock
      else if (port->clock_enabled) {
        if (port->sub_clock == 1) {
          port->on_clock();
        }
        if (port->sub_clock < 3) {
          ++port->sub_clock;
        } else {
          port->sub_clock = 0;
        }
      }
    }
  }
  // set interrupt flag
  TC->INTFLAG.bit.MC0 = 1;
}

PS2Port::PS2Port(uint8_t clock_pin, uint8_t data_pin)
    : observer(NULL), clock_pin(clock_pin), data_pin(data_pin) {
  sub_clock = 0;
  clock_enabled = false;
  clock_inhibited = true;

  digitalWrite(clock_pin, HIGH);
  digitalWrite(data_pin, HIGH);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  assert(num_ports < MAX_PORTS);
  ports[num_ports] = this;
  ++num_ports;
}

void PS2Port::enable_clock() {
  sub_clock = 0;
  clock_enabled = true;
  digitalWrite(clock_pin, HIGH);  // release clock pin
  digitalWrite(data_pin, HIGH);   // release data pin
}

void PS2Port::disable_clock() {
  clock_enabled = false;
  digitalWrite(clock_pin, HIGH);  // release clock pin
  digitalWrite(data_pin, HIGH);   // release data pin
}

bool PS2Port::read() { return (digitalRead(data_pin) == HIGH) ? true : false; }

void PS2Port::write(bool bit) { return digitalWrite(data_pin, bit ? HIGH : LOW); }

void PS2Port::set_observer(PS2PortObserver* const observer) {
  this->observer = observer;
}

void PS2Port::on_clock() {
  if (observer != NULL) {
    observer->on_clock();
  }
}

void PS2Port::on_inhibit() {
  if (observer != NULL) {
    observer->on_inhibit();
  }
}

void PS2Port::on_host_rts() {
  if (observer != NULL) {
    observer->on_host_rts();
  }
}

void PS2Port::init() {
  // clock the TC with the core cpu clock (48MHz)
  GCLK->CLKCTRL.reg =
      GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5);
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
  enable_clock_irq();
  while (TC->STATUS.bit.SYNCBUSY)
    ;  // wait for synchronization
}

void PS2Port::enable_clock_irq() {
  TC->INTENSET.bit.MC0 = 1;
  NVIC_EnableIRQ(TCIRQn);
  while (TC->STATUS.bit.SYNCBUSY)
    ;  // wait for synchronization
}

void PS2Port::disable_clock_irq() {
  NVIC_DisableIRQ(TCIRQn);
  TC->INTENSET.bit.MC0 = 0;
  while (TC->STATUS.bit.SYNCBUSY)
    ;  // wait for synchronization
}
