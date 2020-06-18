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
  String text;
  Message(MsgType, unsigned long);
  Message(MsgType, String);
  Message();
};

Message::Message(MsgType header, unsigned long numeric) : header(header), numeric(numeric) {}
Message::Message(MsgType header, String text) : header(header), text(text) {}
Message::Message(){}
