#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "hardware/pwm.h"

const uint FWD_RIGHT = 2;
const uint REV_RIGHT = 3;
const uint FWD_LEFT = 4;
const uint REV_LEFT = 5;
const uint PWM_RIGHT = 6;
const uint PWM_LEFT = 7;

int main() {
	bi_decl(bi_program_description("This is a test binary."));
	bi_decl(bi_1pin_with_name(FWD_LEFT, "On-board LED"));
//	----------------------------------------------------------
//	i/o initialization...
	stdio_init_all();

	gpio_init(FWD_RIGHT);
	gpio_init(REV_RIGHT);
	gpio_init(FWD_LEFT);
	gpio_init(REV_LEFT);
	gpio_set_dir(FWD_LEFT, GPIO_OUT);
	gpio_set_dir(REV_LEFT, GPIO_OUT);
	gpio_set_dir(FWD_RIGHT, GPIO_OUT);
	gpio_set_dir(REV_RIGHT, GPIO_OUT);
//	----------------------------------------------------------
//	PWM initialization...
	gpio_set_function(PWM_LEFT, GPIO_FUNC_PWM);
	gpio_set_function(PWM_RIGHT, GPIO_FUNC_PWM);
	uint left_slice_num = pwm_gpio_to_slice_num(PWM_LEFT);
	uint right_slice_num = pwm_gpio_to_slice_num(PWM_RIGHT);
	uint left_channel_num = pwm_gpio_to_channel(PWM_LEFT);
	uint right_channel_num = pwm_gpio_to_channel(PWM_RIGHT);
//	This sets a PWM range from 0-255...	
	pwm_set_wrap(left_slice_num, 255);
	pwm_set_wrap(right_slice_num, 255);
	pwm_set_enabled(left_slice_num, true);
	pwm_set_enabled(right_slice_num, true);
//	set PWM level to be fixed at ~50% since 64 is between 0 and 255...
	pwm_set_chan_level(left_slice_num, left_channel_num, 128);
	pwm_set_chan_level(right_slice_num, right_channel_num, 128);
//	--------------------------------------------------------
//	begin the loop
	gpio_put(FWD_LEFT, 0);
	gpio_put(REV_LEFT, 0);
	gpio_put(FWD_RIGHT, 0);
	gpio_put(REV_RIGHT, 0);
	sleep_ms(500);
	gpio_put(FWD_LEFT, 1);
	gpio_put(FWD_RIGHT, 1);
	while(true) {
		pwm_set_chan_level(left_slice_num, left_channel_num, 80);
		sleep_ms(500);
		pwm_set_chan_level(right_slice_num, right_channel_num, 80);
		sleep_ms(2500);
		pwm_set_chan_level(left_slice_num, left_channel_num, 128);
		sleep_ms(500);
		pwm_set_chan_level(right_slice_num, right_channel_num, 128);
		sleep_ms(2500);
	}
}
