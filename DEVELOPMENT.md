# SEIN-IoT Development Guidelines

## Code Style

### C Code Standards
- **Standard**: MISRA C-2012 for embedded systems
- **Indentation**: 4 spaces (no tabs)
- **Line length**: Max 100 characters
- **Comments**: Doxygen-compatible documentation

### File Headers
Every .c and .h file must start with:
```c
/**
 * @file filename.c
 * @brief One-line description of the file
 * 
 * More detailed description...
 */
```

### Function Documentation
```c
/**
 * Brief description of function
 * 
 * Longer description explaining what the function
 * does and when to use it.
 * 
 * @param param1    Description of param1
 * @param param2    Description of param2
 * @return          Description of return value
 */
bool my_function(int param1, const char *param2);
```

### Variable Naming
- `local_variable`: Local variables (snake_case)
- `DEFINE_CONSTANT`: Macros and constants (UPPER_SNAKE_CASE)
- `my_function()`: Functions (lower_snake_case)
- `struct_name_t`: Typedef structures with `_t` suffix
- `enum_value_t`: Enum types with `_t` suffix

### Type Definitions
```c
typedef enum {
    VALUE_ONE = 0,
    VALUE_TWO = 1,
} my_enum_t;

typedef struct {
    uint8_t field1;
    uint16_t field2;
} my_struct_t;
```

## Memory Management

### Stack Allocation
```c
// Good: Fixed-size buffer
uint8_t buffer[256];

// Avoid: Variable-length arrays in production
uint8_t buffer[size];  // Problematic if size is large
```

### Heap Allocation
```c
// Avoid on critical paths:
uint8_t *ptr = malloc(size);  // Can fail silently

// Use pre-allocated pools instead:
static uint8_t buffer_pool[NUM_BUFFERS][BUFFER_SIZE];
```

### PSRAM Usage
```c
// For large data structures:
uint8_t *large_buffer = (uint8_t *)heap_caps_malloc(
    size,
    MALLOC_CAP_SPIRAM
);
```

## Error Handling

### Return Codes
```c
// Preferred for critical functions:
bool function_that_can_fail(void) {
    if (validation_failed()) {
        ESP_LOGE(TAG, "Validation failed");
        return false;
    }
    return true;
}

// For operations with multiple error states:
typedef enum {
    OK = 0,
    ERROR_INVALID_PARAM = 1,
    ERROR_TIMEOUT = 2,
    ERROR_HW_FAULT = 3,
} error_code_t;
```

### NULL Pointer Checks
```c
if (pointer == NULL) {
    ESP_LOGE(TAG, "Null pointer");
    return false;
}

// Or with assert:
assert(pointer != NULL);
```

## Logging

### ESP_LOG Usage
```c
static const char *TAG = "COMPONENT_NAME";

ESP_LOGE(TAG, "Error message");        // Errors
ESP_LOGW(TAG, "Warning message");      // Warnings
ESP_LOGI(TAG, "Information message");  // Info
ESP_LOGD(TAG, "Debug message");        // Debug
ESP_LOGV(TAG, "Verbose message");      // Verbose
```

### Conditional Logging
```c
if (DEBUG_ENABLE) {
    ESP_LOGD(TAG, "Debug: var1=%d, var2=%p", var1, var2);
}
```

## Testing

### Unit Test Template
```c
bool test_my_function(void) {
    // Arrange
    int input = 42;
    int expected = 84;
    
    // Act
    int result = my_function(input);
    
    // Assert
    if (result != expected) {
        ESP_LOGE(TAG, "Test failed: expected %d, got %d", 
                 expected, result);
        return false;
    }
    
    return true;
}
```

## Performance Optimization

### Timing-Critical Code
```c
// For sub-millisecond timing:
uint32_t start_time = esp_timer_get_time();  // µs resolution
// ... code to measure ...
uint32_t elapsed = esp_timer_get_time() - start_time;
```

### Memory Profiling
```bash
idf.py size                # Show binary size breakdown
idf.py monitor             # Watch heap usage in real-time
```

### CPU Load
Use FreeRTOS task profiling:
```c
TaskStatus_t *pxTaskStatusArray;
pxTaskStatusArray = pvPortMalloc(uxTaskGetNumberOfTasks() * 
                                 sizeof(TaskStatus_t));
uxTaskGetSystemState(pxTaskStatusArray, uxTaskGetNumberOfTasks(), NULL);
```

## Security Practices

### Data Handling
```c
// Clear sensitive data before deallocation
memset(sensitive_buffer, 0, sizeof(sensitive_buffer));

// Use constant-time comparisons for crypto:
int crc_verify = crypto_constant_time_compare(
    computed_crc, expected_crc, sizeof(uint16_t)
);
```

### Input Validation
```c
// Always validate external input
bool process_external_data(const uint8_t *data, size_t len) {
    if (data == NULL || len == 0 || len > MAX_SIZE) {
        return false;
    }
    // ... process data ...
    return true;
}
```

## Git Workflow

### Commit Messages
```
[COMPONENT] Brief description (50 chars max)

Longer description explaining the changes,
motivation, and any related issues.

Fixes #123
```

### Branch Naming
- `feature/fpga-acceleration` - New features
- `bugfix/crc-calculation` - Bug fixes
- `docs/api-documentation` - Documentation
- `refactor/memory-optimization` - Refactoring

## Documentation

### README Structure
1. Feature overview
2. Architecture diagram
3. Getting started
4. API reference
5. Examples
6. Troubleshooting
7. Contributing guide

### Code Comments
```c
// Avoid obvious comments:
x = x + 1;  // Don't: increment x

// Do explain why, not what:
/* CRC must be computed before fragmentation due to
   FPGA protocol specification section 5.3 */
crc = crc16_compute(data, len);
```

## Component Development

### Adding New Component
1. Create directory under `components/`
2. Create CMakeLists.txt
3. Create header with public API
4. Create implementation
5. Document in component README
6. Add unit tests

### Component Structure
```
components/mycomponent/
├── CMakeLists.txt         # Build config
├── component_name.c       # Implementation
├── component_name.h       # Public API
├── test/                  # Unit tests
│   └── test_component.c
└── README.md              # Component docs
```

## Debugging Tips

### EnableDebug Output
```bash
# Maximum verbosity
idf.py build -DCMAKE_BUILD_TYPE=Debug
idf.py monitor -p /dev/ttyUSB0 --baudrate 115200
```

### GDB Debugging
```bash
# Start GDB stub on device
idf.py gdb

# In separate terminal
xtensa-esp32s3-elf-gdb build/sein_iot_firmware.elf
```

### Memory Corruption Detection
Enable Heap Poisoning:
```
CONFIG_HEAP_POISONING_LIGHT=y
```

## Continuous Integration

### Pre-commit Checks
```bash
# Run before committing
idf.py build
idf.py size
./scripts/check_style.sh
```

---

Remember: **Correctness > Performance > Features**

When in doubt, ask the team or document the decision!
