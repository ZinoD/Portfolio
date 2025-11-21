#pragma once

#include <stm32f031x6.h>
#include "display.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

extern const uint16_t Duck1Up[];
void DuckMove(int x);

int RandMove(int min, int max);