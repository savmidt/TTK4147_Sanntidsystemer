#include <asf.h>
#include <board.h>
#include <gpio.h>
#include <sysclk.h>
#include "busy_delay.h"

#define CONFIG_USART_IF (AVR32_USART2)

// defines for BRTT interface
#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00


__attribute__((__interrupt__)) static void interrupt_J3(void);

void init(){
    sysclk_init();
    board_init();
    busy_delay_init(BOARD_OSC0_HZ);
    
    cpu_irq_disable();
    INTC_init_interrupts();
    INTC_register_interrupt(&interrupt_J3, AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);
    cpu_irq_enable();
    
    stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
        setbuf(stdout, NULL);
        setbuf(stdin,  NULL);
    #endif
}

/* Task A,B,C
__attribute__((__interrupt__)) static void interrupt_J3(void){ 
    if(gpio_get_pin_interrupt_flag(TEST_A)){
	    if(gpio_get_pin_value(TEST_A) == 0) gpio_set_pin_low(RESPONSE_A);
	    else                                gpio_set_pin_high(RESPONSE_A);
	    gpio_clear_pin_interrupt_flag(TEST_A);
    }
    if(gpio_get_pin_interrupt_flag(TEST_B)){
	    if(gpio_get_pin_value(TEST_B) == 0) {
			busy_delay_us(100); // For task D
			
			gpio_set_pin_low(RESPONSE_B);
		}
	    else gpio_set_pin_high(RESPONSE_B);
	    gpio_clear_pin_interrupt_flag(TEST_B);
    }
    if(gpio_get_pin_interrupt_flag(TEST_C)){
	    if(gpio_get_pin_value(TEST_C) == 0) gpio_set_pin_low(RESPONSE_C);
	    else                                gpio_set_pin_high(RESPONSE_C);
	    gpio_clear_pin_interrupt_flag(TEST_C);
    }
}
*/

// Task D, deferred handling
volatile int flag_A = 0;
volatile int flag_B = 0;
volatile int flag_C = 0;
#define EXPENSIVE_B 1

__attribute__((__interrupt__)) static void interrupt_J3(void){
	if(gpio_get_pin_interrupt_flag(TEST_A)){
		flag_A = 1;
		gpio_clear_pin_interrupt_flag(TEST_A);
	}
	if(gpio_get_pin_interrupt_flag(TEST_B)){
		flag_B = 1;
		gpio_clear_pin_interrupt_flag(TEST_B);
	}
	if(gpio_get_pin_interrupt_flag(TEST_C)){
		flag_C = 1;
		gpio_clear_pin_interrupt_flag(TEST_C);
	}
}

int main (void){
    init();
	gpio_configure_pin(TEST_C, GPIO_DIR_INPUT);
	gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(TEST_B, GPIO_DIR_INPUT);
	gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin(TEST_A, GPIO_DIR_INPUT);
	gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	
	//while(1){
	//    gpio_toggle_pin(LED0_GPIO);

	//    printf("tick\n");
	    
	//    busy_delay_ms(500);
	//}
		
	// task A (pin A) and task B(pins A, B and C)
    //while(1){
        //if(gpio_get_pin_value(TEST_C) == 0) gpio_set_pin_low(RESPONSE_C);
        //else gpio_set_pin_high(RESPONSE_C);

        // if(gpio_get_pin_value(TEST_B) == 0) gpio_set_pin_low(RESPONSE_B);
        // else gpio_set_pin_high(RESPONSE_B);

        //if(gpio_get_pin_value(TEST_A) == 0) gpio_set_pin_low(RESPONSE_A);
        //else gpio_set_pin_high(RESPONSE_A);
    //}
    // Task C and D
	gpio_enable_pin_interrupt(TEST_A, GPIO_PIN_CHANGE);
	gpio_enable_pin_interrupt(TEST_B, GPIO_PIN_CHANGE);
	gpio_enable_pin_interrupt(TEST_C, GPIO_PIN_CHANGE);
	while (1){
		// Do nothing for task C
		// For task D:
		if(flag_A){
			flag_A = 0;
			if(gpio_get_pin_value(TEST_A) == 0) gpio_set_pin_low(RESPONSE_A);
			else gpio_set_pin_high(RESPONSE_A);
		}
		if(flag_B){
			flag_B = 0;
			if(gpio_get_pin_value(TEST_B) == 0){
				#if EXPENSIVE_B
				busy_delay_us(100);   // expensive calculation for B
				#endif
				gpio_set_pin_low(RESPONSE_B);
			}
			else gpio_set_pin_high(RESPONSE_B);
		}
		if(flag_C){
			flag_C = 0;
			if(gpio_get_pin_value(TEST_C) == 0) gpio_set_pin_low(RESPONSE_C);
			else                                gpio_set_pin_high(RESPONSE_C);
		}
	}
	

}
