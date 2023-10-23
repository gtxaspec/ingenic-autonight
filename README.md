# Ingenic-AutoNight

Ingenic-AutoNight is a utility designed to automate the night mode for devices based on the Ingenic platform. It monitors the "SENSOR analog gain" value from the ISP (`/proc/jz/isp/isp-m0`) to determine if it's day or night and controls a GPIO pin to activate or deactivate a backlight accordingly.

## Features

- Monitors the ISP sensor's analog gain value to dynamically adjust the backlight.
- Can be run in a polling-only mode for debugging purposes.
- Provides command-line arguments for configuration overrides.
- Logging mechanism to syslog with program-specific prefix.
- For OpenIPC users, it sets the GPIO for the IR lights based off your `majestic.yaml` file

## Usage

```
./ingenic-autonight [options]
```

### Options

- `--debug`: Enable debug mode which outputs extra logging to the console.
- `--gpio GPIO_NUM`: Override the GPIO number from the configuration file.
- `--threshold VALUE`: Override the default threshold value.
- `--poll-only`: Run the program in a mode where it only polls the ISP value and logs/prints it without taking any actions.
- `--polling-delay SECONDS`: Override the built-in polling delay between checks to the ISP.

## Installation

1. Clone the repository.
2. Navigate to the directory and run `make`.
3. Deploy the binary `ingenic-autonight` to the desired location on your device.

## Contribution

Contributions are welcome! Please submit a pull request or open an issue on GitHub.
