/*! \file music.c
  \brief Music Application
  
  Plays music with the buzzer

*/

#include <stdio.h>
#include <string.h>
#include <msp430.h>

#include "api.h"

/*! this defines a song 
 * key defines the key press on the keypad
 * */
typedef struct {
    const char key;
    const uint16_t *notes;
} music_t;

#define DURATION_100  0x2000
#define DURATION_200  0x4000
#define DURATION_400  0x6000
#define DURATION_800  0x8000
#define DURATION_1600 0xa000
#define DURATION_3200 0xc000
#define DURATION_6400 0xe000

#define TONE_MASK 0x1fff
#define DURATION_SHIFT 13
#define DURATION_MASK 0xe000

#define PAUSE       0

#define END         0

#define NUM_MUSIC (sizeof(music) / sizeof(music_t))

static const uint16_t zelda_lullaby[] = {
    NOTE_E5 | DURATION_400, NOTE_G5 | DURATION_200,
    NOTE_D5 | DURATION_400, NOTE_C5 | DURATION_100, NOTE_D5 | DURATION_100, 
    NOTE_E5 | DURATION_400, NOTE_G5 | DURATION_200,
    NOTE_D5 | DURATION_100,
    NOTE_E5 | DURATION_400, NOTE_G5 | DURATION_200,
    NOTE_D6 | DURATION_400, NOTE_C6 | DURATION_200,
    NOTE_G5 | DURATION_400, NOTE_F5 | DURATION_100, NOTE_E5 | DURATION_100, 
    NOTE_D5 | DURATION_100,
    NOTE_E5 | DURATION_400, NOTE_G5 | DURATION_200,
    NOTE_D5 | DURATION_400, NOTE_C5 | DURATION_100, NOTE_D5 | DURATION_100, 
    NOTE_E5 | DURATION_400, NOTE_G5 | DURATION_200,
    NOTE_D5 | DURATION_100,
    NOTE_E5 | DURATION_400, NOTE_G5 | DURATION_200,

    NOTE_D6 | DURATION_400, NOTE_C6 | DURATION_200,
    NOTE_G5 | DURATION_400, NOTE_F5 | DURATION_100, NOTE_E5 | DURATION_100, 
    NOTE_F5 | DURATION_100, NOTE_E5 | DURATION_100, NOTE_C5 | DURATION_400,
    NOTE_F5 | DURATION_400, NOTE_E5 | DURATION_100, NOTE_D5 | DURATION_100, 
    NOTE_E5 | DURATION_100, NOTE_D5 | DURATION_100, NOTE_A4 | DURATION_400,
    NOTE_G5 | DURATION_400, NOTE_F5 | DURATION_100, NOTE_E5 | DURATION_100, 
    NOTE_F5 | DURATION_100, NOTE_E5 | DURATION_100, NOTE_C5 | DURATION_200, NOTE_F5 | DURATION_200,
    NOTE_C6 | DURATION_100,
    END,
};

static const uint16_t zelda_song_of_storms[] = {
    NOTE_D5 | DURATION_100, 
    NOTE_F5 | DURATION_100, 
    NOTE_D6 | DURATION_200, 

    NOTE_D5 | DURATION_100, 
    NOTE_F5 | DURATION_100, 
    NOTE_D6 | DURATION_200, 

    NOTE_E6 | DURATION_200, 
    NOTE_F6 | DURATION_100, 
    NOTE_E6 | DURATION_100, 
    NOTE_F6 | DURATION_100, 
    NOTE_E6 | DURATION_100, 
    NOTE_C6 | DURATION_100, 
    NOTE_A5 | DURATION_100, 

    NOTE_A5 | DURATION_200, 
    NOTE_D5 | DURATION_200, 
    NOTE_F5 | DURATION_100, 
    NOTE_G5 | DURATION_100, 
    NOTE_A5 | DURATION_400, 

    NOTE_A5 | DURATION_200, 
    NOTE_D5 | DURATION_200, 
    NOTE_F5 | DURATION_100, 
    NOTE_G5 | DURATION_100, 
    NOTE_E5 | DURATION_400, 

    NOTE_D5 | DURATION_100, 
    NOTE_F5 | DURATION_100, 
    NOTE_D6 | DURATION_200, 

    NOTE_D5 | DURATION_100, 
    NOTE_F5 | DURATION_100, 
    NOTE_D6 | DURATION_200, 

    NOTE_E6 | DURATION_200, 
    NOTE_F6 | DURATION_100, 
    NOTE_E6 | DURATION_100, 
    NOTE_F6 | DURATION_100, 
    NOTE_E6 | DURATION_100, 
    NOTE_C6 | DURATION_100, 
    NOTE_A5 | DURATION_100, 

    NOTE_A5 | DURATION_200, 
    NOTE_D5 | DURATION_200, 
    NOTE_F5 | DURATION_100, 
    NOTE_G5 | DURATION_100, 
    NOTE_A5 | DURATION_400, 
    NOTE_A5 | DURATION_200, 
    NOTE_D5 | DURATION_400,
    END,
};

static const uint16_t zelda_item[] = {
    NOTE_F5 | DURATION_200,
    NOTE_FS5 | DURATION_200,
    NOTE_G5 | DURATION_200,
    NOTE_GS5 | DURATION_800,
    END,
};

static const uint16_t zelda_secret[] = {
    NOTE_G5 | DURATION_100,
    NOTE_FS5 | DURATION_100,
    NOTE_DS5 | DURATION_100,
    NOTE_A4 | DURATION_100,
    NOTE_GS4 | DURATION_100,
    NOTE_E5 | DURATION_100,
    NOTE_GS5 | DURATION_100,
    NOTE_C6 | DURATION_100,
    END,
};

static const uint16_t happy_birthday[] = {
    NOTE_C5 | DURATION_100, 
    NOTE_C5 | DURATION_100, 
    NOTE_D5 | DURATION_200, 
    NOTE_C5 | DURATION_200, 
    NOTE_F5 | DURATION_200, 
    NOTE_E5 | DURATION_400, 

    NOTE_C5 | DURATION_100, 
    NOTE_C5 | DURATION_100, 
    NOTE_D5 | DURATION_200, 
    NOTE_C5 | DURATION_200, 
    NOTE_G5 | DURATION_200, 
    NOTE_F5 | DURATION_100, 

    NOTE_C5 | DURATION_100, 
    NOTE_C5 | DURATION_100, 
    NOTE_C6 | DURATION_200, 
    NOTE_A5 | DURATION_200, 
    NOTE_F5 | DURATION_200, 
    NOTE_E5 | DURATION_200, 
    NOTE_D5 | DURATION_200,

    NOTE_AS5 | DURATION_100, 
    NOTE_AS5 | DURATION_100, 
    NOTE_A5 | DURATION_200, 
    NOTE_F5 | DURATION_200, 
    NOTE_G5 | DURATION_200, 
    NOTE_F5 | DURATION_200,
    END,
};

static const uint16_t game_of_thrones[] = {
    NOTE_G5 | DURATION_400, NOTE_C5 | DURATION_400,

    NOTE_DS5 | DURATION_100, NOTE_F5 | DURATION_100, NOTE_G5 | DURATION_400, NOTE_C5 | DURATION_400, NOTE_DS5 | DURATION_100, NOTE_F5 | DURATION_100,
    NOTE_D5 | DURATION_800,
    NOTE_F5 | DURATION_400, NOTE_AS4 | DURATION_400,
    NOTE_DS5 | DURATION_100, NOTE_D5 | DURATION_100, NOTE_F5 | DURATION_400, NOTE_AS4 | DURATION_400,
    NOTE_DS5 | DURATION_100, NOTE_D5 | DURATION_100, NOTE_C5 | DURATION_800,

    END,
};


/*! this is the music library */

static const music_t music[] = {
    { .key = '1', .notes = zelda_song_of_storms},
    { .key = '2', .notes = zelda_lullaby},
    { .key = '3', .notes = zelda_item},
    { .key = '4', .notes = zelda_secret},
    { .key = '5', .notes = happy_birthday},
    { .key = '6', .notes = game_of_thrones},
};


static void play_song(const music_t *music){
    const uint16_t *ptr = music->notes;
    while (*ptr != END){
        tone(*ptr & TONE_MASK, (1 << (((*ptr & DURATION_MASK) >> DURATION_SHIFT) - 1)) * 100);

        ptr++;
    }
}


//! Enter the Music application.
void music_init(){
}

//! Exit the Music application.
int music_exit(){
  return 0;
}

enum STATE { IDLE, ABOUT_TO_PLAY, PLAYING };

static enum STATE state = IDLE;
static uint8_t selected = 0;

//! Draw the Music screen.
void music_draw(){
    switch (state){
    case IDLE:
        lcd_string("press bt");
        break;

    case ABOUT_TO_PLAY:
        lcd_string("playing ");
        state = PLAYING;
        break;

    case PLAYING:
        play_song(&music[selected]);
        state = IDLE;
        break;
    };
}

//! Keypress handler for the music applet.
int music_keypress(char ch){
    for (int i = 0; i < NUM_MUSIC; i++){
        if (ch == music[i].key){
            selected = i;
            state = ABOUT_TO_PLAY;
            return 1;
        }
    }
    return 0;
}


//! Fallthrough handler for the music applet.
int music_fallthrough(char ch){
    return music_keypress(ch);
}
