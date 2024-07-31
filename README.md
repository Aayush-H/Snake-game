# Snake Game

A classic Snake game implemented in C++ using graphics.h library.

## Description

This project is a modern implementation of the classic Snake game. The player controls a snake that grows longer as it eats food, while avoiding collisions with the screen edges and itself. The game features multiple modes, power-ups, and increasing difficulty levels.

## Features

- Three game modes: Classic, Power-up, and Survival
- Increasing difficulty levels
- Power-ups with various effects
- Score tracking and display
- Lives system in Survival mode

## Getting Started

### Prerequisites

- C++ compiler (supporting C++11 or later)
- graphics.h library

### Installation

1. Clone the repository:git clone https://github.com/Aayush-H/snake-game.git
2. Navigate to the project directory:cd snake-game
3. Compile the source code:g++ src/main.cpp -o snake_game -lbgi -lgdi32 -lcomdlg32 -luuid -loleaut32 -lole32

In case of errors due to some problems regarding the setup of directory or graphics .h you can follow: https://github.com/sagargoswami2001/How-to-Setup-Graphics.h-for-Visual-Studio-Code

### Running the Game

Run the compiled executable:./snake_game

## How to Play

- Use W, A, S, D keys to control the snake's direction:
  - W: Up
  - S: Down
  - A: Left
  - D: Right
- Eat food to grow longer and increase your score
- Avoid colliding with the screen edges and the snake's own body
- In Power-up mode, collect power-ups for various effects
- In Survival mode, eat food to increase your time and avoid obstacles

## Game Modes

1. Classic: Traditional snake game
2. Power-up: Features power-ups with different effects
3. Survival: Race against time and avoid obstacles

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Acknowledgments

- graphics.h library for providing the graphical interface
- Inspiration from the classic Snake game
