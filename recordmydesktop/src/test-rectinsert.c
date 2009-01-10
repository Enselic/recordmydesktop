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
#include "rmd_rectinsert.h"

#include "test-rectinsert-types.h"
#include "test-rectinsert-data.h"
#include "rmd_types.h"



static void rmdSetPixel(boolean *state, int x, int y, boolean value) {
    int index = y * STATE_WIDTH + x;

    // Guard against non-optimal implementations
    if (index < 0 || index >= STATE_WIDTH * STATE_HEIGHT) {
        return;
    }

    state[y * STATE_WIDTH + x] = value;
}

static boolean rmdGetPixel(boolean *state, int x, int y) {
    int index = y * STATE_WIDTH + x;

    // Guard against non-optimal implementations
    if (index < 0 || index >= STATE_WIDTH * STATE_HEIGHT) {
        return FALSE;
    }

    return state[y * STATE_WIDTH + x];
}

static void rmdWriteGeomToState(XRectangle *rect, boolean *state) {
    int x, y;

    for (y = rect->y; y < rect->y + rect->height; y++) {
        for (x = rect->x; x < rect->x + rect->width; x++) {
            rmdSetPixel(state, x, y, TRUE);
        }
    }
}

static void rmdClearState(boolean *state) {
    int x, y;

    for (y = 0; y < STATE_HEIGHT; y++) {
        for (x = 0; x < STATE_WIDTH; x++) {
            rmdSetPixel(state, x, y, FALSE);
        }
    }
}

static void rmdWarnIfNonOptimal(XRectangle *rect) {
    if (rect->x      < 0 || rect->x      >= STATE_WIDTH  ||
        rect->y      < 0 || rect->y      >= STATE_HEIGHT ||
        rect->width  == 0 || rect->width  >  STATE_WIDTH  ||
        rect->height == 0 || rect->height >  STATE_HEIGHT)
    {
        // The rmdRectInsert() implementation is not optimal
        printf("  Non-optimal rect (and rmdRectInsert() implementation) encountered!\n"
               "    rect x = %d, y = %d, width = %hu, height = %hu\n",
               rect->x,
               rect->y,
               rect->width,
               rect->height);
    }
}

static void GetState(RectArea *root, boolean *state) {
    RectArea *current = root;

    rmdClearState(state);

    while (current)
    {
        rmdWarnIfNonOptimal(&current->rect);

        rmdWriteGeomToState(&current->rect, state);

        current = current->next;
    }
}
 
static boolean rmdStatesEqual(boolean *a, boolean *b) {
    int x, y;

    for (y = 0; y < STATE_HEIGHT; y++) {
        for (x = 0; x < STATE_WIDTH; x++) {
            if (rmdGetPixel(a, x, y) != rmdGetPixel(b, x, y)) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

static void rmdPrintState(boolean *state) {
    int x, y;
    
    for (y = 0; y < STATE_HEIGHT; y++) {
        printf("    ");

        for (x = 0; x < STATE_WIDTH; x++) {
            printf(rmdGetPixel(state, x, y) ? "X" : "O");
            printf(x != STATE_WIDTH - 1 ? "," : "");
            printf((x + 1) % 5 == 0 ? " " : "");
        }

        printf("\n");
        printf((y + 1) % 5 == 0 ? "\n" : "");
    }
}

/**
 * This program tests the rmdRectInsert() functionality by calling
 * rmdRectInsert() with various testdata and after each call comparing
 * the current state with a predefied set of expected states.
 */
int main(int argc, char **argv) {
    boolean current_state[STATE_WIDTH * STATE_HEIGHT];
    RectArea *root = NULL;
    int i = 0;
    int result = 0;

    printf("== Testing rmdRectInsert() ==\n");

    // Run until there we find end of tests data
    while (rectinsert_test_data[i].description != NULL) {

        printf("Test #%d: %s\n", i + 1, rectinsert_test_data[i].description);
        rmdRectInsert(&root, &rectinsert_test_data[i].new_rect);
        GetState(root, current_state);

        if (!rmdStatesEqual(current_state, rectinsert_test_data[i].expected_state)) {
            printf("  FAILURE!\n");
            printf("  Current state:\n");
            rmdPrintState(current_state);
                
            printf("  Expected state:\n");
            rmdPrintState(rectinsert_test_data[i].expected_state);

            // Just set to failure and keep going...
            result = -1;
        }
        else {
            printf("  PASS\n");
        }

        // Run next test
        printf("\n");
        i++;
    }
  
    return result;
}
