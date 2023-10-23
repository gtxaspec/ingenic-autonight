# ingenic-autonight

ingenic-autonight is a utility designed to automate the night mode for devices based on the Ingenic platform. It monitors the "SENSOR analog gain" value from the ISP (`/proc/jz/isp/isp-m0`) to determine if it's day or night and controls a GPIO pin to activate or deactivate a backlight accordingly.

## Features

- Monitors the ISP sensor's analog gain value to dynamically adjust the backlight.
- Can be run in a polling-only mode for debugging purposes.
- Provides command-line arguments for configuration overrides.
- Logs to syslog
- For OpenIPC users, it sets the GPIO for the IR lights based on your `majestic.yaml` file, and uses night mode to toggle the IRCUT filter.
- For Wyze device, supports additional GPIOs for multiple sets of IR lights on specific devices, or running a custom command for PWM devices.

## Usage

```
./ingenic-autonight [--debug] [--gpio GPIO_NUM] [--gpio2 GPIO_NUM2] [--gpio3 GPIO_NUM3] [--threshold VALUE] [--poll-only] [--polling-delay SECONDS] [--http-off] [--custom-command "COMMAND"] [--daemon]
```

### Options

- `--debug`: Enable debug mode, which outputs extra logging to the console.
- `--gpio GPIO_NUM`: Override the GPIO number from the configuration file.
- `--gpio2 GPIO_NUM2`: Toggle an additional GPIO
- `--gpio3 GPIO_NUM3`: Toggle another additional GPIO
- `--threshold VALUE`: Override the default threshold value.
- `--poll-only`: Run the program in a mode where it only polls the ISP value and logs/prints it without taking any actions.
- `--polling-delay SECONDS`: Override the built-in polling delay between checks to the ISP.
- `--http-off`: Disable sending HTTP requests to majestic when night mode is enabled.
- `--custom-command "COMMAND"`: Specify a custom shell command to execute when night mode is enabled.
- `--daemon`: Run the program as a daemon in the background.

## Installation

1. Clone the repository.
2. Navigate to the directory and run `make`.
3. Deploy the binary `ingenic-autonight` to the desired location on your device.

## Contribution

Contributions are welcome! Please submit a pull request or open an issue to discuss improvements or report any issues.
