#pragma once

#define R 10
#define G 11
#define B 12

void show_RGB(unsigned long rgb){
    byte r_val = rgb >> 16;
    byte g_val = rgb >> 8;
    byte b_val = rgb;
    analogWrite(R,r_val);
    analogWrite(G,g_val);
    analogWrite(B,b_val);
  }
