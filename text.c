#include "text.h"
#include "led.h"

#define NUM_DEVICES     8	 // Number of cascaded max7219's, or just 1
#define DEL 		14000    // Delay for scrolling speed in microseconds


// Buffer array of bytes to store current display data for each column in each cascaded device
uint8_t buffer [NUM_DEVICES*8];

// Sends word through SPI
void writeWord(uint8_t address, uint8_t data)
{
  SPI_MasterTransmit(address);	// Write first byte
  SPI_MasterTransmit(data);     // Write Second byte
}

// Initializes all cascaded devices
void initMatrix()
{
	uint8_t i;	// Var used in for loops

	// Set display brighness
	LED_SELECT;
	for(i = 0; i < NUM_DEVICES; i++)   // Loop through number of cascaded devices
	{
		SPI_MasterTransmit(0x0A); // Select Intensity register
		SPI_MasterTransmit(0x07); // Set brightness
	}
	LED_DESELECT;


	// Set display refresh
	LED_SELECT;
	for(i = 0; i < NUM_DEVICES; i++)
	{
		SPI_MasterTransmit(0x0B); // Select Scan-Limit register
		SPI_MasterTransmit(0x07); // Select columns 0-7
	}
	LED_DESELECT;


	// Turn on the display
	LED_SELECT;
	for(i = 0; i < NUM_DEVICES; i++)
	{
		SPI_MasterTransmit(0x0C); // Select Shutdown register
		SPI_MasterTransmit(0x01); // Select Normal Operation mode
	}
	LED_DESELECT;


	// Disable Display-Test
	LED_SELECT;
	for(i = 0; i < NUM_DEVICES; i++)
	{
		SPI_MasterTransmit(0x0F); // Select Display-Test register
		SPI_MasterTransmit(0x00); // Disable Display-Test
	}
	LED_DESELECT;
}


// Clears all columns on all devices
void clearMatrix(void)
{
	for(uint8_t x = 1; x < 9; x++) // for all columns
	{
    LED_SELECT;
    for(uint8_t i = 0; i < NUM_DEVICES; i++)
		{
			SPI_MasterTransmit(x);    // Select column x
			SPI_MasterTransmit(0x00); // Set column to 0
		}
		LED_DESELECT;
	}
}

// Initializes buffer empty
void initBuffer(void)
{
	for(uint8_t i = 0; i < NUM_DEVICES*8; i++)
		buffer[i] = 0x00;
}


// Moves each byte forward in the buffer and adds next byte in at the end
void pushBuffer(uint8_t x)
{
	for(uint8_t i = 0; i < NUM_DEVICES*8 - 1; i++)
		buffer[i] = buffer[i+1];

	buffer[NUM_DEVICES*8 - 1] = x;
}

// Pushes in 5 characters columns into the buffer.
void pushCharacter(uint8_t c)
{
		for(uint8_t i = 0; i < 5; i++)				// For 5 bytes representing each character
		{
			pushBuffer(pgm_read_byte(&characters[c][i]));   // Push the byte from the characters array to the display buffer
			displayBuffer();				// Display the current buffer on the devices
			_delay_us(DEL);					// and delay
		}
}

// Takes a pointer to the beginning of a char array holding message, and array size, scrolls message.
void displayMessage(const char *arrayPointer, uint16_t arraySize)
{
	for(uint16_t i = 0; i < arraySize; i++)
	{
		pushCharacter(pgm_read_byte_near(arrayPointer + i) - 32);	// Send converted ASCII value of character in message to index in characters array (-32 sends corrent index to characters array)
		pushBuffer(0x00);						// Add empty column after character for letter spacing
		displayBuffer();						// Display &
		_delay_us(DEL); 						// Delay
	}

}


// Displays current buffer on the cascaded devices
void displayBuffer()
{
   for(uint8_t i = 0; i < NUM_DEVICES; i++) // For each cascaded device
   {
	   for(uint8_t j = 1; j < 9; j++) // For each column
	   {
		   SLAVE_SELECT;

		   for(uint8_t k = 0; k < i; k++) // Write Pre No-Op code
			   writeWord(0x00, 0x00);

		   writeWord(j, buffer[j + i*8 - 1]); // Write column data from buffer

		   for(uint8_t k = NUM_DEVICES-1; k > i; k--) // Write Post No-Op code
			   writeWord(0x00, 0x00);

		   SLAVE_DESELECT;
	   }
   }
}

// Main Loop
int main(void)
{
  // Inits
  SPI_MasterInit();
  initMatrix();
  clearMatrix();
  initBuffer();

  // Pointer to beginning of message
  const char *messagePointer = &message[0];

  // Size of message matrix
  uint16_t messageSize = sizeof(message);

  // Event loop
  while (1)
  {

   displayMessage(messagePointer, messageSize);	// Display the message

  }
  return (0);
}
