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
#ifndef _PIXY_H
#define _PIXY_H
#include "simpletools.h"                      // Include simple tools

//defines
#define PIXY_INITIAL_ARRAYSIZE      30
#define PIXY_MAXIMUM_ARRAYSIZE      130
#define PIXY_START_WORD             0xaa55
#define PIXY_START_WORD_CC          0xaa56
#define PIXY_START_WORDX            0x55aa
#define PIXY_SYNC_BYTE              0x5a
#define PIXY_SYNC_BYTE_DATA         0x5b
#define PIXY_OUTBUF_SIZE            6
//data
enum BlockType
{
	NORMAL_BLOCK,
	CC_BLOCK
};

struct Block 
{
  uint16_t signature;
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  uint16_t angle;
};

 uint8_t outBuf[PIXY_OUTBUF_SIZE];
 uint8_t outLen;
 uint8_t outIndex;
 uint8_t addr_;
int  skipStart;
enum BlockType blockType;
uint16_t blockCount;
uint16_t blockArraySize;
struct Block *blocks;

//functions 
uint16_t pixy_get_block(uint16_t maxBlocks);
uint8_t getByte();
int getStart();
uint16_t getWord();
void resize();
int16_t spiread( int command);
void pixyStop();
void pixyInit();
void pixyStart();

#endif