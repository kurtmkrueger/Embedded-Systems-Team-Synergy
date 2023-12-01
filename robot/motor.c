#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "hardware/pwm.h"

const uint FWD_RIGHT = 2;
const uint REV_RIGHT = 3;
const uint FWD_LEFT = 4;
const uint REV_LEFT = 5;

int main() {
	bi_decl(bi_program_description("This is a test binary."));
	bi_decl(bi_1pin_with_name(FWD_LEFT, "On-board LED"));

	stdio_init_all();

	gpio_init(FWD_LEFT);
	//------------------------------------------
	gpio_set_function(FWD_LEFT, GPIO_FUNC_PWM); // PWM stuff...
	uint slice_num = pwm_gpio_to_slice_num(FWD_LEFT);
	uint channel_num = pwm_gpio_to_channel(FWD_LEFT);
	pwm_set_enabled(slice_num, 1);
	//------------------------------------------
	gpio_init(REV_LEFT);
	gpio_init(FWD_RIGHT);
	gpio_init(REV_RIGHT);
	gpio_set_dir(FWD_LEFT, GPIO_OUT);
	gpio_set_dir(REV_LEFT, GPIO_OUT);
	gpio_set_dir(FWD_RIGHT, GPIO_OUT);
	gpio_set_dir(REV_RIGHT, GPIO_OUT);
	while(1) {
		gpio_put(FWD_LEFT, 0);
		gpio_put(REV_LEFT, 0);
		gpio_put(FWD_RIGHT, 0);
		gpio_put(REV_RIGHT, 0);
		sleep_ms(500);
/*		gpio_put(FWD_LEFT, 1);
		gpio_put(FWD_RIGHT, 1);
		puts("Hello World\n");*/
		pwm_set_gpio_level(FWD_LEFT, 2^8);
		gpio_put(FWD_RIGHT, 1);
		sleep_ms(500);
	}
}
