#include "Watchdog.h"
#include <Arduino.h>

void Watchdog::init() {
  // use low-power 32.768kHz oscillator as clock source @ 1024Hz.
  GCLK->GENDIV.reg = GCLK_GENDIV_ID(GCM_FDPLL96M_32K) |
                     GCLK_GENDIV_DIV(4);  // use divisor of 32=2^(x+1)|x=4
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;  // wait for synchronization

  GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(GCM_FDPLL96M_32K) | GCLK_GENCTRL_GENEN |
                      GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_DIVSEL |
                      GCLK_GENCTRL_IDC;
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;  // wait for synchronization

  GCLK->CLKCTRL.reg =  // feed WDT with GCLK2
      GCLK_CLKCTRL_ID_WDT | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2;
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;  // wait for synchronization

  // disable early-warning interrupt
  NVIC_DisableIRQ(WDT_IRQn);
  NVIC_ClearPendingIRQ(WDT_IRQn);
  WDT->INTENSET.bit.EW = 0;

  // Set period to 8192=2^(x+3)|x=10|00<=x<=0B clock cycles (8s)
  WDT->CONFIG.bit.PER = 10;
  // disable windowed mode (also resets on underflow)
  WDT->CTRL.bit.WEN = 0;
  // start WDT
  WDT->CTRL.bit.ENABLE = 1;
  while (WDT->STATUS.bit.SYNCBUSY)
    ;  // wait for synchronization
}

void Watchdog::reset() {
  if (!WDT->STATUS.bit.SYNCBUSY) {
    WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
  }
}
