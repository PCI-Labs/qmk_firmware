/* Copyright 2020 DmNosachev
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "tmtng3.h"

led_config_t g_led_config = {{// Key Matrix to LED Index
                              {0, 1, 2},
                              {3, KC_NO, 4}},
                             {// LED Index to Physical Position
                              {0, 0},
                              {112, 0},
                              {224, 0},
                              {0, 64},
                              {224, 224}},
                             {// LED Index to Flag
                              2, 2, 2, 2, 2}};

void suspend_power_down_kb(void)
{
    rgb_matrix_set_suspend_state(true);
}

void suspend_wakeup_init_kb(void)
{
    rgb_matrix_set_suspend_state(false);
}