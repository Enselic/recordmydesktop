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


#include "rmdtypes.h"
#include "test-rectinsert-types.h"

#include "rectinsert.h"
#include "test-rectinsert-data.h"


static void SetPixel(boolean *state, int x, int y, boolean value) {
    int index = y * STATE_WIDTH + x;

    // Guard against non-optimal implementations
    if (index < 0 || index >= STATE_WIDTH * STATE_HEIGHT) {
        return;
    }

    state[y * STATE_WIDTH + x] = value;
}

static boolean GetPixel(boolean *state, int x, int y) {
    int index = y * STATE_WIDTH + x;

    // Guard against non-optimal implementations
    if (index < 0 || index >= STATE_WIDTH * STATE_HEIGHT) {
        return FALSE;
    }

    return state[y * STATE_WIDTH + x];
}

static void WriteGeomToState(WGeometry *geom, boolean *state) {
    int x, y;

    for (y = geom->y; y < geom->y + geom->height; y++) {
        for (x = geom->x; x < geom->x + geom->width; x++) {
            SetPixel(state, x, y, TRUE);
        }
    }
}

static void ClearState(boolean *state) {
    int x, y;

    for (y = 0; y < STATE_HEIGHT; y++) {
        for (x = 0; x < STATE_WIDTH; x++) {
            SetPixel(state, x, y, FALSE);
        }
    }
}

static void WarnIfNonOptimal(WGeometry *geom) {
    if (geom->x      < 0 || geom->x      >= STATE_WIDTH  ||
        geom->y      < 0 || geom->y      >= STATE_HEIGHT ||
        geom->width  < 0 || geom->width  >  STATE_WIDTH  ||
        geom->height < 0 || geom->height >  STATE_HEIGHT)
    {
        // The RectInsert() implementation is not optimal
        printf("  Non-optimal geom (and RectInsert() implementation) encountered!\n"
               "    geom x = %d, y = %d, width = %d, height = %d\n",
               geom->x,
               geom->y,
               geom->width,
               geom->height);
    }
}

static void GetState(RectArea *root, boolean *state) {
    RectArea *current = root;

    ClearState(state);

    while (current)
    {
        WarnIfNonOptimal(&current->geom);

        WriteGeomToState(&current->geom, state);

        current = current->next;
    }
}
 
static boolean StatesEqual(boolean *a, boolean *b) {
    int x, y;

    for (y = 0; y < STATE_HEIGHT; y++) {
        for (x = 0; x < STATE_WIDTH; x++) {
            if (GetPixel(a, x, y) != GetPixel(b, x, y)) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

static void PrintState(boolean *state) {
    int x, y;
    
    for (y = 0; y < STATE_HEIGHT; y++) {
        printf("    ");

        for (x = 0; x < STATE_WIDTH; x++) {
            printf(GetPixel(state, x, y) ? "X" : "O");
            printf(x != STATE_WIDTH - 1 ? "," : "");
            printf((x + 1) % 5 == 0 ? " " : "");
        }

        printf("\n");
        printf((y + 1) % 5 == 0 ? "\n" : "");
    }
}

/**
 * This program tests the RectInsert() functionality by calling
 * RectInsert() with various testdata and after each call comparing
 * the current state with a predefied set of expected states.
 */
int main(int argc, char **argv) {
    boolean current_state[STATE_WIDTH * STATE_HEIGHT];
    RectArea *root = NULL;
    int i = 0;
    int result = 0;

    printf("== Testing RectInsert() ==\n");

    // Run until there we find end of tests data
    while (rectinsert_test_data[i].description != NULL) {

        printf("Test #%d: %s\n", i + 1, rectinsert_test_data[i].description);
        RectInsert(&root, &rectinsert_test_data[i].new_geom);
        GetState(root, current_state);

        if (!StatesEqual(current_state, rectinsert_test_data[i].expected_state)) {
            printf("  FAILURE!\n");
            printf("  Current state:\n");
            PrintState(current_state);
                
            printf("  Expected state:\n");
            PrintState(rectinsert_test_data[i].expected_state);

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
