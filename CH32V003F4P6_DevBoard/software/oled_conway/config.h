// ===================================================================================
// Conway's Game of Life for CH32V003 and SSD1306 128x64 Pixels I2C OLED
// ===================================================================================
//
// Description:
// ------------
// The Game of Life is a cellular automaton, and was invented by Cambridge 
// mathematician John Conway.
// This game became widely known when it was mentioned in an article published by 
// Scientific American in 1970. It consists of a grid of cells which, based on a 
// few mathematical rules, can live, die or multiply. Depending on the initial 
// conditions, the cells form various patterns throughout the course of the game.
//
// Rules:
// - For a space that is populated:
//   - Each cell with one or no neighbors dies, as if by solitude.
//   - Each cell with four or more neighbors dies, as if by overpopulation.
//   - Each cell with two or three neighbors survives.
// - For a space that is empty or unpopulated:
//   - Each cell with three neighbors becomes populated.
//
// Connect an SSD1306 128x64 Pixels I2C OLED to PC1 (SDA) and PC2 (SCL). 
// The implementation utilizes DMA for data transfer to the OLED while simultaneously 
// computing the next game step.

#pragma once

#define GAME_START    0xBEEFAFFE  // define 32-bit game start code
