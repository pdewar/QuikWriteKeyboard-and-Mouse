## How to build project using PlatformIO

1. [Install PlatformIO Core](http://docs.platformio.org/page/core.html)
2. Download [project source code](https://github.com/pdewar/QuikWriteKeyboard-and-Mouse/archive/refs/heads/main.zip)
3. Extract ZIP archive
4. Run these commands:

```sh
# Change directory to project
> cd QuikWriteKeyboard-and-Mouse-main/Project

# Build specific environment
> platformio run -e esp32dev

# Upload firmware for the specific environment
> platformio run -e esp32dev --target upload

# Clean build files
> platformio run --target clean
```
