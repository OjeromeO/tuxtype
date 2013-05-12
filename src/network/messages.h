/*******************************************************************************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 ******************************************************************************/

/**
 * @file    messages.h
 * @author  Jerome Portal
 * @version 0.2
 *
 * @brief   Macros for client/server messages and commands.
 */

#ifndef _MESSAGES_H_
#define _MESSAGES_H_



#define CMD_SHUTDOWN    "SHUTDOWN"
#define CMD_QUIT        "QUIT"
#define CMD_COUNT       "COUNT"
#define CMD_WHO         "WHO"
#define CMD_NICKNAME    "NICKNAME"

#define CMD_SHUTDOWN_SIZE   strlen(CMD_SHUTDOWN)
#define CMD_QUIT_SIZE       strlen(CMD_QUIT)
#define CMD_COUNT_SIZE      strlen(CMD_COUNT)
#define CMD_WHO_SIZE        strlen(CMD_WHO)
#define CMD_NICKNAME_SIZE   strlen(CMD_NICKNAME)



#endif

