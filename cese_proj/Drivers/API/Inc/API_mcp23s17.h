/**
  ******************************************************************************
  * @file           : API_mcp23s17.h
  * @brief          : API mcp23s17 to manage this IOs expansion that establish communication by SPI bus..
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  * @date           : 2024-04-18
  ******************************************************************************
*/

#ifndef __API_mcp23s17_H /**< To prevent recursive inclusion*/
#define __API_mcp23s17_H

#include "API_common.h"


void MCP_WriteReg(uint8_t reg, uint8_t data);
uint8_t MCP_ReadReg(uint8_t reg);
char ScanKeypad(void);
#endif /* __API_mcp23s17_H */
