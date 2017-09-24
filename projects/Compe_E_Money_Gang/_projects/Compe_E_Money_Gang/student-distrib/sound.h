#ifndef _SOUND_H
#define _SOUND_H
#include "types.h"

extern int sound_freq;

 void set_speaker(uint32_t nFrequence);
 void stop_sound();	
 void beep();
 void realsound();
 void bootsound();
 
 #endif /* _SOUND_H */
 
 
 
 
 