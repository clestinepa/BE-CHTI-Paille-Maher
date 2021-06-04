#include "DriverJeuLaser.h"
#include <stdio.h>
#include "GestionSon.h"
#include "Affichage_Valise.h"

int DFT_ModuleAuCarre( short int * Signal64ech, char k) ;

int signal_recu[64];
short int dma_buf[64];

int score[6];
int compteur_temps = 0 ;
const int Timout = 20 ; //pour éviter de compter plusieurs fois le même score, on met en place des salves de tirs


void callback_Systick() {
	Start_DMA1(64);
	Wait_On_End_Of_DMA1();
	
	compteur_temps = (compteur_temps+1) %Timout ; 
	for (int k=0 ; k<64; k++){
		if (compteur_temps!=0) break; //attendre la salve suivante pour pouvoir gagner des points
		signal_recu[k] = DFT_ModuleAuCarre(dma_buf,k);
	}
	
	//on regarde qui a marqué le point
	//pas très optimal comme code, mais on a plus le temps...
	if (signal_recu[17]>0x10000){
		score[0]++;
		Prepare_Afficheur(1, score[0]);
		StartSon();
	}
	if (signal_recu[18]>0x10000){
		score[1]++;
		Prepare_Afficheur(2, score[1]);
		StartSon();
	}
	if (signal_recu[19]>0x10000){
		score[2]++;
		Prepare_Afficheur(3, score[3]);
		StartSon();
	}
	if (signal_recu[20]>0x10000){
		score[3]++;
		Prepare_Afficheur(4, score[3]);
		StartSon();
	}
	if (signal_recu[23]>0x10000){
		score[4]++; //pas d'affichage, la boite ne prévoit que 4 joueurs
		StartSon();
	}
	if (signal_recu[24]>0x10000){
		score[5]++;//pas d'affichage, la boite ne prévoit que 4 joueurs
		StartSon();
	}
	Mise_A_Jour_Afficheurs_LED();
	Stop_DMA1;

}


int main(void)
{

// ===========================================================================
// ============= INIT PERIPH (faites qu'une seule fois)  =====================
// ===========================================================================

// Après exécution : le coeur CPU est clocké à 72MHz ainsi que tous les timers
	CLOCK_Configure();
	
	Timer_1234_Init_ff( TIM4, 6552 ) ; //91 µs * 72 MHz = fréquence timer
	Active_IT_Debordement_Timer( TIM4, 2, CallbackSon );
	Run_Timer( TIM4 );
	GPIO_Configure(GPIOB, 1, OUTPUT, OUTPUT_PPULL);
  GPIO_Configure(GPIOB, 0, OUTPUT, ALT_PPULL);
	PWM_Init_ff( TIM3, 3, 720); 
	
	//A FAIRE : TIM1 pour l'affichage
	
	Systick_Period_ff( 360000 ); //1/72MHz * x = 5ms donc x =360000
	Systick_Prio_IT(2, callback_Systick);
	SysTick_On ;
	SysTick_Enable_IT ;
	
	Init_TimingADC_ActiveADC_ff( ADC1, 72 );
	Single_Channel_ADC( ADC1, 2 );
	Init_Conversion_On_Trig_Timer_ff( ADC1, TIM2_CC2, 225 );
	Init_ADC1_DMA1( 0, dma_buf );
	
	Init_Affichage();
	for (int i=1; i<5; i++){
		Prepare_Afficheur(i,0);
	}
	Mise_A_Jour_Afficheurs_LED();
	
	for (int i=0; i>5 ;i++) score[i]=0;

//============================================================================	

while	(1)
	{
		
	}
}

