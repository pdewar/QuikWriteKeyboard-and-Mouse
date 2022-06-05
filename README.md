# QuikWrite Keyboard and Mouse

Cross-platform keyboard and mouse via Bluetooth using joystick

## Getting Started

### Hardware requirements
  - ESP32-Wroom 
  - 3 Push buttons
  - Analog 2-axis Thumb Joystick with Select Button module
  - ILI9341 320x240 TFT 

#### Schematic

![QuikWriteKeyboard-and-Mouse-0](https://github.com/pdewar/QuikWriteKeyboard-and-Mouse/blob/media/images/Project%20schematic.jpeg) 

#### Pin configuration
| ILI9341 320x240 TFT | ESP32-Wroom Dev Board |
| ------------------  | --------------------  |
| MOSI_PIN            | GPIO_NUM_23           |
| MISO_PIN            | GPIO_NUM_19           |
| CLK_PIN             | GPIO_NUM_18           |
| CS_PIN              | GPIO_NUM_15           |
| DC_PIN              | GPIO_NUM_2            |
| RST_PIN             | GPIO_NUM_4            |
| LED_PIN             | GPIO_NUM_5            |
| VCC                 | 3V3                   |
| GND                 | GND                   |

| Button 1            | ESP32-Wroom Dev Board |
| -----------------   | --------------------  |
| Pin A               | GPIO_NUM_16           |
| Pin B               | GND                   |

| Button 2            | ESP32-Wroom Dev Board |
| -----------------   | --------------------  |
| Pin A               | GPIO_NUM_17           |
| Pin B               | GND                   |

| Button 3            | ESP32-Wroom Dev Board |
| -----------------   | --------------------  |
| Pin A               | GPIO_NUM_21           |
| Pin B               | GND                   |

| Joystick            | ESP32-Wroom Dev Board |
| -----------------   | --------------------  |
| Vcc                 | 3V3                   |
| GND                 | GND                   |
| X PIN               | GPIO_NUM_32           |
| Y PIN               | GPIO_NUM_33           |
| Button              | GPIO_NUM_0            |

### Software requirements
#### How to build project using PlatformIO

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


### How to use
  - Switching functionality between keyboard and mouse is done by pushing button on joystick.

#### Keyboard 
  - The device reads joystick movements and translates them into text on the host device by using the quikwrite algorithm. The device detects that the joystick has moved out of the resting zone or neutral position, into the start zone which is the zone entered after leaving the neutral zone. The device then detects that the joystick has moved back into the resting zone, from the end zone which is the zone left before returning to the neutral zone. A table lookup is done, retrieving entry character = C[start*10+end], where C is a table that stores the character of the current character set. For example the sequence below would enter the character 'h'.

![QuikWriteKeyboard-and-Mouse-1](https://github.com/pdewar/QuikWriteKeyboard-and-Mouse/blob/media/images/character%20entry.jpeg)
![QuikWriteKeyboard-and-Mouse-2](https://github.com/pdewar/QuikWriteKeyboard-and-Mouse/blob/d3831cabbfba351a5756c942e5c84242e2542ebe/images/quikwrite%20zones.jpeg)

  - Push button 1 to switch between character layouts

![QuikWriteKeyboard-and-Mouse-3](https://github.com/pdewar/QuikWriteKeyboard-and-Mouse/blob/media/images/layout%201.jpeg) 
![QuikWriteKeyboard-and-Mouse-4](https://github.com/pdewar/QuikWriteKeyboard-and-Mouse/blob/media/images/layout%202.jpeg)

![QuikWriteKeyboard-and-Mouse-5](https://github.com/pdewar/QuikWriteKeyboard-and-Mouse/blob/media/images/layout%203.jpeg) 
![QuikWriteKeyboard-and-Mouse-6](https://github.com/pdewar/QuikWriteKeyboard-and-Mouse/blob/media/images/layout%204.jpeg)

  - Push button 2 to initiate switch characters between layouts

![QuikWriteKeyboard-and-Mouse-7](https://github.com/pdewar/QuikWriteKeyboard-and-Mouse/blob/media/images/Switch%20char%201st.jpeg)
![QuikWriteKeyboard-and-Mouse-8](https://github.com/pdewar/QuikWriteKeyboard-and-Mouse/blob/media/images/Switch%20char%202nd.jpeg)
  
  - Push button 3  
    - long press - to add keyboard shortcut for any special function key or a combination of special function keys such as Ctrl+Alt+Delete.
    - short press - excute an added keyboard shortcut. 

#### Mouse
  - The device reads joystick movements and translates it to cursor movements on the host device.
  - Push button 1 to do a right click
  - Push button 2 to do a left click
