#include <iostream>
#include <cassert>
#include "inout/lib_bits.h"
#include "logger/lib_logger.h"

#define TAG "RegBits"
using namespace std;

#define NDEBUG //Just to enable Assert

RegBits::RegBits( unsigned long ulRegAddress ):
mulRegisterAddress(ulRegAddress)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	ALOGD(TAG, __FUNCTION__, "Initial Reg Value = 0x%08X", RegValueCast(ulRegAddress));
}

RegBits::~RegBits()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
}

inline unsigned long RegBits::BitFieldMaskCalculate(unsigned long BitPos, unsigned long BitLen)
{
	ALOGD(TAG, __FUNCTION__, "BitPos = 0x%08X    BitLen= 0x%08X", BitPos, BitLen);
	unsigned long ulBitMask = 0ul;
	for(int j=0; j < BitLen; j++) {
		ulBitMask |= static_cast<unsigned long>( 1 << j );
	}
	ulBitMask = static_cast<unsigned long>(ulBitMask << BitPos );
	return static_cast<unsigned long>(ulBitMask & 0xFFFFFFFF);
}

unsigned long RegBits::getBitFieldValue (unsigned long BitPos, unsigned long BitLen)
{
	ALOGD(TAG, __FUNCTION__, "BitPos = 0x%08X    BitLen= 0x%08X", BitPos, BitLen);
	return static_cast<unsigned long>(RegValueCast(mulRegisterAddress) & BitFieldMaskCalculate(BitPos, BitLen));
}

void RegBits::SetBitFieldValue (unsigned long BitPos, unsigned long BitLen, unsigned long BitValue)
{
	ALOGD(TAG, __FUNCTION__, "BitPos = 0x%08X    BitLen= 0x%08X   value= 0x%08X", BitPos, BitLen, BitValue);
	unsigned long ulFinalValue = BitFieldMaskCalculate(BitPos, BitLen);
	ulFinalValue &= static_cast<unsigned long>(BitValue << BitPos);
	unsigned long ulRegisterValue = RegValueCast(mulRegisterAddress);
	ulRegisterValue |= ulFinalValue;
	mRegMutex.lock();
	RegValueCast(mulRegisterAddress) = ulRegisterValue;
	mRegMutex.unlock();
	
}

void RegBits::ResetBitFieldValue (unsigned long BitPos, unsigned long BitLen)
{
	ALOGD(TAG, __FUNCTION__, "BitPos = 0x%08X    BitLen= 0x%08X   value= 0x%08X", BitPos, BitLen);
	unsigned long ulRegisterValue = RegValueCast(mulRegisterAddress);
	ulRegisterValue &= static_cast<unsigned long>(~(BitFieldMaskCalculate(BitPos, BitLen)) & 0xFFFFFFFF);
	mRegMutex.lock();
	RegValueCast(mulRegisterAddress) = ulRegisterValue;
	mRegMutex.unlock();
}
