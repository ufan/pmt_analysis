/*
 T his library is free s*oftware; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License version 2 as published by the Free Software Foundation.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.
 
 You should have received a copy of the GNU Library General Public License
 along with this library; see the file COPYING.LIB.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
 
 ---
 Copyright (C) 2015, ufan <>
 */
#ifndef _PTAnaBase_h_
#define _PTAnaBase_h_

namespace PTAnaBase {
  /*
   * voltage: 12 bits, unit: V, range:0-4095
   * width:   10 bits, unit: ns,range:0-1023
   * amplitude:10bits, unit: V*100, range:0-1023
   * 
   * example : 1000V,40ns,4.55V --> voltage=1000,width=40,amplitude=455
   */ 
  inline int Encode(int voltage,int width,int amplitude)
  {
    return (voltage<<20) + (width<<10) + amplitude;
  }
  
  inline void Decode(int gid,int& voltage,int& width,int& amplitude)
  {
    voltage=(gid>>20)&0xFFF;
    width = (gid>>10)&0x3FF;
    amplitude=gid&0x3FF;
  }
  
  inline int DecodeVoltage(int gid)
  {
    return (gid>>20)&0xFFF;
  }
  
  inline int DecodeWidth(int gid)
  {
    return (gid>>10)&0x3FF;
  }
  
  inline int DecodeAmplitude(int gid)
  {
    return gid&0x3FF;
  }
  
  ////////////////////
  
}

#endif // _PTAnaBase_h_