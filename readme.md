# Pico Logger

A single-header lightweight and versatile logging utility for C projects, offering configurable log levels, performance metrics, and advanced debugging tools like stack traces and memory dumps.

## Features

- **Log Levels**: Support for `INFO`, `WARNING`, `ERROR`, and `CRITICAL` log levels.
- **Color-Coded Output**: Terminal-friendly colored output for better log readability.
- **Performance Metrics**: Measure execution time of functions or blocks of code.
- **Stack Traces**: Easily debug runtime errors with stack trace generation.
- **Memory Dumping**: Inspect memory contents for debugging low-level issues.
- **Log Persistence**: Save logs to a file for later analysis.

## Usage

1. **Include the Header**:
   Add `pico_logger.h` to your project and include it in your source files:
   ```c
   #include "pico_logger.h"
   ```

2. **Log Messages**:
   Use macros for logging messages at different levels:
   ```c
   LOG_INFO("This is an info message.");
   LOG_WARNING("This is a warning message.");
   LOG_ERROR("This is an error message.");
   LOG_CRITICAL("This is a critical error message.");
   ```

3. **Enable/Disable Logging**:
   Enable or disable logging globally:
   ```c
   set_logging_enabled(true);  // Enable logging
   set_logging_enabled(false); // Disable logging
   ```

4. **Set Minimum Log Level**:
   Control the verbosity of the logs:
   ```c
   set_minimum_log_level(DEBUG_LEVEL_WARNING); // Only log warnings or higher
   ```

5. **Performance Monitoring**:
   Measure the execution time of a block or function:
   ```c
   log_performance(NULL); // Start timing
   // ... code to measure ...
   log_performance("My Function"); // End timing and log duration
   ```

6. **Save Logs to File**:
   Persist logs for offline review:
   ```c
   save_log_file("app_logs.txt");
   ```

7. **Debugging Utilities**:
   - Print stack trace:
     ```c
     print_stack_trace();
     ```
   - Dump memory:
     ```c
     int buffer[4] = {1, 2, 3, 4};
     dump_memory("Buffer Contents", buffer, sizeof(buffer));
     ```

## License

This project is licensed under the [MIT License](https://opensource.org/license/mit).

---

### Author

Created by Yassine Ahmed Ali. Feel free to contribute or report issues on GitHub!