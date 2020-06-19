#pragma once

enum MsgType{
  HRDY,
  ERDY,
  ASET,
  ASND,
  CSND,
  RSND,
  TSND,
  KEYP,
  VSND,
  HGET,
  HSND
};

struct Message {
  MsgType header;
  unsigned long numeric;
  char * text;
};
