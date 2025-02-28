#include "PCSX2MemoryDomain.h"
#include "UnmanagedWrapper.h"

using namespace cli;
using namespace System;
using namespace RTCV;
using namespace NetCore;
using namespace Vanguard;
using namespace Runtime::InteropServices;
using namespace Diagnostics;

#using < System.dll >
#define EERAM_SIZE 33554432
#define WORD_SIZE 4
#define BIG_ENDIAN false


delegate void MessageDelegate(Object ^);
#pragma region EERAMUserMemory
String ^ EERAM::Name::get()
{
    return "EERAM";
}

long long EERAM::Size::get()
{
    return EERAM_SIZE;
}

int EERAM::WordSize::get()
{
    return WORD_SIZE;
}

bool EERAM::BigEndian::get()
{
    return BIG_ENDIAN;
}

unsigned char EERAM::PeekByte(long long addr)
{
    if (addr < EERAM_SIZE) {
        return UnmanagedWrapper::EERAM_PEEKBYTE(addr);
    }
    return 0;
}

array<unsigned char> ^ EERAM::PeekBytes(long long address, int length)
{
    array<unsigned char> ^ bytes = gcnew array<unsigned char>(length);
    for (int i = 0; i < length; i++) {
        bytes[i] = PeekByte(address + i);
    }
    return bytes;
}

void EERAM::PokeByte(long long addr, unsigned char val)
{
    if (addr < EERAM_SIZE) {

        UnmanagedWrapper::EERAM_POKEBYTE(addr, val);
    }
}
#pragma endregion
#pragma region VURAMDomain
String ^ VURAM::Name::get()
{
    return "VURAM";
}

long long VURAM::Size::get()
{
    return 0x10000;
}

int VURAM::WordSize::get()
{
    return WORD_SIZE;
}

bool VURAM::BigEndian::get()
{
    return BIG_ENDIAN;
}

unsigned char VURAM::PeekByte(long long addr)
{
    if ((addr >= 0x1000 && addr < 0x4000) || (addr >= 0x9000 && addr < 0xC000)) {
        return 0x0;
    }
    if (addr < Size) {
        return UnmanagedWrapper::EERAM_PEEKBYTE(addr + 0x11000000);
    }
    return 0;
}

array<unsigned char> ^ VURAM::PeekBytes(long long address, int length)
{
    array<unsigned char> ^ bytes = gcnew array<unsigned char>(length);
    for (int i = 0; i < length; i++) {
        bytes[i] = PeekByte(address + i);
    }
    return bytes;
}

void VURAM::PokeByte(long long addr, unsigned char val)
{
    if ((addr >= 0x1000 && addr < 0x4000) || (addr >= 0x9000 && addr < 0xC000)) {
        return;
    }
    if (addr < 0x10000) {

        UnmanagedWrapper::EERAM_POKEBYTE(addr + 0x11000000, val);
    }
}
#pragma endregion
#pragma region IORegisters
String ^ IORegs::Name::get()
{
    return "IO Registers";
}

long long IORegs::Size::get()
{
    return 64*1024;
}

int IORegs::WordSize::get()
{
    return WORD_SIZE;
}

bool IORegs::BigEndian::get()
{
    return BIG_ENDIAN;
}

unsigned char IORegs::PeekByte(long long addr)
{
    if (addr < Size) {
        return UnmanagedWrapper::EERAM_PEEKBYTE(addr + 0x10000000);
    }
    return 0;
}

array<unsigned char> ^ IORegs::PeekBytes(long long address, int length)
{
    array<unsigned char> ^ bytes = gcnew array<unsigned char>(length);
    for (int i = 0; i < length; i++) {
        bytes[i] = PeekByte(address + i);
    }
    return bytes;
}

void IORegs::PokeByte(long long addr, unsigned char val)
{
    if (addr < 0x10000) {

        UnmanagedWrapper::EERAM_POKEBYTE(addr + 0x10000000, val);
    }
}
#pragma endregion

#pragma region IOPRAMUserMemory
String ^ IOPRAM::Name::get()
{
    return "IOPRAM";
}

long long IOPRAM::Size::get()
{
    return 2*1024*1024;
}

int IOPRAM::WordSize::get()
{
    return WORD_SIZE;
}

bool IOPRAM::BigEndian::get()
{
    return BIG_ENDIAN;
}

unsigned char IOPRAM::PeekByte(long long addr)
{
    if (addr < IOPRAM::Size) {
        return UnmanagedWrapper::IOP_PEEKBYTE(addr);
    }
    return 0;
}

array<unsigned char> ^ IOPRAM::PeekBytes(long long address, int length)
{
    array<unsigned char> ^ bytes = gcnew array<unsigned char>(length);
    for (int i = 0; i < length; i++) {
        bytes[i] = PeekByte(address + i);
    }
    return bytes;
}

void IOPRAM::PokeByte(long long addr, unsigned char val)
{
    if (addr < IOPRAM::Size) {

        UnmanagedWrapper::IOP_POKEBYTE(addr, val);
    }
}
#pragma endregion
#pragma region SPU2Registers
String ^ SPU2Regs::Name::get()
{
    return "SPU2 Registers";
}

long long SPU2Regs::Size::get()
{
    return 1024;
}

int SPU2Regs::WordSize::get()
{
    return 2;
}

bool SPU2Regs::BigEndian::get()
{
    return BIG_ENDIAN;
}

unsigned char SPU2Regs::PeekByte(long long addr)
{
    if (addr < 1024) {
        return UnmanagedWrapper::IOP_PEEKBYTE(addr + 0x1F900000);
    }
    return 0;
}

array<unsigned char> ^ SPU2Regs::PeekBytes(long long address, int length)
{
    array<unsigned char> ^ bytes = gcnew array<unsigned char>(length);
    for (int i = 0; i < length; i++) {
        bytes[i] = PeekByte(address + i);
    }
    return bytes;
}

void SPU2Regs::PokeByte(long long addr, unsigned char val)
{
    if (addr < 1024) {

        UnmanagedWrapper::IOP_POKEBYTE(addr + 0x1F900000, val);
    }
}
#pragma endregion

#pragma region EERAMKernelMemory
String ^ KernelMemory::Name::get()
{
    return "EERAM (Kernel Level)";
}

long long KernelMemory::Size::get()
{
    return EERAM_SIZE;
}

int KernelMemory::WordSize::get()
{
    return WORD_SIZE;
}

bool KernelMemory::BigEndian::get()
{
    return BIG_ENDIAN;
}

unsigned char KernelMemory::PeekByte(long long addr)
{
    if (addr < KernelMemory::Size) {
        return UnmanagedWrapper::EERAM_PEEKBYTE(addr + 0x80000000);
    }
    return 0;
}

array<unsigned char> ^ KernelMemory::PeekBytes(long long address, int length)
{
    array<unsigned char> ^ bytes = gcnew array<unsigned char>(length);
    for (int i = 0; i < length; i++) {
        bytes[i] = PeekByte(address + i);
    }
    return bytes;
}

void KernelMemory::PokeByte(long long addr, unsigned char val)
{
    if (addr < KernelMemory::Size) {

        UnmanagedWrapper::EERAM_POKEBYTE(addr + 0x80000000, val);
    }
}
#pragma endregion
#pragma region EERAMScratchpad
String ^ Scratchpad::Name::get()
{
    return "Scratchpad";
}

long long Scratchpad::Size::get()
{
    return 16384;
}

int Scratchpad::WordSize::get()
{
    return WORD_SIZE;
}

bool Scratchpad::BigEndian::get()
{
    return BIG_ENDIAN;
}

unsigned char Scratchpad::PeekByte(long long addr)
{
    if (addr < Scratchpad::Size) {
        return UnmanagedWrapper::EERAM_PEEKBYTE(addr + 0x70000000);
    }
    return 0;
}

array<unsigned char> ^ Scratchpad::PeekBytes(long long address, int length)
{
    array<unsigned char> ^ bytes = gcnew array<unsigned char>(length);
    for (int i = 0; i < length; i++) {
        bytes[i] = PeekByte(address + i);
    }
    return bytes;
}

void Scratchpad::PokeByte(long long addr, unsigned char val)
{
    if (addr < Scratchpad::Size) {

        UnmanagedWrapper::EERAM_POKEBYTE(addr + 0x70000000, val);
    }
}
#pragma endregion
#pragma region GSRAM
String ^ GSRAM::Name::get()
{
    return "GSRAM";
}

long long GSRAM::Size::get()
{
    return 4*1024*1024;
}

int GSRAM::WordSize::get()
{
    return WORD_SIZE;
}

bool GSRAM::BigEndian::get()
{
    return BIG_ENDIAN;
}

unsigned char GSRAM::PeekByte(long long addr)
{
    if (addr < GSRAM::Size) {
        return UnmanagedWrapper::GS_PEEKBYTE(addr);
    }
    return 0;
}

array<unsigned char> ^ GSRAM::PeekBytes(long long address, int length)
{
    array<unsigned char> ^ bytes = gcnew array<unsigned char>(length);
    for (int i = 0; i < length; i++) {
        bytes[i] = PeekByte(address + i);
    }
    return bytes;
}

void GSRAM::PokeByte(long long addr, unsigned char val)
{
    if (addr < GSRAM::Size) {

        UnmanagedWrapper::GS_POKEBYTE(addr, val);
    }
}
#pragma endregion
#pragma region SPURAM
//spu can only be read and wrote in 16-bit ewww
String ^ SPU2RAM::Name::get()
{
    return "GSRAM";
}

long long SPU2RAM::Size::get()
{
    return 2 * 1024 * 1024;
}

int SPU2RAM::WordSize::get()
{
    return 2;
}

bool SPU2RAM::BigEndian::get()
{
    return BIG_ENDIAN;
}

unsigned char SPU2RAM::PeekByte(long long addr)
{
    if (addr < SPU2RAM::Size) {
        return UnmanagedWrapper::SPU_PEEKBYTE(addr);
    }
    return 0;
}

array<unsigned char> ^ SPU2RAM::PeekBytes(long long address, int length)
{
    array<unsigned char> ^ bytes = gcnew array<unsigned char>(length);
    for (int i = 0; i < length; i++) {
        bytes[i] = PeekByte(address + i);
    }
    return bytes;
}

void SPU2RAM::PokeByte(long long addr, unsigned char val)
{
    if (addr < SPU2RAM::Size) {

        UnmanagedWrapper::SPU_POKEBYTE(addr, val);
    }
}
#pragma endregion