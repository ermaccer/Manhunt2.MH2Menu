#pragma once

void PSPPatcher_Init(int base);
unsigned int GetBaseAddr();
void PatchInt(unsigned int addr, unsigned int value);
void PatchShort(unsigned int addr, unsigned short value);
void PatchChar(unsigned int addr, unsigned char value);
void MakeCall(unsigned int addr, unsigned int func);


void MakeLUI(unsigned int addr, unsigned int value);
void MakeADDIU(unsigned int addr, unsigned int value);
void Nop(unsigned int addr);
int    ReadInt(unsigned int addr);
short  ReadShort(unsigned int addr);
char   ReadChar(unsigned int addr);


#define MAKE_JUMP(a, f) _sw(0x08000000 | (((u32)(f) & 0x0FFFFFFC) >> 2), a);


#define HIJACK_FUNCTION(a, f, ptr) \
{ \
  u32 _func_ = a; \
  static u32 patch_buffer[3]; \
  _sw(_lw(_func_), (u32)patch_buffer); \
  _sw(_lw(_func_ + 4), (u32)patch_buffer + 8);\
  MAKE_JUMP((u32)patch_buffer + 4, _func_ + 8); \
  _sw(0x08000000 | (((u32)(f) >> 2) & 0x03FFFFFF), _func_); \
  _sw(0, _func_ + 4); \
  ptr = (void *)patch_buffer; \
}
