Duck Hunters
A retro-style Duck Hunt game built for the STM32F031K6 microcontroller with ST7735 TFT display.
Overview
Duck Hunters is an embedded systems game inspired by the classic Nintendo Duck Hunt. Players use physical buttons to aim and shoot ducks that fly across the screen, with scoring, multiple difficulty levels, and a menu system.

Hardware
STM32F031K6 Nucleo board
ST7735 1.8" TFT LCD display (128x160)
Push buttons for controls (GPIO input)
Breadboard and jumper wires

Features

Main menu with game options
2 difficulty levels
Score tracking
Sprite-based duck animations
Sound effects (if buzzer connected)
Target Selection
Leaderboard


Project Structure
├── Core/
│   ├── Inc/          # Header files
│   └── Src/          # Source files
├── Drivers/          # HAL drivers
└── README.md
