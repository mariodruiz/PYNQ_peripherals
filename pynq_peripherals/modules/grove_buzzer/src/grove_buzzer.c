/******************************************************************************
 *  Copyright (c) 2021, Xilinx, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1.  Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2.  Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *  3.  Neither the name of the copyright holder nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION). HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 *  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

#include <grove_interfaces.h>
#include <grove_buzzer.h>
#include <timer.h>
#include <gpio.h>

#define DEVICE_MAX 4

struct info {
    gpio pin;
    int count;
};

static struct info info[DEVICE_MAX];

/*
 * Documentation for public functions is provided as part of the external 
 * public header file.
 * Documentation for static/private functions is provided above each
 * function in this file.
 */ 

/* Get next instance space
 * 
 * Parameters
 * ----------
 *     None
 * 
 * Return
 * ------
 * int
 *     The next index. Return -12 (ENOMEM, no memory) if errors on read.
 * 
 */
static int next_index() {
    for (int i = 0; i < DEVICE_MAX; ++i) {
        if (info[i].count == 0) return i;
    }
    return -ENOMEM;
}

grove_buzzer grove_buzzer_open(int grove_id) {
    grove_buzzer dev_id = next_index();
    if (dev_id >= 0) {
        info[dev_id].count++;
        info[dev_id].pin = gpio_open_grove(grove_id);
        gpio_set_direction(info[dev_id].pin, GPIO_OUT);
    }
    return dev_id;
}

void grove_buzzer_close(grove_buzzer buzzer) {
    if (--info[buzzer].count != 0) return;
    gpio pin = info[buzzer].pin;
    gpio_close(pin);
}

py_void grove_buzzer_play_tone(grove_buzzer buzzer, int tone, int duration) {
    gpio pin = info[buzzer].pin;
    int period = 1000000/tone;
    for (long i = 0; i < duration * 1000L; i += period) {
        gpio_write(pin, 1);
        delay_us(period/2);
        gpio_write(pin, 0);
        delay_us(period/2);
    }
    return PY_SUCCESS;
}

py_void grove_buzzer_play_note(grove_buzzer buzzer, const char* note, int duration) {
    char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C', 'D' };
    int tones[] = {262, 294, 330, 349, 392, 440, 494, 523, 587};


    for (int i = 0; i < 8; i++) {
        if (names[i] == note[0]) {
          grove_buzzer_play_tone(buzzer, tones[i], duration);
        }
    }
    return PY_SUCCESS;
}
