#include "headers/vm_hal.h"
#include <iostream>
#include <thread>
#include <chrono>

static auto startTimePoint = std::chrono::steady_clock::now();

void hal_pinMode(int pin, int mode) {
    std::cout << "[HAL DRIVER]: GPIO Pin " << pin 
              << " configured as " << (mode == HAL_PIN_OUTPUT ? "OUTPUT" : "INPUT") << "\n";
}

void hal_digitalWrite(int pin, int state) {
    std::cout << "[HAL DRIVER]: GPIO Pin " << pin 
              << " set to " << (state == HAL_PIN_HIGH ? "HIGH (ON)" : "LOW (OFF)") << "\n";
}

void hal_delayMs(int milliseconds) {
    std::cout << "[HAL DRIVER]: [TIMER] Sleeping for " << milliseconds << " ms...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void hal_log(const std::string& message) {
    std::cout << "[VM LOG]: " << message << "\n";
}

uint32_t hal_getSystemTimeMs() {
    auto now = std::chrono::steady_clock::now();
    return static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(now - startTimePoint).count()
    );
}

void hal_yieldTask() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}