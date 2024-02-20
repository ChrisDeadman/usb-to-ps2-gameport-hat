#include "PS2Port.h"

#include <assert.h>

#define MAX_PORTS 2

static PS2Port* volatile ports[MAX_PORTS];
static volatile uint8_t num_ports = 0;
static volatile uint8_t port_idx = 0;

// use TIMER/COUNTER#5
static volatile TcCount16* const TC = (TcCount16*)TC5;
static volatile const IRQn TCIRQn = TC5_IRQn;

void TC5_Handler() {
  // acknowledge the interrupt and re-arm
  TC->INTFLAG.bit.MC0 = 1;

  // service next port
  if (++port_idx >= MAX_PORTS) {
    port_idx = 0;
  }

  // ignore disabled ports
  if (port_idx >= num_ports) {
    return;
  }

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
    // 0: HIGH -> on_clock() is called here
    // 1: LOW
    uint8_t clk = (port->sub_clock < 1) ? HIGH : LOW;
    digitalWrite(port->clock_pin, clk);
    pinMode(port->clock_pin, clk ? INPUT_PULLUP : OUTPUT);

    // according to https://www.burtonsys.com/ps2_chapweske.htm:
    // The time from the rising edge of a clock pulse to a Data transition must
    // be at least 5 microseconds.
    if (clk == HIGH) {
      delayMicroseconds(2);  // adjust depending on platform
    }

    // check if host inhibits clock
    if ((clk == HIGH) && (digitalRead(port->clock_pin) == LOW)) {
      port->clock_inhibited = true;
      port->on_clock();
    } else if (port->clock_enabled) {
      if (++port->sub_clock > 1) port->sub_clock = 0;
      if (port->sub_clock == 1) port->on_clock();
    }
  }
}

PS2Port::PS2Port(uint8_t clock_pin, uint8_t data_pin)
    : observer(NULL), clock_pin(clock_pin), data_pin(data_pin) {
  clock_inhibited = false;

  disable_clock();

  assert(num_ports < MAX_PORTS);
  ports[num_ports] = this;
  ++num_ports;
}

void PS2Port::set_observer(PS2PortObserver* const observer) {
  this->observer = observer;
}

void PS2Port::enable_clock() {
  sub_clock = 0;
  clock_enabled = true;
}

void PS2Port::disable_clock() {
  sub_clock = 0;
  clock_enabled = false;
  // release pins
  digitalWrite(clock_pin, HIGH);
  digitalWrite(data_pin, HIGH);
  pinMode(clock_pin, INPUT_PULLUP);
  pinMode(data_pin, INPUT_PULLUP);
}

int PS2Port::read() { return digitalRead(data_pin); }

void PS2Port::write(uint32_t bit) {
  digitalWrite(data_pin, bit);
  pinMode(data_pin, bit ? INPUT_PULLUP : OUTPUT);
}

void PS2Port::on_clock() {
  if (observer != NULL) {
    observer->on_clock();
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
  // set counter to 10kHz * 2 sub-clocks per port
  TC->CC[0].reg = (uint16_t)((SystemCoreClock / 10000 / MAX_PORTS / 2) - 1);
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
