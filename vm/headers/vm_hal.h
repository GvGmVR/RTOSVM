#ifndef VM_HAL_H
#define VM_HAL_H

#include <string>
#include <cstdint>

// Hardware Pin Direction Constants
#define HAL_PIN_INPUT  0
#define HAL_PIN_OUTPUT 1

// Hardware Pin Voltage Level Constants
#define HAL_PIN_LOW    0
#define HAL_PIN_HIGH   1

// Hardware Abstraction Layer Interface
void hal_pinMode(int pin, int mode);
void hal_digitalWrite(int pin, int state);
void hal_delayMs(int milliseconds);
void hal_log(const std::string& message);

// Added for Polymorphic Non-Blocking FSM Evaluation & Task Scheduling
uint32_t hal_getSystemTimeMs();
void hal_yieldTask();

#endif // VM_HAL_H