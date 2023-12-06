#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/timer.h"

#define TRIG_PIN 18     // GPIO pin connected to the TRIG pin of the HC-SR04
#define ECHO_PIN 19     // GPIO pin connected to the ECHO pin of the HC-SR04
#define STOP_DISTANCE 10 // Distance in centimeters at which the robot should stop

const uint FWD_LEFT = 2;
const uint REV_LEFT = 3;
const uint FWD_RIGHT = 4;
const uint REV_RIGHT = 5;
const uint LED_PIN   = 25;

void initMotors() {
    gpio_init(FWD_LEFT);
    gpio_init(REV_LEFT);
    gpio_init(FWD_RIGHT);
    gpio_init(REV_RIGHT);

    gpio_set_dir(FWD_LEFT, GPIO_OUT);
    gpio_set_dir(REV_LEFT, GPIO_OUT);
    gpio_set_dir(FWD_RIGHT, GPIO_OUT);
    gpio_set_dir(REV_RIGHT, GPIO_OUT);

    gpio_put(FWD_LEFT, 0);
    gpio_put(REV_LEFT, 0);
    gpio_put(FWD_RIGHT, 0);
    gpio_put(REV_RIGHT, 0);
}

void startMotors() {
    // Assuming a simple forward motion
    gpio_put(FWD_LEFT, 1);
    gpio_put(REV_LEFT, 0);
    gpio_put(FWD_RIGHT, 1);
    gpio_put(REV_RIGHT, 0);
}

void stopMotors() {
    // Stop the motors by setting all control pins to 0
    gpio_put(FWD_LEFT, 0);
    gpio_put(REV_LEFT, 0);
    gpio_put(FWD_RIGHT, 0);
    gpio_put(REV_RIGHT, 0);

}
void initHC_SR04() {
    gpio_init(TRIG_PIN);
    gpio_init(ECHO_PIN);

    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    gpio_put(TRIG_PIN, 0);
    sleep_ms(2);
}

void triggerPulse() {
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);
}

uint32_t measureDistance() {
    // Wait for the ECHO_PIN to go high
    while (!gpio_get(ECHO_PIN))
        ;

    // Record the start time
    uint32_t start_time = time_us_32();

    // Wait for the ECHO_PIN to go low
    while (gpio_get(ECHO_PIN))
        ;

    // Record the end time
    uint32_t end_time = time_us_32();

    // Calculate the duration of the pulse in microseconds
    uint32_t pulse_duration = end_time - start_time;

    // Convert the duration to distance (in cm) using the speed of sound
    // (343 meters per second)
    uint32_t distance = pulse_duration * 0.0343 / 2;

    return distance;
}
int main() {
    stdio_init_all();
    initMotors();
    initHC_SR04();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1) {
        triggerPulse();
        uint32_t distance = measureDistance();

        printf("Distance: %u cm\n", distance);

        // Check if the robot is too close to an obstacle
        if (distance < STOP_DISTANCE) {
            // Stop the motors
            printf("Obstacle detected! Stopping the robot.\n");
            gpio_put(LED_PIN, 1);
            stopMotors();
        } else {
            // Start or continue moving
            gpio_put(LED_PIN, 0);
            startMotors();
        }

        sleep_ms(250);  // Add a delay between measurements
    }

    return 0;
}
