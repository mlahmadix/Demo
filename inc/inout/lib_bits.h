#ifndef __LIB_BITS_H__
#define __LIB_BITS_H__

#define RegValueCast(x) (*(volatile unsigned long *)(x))

class RegBits
{
	public:
		RegBits( unsigned long ulRegAddress );
		~RegBits();
		unsigned long getBitFieldValue (unsigned long BitPos, unsigned long BitLen);
		void SetBitFieldValue (unsigned long BitPos, unsigned long BitLen, unsigned long BitValue);
		void ResetBitFieldValue (unsigned long BitPos, unsigned long BitLen);
		unsigned long dumpRegValue() { return RegValueCast(mulRegisterAddress); }
		
	private:
		inline unsigned long BitFieldMaskCalculate(unsigned long BitPos, unsigned long BitLen);
		unsigned long mulRegisterAddress;
	
};

#endif
