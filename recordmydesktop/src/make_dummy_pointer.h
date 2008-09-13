/******************************************************************************
*                            recordMyDesktop                                  *
*******************************************************************************
*                                                                             *
*            Copyright (C) 2006,2007,2008 John Varouhakis                     *
*                                                                             *
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
*                                                                             *
*                                                                             *
*   For further information contact me at johnvarouhakis@gmail.com            *
******************************************************************************/

#ifndef MAKE_DUMMY_POINTER_H
#define MAKE_DUMMY_POINTER_H 1

#include "rmdtypes.h"


/**
* Create an array containing the data for the dummy pointer
*
* \param specs DisplaySpecs struct with
*              information about the display to be recorded
*
* \param size  Pointer size, always square, always 16.(exists only
*              for the possibility to create more dummy cursors)
* \param color 0 white, 1 black
*
* \param type Always 0.(exists only for the possibility to create
*               more dummy cursors)
*
* \param npxl Return of pixel value that denotes non-drawing, while
*             applying the cursor on the target image
*
* \returns Pointer to pixel data of the cursor
*/
unsigned char *MakeDummyPointer(DisplaySpecs *specs,
                                int size,
                                int color,
                                int type,
                                unsigned char *npxl);


#endif
