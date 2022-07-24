# QuikWrite Keyboard and Mouse

Cross-platform keyboard and mouse via Bluetooth using joystick

## Getting Started

### Hardware requirements
  - ESP-WROOM-32 38 PIN Development board, 
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
  
  <img width="263" alt="KeyboardLabel" src="https://user-images.githubusercontent.com/101536624/180634259-b09ddc9f-8d01-4689-9009-a76c1584c25c.png">&nbsp;&nbsp;<img width="263" alt="MouseLabel" src="https://user-images.githubusercontent.com/101536624/180634270-1d273ccd-6121-481f-8b27-1bd446d51061.png">

#### Keyboard 
  - The device reads joystick movements and translates them into text on the host device by using the quikwrite algorithm. The device detects that the joystick has moved out of the resting zone or neutral position, into the start zone which is the zone entered after leaving the neutral zone. The device then detects that the joystick has moved back into the resting zone, from the end zone which is the zone left before returning to the neutral zone. A table lookup is done, retrieving entry character = C[start*10+end], where C is a table that stores the character of the current character set. For example the sequence below would enter the character 'h'.

<img width="264" alt="CharacterEntry" src="https://user-images.githubusercontent.com/101536624/180634014-6d099732-dd53-4d4a-8162-92e3797dcf66.png">&nbsp;&nbsp;<img width="261" alt="QuikwriteZones" src="https://user-images.githubusercontent.com/101536624/180634032-ee3b931d-34fa-4405-ac39-2919b105bd1d.png">

  - Push button 1 to switch between character layouts

<img width="263" alt="Layout1" src="https://user-images.githubusercontent.com/101536624/180634086-8a2f4169-d513-4213-b224-3e98d0645265.png">&nbsp;&nbsp;<img width="263" alt="Layout2" src="https://user-images.githubusercontent.com/101536624/180634103-d5a6fa2f-2d64-488d-bff6-28bcb375efee.png">
  
  <img width="263" alt="Layout3" src="https://user-images.githubusercontent.com/101536624/180634113-8227d07c-cb37-4858-aa99-955eead139c3.png">&nbsp;&nbsp;<img width="263" alt="Layout4" src="https://user-images.githubusercontent.com/101536624/180634118-14168aef-017c-4ca4-8daa-26d6dab57b32.png">

  - Push button 2 to initiate switch characters between layouts

<img width="263" alt="SwitchCharacter(1)" src="https://user-images.githubusercontent.com/101536624/180634182-cda227d6-b889-4d11-8535-705ed82aa2ff.png">&nbsp;&nbsp;<img width="262" alt="SwitchCharacter(2)" src="https://user-images.githubusercontent.com/101536624/180634191-c4c68980-f2af-4477-ae5d-2c259f24098d.png">&nbsp;&nbsp;<img width="262" alt="SwitchedCharacter" src="https://user-images.githubusercontent.com/101536624/180634206-fd140a05-1eab-4362-9ac3-3a1fd4bf5803.png">
  
  - Push button 3  
    - long press - to add keyboard shortcut for any special function key or a combination of special function keys such as Ctrl+Alt+Delete.
    - short press - excute an added keyboard shortcut. 

#### Mouse
  - The device reads joystick movements and translates it to cursor movements on the host device.
  - Push button 1 to do a right click
  - Push button 2 to do a left click
  
  
  

  
  
  




  
