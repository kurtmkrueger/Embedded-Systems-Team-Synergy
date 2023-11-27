#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

const uint FWD_LEFT = 10;
const uint REV_LEFT = 11;
const uint FWD_RIGHT = 12;
const uint REV_RIGHT = 13;

int main() {
	bi_decl(bi_program_description("This is a test binary."));
	bi_decl(bi_1pin_with_name(FWD_LEFT, "On-board LED"));

	stdio_init_all();

	gpio_init(FWD_LEFT);
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
		sleep_ms(250);
		gpio_put(FWD_LEFT, 1);
		gpio_put(FWD_RIGHT, 1);
		puts("Hello World\n");
		sleep_ms(250);
		gpio_put(FWD_LEFT, 0);
		gpio_put(FWD_RIGHT, 0);
		sleep_ms(250);
		gpio_put(REV_LEFT, 1);
		gpio_put(REV_RIGHT, 1);
		sleep_ms(250);
	}
}
