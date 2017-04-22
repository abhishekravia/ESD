/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "gpio_defs.h"
#include "LEDs.h"
#include "i2c.h"
#include "mma8451.h"
#include "LPTimer.h"
#include "Delay.h"

void Init_Accel(void) {
	Delay(50);
	if (!init_mma()) {												/* init mma peripheral */
		Control_RGB_LEDs(1, 0, 0);							/* light red error LED */
		while (1)																/* not able to initialize mma */
			;
	}
	Delay(50);
}

void Tilt( void ) {

	
	// Start LPTimer for future use
	Init_LPTMR(2);
	Start_LPTMR();
	__enable_irq();

	while (1) {
			__wfi() ;
	}
}

int main (void) {
	
	Init_RGB_LEDs();
	Control_RGB_LEDs(0, 0, 0);			// yellow: starting up 
	i2c_init();											// init i2c
	Init_Accel();										// init accelerometer
		// Allow low leakage stop mode
		SMC->PMPROT = SMC_PMPROT_ALLS_MASK; // 
		// Enable low-leakage stop mode and regular run mode
		SMC->PMCTRL = SMC_PMCTRL_STOPM(3) | SMC_PMCTRL_RUNM(0);
		SMC->STOPCTRL = SMC_STOPCTRL_PSTOPO(0) | SMC_STOPCTRL_VLLSM(3);
	
		// Enable LLWU
		// allow LPTMR0 to wake LLWU
		LLWU->ME |= LLWU_ME_WUME0_MASK;
	
	// Enable stop mode (deep sleep)
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	Tilt();
}
