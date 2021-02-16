/*! \file beats.c
  \brief Swatch Internet Time (aka beats) library
  
  Convert standard time to swatch internet time, which is a metric time system that divides each
  mean solar day into 1000 "beats". This code counts the number of seconds since midnight UTC+1.
  It's a bit lossy because each "beat" is 86.4 seconds, but oh well

  https://en.wikipedia.org/wiki/Swatch_Internet_Time
*/

#include <stdint.h>

uint16_t clock2beats(uint16_t hours, uint16_t minutes, uint16_t seconds, int16_t utc_offset) {
    uint32_t beats = seconds;
    beats += 60 * minutes;
    beats += (uint32_t)hours * 60 * 60; //explicit hour cast to work with 16-bit MSP430 arch
    beats += (utc_offset + 1) * 60 * 60; // offset from utc + 1 since beats in in UTC+1

    beats /= 86.4; // convert to beats
    beats %= 1000; // truncate to 3 digits for overflow

    return (uint16_t) beats;
}

#ifdef STANDALONE

#include <stdio.h>
#include <time.h>

#define UTC_OFFSET 4

int main(void) {
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("Current local time and date: %s", asctime(timeinfo));
    uint16_t beats = clock2beats(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, UTC_OFFSET);
    printf("@%u\n", beats);
}

#endif