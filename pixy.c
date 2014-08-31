
//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//

/*
  Pixy.h - Library for interfacing with Pixy.
  Created by Scott Robinson, October 22, 2013.
  Released into the public domain.

  06.04.2014 v0.1.3 John Leimon 
    + LinkSPI.init() should be called from the setup() 
      function instead of being called automatically from
      the TPixy<LinkSPI> constructor in global scope. This
      is a workaround for a bug (?) in the Arduino DUE in which
      calling SPI.begin() from global scope (via a constructor)
      inhibits the operation of the Serial peripheral in the
      DUE. [As of: Arduino 1.5.6-r2]
      
            
  07/31/2014 v0.1.1 William Orem
    modified to work with SPI on parralax activity board. 
      
*/

#include "pixy.h"


const int pixy_clk = 15;
const int pixy_tx = 13;
const int pixy_rx = 14;

void pixyStart(){
  pixyInit();
  while(1){
    pixy_get_block(1000);
  }  
  pixyStop();  
}  

void pixyStop(){
  free(blocks);    
}
  
void pixyInit()
{
  print("pixy init\n");
  low(pixy_clk);
  skipStart = 0;
  blockCount = 0;
  blockArraySize = PIXY_INITIAL_ARRAYSIZE;
  blocks = (struct Block *)malloc(sizeof(struct Block)*blockArraySize);
  //print("pixy init finish\n");
}  



void resize()
{
  blockArraySize += PIXY_INITIAL_ARRAYSIZE;
  blocks = (struct Block *)realloc(blocks, sizeof(struct Block)*blockArraySize); 
}  

uint16_t pixy_get_block(uint16_t maxBlocks)
{
  uint8_t i;
  uint16_t w, checksum, sum;
  struct Block *block;
  if (!skipStart)
  {
    if (getStart()==0)
      return 0;
  }
  else
    skipStart = 0;
    
  for(blockCount=0; blockCount<maxBlocks && blockCount<PIXY_MAXIMUM_ARRAYSIZE;)
  {
    
    checksum = getWord();
    if (checksum==PIXY_START_WORD) // we've reached the beginning of the next frame
    {
      skipStart = 1;
	    blockType = NORMAL_BLOCK;
      return blockCount;
    }
	else if (checksum==PIXY_START_WORD_CC)
	{
	  skipStart = 1;
	  blockType = CC_BLOCK;
	  return blockCount;
	}
    else if (checksum==0)
      return blockCount;
	if (blockCount>blockArraySize)
		resize();
	
	block = blocks + blockCount;
	
    for (i=0, sum=0; i<sizeof(struct Block)/sizeof(uint16_t); i++)
    {
	  if (blockType==NORMAL_BLOCK && i>=5) // skip 
	  {
		block->angle = 0;
		break;
	  }
      w = getWord();
      sum += w;
      *((uint16_t *)block + i) = w;
    }

    if (checksum==sum)
      blockCount++;
    else
      print( "cs error\n");
	
	w = getWord();
	if (w==PIXY_START_WORD)
	  blockType = NORMAL_BLOCK;
	else if (w==PIXY_START_WORD_CC)
	  blockType = CC_BLOCK;
	else
      return blockCount;
  }
}  

int getStart()
{
  uint16_t w, lastw;
 
  lastw = 0xffff;
  while(1)
  {
    w = getWord();
    if (w==0 && lastw==0)
	{
      pause(10);
	   return 0;
	}		
    else if (w==PIXY_START_WORD && lastw==PIXY_START_WORD)
	{
      blockType = NORMAL_BLOCK;
      return 1;
	}
    else if (w==PIXY_START_WORD_CC && lastw==PIXY_START_WORD)
	{
      blockType = CC_BLOCK;
      return 1;
	}	
	else if (w==PIXY_START_WORDX)
	{
	   getByte(); // resync
	}
	lastw = w; 
  }
}
uint16_t getWord()
  {
     // ordering is different because Pixy is sending 16 bits through SPI 
      // instead of 2 bytes in a 16-bit word as with I2C
      uint16_t w;
      uint8_t c, cout = 0;

      if (outLen)
      {
        w = spiread(PIXY_SYNC_BYTE_DATA);
        cout = outBuf[outIndex++];
        if (outIndex==outLen)
          outLen = 0; 
      }
      else
        w = spiread(PIXY_SYNC_BYTE);
      w <<= 8;
      c = spiread(cout);
      w |= c;
      return w;
}
  
uint8_t getByte()
{
      return spiread(0x00);
}

int16_t spiread(int command)
{
  int16_t w;
  w = shift_in(pixy_rx, pixy_clk , MSBPRE, 8); 
  return w;
}  

