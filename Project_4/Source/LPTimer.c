#include "LPTimer.h"
#include "MKL25Z4.h"
#include "LEDs.h"
#include "GPIO_defs.h"
#include "Delay.h"
#include "mma8451.h"
#include <math.h>
volatile int32_t LPT_ticks=0;

void Init_LPTMR(uint32_t freq) {
	SIM->SCGC5 |=  SIM_SCGC5_LPTMR_MASK;

	// Configure LPTMR
	// select 1 kHz LPO clock with prescale factor 0, dividing clock by 2
	// resulting in 500 Hz clock
	LPTMR0->PSR = /* LPTMR_PSR_PBYP_MASK | */ LPTMR_PSR_PCS(1) | LPTMR_PSR_PRESCALE(0); 
	LPTMR0->CSR = LPTMR_CSR_TIE_MASK;
	LPTMR0->CMR = (FREQ_LPO/(2*freq))-1; // Period - 1

#if 1
	// Configure NVIC 
	NVIC_SetPriority(LPTimer_IRQn, 3); 
	NVIC_ClearPendingIRQ(LPTimer_IRQn); 
	NVIC_EnableIRQ(LPTimer_IRQn);	
#endif
}

void Start_LPTMR(void) {
	LPTMR0->CSR |= LPTMR_CSR_TEN_MASK;
}

void Stop_LPTMR(void) {
	LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
}

	int16_t accel[3];
	float roll, pitch;
void LPTimer_IRQHandler(void) {
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;	
	LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;

		LPT_ticks++;
		read_full_xyz(accel);
		convert_xyz_to_roll_pitch(accel, &roll, &pitch);
		
		if ((fabs(roll) > 30) || (fabs(pitch) > 30))
		Control_RGB_LEDs(1, 0, 0);
		else if ((fabs(roll) > 15) || (fabs(pitch) > 15))
		Control_RGB_LEDs(1, 1, 0);
		else
		Control_RGB_LEDs(0, 1, 0);
      //Delay(1);
		ShortDelay(50);
			Control_RGB_LEDs(0, 0, 0);

			//Delay(1);
			SIM->SCGC5 &= ~SIM_SCGC5_PORTB_MASK; 
}