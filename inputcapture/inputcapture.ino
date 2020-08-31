/*
 * THIS ITERATION OF THE DMA CODE IS FROM MARTINL 
 * https://forum.arduino.cc/index.php?topic=396804.102
 * 
 * WIP reconfiguring being done by me
 * /


// Setup TCC0 to capture pulse-width and period with DMAC transfer
volatile uint32_t dmacPeriod;
volatile uint32_t dmacPulsewidth;

typedef struct                              // DMAC descriptor structure
{
  uint16_t btctrl;
  uint16_t btcnt;
  uint32_t srcaddr;
  uint32_t dstaddr;
  uint32_t descaddr;
} dmacdescriptor ;

volatile dmacdescriptor wrb[12] __attribute__ ((aligned (16)));               // Write-back DMAC descriptors
dmacdescriptor descriptor_section[12] __attribute__ ((aligned (16)));         // DMAC channel descriptors
dmacdescriptor descriptor __attribute__ ((aligned (16)));                     // Place holder descriptor

void setup()
{
  Serial.begin(115200);                  // Send data back on the Zero's native port
  while(!Serial);                        // Wait for the Serial port to be ready
 
  DMAC->BASEADDR.reg = (uint32_t)descriptor_section;                // Set the descriptor section base address
  DMAC->WRBADDR.reg = (uint32_t)wrb;                                // Set the write-back descriptor base adddress
  DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);      // Enable the DMAC and priority levels
 
  DMAC->CHID.reg = DMAC_CHID_ID(0);                                 // Select DMAC channel 0
  // Set DMAC channel 1 to priority level 0 (lowest), to trigger on TCC0 match compare 1 and to trigger every beat
  DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(0) | DMAC_CHCTRLB_TRIGSRC(TCC0_DMAC_ID_MC_0) | DMAC_CHCTRLB_TRIGACT_BEAT;
  //DMAC->CHINTENSET.reg = DMAC_CHINTENSET_MASK ;                   // Enable all DMAC interrupts
  descriptor.descaddr = (uint32_t)&descriptor_section[0];           // Set up a circular descriptor
  descriptor.srcaddr = (uint32_t)&TCC0->CC[0].reg;                  // Take the contents of the TCC0 counter comapare 0 register
  descriptor.dstaddr = (uint32_t)&dmacPeriod;                       // Copy it to the "dmacPeriod" variable
  descriptor.btcnt = 1;                                             // This takes one beat
  descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD | DMAC_BTCTRL_VALID;   // Copy 16-bits (HWORD) and flag the descriptor as valid
  memcpy(&descriptor_section[0], &descriptor, sizeof(dmacdescriptor));  // Copy to the channel 0 descriptor
 
  DMAC->CHID.reg = DMAC_CHID_ID(1);                                 // Select DMAC channel 1
  // Set DMAC channel 1 to priority level 0 (lowest), to trigger on TCC0 match compare 1 and to trigger every beat
  DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(0) | DMAC_CHCTRLB_TRIGSRC(TCC0_DMAC_ID_MC_1) | DMAC_CHCTRLB_TRIGACT_BEAT;
  //DMAC->CHINTENSET.reg = DMAC_CHINTENSET_MASK ;                   // Enable all DMAC interrupts
  descriptor.descaddr = (uint32_t)&descriptor_section[1];           // Set up a circular descriptor
  descriptor.srcaddr = (uint32_t)&TCC0->CC[1].reg;                  // Take the contents of the TCC0 counter comapare 1 register
  descriptor.dstaddr = (uint32_t)&dmacPulsewidth;                   // Copy it to the "dmacPulseWidth" variable
  descriptor.btcnt = 1;                                             // This takes 1 beat
  descriptor.btctrl = DMAC_BTCTRL_BEATSIZE_HWORD | DMAC_BTCTRL_VALID;    // Copy 16-bits (HWORD) and flag the descriptor as valid
  memcpy(&descriptor_section[1], &descriptor, sizeof(dmacdescriptor));   // Copy to the channel 1 descriptor
 
  REG_PM_APBCMASK |= PM_APBCMASK_EVSYS;     // Switch on the event system peripheral
 
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(1) |    // Divide the 48MHz system clock by 1 = 48MHz
                    GCLK_GENDIV_ID(5);      // Set division on Generic Clock Generator (GCLK) 5
  while (GCLK->STATUS.bit.SYNCBUSY);        // Wait for synchronization

  REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |           // Set the duty cycle to 50/50 HIGH/LOW
                     GCLK_GENCTRL_GENEN |         // Enable GCLK 5
                     GCLK_GENCTRL_SRC_DFLL48M |   // Set the clock source to 48MHz
                     GCLK_GENCTRL_ID(5);          // Set clock source on GCLK 5
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization*/

  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |         // Enable the generic clock...
                     GCLK_CLKCTRL_GEN_GCLK5 |     // ....on GCLK5
                     GCLK_CLKCTRL_ID_TCC0_TCC1;   // Feed the GCLK5 to TCC0 and TCC1
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  // Enable the port multiplexer on digital pin D12
  PORT->Group[g_APinDescription[12].ulPort].PINCFG[g_APinDescription[12].ulPin].bit.PMUXEN = 1;
  // Set-up the pin as an EIC (interrupt) peripheral on D12
  PORT->Group[g_APinDescription[12].ulPort].PMUX[g_APinDescription[12].ulPin >> 1].reg |= PORT_PMUX_PMUXO_A;

  //attachInterrupt(12, NULL, HIGH);                                        // Attach interrupts to digital pin 12 (external interrupt 3)
  REG_EIC_EVCTRL |= EIC_EVCTRL_EXTINTEO3;                                 // Enable event output on external interrupt 3
  REG_EIC_CONFIG0 |= EIC_CONFIG_SENSE3_HIGH;                              // Set event detecting a HIGH level
  REG_EIC_CTRL |= EIC_CTRL_ENABLE;                                        // Enable EIC peripheral
  while (EIC->STATUS.bit.SYNCBUSY);                                       // Wait for synchronization
 
  REG_EVSYS_USER = EVSYS_USER_CHANNEL(1) |                                // Attach the event user (receiver) to channel 0 (n + 1)
                   EVSYS_USER_USER(EVSYS_ID_USER_TCC0_EV_1);              // Set the event user (receiver) as timer TCC0, event 1

  REG_EVSYS_CHANNEL = EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT |                // No event edge detection
                      EVSYS_CHANNEL_PATH_ASYNCHRONOUS |                   // Set event path as asynchronous
                      EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_EIC_EXTINT_3) |    // Set event generator (sender) as external interrupt 3
                      EVSYS_CHANNEL_CHANNEL(0);                           // Attach the generator (sender) to channel 0

  REG_TCC0_EVCTRL |= TCC_EVCTRL_MCEI1 |           // Enable the match or capture channel 1 event input
                     TCC_EVCTRL_MCEI0 |           //.Enable the match or capture channel 0 event input
                     TCC_EVCTRL_TCEI1 |           // Enable the TCC event 1 input
                     /*TCC_EVCTRL_TCINV1 |*/      // Invert the event 1 input         
                     TCC_EVCTRL_EVACT1_PPW;       // Set up the timer for capture: CC0 period, CC1 pulsewidth                                   
 
  REG_TCC0_CTRLA |= TCC_CTRLA_CPTEN1 |              // Enable capture on CC1
                    TCC_CTRLA_CPTEN0 |              // Enable capture on CC0
                    TCC_CTRLA_PRESCALER_DIV1 |      // Set prescaler to 1, 48MHz/1 = 48MHz
                    TCC_CTRLA_ENABLE;               // Enable TCC0
  while (TCC0->SYNCBUSY.bit.ENABLE);                // Wait for synchronization

  DMAC->CHID.reg = DMAC_CHID_ID(0);                 // Select DMAC channel 0
  DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;         // Enable DMAC channel 0
  DMAC->CHID.reg = DMAC_CHID_ID(1);                 // Select DMAC channel 1
  DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;         // Enable DMAC channel 1
}

void loop()
{
  delay(100);
  Serial.print(dmacPeriod);                      // Output the results
  Serial.print(F("   "));
  Serial.println(dmacPulsewidth);
}
