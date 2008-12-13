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

#include "config.h"
#include "test-rectinsert-data.h"

#include "test-rectinsert-types.h"


// Defines to increase readability of test data
#define O FALSE
#define E TRUE


RectInsertTestdataEntry rectinsert_test_data[] = {

    // Test #1

    { "Put a rect in place",

      { 2, 2, 2, 2 },

      { O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O } },


    // Test #2

    { "Put a rect in place and make sure it gets an even size and position",
      
      { 15, 2, 4, 3 },

      { O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O } },


    // Test #3

    { "Put a new rect within an existing rect",

      { 14, 2, 4, 2 },

      { O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O } },


    // Test #4

    { "Put a new rect over an existing rect and make sure it beocomes even",

      { 1, 1, 3, 3 },

      { E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O } },



    // Test #5

    { "Put a new rect that partly covers an existing rect",

      { 10, 4, 6, 6 },

      { E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,O,O,O, O,O,O,O,O, E,E,E,E,E, E,E,E,E,E,

        O,O,O,O,O, O,O,O,O,O, E,E,E,E,E, E,E,E,E,E,
        O,O,O,O,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O } },


    // Test #6

    { "Put new small heighted but wide rect over an existing rect area",

      { 0, 8, 20, 2 },

      { E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,O,O,O, O,O,O,O,O, E,E,E,E,E, E,E,E,E,E,

        O,O,O,O,O, O,O,O,O,O, E,E,E,E,E, E,E,E,E,E,
        O,O,O,O,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,

        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,O,O,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O } },


    // Test #7

    { "Put new small widthed but high rect over two existing rect areas",

      { 2, 2, 2, 18 },

      { E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,E,E,E,E,

        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,E,E,E,E,
        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,

        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,

        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O } },


    // Test #8

    { "Prepare for test 10",

      { 8, 14, 4, 4 },

      { E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,E,E,E,E,

        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,E,E,E,E,
        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,

        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,E,E, E,E,O,O,O, O,O,O,O,O,

        O,O,E,E,O, O,O,O,E,E, E,E,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,E,E, E,E,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,E,E, E,E,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O } },


    // Test #9

    { "Prepare for test 10 (again)",

      { 16, 14, 2, 2 },

      { E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,E,E,E,E,

        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,E,E,E,E,
        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,

        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,E,E, E,E,O,O,O, O,E,E,O,O,

        O,O,E,E,O, O,O,O,E,E, E,E,O,O,O, O,E,E,O,O,
        O,O,E,E,O, O,O,O,E,E, E,E,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,E,E, E,E,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O } },


    // Test #10

    { "Put a rect that covers two separate rects",

      { 6, 12, 14, 8 },

      { E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        E,E,E,E,O, O,O,O,O,O, O,O,O,O,E, E,E,E,E,E,
        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,E,E,E,E,

        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,E,E,E,E,
        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, E,E,E,E,E, E,O,O,O,O,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,

        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,O,O,O,O, O,O,O,O,O, O,O,O,O,O,
        O,O,E,E,O, O,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        O,O,E,E,O, O,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        O,O,E,E,O, O,E,E,E,E, E,E,E,E,E, E,E,E,E,E,

        O,O,E,E,O, O,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        O,O,E,E,O, O,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        O,O,E,E,O, O,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        O,O,E,E,O, O,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        O,O,E,E,O, O,E,E,E,E, E,E,E,E,E, E,E,E,E,E } },


    // Test #11

    { "Cover the whole area",

      { 0, 0, 20, 20 },

      { E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,

        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,

        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,

        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E,
        E,E,E,E,E, E,E,E,E,E, E,E,E,E,E, E,E,E,E,E } },


    // Mark end of tests

    { NULL, }
};

