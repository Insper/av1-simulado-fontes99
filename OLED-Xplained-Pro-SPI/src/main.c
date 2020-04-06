#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

// LED
#define LED_PIO       PIOC
#define LED_PIO_ID    ID_PIOC
#define LED_IDX       8
#define LED_IDX_MASK  (1 << LED_IDX)

// Botão
#define BUT_PIO       PIOA
#define BUT_PIO_ID    ID_PIOA
#define BUT_IDX       11
#define BUT_IDX_MASK  (1 << BUT_IDX)

// Botão 1
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

void but_callback1(void)
{
}

void but_callback2(void)
{
}

void but_callback3(void)
{
}

int init(){
		
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT);
	
	pmc_enable_periph_clk(LED1_PIO_ID);
	pio_configure(LED1_PIO, PIO_OUTPUT_1, LED1_IDX_MASK, PIO_DEFAULT);

	pmc_enable_periph_clk(LED2_PIO_ID);
	pio_configure(LED2_PIO, PIO_OUTPUT_1, LED2_IDX_MASK, PIO_DEFAULT);

	pmc_enable_periph_clk(LED3_PIO_ID);
	pio_configure(LED3_PIO, PIO_OUTPUT_1, LED3_IDX_MASK, PIO_DEFAULT);
	
	// Inicializa clock do periférico PIO responsavel pelo botao
	pmc_enable_periph_clk(BUT_PIO_ID);
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);

	// Configura PIO para lidar com o pino do botão como entrada
	// com pull-up
	pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_PIO, BUT_IDX_MASK, 50);

	pio_configure(BUT1_PIO, PIO_INPUT, BUT1_IDX_MASK, PIO_PULLUP);
	pio_configure(BUT2_PIO, PIO_INPUT, BUT2_IDX_MASK, PIO_PULLUP);
	pio_configure(BUT3_PIO, PIO_INPUT, BUT3_IDX_MASK, PIO_PULLUP);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	pio_handler_set(BUT1_PIO,
					BUT1_PIO_ID,
					BUT1_IDX_MASK,
					PIO_IT_RISE_EDGE,
					but_callback1);
					
	pio_handler_set(BUT2_PIO,
					BUT2_PIO_ID,
					BUT2_IDX_MASK,
					PIO_IT_RISE_EDGE,
					but_callback2);
	
	pio_handler_set(BUT3_PIO,
					BUT3_PIO_ID,
					BUT3_IDX_MASK,
					PIO_IT_RISE_EDGE,
					but_callback3);

	// Ativa interrupção
	pio_enable_interrupt(BUT_PIO, BUT_IDX_MASK);
	pio_enable_interrupt(BUT1_PIO, BUT1_IDX_MASK);
	pio_enable_interrupt(BUT2_PIO, BUT2_IDX_MASK);
	pio_enable_interrupt(BUT3_PIO, BUT3_IDX_MASK);

	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(BUT_PIO_ID);
	NVIC_SetPriority(BUT_PIO_ID, 4); // Prioridade 4
}

int main (void)
{
	// // Disativa WatchDog Timer
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	board_init();
	sysclk_init();
	delay_init();
	init();

  // Init OLED
	gfx_mono_ssd1306_init();
  
  // Escreve na tela um circulo e um texto
	gfx_mono_draw_filled_circle(20, 16, 16, GFX_PIXEL_SET, GFX_WHOLE);
  gfx_mono_draw_string("mundo", 50,16, &sysfont);

  /* Insert application code here, after the board has been initialized. */
	while(1) {
		

	}
}
