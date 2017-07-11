//String
// File: string.h
// Last Edit: 11/11/2013 00:26 (UTC-3)
// Author: CGR

#ifndef __STRING_H__
#define __STRING_H__

#include "types.h"


enum {
	ENC_SYSTEM,
	ENC_UTF8,
	ENC_UTF16
};

enum {
	DEC,
	HEX,
	OCT
};


class string {

	char *m_buf;
	u32 m_bufferSize;

	
public:
	static s32 convert(u8 destencoding, u8 *pDest, u16 size, u8 srcencoding, const u8 *pSrc)
	{
		extern s32 system_copyconvertstr(u8 destencoding, u8 *pDest, u16 size, u8 srcencoding, const u8 *pSrc);
		return system_copyconvertstr(destencoding,pDest,size,srcencoding,pSrc);
	}

	string(const char *source);
	string(const string &source);
	string();
	~string();
	bool reserve(u32 size);

	void append(int value, int type);

	string& operator=(const string &source);
	string& operator=(const char *source);
	string& operator+=(string &source);
	string& operator+=(const char *source);
	string& operator+=(const int value);
	string& operator+=(const unsigned int value) { return operator+=((int)value); }
	string& operator+=(const unsigned long value) { return operator+=((int)value); }
	string& operator+=(const char c);
	bool operator==(const string& source) const;
	bool operator==(const char *source) const;
	bool operator!=(const string& source) const;
	bool operator!=(const char *source) const;

	u32 length() const;
	u32 size() const { return length()+1; }
	char *data() { return m_buf; }
	const char* data() const { return (const char*)m_buf; }
	const char* constData() const { return (const char*)m_buf; }


	s32 toInteger();
	

};




class string_op
{
public:
	static s32 copy(char *dest, const char *src, u32 count = 0xffffffff);
	static s32 compare(const char *string1, const char *string2, u32 count = 0xffffffff, bool cs = true);
	static bool match(const char *string1, const char *string2, bool cs = true);
	static bool contains(const char *string1, const char *string2, bool cs = true);
	static u32 length(const char *string);
	static s32 integer(const char *string);
	static float decimal(const char *string);
};



#endif
