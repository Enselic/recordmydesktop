/******************************************************************************
*                         for recordMyDesktop                                 *
*******************************************************************************
*                                                                             *
*            Copyright (C) 2008 Martin Nordholts                              *
*                                                                             *
*   This program is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 2 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with this program; if not, write to the Free Software               *
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA  *
*                                                                             *
******************************************************************************/

#ifndef TEST_RECTINSERT_TYPES_H
#define TEST_RECTINSERT_TYPES_H


#include "rmd_types.h"


#define STATE_WIDTH  20
#define STATE_HEIGHT 20


typedef struct RectInsertTestdataEntry {
    const char *description;
    XRectangle   new_rect;
    boolean     expected_state[STATE_WIDTH * STATE_HEIGHT];
} RectInsertTestdataEntry;


#endif /* TEST_RECTINSERT_TYPES_H */
