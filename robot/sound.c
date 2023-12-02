#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#define SOUND_SENDOR 0


int main() {

    int clap_count = 1;

    gpio_init(SOUND_SENSOR);
    gpio_set_dir(SOUND_SENSOR, GPIO_IN);

    while (true) {

      
      if (gpio_get(SOUND_SENSOR) == 1 && clap_count == 1) {
        clap_count = 2;
        // do something

      }
      else if (gpio_get(SOUND_SENSOR) == 1 && clap_count == 2) {
        clap_count = 1;
        //do something

      }

    }
}
