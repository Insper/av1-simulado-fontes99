#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

// LED
#define LED_PIO       PIOC
#define LED_PIO_ID    ID_PIOC
#define LED_IDX       8
#define LED_IDX_MASK  (1 << LED_IDX)

// Bot�o
#define BUT_PIO       PIOA
#define BUT_PIO_ID    ID_PIOA
#define BUT_IDX       11
#define BUT_IDX_MASK  (1 << BUT_IDX)

// Bot�o 1
#define BUT1_PIO            PIOD
#define BUT1_PIO_ID         16
#define BUT1_IDX        28
#define BUT1_IDX_MASK   (1u << BUT1_PIO_IDX)

// =====================  BUT  =============================

#define BUT1_PIO             PIOD
#define BUT1_PIO_ID          16
#define BUT1_IDX         28
#define BUT1_IDX_MASK    (1u << BUT1_IDX)

#define BUT3_PIO             PIOA
#define BUT3_PIO_ID          10
#define BUT3_IDX         19
#define BUT3_IDX_MASK    (1u << BUT3_IDX)

#define BUT2_PIO             PIOC
#define BUT2_PIO_ID          12
#define BUT2_IDX         31
#define BUT2_IDX_MASK    (1u << BUT2_IDX)

// =====================  LED  =============================

// LED1 oled
#define LED1_PIO      PIOA
#define LED1_PIO_ID   ID_PIOA
#define LED1_IDX      0
#define LED1_IDX_MASK (1 << LED1_IDX)

// LED2 oled
#define LED2_PIO      PIOC
#define LED2_PIO_ID   ID_PIOC
#define LED2_IDX      30
#define LED2_IDX_MASK (1 << LED2_IDX)

// LED3 oled
#define LED3_PIO      PIOB
#define LED3_PIO_ID   ID_PIOB
#define LED3_IDX      2
#define LED3_IDX_MASK (1 << LED3_IDX)

//=================================================== FLAGs ===============================================================

volatile char flag_tc1 = 0;
volatile char flag_tc2 = 0;
volatile char flag_tc3 = 0;

volatile char flag_but1 = 0;
volatile char flag_but2 = 0;
volatile char flag_but3 = 0;


// =======================================================================================================================

void but_callback1(void)
{
	flag_but1 = !flag_but1;
}

void but_callback2(void)
{
	flag_but2 = !flag_but2;
}

void but_callback3(void)
{
	flag_but3 = !flag_but3;
}


//================================================== FUN��ES ==============================================================

void TC1_Handler(void){
	volatile uint32_t ul_dummy;

	/****************************************************************
	* Devemos indicar ao TC que a interrup��o foi satisfeita.
	******************************************************************/
	ul_dummy = tc_get_status(TC0, 1);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/** Muda o estado do LED */
	flag_tc1 = !flag_tc1;
}

void TC4_Handler(void){
	volatile uint32_t ul_dummy;

	/****************************************************************
	* Devemos indicar ao TC que a interrup��o foi satisfeita.
	******************************************************************/
	ul_dummy = tc_get_status(TC1, 1);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/** Muda o estado do LED */
	flag_tc2 = !flag_tc2;
}

void TC7_Handler(void){
	volatile uint32_t ul_dummy;

	/****************************************************************
	* Devemos indicar ao TC que a interrup��o foi satisfeita.
	******************************************************************/
	ul_dummy = tc_get_status(TC2, 1);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/** Muda o estado do LED */
	flag_tc3 = !flag_tc3;
}


//====================================================== INITs ============================================================

void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	/* Configura o PMC */
	/* O TimerCounter � meio confuso
	o uC possui 3 TCs, cada TC possui 3 canais
	TC0 : ID_TC0, ID_TC1, ID_TC2
	TC1 : ID_TC3, ID_TC4, ID_TC5
	TC2 : ID_TC6, ID_TC7, ID_TC8
	*/
	pmc_enable_periph_clk(ID_TC);

	/** Configura o TC para operar em  4Mhz e interrup�c�o no RC compare */
	tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

	/* Configura e ativa interrup�c�o no TC canal 0 */
	/* Interrup��o no C */
	NVIC_EnableIRQ((IRQn_Type) ID_TC);
	tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);

	/* Inicializa o canal 0 do TC */
	tc_start(TC, TC_CHANNEL);
}

int init(){
		
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT);
	
	pmc_enable_periph_clk(LED1_PIO_ID);
	pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_IDX_MASK, PIO_DEFAULT);

	pmc_enable_periph_clk(LED2_PIO_ID);
	pio_configure(LED2_PIO, PIO_OUTPUT_1, LED2_IDX_MASK, PIO_DEFAULT);

	pmc_enable_periph_clk(LED3_PIO_ID);
	pio_configure(LED3_PIO, PIO_OUTPUT_0, LED3_IDX_MASK, PIO_DEFAULT);
	
	// Inicializa clock do perif�rico PIO responsavel pelo botao
	pmc_enable_periph_clk(BUT_PIO_ID);
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);

	// Configura PIO para lidar com o pino do bot�o como entrada
	// com pull-up
	pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_PIO, BUT_IDX_MASK, 100);

	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT1_PIO, BUT1_IDX_MASK, 100);
	
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT2_PIO, BUT2_IDX_MASK, 100);
	
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT3_PIO, BUT3_IDX_MASK, 100);

	// Configura interrup��o no pino referente ao botao e associa
	// fun��o de callback caso uma interrup��o for gerada
	// a fun��o de callback � a: but_callback()
	pio_handler_set(BUT1_PIO,
					BUT1_PIO_ID,
					BUT1_IDX_MASK,
					PIO_IT_RISE_EDGE,
					but_callback1);
					
	pio_handler_set(BUT2_PIO,
					BUT2_PIO_ID,
					BUT2_IDX_MASK,
					PIO_IT_FALL_EDGE,
					but_callback2);
	
	pio_handler_set(BUT3_PIO,
					BUT3_PIO_ID,
					BUT3_IDX_MASK,
					PIO_IT_RISE_EDGE,
					but_callback3);

	// Ativa interrup��o
	pio_enable_interrupt(BUT_PIO, BUT_IDX_MASK);
	pio_enable_interrupt(BUT1_PIO, BUT1_IDX_MASK);
	pio_enable_interrupt(BUT2_PIO, BUT2_IDX_MASK);
	pio_enable_interrupt(BUT3_PIO, BUT3_IDX_MASK);

	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais pr�ximo de 0 maior)
	NVIC_EnableIRQ(BUT1_PIO_ID);
	NVIC_SetPriority(BUT1_PIO_ID, 4); // Prioridade 4

	NVIC_EnableIRQ(BUT2_PIO_ID);
	NVIC_SetPriority(BUT2_PIO_ID, 4); // Prioridade 4

	NVIC_EnableIRQ(BUT3_PIO_ID);
	NVIC_SetPriority(BUT3_PIO_ID, 4); // Prioridade 4
	
}
// ========================================================================================================================
int main (void)
{
	// // Disativa WatchDog Timer
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	board_init();
	sysclk_init();
	delay_init();
	init();
	
	TC_init(TC0, ID_TC1, 1, 5);
	TC_init(TC1, ID_TC4, 1, 10);
	TC_init(TC2, ID_TC7, 1, 1);

  // Init OLED
	gfx_mono_ssd1306_init();
  
  // Escreve na tela um circulo e um texto
  //gfx_mono_draw_filled_circle(20, 16, 16, GFX_PIXEL_SET, GFX_WHOLE);
  gfx_mono_draw_string("5Hz 10Hz 1Hz", 5,10, &sysfont);

  /* Insert application code here, after the board has been initialized. */
	while(1) {
		
		if(flag_but1){
			if(flag_tc1){
				pio_set(LED1_PIO, LED1_IDX_MASK);
			} 
			else pio_clear(LED1_PIO, LED1_IDX_MASK);
		}

		if(flag_but2){
			if(flag_tc2){
				pio_clear(LED2_PIO, LED2_IDX_MASK);
			}                                                                    // clear e set invertidos porcausa do valor inicial dos LEDs
			else pio_set(LED2_PIO, LED2_IDX_MASK);
		}

		if(flag_but3){
			if(flag_tc3){
				pio_set(LED3_PIO, LED3_IDX_MASK);
			} 
			else pio_clear(LED3_PIO, LED3_IDX_MASK);
		}
		
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
}
