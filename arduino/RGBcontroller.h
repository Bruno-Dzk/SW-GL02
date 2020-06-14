#pragma once

const int R=A1;
const int G=A2;
const int B=A3;

void showRGB(int Rval,int Gval,int Bval){
    analogWrite(R,Rval);
    analogWrite(G,Gval);
    analogWrite(B,Bval);
  }
