/* 
N2kMsg.cpp

2015 Copyright (c) Kave Oy, www.kave.fi  All right reserved.

Author: Timo Lappalainen

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-
  1301  USA
*/

#include <N2kMsg.h>

#define Escape 0x10
#define StartOfText 0x02
#define EndOfText 0x03
#define MsgTypeN2k 0x93

#define MaxActisenseMsgBuf 400
unsigned char ActisenseMsgBuf[MaxActisenseMsgBuf];

//*****************************************************************************
tN2kMsg::tN2kMsg(unsigned char _Source) {
  Init(6,0,_Source,255);
}

//*****************************************************************************
void tN2kMsg::SetPGN(unsigned long _PGN) {
  PGN=_PGN;
  MsgTime=millis();
}

//*****************************************************************************
void tN2kMsg::Init(unsigned char _Priority, unsigned long _PGN, unsigned char _Source, unsigned char _Destination) {
  DataLen=0;
  Priority=_Priority & 0x7;
  SetPGN(_PGN);
  Source=_Source;
  Destination=_Destination;
}

//*****************************************************************************
void tN2kMsg::Clear() {
  PGN=0;
  DataLen=0;
  MsgTime=0;
}

//*****************************************************************************
void tN2kMsg::Add8ByteDouble(double v, double precision) {
  SetBuf8ByteDouble(v,precision,DataLen,Data);
}

//*****************************************************************************
void tN2kMsg::Add4ByteDouble(double v, double precision) {
  SetBuf4ByteDouble(v,precision,DataLen,Data);
}


//*****************************************************************************
void tN2kMsg::Add2ByteDouble(double v, double precision) {
  SetBuf2ByteDouble(v,precision,DataLen,Data);
}


//*****************************************************************************
void tN2kMsg::Add2ByteInt(int v) {
  SetBuf2ByteInt(v,DataLen,Data);
}

//*****************************************************************************
void tN2kMsg::Add4ByteUInt(unsigned long v) {
  SetBuf4ByteUInt(v,DataLen,Data);
}

//*****************************************************************************
void tN2kMsg::AddUInt64(uint64_t v) {
  SetBufUInt64(v,DataLen,Data);
}

//*****************************************************************************
void tN2kMsg::AddByte(unsigned char v) {
  Data[DataLen]=v; DataLen++;
}

//*****************************************************************************
void tN2kMsg::AddStr(const char *str, int len) {
  SetBufStr(str,len,DataLen,Data);
}

//*****************************************************************************
unsigned char tN2kMsg::GetByte(int &Index) const {
  if (Index<DataLen) {
    return Data[Index++];
  } else return 0xff;
}  
  
//*****************************************************************************
double tN2kMsg::Get2ByteDouble(double precision, int &Index, double def) const {
  if (Index+2<DataLen) {
    return GetBuf2ByteDouble(precision,Index,Data,def);
  } else return def;
}

//*****************************************************************************
double tN2kMsg::Get4ByteDouble(double precision, int &Index, double def) const {
  if (Index+4<DataLen) {
    return GetBuf4ByteDouble(precision,Index,Data,def);
  } else return def;
}

//*****************************************************************************
void SetBuf8ByteDouble(double v, double precision, int &index, unsigned char *buf) {
  double fp=precision*1e6;
  long long fpll=1/fp;
  long long vll=v*1e6;
    vll*=fpll;

/* Does not work. Why?
  long long *vll=(long long *)(&buf[index]);
    (*vll)=v*1e6;
    (*vll)*=fpll;
    
    index+=8;
*/    

    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;
    vll>>=8;
    buf[index]=vll&255; index++;

}

//*****************************************************************************
void SetBuf4ByteDouble(double v, double precision, int &index, unsigned char *buf) {
  int32_t *vi=(int32_t *)(&buf[index]);
  index+=4;
  
  (*vi)=(int32_t)(v/precision);
/*  long vl;
  vl=(long)(v/precision);
    buf[index]=vl&255; index++;
    vl>>=8;
    buf[index]=vl&255; index++;
    vl>>=8;
    buf[index]=vl&255; index++;
    vl>>=8;
    buf[index]=vl&255; index++;
*/
}

//*****************************************************************************
double GetBuf2ByteDouble(double precision, int &index, const unsigned char *buf, double def) {
  int16_t *vi=(int16_t *)(&buf[index]);
  index+=2;
  
  if (*vi==0xffff) return def;
  
  return ((double)(*vi))*precision;
}

//*****************************************************************************
double GetBuf4ByteDouble(double precision, int &index, const unsigned char *buf, double def) {
  long *vl=(long *)(&buf[index]);
  index+=4;

  if (*vl==0xffffffff) return def;
  
  return ((double)(*vl))*precision;
}

//*****************************************************************************
void SetBuf2ByteDouble(double v, double precision, int &index, unsigned char *buf) {
  int16_t *vi=(int16_t *)(&buf[index]);
  index+=2;
  
  (*vi)=(int16_t)(v/precision);
//  long vl;
//  vl=(long)(v/precision);
//    buf[index]=vl&255; index++;
//    vl>>=8;
//    buf[index]=vl&255; index++;
}

//*****************************************************************************
void SetBuf2ByteInt(int v, int &index, unsigned char *buf) {
    buf[index]=lowByte(v); index++;
    buf[index]=highByte(v); index++;
}

//*****************************************************************************
void SetBuf4ByteUInt(unsigned long v, int &index, unsigned char *buf) {
  uint32_t *vl=(uint32_t *)(&buf[index]);
  index+=4;
  
  (*vl)=v;
}

//*****************************************************************************
void SetBufUInt64(uint64_t v, int &index, unsigned char *buf) {
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; v>>=8; index++;
    buf[index]=v&0xff; index++;
}

//*****************************************************************************
void SetBufStr(const char *str, int len, int &index, unsigned char *buf) {
  int i=0;
  for (; i<len && str[i]!=0; i++, index++) {
    buf[index]=str[i];
  }
  for (; i<len; i++, index++) {
    buf[index]=0;
  }
}

//*****************************************************************************
void PrintBuf(Stream *port, unsigned char len, const unsigned char *pData) {
    for(int i = 0; i<len; i++) {
      if (i>0) { port->print(","); };
      port->print(pData[i],HEX);
    }
}

//*****************************************************************************
void tN2kMsg::Print(Stream *port, bool NoData) const {
  if (!IsValid()) return;
  port->print("Pri:"); port->print(Priority);
  port->print(" PGN:"); port->print(PGN);
  port->print(" Source:"); port->print(Source);
  port->print(" Dest:"); port->print(Destination);
  port->print(" Len:"); port->print(DataLen);
  if (!NoData) {
    port->print(" Data:");
    PrintBuf(port,DataLen,Data);
  }
  port->print("\r\n");
}

//*****************************************************************************
void AddByteEscapedToBuf(unsigned char byteToAdd, byte &idx, unsigned char *buf, int &byteSum)
{
  buf[idx++]=byteToAdd;
  byteSum+=byteToAdd;

  if (byteToAdd == Escape) {
    buf[idx++]=Escape;
  }
}

//*****************************************************************************
// Actisense Format:
// <10><02><93><length (1)><priority (1)><PGN (3)><destination (1)><source (1)><time (4)><len (1)><data (len)><CRC (1)><10><03>
void tN2kMsg::SendInActisenseFormat(Stream *port) const {
  unsigned long _PGN=PGN;
  unsigned long _MsgTime=MsgTime;
  byte msgIdx=0;
  int byteSum = 0;
  byte CheckSum;
  
  if (!IsValid()) return;
  
  ActisenseMsgBuf[msgIdx++]=Escape;
  ActisenseMsgBuf[msgIdx++]=StartOfText;
  AddByteEscapedToBuf(MsgTypeN2k,msgIdx,ActisenseMsgBuf,byteSum);
  AddByteEscapedToBuf(DataLen+11,msgIdx,ActisenseMsgBuf,byteSum); //length does not include escaped chars
  AddByteEscapedToBuf(Priority,msgIdx,ActisenseMsgBuf,byteSum);
  AddByteEscapedToBuf(_PGN & 0xff,msgIdx,ActisenseMsgBuf,byteSum); _PGN>>=8;
  AddByteEscapedToBuf(_PGN & 0xff,msgIdx,ActisenseMsgBuf,byteSum); _PGN>>=8;
  AddByteEscapedToBuf(_PGN & 0xff,msgIdx,ActisenseMsgBuf,byteSum);
  AddByteEscapedToBuf(Destination,msgIdx,ActisenseMsgBuf,byteSum);
  AddByteEscapedToBuf(Source,msgIdx,ActisenseMsgBuf,byteSum);
  // Time?
  AddByteEscapedToBuf(_MsgTime & 0xff,msgIdx,ActisenseMsgBuf,byteSum); _MsgTime>>=8;
  AddByteEscapedToBuf(_MsgTime & 0xff,msgIdx,ActisenseMsgBuf,byteSum); _MsgTime>>=8;
  AddByteEscapedToBuf(_MsgTime & 0xff,msgIdx,ActisenseMsgBuf,byteSum); _MsgTime>>=8;
  AddByteEscapedToBuf(_MsgTime & 0xff,msgIdx,ActisenseMsgBuf,byteSum);
  AddByteEscapedToBuf(DataLen,msgIdx,ActisenseMsgBuf,byteSum);
  

  for (int i = 0; i < DataLen; i++) AddByteEscapedToBuf(Data[i],msgIdx,ActisenseMsgBuf,byteSum);
  byteSum %= 256;

  CheckSum = (byte)((byteSum == 0) ? 0 : (256 - byteSum));
  ActisenseMsgBuf[msgIdx++]=CheckSum;
  if (CheckSum==Escape) ActisenseMsgBuf[msgIdx++]=CheckSum;

  ActisenseMsgBuf[msgIdx++] = Escape;
  ActisenseMsgBuf[msgIdx++] = EndOfText;
  
  port->write(ActisenseMsgBuf,msgIdx);
//  Serial.print("Actisense data:");
//  PrintBuf(msgIdx,ActisenseMsgBuf);
//  Serial.print("\r\n");
}