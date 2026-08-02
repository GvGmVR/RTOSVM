// vm/ports/hal_esp32.cpp
// DEDICATED ESP32 / FreeRTOS HAL PORT
// Compile THIS file (and NOT hal_pc.cpp) when building for ESP32.

#include "headers/vm_hal.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"

void hal_pinMode(int pin, int mode) {
    gpio_reset_pin((gpio_num_t)pin);
    if (mode == HAL_PIN_OUTPUT) {
        gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT);
    } else {
        gpio_set_direction((gpio_num_t)pin, GPIO_MODE_INPUT);
    }
}

void hal_digitalWrite(int pin, int state) {
    // REAL ESP32 HARDWARE GPIO CONTROL
    gpio_set_level((gpio_num_t)pin, state == HAL_PIN_HIGH ? 1 : 0);
}

void hal_delayMs(int milliseconds) {
    // REAL FREERTOS TASK SLEEP (Yields CPU to other FreeRTOS tasks)
    vTaskDelay(pdMS_TO_TICKS(milliseconds));
}

void hal_log(const std::string& message) {
    // ESP32 Serial / UART Print
    printf("[ESP32 VM LOG]: %s\n", message.c_str());
}

uint32_t hal_getSystemTimeMs() {
    // Hardware timer: returns milliseconds since ESP32 boot
    return static_cast<uint32_t>(esp_timer_get_time() / 1000ULL);
}

void hal_yieldTask() {
    // Yields CPU slice in FreeRTOS
    vTaskDelay(1); 
}