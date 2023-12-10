#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/dma.h"
#include "hardware/pwm.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#define TRIG_PIN 18     // GPIO pin connected to the TRIG pin of the HC-SR04
#define ECHO_PIN 19     // GPIO pin connected to the ECHO pin of the HC-SR04
#define STOP_DISTANCE 10 // Distance in centimeters at which the robot should stop
#define SOUND_SENSOR 0
// GLOBALS
const uint FWD_RIGHT = 6;
const uint REV_RIGHT = 7;
const uint FWD_LEFT = 8;
const uint REV_LEFT = 9;
const uint PWM_RIGHT = 10;
const uint PWM_LEFT = 11;
const uint LED_PIN   = 15;


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

void initSoundSensor() {
    gpio_init(SOUND_SENSOR);
    gpio_set_dir(SOUND_SENSOR, GPIO_IN);
}

void startMotors() {
    // Assuming a simple forward motion
    gpio_put(FWD_LEFT, 1);
    gpio_put(REV_LEFT, 0);
    gpio_put(FWD_RIGHT, 1);
    gpio_put(REV_RIGHT, 0);
}

void reverseMotors() {
    // Assuming a simple reverse motion
    gpio_put(FWD_LEFT, 0);
    gpio_put(REV_LEFT, 1);
    gpio_put(FWD_RIGHT, 0);
    gpio_put(REV_RIGHT, 1);
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


// Gyroscope code from pico-examples...........................................
// By default these devices  are on bus address 0x68
static int addr = 0x68;

#ifdef i2c_default
static void mpu6050_reset() {
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {0x6B, 0x80};
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
}

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c_default, addr, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++) {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    val = 0x43;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);  // False - finished with bus

    for (int i = 0; i < 3; i++) {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);;
    }

    // Now temperature from reg 0x41 for 2 bytes
    // The register is auto incrementing on each read
    val = 0x41;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 2, false);  // False - finished with bus

    *temp = buffer[0] << 8 | buffer[1];
}
#endif // ........................................................................



int main() {
    stdio_init_all();
    initMotors();
    initHC_SR04();
    initSoundSensor();

#if !defined(i2c_default) || !defined(PICO_DEFAULT_I2C_SDA_PIN) || !defined(PICO_DEFAULT_I2C_SCL_PIN)
    #warning i2c/mpu6050_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
    return 0;
#else
    printf("Hello, MPU6050! Reading raw data from registers...\n");

    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    mpu6050_reset();

    int16_t acceleration[3], gyro[3], temp;
#endif

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    // Initialize pwm stuff
    gpio_set_function(PWM_LEFT, GPIO_FUNC_PWM);
	gpio_set_function(PWM_RIGHT, GPIO_FUNC_PWM);
    uint left_slice_num = pwm_gpio_to_slice_num(PWM_LEFT);
    uint right_slice_num = pwm_gpio_to_slice_num(PWM_RIGHT);
    uint left_channel_num = pwm_gpio_to_channel(PWM_LEFT);
    uint right_channel_num = pwm_gpio_to_channel(PWM_RIGHT);
    // This sets a PWM range from 0-255...	
	pwm_set_wrap(left_slice_num, 255);
	pwm_set_wrap(right_slice_num, 255);
	pwm_set_enabled(left_slice_num, true);
	pwm_set_enabled(right_slice_num, true);
    // Set initial PWM level to be fixed at ~50% since 128 is between 0 and 255...
	pwm_set_chan_level(left_slice_num, left_channel_num, 128);
	pwm_set_chan_level(right_slice_num, right_channel_num, 128);


    int clap_count = 1;

    while (true) {
        triggerPulse();
        uint32_t distance = measureDistance();
        mpu6050_read_raw(acceleration, gyro, &temp);
        printf("Distance: %u cm\n", distance);

        // Check if the robot is too close to an obstacle
        if (distance < STOP_DISTANCE) {
            // Stop the motors
            printf("Obstacle detected! Stopping the robot.\n");
            gpio_put(LED_PIN, 1);
            stopMotors();
        }
        else if (acceleration[2] < 0) {
            gpio_put(LED_PIN, 1);
            sleep_ms(50);
            gpio_put(LED_PIN, 0);
            stopMotors();
        }
        else {
            // Start or continue moving
            gpio_put(LED_PIN, 0);
            // Check the sound sensor for a clap
            if (gpio_get(SOUND_SENSOR) == 1) {
                if (clap_count == 1) {
                    clap_count = 2;
                    pwm_set_chan_level(left_slice_num, left_channel_num, 255);
                    pwm_set_chan_level(right_slice_num, right_channel_num, 255);
                    startMotors();
                }
                else if (clap_count == 2) {
                    clap_count = 3;
                    stopMotors();
                }
                else if (clap_count == 3) {
                    clap_count = 4;
                    pwm_set_chan_level(left_slice_num, left_channel_num, 127);
                    pwm_set_chan_level(right_slice_num, right_channel_num, 127);
                    reverseMotors();
                }
                else {
                    clap_count = 1;
                    stopMotors();
                }
            }
        }
        // Add a delay between measurements
        sleep_ms(100);
    }
    return 0;
}
