// ===================================================================================
// Arduino IDE Wrapper for ch55xduino
// ===================================================================================
//
// Compilation Instructions for the Arduino IDE:
// ---------------------------------------------
// - Make sure you have installed ch55xduino: https://github.com/DeqingSun/ch55xduino
// - Copy the .ino and .c files as well as the /src folder together into one folder
//   and name it like the .ino file. Open the .ino file in the Arduino IDE. Go to 
//   "Tools -> Board -> CH55x Boards -> CH552 Board". Under "Tools" select the 
//   following board options:
//   - Clock Source:  16 MHz (internal)
//   - Upload Method: USB
//   - USB Settings:  USER CODE /w 266B USB RAM
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Click on "Upload" immediatly afterwards.
// - To compile the firmware using the makefile, follow the instructions in the 
//   .c file.

#ifndef USER_USB_RAM
#error "This firmware needs to be compiled with a USER USB setting"
#endif

unsigned char _sdcc_external_startup (void) __nonbanked {
  return 0;
}
