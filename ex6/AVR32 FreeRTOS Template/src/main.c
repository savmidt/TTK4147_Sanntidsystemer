#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gpio.h>
#include <power_clocks_lib.h>
#include <sysclk.h>
#include <compiler.h>
#include <board.h>
#include <stdio_usb.h>


#define CONFIG_USART_IF (AVR32_USART2)

#include "FreeRTOS.h"
#include "task.h"

#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00


void busy_delay_ms(int delay){
    for(; delay != 0; delay--){
        for(int i = 0; i < 2108; i++){
            asm volatile ("" ::: "memory");
        }
    }
}

void busy_delay_short(void){
    for(int i = 0; i < 10; i++){
        asm volatile ("" ::: "memory");
    }
}

void init(){
	board_init();
	
    gpio_configure_pin(TEST_A, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(TEST_B, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(TEST_C, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);

	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
	
	stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
	    setbuf(stdout, NULL);
	    setbuf(stdin,  NULL);
    #endif
}


static void taskFn(void* args){
	const portTickType delay = 1000 / portTICK_RATE_MS;
	
    int iter = 0;

	while(1){
		gpio_toggle_pin(LED0_GPIO);
		printf("tick %d\n", iter++);
		
		vTaskDelay(delay);
	}
}

// Task A start
void light_nr0(){
	const portTickType delay0 = 500 / portTICK_RATE_MS;
	
	int iter0 = 0;

	while(1){
		gpio_toggle_pin(LED0_GPIO);
		printf("tick0 %d\n", iter0++);
		
		vTaskDelay(delay0);
	}
}

void light_nr1(){
	const portTickType delay1 = 200 / portTICK_RATE_MS;
	
	int iter1 = 0;

	while(1){
		gpio_toggle_pin(LED1_GPIO);
		printf("tick1 %d\n", iter1++);
		
		vTaskDelay(delay1);
	}
}
// Task A stop
// Task B start
#define USE_YIELD 1
struct responseTaskArgs{
	struct  {
		uint32_t test;
		uint32_t response;
	} pin;
	uint32_t work_ms; // For task C
};

static void responseTask(void* args){
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	int responded = 0;
	
	while(1){
		if(gpio_get_pin_value(a.pin.test) == 0){
			if(!responded){
				gpio_set_pin_low(a.pin.response);
				responded = 1;
				#if USE_YIELD
					vTaskDelay(0);
				#endif
			}
		} else if(responded){
			gpio_set_pin_high(a.pin.response);
			responded = 0;
		}
		
	}
}
// Task B stop
// Task C start
static void responseTaskC(void* args){
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	int responded = 0;

	while(1){
		if(gpio_pin_is_low(a.pin.test)){
			if(!responded){
				busy_delay_ms(a.work_ms);
				gpio_set_pin_low(a.pin.response);
				responded = 1;
				#if USE_YIELD
					vTaskDelay(0);
				#endif
			}
			} else if(responded){
				gpio_set_pin_high(a.pin.response);
				responded = 0;
		}
	}
}
// Task C stop
// Task D start
static void responseTaskD(void* args){
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	int responded = 0;

	while(1){
		if(gpio_pin_is_low(a.pin.test)){
			if(!responded){
				busy_delay_ms(a.work_ms);
				gpio_set_pin_low(a.pin.response);
				responded = 1;
				#if USE_YIELD
					vTaskDelay(0);
				#endif
			}
			} else if(responded){
				gpio_set_pin_high(a.pin.response);
				responded = 0;
		} else {
			vTaskDelay(1);
		};
	}
}
// Task D stop
int main(){
	init();
    
	// Task A    
	//xTaskCreate(light_nr0, "", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
	//xTaskCreate(light_nr1, "", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);


	// Task B
	/*
	xTaskCreate(responseTask, "A", 1024,
		(&(struct responseTaskArgs){{TEST_A, RESPONSE_A}, 0}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTask, "B", 1024,
		(&(struct responseTaskArgs){{TEST_B, RESPONSE_B}, 0}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTask, "C", 1024,
		(&(struct responseTaskArgs){{TEST_C, RESPONSE_C}, 0}), tskIDLE_PRIORITY + 1, NULL);
	*/
	
	
	// Task C
	/*
	xTaskCreate(responseTaskC, "A", 1024,
		(&(struct responseTaskArgs){{TEST_A, RESPONSE_A}, 0}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTaskC, "B", 1024,
		(&(struct responseTaskArgs){{TEST_B, RESPONSE_B}, 0}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTaskC, "C", 1024,
		(&(struct responseTaskArgs){{TEST_C, RESPONSE_C}, 3}), tskIDLE_PRIORITY + 1, NULL);
	*/
			
	// Task D
	/*
	xTaskCreate(responseTaskD, "A", 1024,
		(&(struct responseTaskArgs){{TEST_A, RESPONSE_A}, 0}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTaskD, "B", 1024,
		(&(struct responseTaskArgs){{TEST_B, RESPONSE_B}, 0}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTaskD, "C", 1024,
		(&(struct responseTaskArgs){{TEST_C, RESPONSE_C}, 3}), tskIDLE_PRIORITY + 1, NULL);
	*/
	
	// Task E
	xTaskCreate(responseTaskD, "A", 1024,
		(&(struct responseTaskArgs){{TEST_A, RESPONSE_A}, 0}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTaskD, "B", 1024,
		(&(struct responseTaskArgs){{TEST_B, RESPONSE_B}, 0}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTaskD, "C", 1024,
		(&(struct responseTaskArgs){{TEST_C, RESPONSE_C}, 3}), tskIDLE_PRIORITY + 10, NULL);
	
	// Start the scheduler, anything after this will not run.
	vTaskStartScheduler();
    
}

