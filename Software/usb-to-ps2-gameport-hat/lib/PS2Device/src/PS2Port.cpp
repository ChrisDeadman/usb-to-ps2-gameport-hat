#include "PS2Port.h"

#include <assert.h>

#define MAX_PORTS 2

static PS2Port* volatile ports[MAX_PORTS];
static volatile uint8_t num_ports = 0;

// use TIMER/COUNTER#5
static volatile TcCount16* const TC = (TcCount16*)TC5;
static volatile const IRQn TCIRQn = TC5_IRQn;

void TC5_Handler() {
  for (uint8_t port_idx = 0; port_idx < num_ports; port_idx++) {
    PS2Port* port = ports[port_idx];

    // wait until host releases the clock
    if (port->clock_inhibited) {
      if (digitalRead(port->clock_pin) == HIGH) {
        port->clock_inhibited = false;
        // Host requests to send
        if (digitalRead(port->data_pin) == LOW) {
          port->on_host_rts();
        }
      }
    } else {
      // Sub-Clock:
      //<0: HIGH -> used for delay
      // 0: HIGH
      // 1: HIGH -> on_clock() is called here
      // 2: LOW
      // 3: LOW
      uint8_t clk = HIGH;
      if (port->clock_enabled) {
        switch (port->sub_clock++) {
          case 0:
            clk = HIGH;
            break;
          case 1:
            port->on_clock();
            clk = HIGH;
            break;
          case 2:
            clk = LOW;
            break;
          case 3:
            clk = LOW;
            port->sub_clock = 0;
            break;
        }
      }
      digitalWrite(port->clock_pin, clk);

      // check if host inhibits clock
      if ((clk == HIGH) && (digitalRead(port->clock_pin) == LOW)) {
        port->clock_inhibited = true;
        port->on_inhibit();
        digitalWrite(port->data_pin, HIGH);  // release data pin
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
  clock_inhibited = false;

  digitalWrite(clock_pin, HIGH);
  digitalWrite(data_pin, HIGH);
  pinMode(clock_pin, OUTPUT);
  pinMode(data_pin, OUTPUT);

  assert(num_ports < MAX_PORTS);
  ports[num_ports] = this;
  ++num_ports;
}

void PS2Port::set_observer(PS2PortObserver* const observer) {
  this->observer = observer;
}

volatile void PS2Port::enable_clock() {
  sub_clock = 0;
  clock_enabled = true;
}

volatile void PS2Port::disable_clock() {
  sub_clock = 0;
  clock_enabled = false;
  digitalWrite(data_pin, HIGH);  // release data pin
}

volatile bool PS2Port::read() {
  return (digitalRead(data_pin) == HIGH) ? true : false;
}

volatile void PS2Port::write(bool bit) { digitalWrite(data_pin, bit ? HIGH : LOW); }

volatile void PS2Port::on_clock() {
  if (observer != NULL) {
    observer->on_clock();
  }
}

volatile void PS2Port::on_inhibit() {
  if (observer != NULL) {
    observer->on_inhibit();
  }
}

volatile void PS2Port::on_host_rts() {
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
