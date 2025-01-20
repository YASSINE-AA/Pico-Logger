/*
 Copyright (c) 2025 Yassine Ahmed Ali

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


/**
 * @file pico_logger.h
 * @brief Provides logging and debugging utilities with configurable log levels.
 */

#ifndef PICO_LOGGER_H
#define PICO_LOGGER_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <execinfo.h>
#include <unistd.h>
#include <stdbool.h>
#include <stddef.h>

/* ANSI color codes for colored terminal output. */
#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

/* Log levels for message categorization. */
typedef enum {
    DEBUG_LEVEL_INFO,
    DEBUG_LEVEL_WARNING,
    DEBUG_LEVEL_ERROR,
    DEBUG_LEVEL_CRITICAL
} DebugLevel;

/* Logging macros. */
#define LOG_MESSAGE(level, fmt, ...) log_message(level, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) LOG_MESSAGE(DEBUG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) LOG_MESSAGE(DEBUG_LEVEL_WARNING, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_MESSAGE(DEBUG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...) LOG_MESSAGE(DEBUG_LEVEL_CRITICAL, fmt, ##__VA_ARGS__)
#define LOG_PERFORMANCE(message) log_performance(message)

/* Function prototypes. */
void log_message(DebugLevel level, const char *file, int line, const char *func, const char *fmt, ...);
void log_performance(char *message);
void set_logging_enabled(bool enabled);
void set_minimum_log_level(DebugLevel level);
void print_stack_trace(void);
void dump_memory(const char *label, const void *buffer, size_t size);
void save_log_file(const char *path);

/* Internal structures and variables. */
typedef struct LogEntry {
    char *message;
} LogEntry;

static bool logging_enabled = true;
static DebugLevel min_log_level = DEBUG_LEVEL_INFO;
static struct timespec start_time = {0};
static LogEntry *log_entries = NULL;
static size_t log_capacity = 0;
static size_t log_count = 0;

/* Implementation of functions. */
static void add_log_entry(const char *log_message) {
    if (log_count == log_capacity) {
        size_t new_capacity = log_capacity == 0 ? 16 : log_capacity * 2;
        LogEntry *new_entries = realloc(log_entries, new_capacity * sizeof(LogEntry));
        if (!new_entries) {
            perror("Failed to allocate memory for log entries");
            exit(EXIT_FAILURE);
        }
        log_entries = new_entries;
        log_capacity = new_capacity;
    }

    log_entries[log_count].message = strdup(log_message);
    if (!log_entries[log_count].message) {
        perror("Failed to allocate memory for log message");
        exit(EXIT_FAILURE);
    }
    log_count++;
}

static void free_log_entries(void) {
    for (size_t i = 0; i < log_count; i++) {
        free(log_entries[i].message);
    }
    free(log_entries);
    log_entries = NULL;
    log_capacity = log_count = 0;
}

void log_message(DebugLevel level, const char *file, int line, const char *func, const char *fmt, ...) {
    if (!logging_enabled || level < min_log_level) {
        return;
    }

    const char *color;
    const char *level_str;

    switch (level) {
        case DEBUG_LEVEL_INFO:    level_str = "INFO";    color = KBLU; break;
        case DEBUG_LEVEL_WARNING: level_str = "WARNING"; color = KYEL; break;
        case DEBUG_LEVEL_ERROR:   level_str = "ERROR";   color = KRED; break;
        case DEBUG_LEVEL_CRITICAL:level_str = "CRITICAL";color = KMAG; break;
        default:                  level_str = "UNKNOWN"; color = KNRM; break;
    }

    time_t raw_time;
    struct tm *time_info;
    char time_buffer[20];
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", time_info);

    char log_line[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(log_line, sizeof(log_line), fmt, args);
    va_end(args);

    printf("[%s] %s%s%s [%s:%d] %s: %s\n", time_buffer, color, level_str, KNRM, file, line, func, log_line);

    char full_log[1024];
    snprintf(full_log, sizeof(full_log), "[%s] %s [%s:%d] %s: %s", time_buffer, level_str, file, line, func, log_line);
    add_log_entry(full_log);
}

void log_performance(char *message) {
    if (message) {
        if (start_time.tv_nsec || start_time.tv_sec) {
            struct timespec end;
            clock_gettime(CLOCK_MONOTONIC, &end);

            double time_taken = (end.tv_sec - start_time.tv_sec) +
                                (end.tv_nsec - start_time.tv_nsec) / 1e9;

            printf("METRICS Function %s took %.9f seconds to execute.\n", message, time_taken);
            return;
        }
        LOG_ERROR("Start time not defined.");
    } else {
        clock_gettime(CLOCK_MONOTONIC, &start_time);
    }
}

void set_logging_enabled(bool enabled) {
    logging_enabled = enabled;
}

void set_minimum_log_level(DebugLevel level) {
    min_log_level = level;
}

void save_log_file(const char *path) {
    FILE *fp = fopen(path, "w");
    if (!fp) {
        perror("Failed to open log file");
        return;
    }

    for (size_t i = 0; i < log_count; i++) {
        fprintf(fp, "%s\n", log_entries[i].message);
    }

    fclose(fp);
}

void print_stack_trace(void) {
    void *buffer[10];
    int size = backtrace(buffer, 10);
    char **symbols = backtrace_symbols(buffer, size);

    printf("\nStack trace:\n");
    for (int i = 0; i < size; i++) {
        printf("%s\n", symbols[i]);
    }
    free(symbols);
}

void dump_memory(const char *label, const void *buffer, size_t size) {
    printf("\nMemory dump (%s):\n", label);
    unsigned char *byte_buffer = (unsigned char *)buffer;
    for (size_t i = 0; i < size; i++) {
        printf("%02x ", byte_buffer[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    if (size % 16 != 0) {
        printf("\n");
    }
}

#endif 
