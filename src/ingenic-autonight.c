#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#define CONFIG_PATH "/etc/majestic.yaml"
#define ISP_PATH "/proc/jz/isp/isp-m0"
#define DEFAULT_THRESHOLD 50

// Extract backlight GPIO pin from the configuration file
int get_backlight_pin() {
    FILE *config_file = fopen(CONFIG_PATH, "r");
    if (!config_file) {
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), config_file)) {
        if (strstr(line, "  backlightPin:") != NULL) {
            int pin;
            if (sscanf(line, "  backlightPin: %2d", &pin) == 1) {
                fclose(config_file);
                return pin;
            }
        }
    }

    fclose(config_file);
    return -1;
}

// Extract the SENSOR analog gain value from the ISP
int get_isp_value() {
    FILE *isp_file = fopen(ISP_PATH, "r");
    if (!isp_file) {
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), isp_file)) {
        if (strstr(line, "SENSOR analog gain :") != NULL) {
            int value;
            sscanf(line, "SENSOR analog gain : %d", &value);
            fclose(isp_file);
            return value;
        }
    }

    fclose(isp_file);
    return -1;
}

// Set the GPIO pin value
void set_gpio_value(int pin, int value) {
    char gpio_path[64];
    snprintf(gpio_path, sizeof(gpio_path), "/sys/class/gpio/gpio%d/value", pin);

    FILE *gpio_file = fopen(gpio_path, "w");
    if (!gpio_file) {
        return;
    }

    fprintf(gpio_file, "%d", value);
    fclose(gpio_file);
}

// Send an HTTP GET request using the curl utility
void send_http_request(const char *url) {
    char command[256];
    snprintf(command, sizeof(command), "curl -s \"%s\" > /dev/null", url);
    system(command);
}

int main(int argc, char *argv[]) {
    int debug_mode = 0;
    int threshold = DEFAULT_THRESHOLD;
    int backlight_pin = -1;
    int poll_only = 0;
    int polling_delay = 2;  // Default polling delay

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--debug") == 0) {
            debug_mode = 1;
        } else if (strcmp(argv[i], "--gpio") == 0 && i + 1 < argc) {
            backlight_pin = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--threshold") == 0 && i + 1 < argc) {
            threshold = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--poll-only") == 0) {
            poll_only = 1;
        } else if (strcmp(argv[i], "--polling-delay") == 0 && i + 1 < argc) {
            polling_delay = atoi(argv[++i]);
            if (polling_delay < 1) {
                polling_delay = 2;  // default polling delay if invalid value provided
                syslog(LOG_WARNING, "Invalid polling delay provided. Using default value of 2 seconds.");
                if (debug_mode) {
                    printf("Invalid polling delay provided. Using default value of 2 seconds.\n");
                }
            }
        } else {
            printf("Usage: %s [--debug] [--gpio GPIO_NUM] [--threshold VALUE] [--poll-only] [--polling-delay SECONDS]\n", argv[0]);
            return 1;
        }
    }

    if (!poll_only) {
        if (backlight_pin == -1) {
            backlight_pin = get_backlight_pin();
        }

        if (backlight_pin == -1) {
            if (debug_mode) {
                printf("Failed to extract backlight pin\n");
            }
            syslog(LOG_ERR, "Failed to extract backlight pin");
            return 1;
        }

        if (debug_mode) {
            printf("Polling Delay: %d\n", polling_delay);
            printf("Backlight GPIO pin: %d\n", backlight_pin);
            printf("ISP threshold: %d\n", threshold);
        }
        syslog(LOG_INFO, "Polling Delay: %d", polling_delay);
        syslog(LOG_INFO, "Backlight GPIO pin: %d", backlight_pin);
        syslog(LOG_INFO, "ISP threshold: %d", threshold);
    }

    int last_gpio_value = -1;

    while (1) {
        int isp_value = get_isp_value();
        if (isp_value == -1) {
            syslog(LOG_ERR, "Failed to read ISP value");
            sleep(2);
            continue;
        }

        if (debug_mode || poll_only) {
            printf("Polled ISP value: %d\n", isp_value);
        }

        if (poll_only) {
            sleep(2);
            continue;
        }

        // For "SENSOR analog gain", higher values mean it's night and lower values mean it's day.
        int new_gpio_value = (isp_value > threshold) ? 1 : 0;

        if (new_gpio_value != last_gpio_value) {
            set_gpio_value(backlight_pin, new_gpio_value);
            if (new_gpio_value) {
                syslog(LOG_INFO, "Night mode enabled");
                send_http_request("http://localhost/night/on");
            } else {
                syslog(LOG_INFO, "Night mode disabled");
                send_http_request("http://localhost/night/off");
            }
            last_gpio_value = new_gpio_value;
        }

        sleep(polling_delay);  // Use the custom polling delay

    }

    return 0;
}
