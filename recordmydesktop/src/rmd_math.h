/******************************************************************************
*                      recordMyDesktop - rmd_math.h                           *
*******************************************************************************
*                                                                             *
*            Copyright (C) 2008 John Varouhakis                               *
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

#ifndef RMD_MATH_H
#define RMD_MATH_H 1

/*
 *  Since roundf depends on C99, using it might make
 *  the code non-portable. rmdRoundf solves this
 *  problem, by behaving identically with roundf
 *  and being portable (floorf and ceilf, that are
 *  used in the implementation, are defined in C89)
 * 
 *  \param val Number to be rounded
 *
 *  \returns val rounded
 *
 */
double rmdRoundf( double val );

#endif

