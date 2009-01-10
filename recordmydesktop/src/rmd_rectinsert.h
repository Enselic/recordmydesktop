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

#ifndef RECTINSERT_H
#define RECTINSERT_H 1


#include "rmd_types.h"


/**
* Insert a new rectangle on the list, making sure it doesn't overlap
* with the existing ones
* \param root Root entry of the list
*
* \param xrect New area to be inserted
*
* \returns Number of insertions during operation
*
* \note This function is reentrant and recursive. The number
* of insertions takes this into account.
*/
int rmdRectInsert(RectArea **root, XRectangle *xrect);

/**
* Clean up a list of areas marked for update.
* \param root Root entry of the list
*/
void rmdClearList(RectArea **root);


#endif
