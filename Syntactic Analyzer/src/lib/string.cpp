
//String
// File: string.cpp
// Last Edit: 11/11/2013 00:28 (UTC-3)
// Author: CGR

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string.h"



bool string::reserve(u32 size)
{
	char *newbuf;
	if(size > m_bufferSize) {
		if(m_bufferSize != 0) {
			//if not first reserve
			//set 256 bytes padding
			size = (size+255)&~255;
		}

		newbuf = new char[size];
		if(!newbuf) return false;
		memset(newbuf,0,size);
		if(m_buf) {
			memcpy(newbuf,m_buf,m_bufferSize);
			delete [] m_buf;
		}
		m_buf = newbuf;
		m_bufferSize = size;
	}
		
	return true;
}

void string::append(int value, int type)
{
	unsigned int len = strlen(m_buf)+12;
	reserve(len+1);

	if(type == HEX)
		sprintf(&m_buf[this->length()],"%x",value);
	else if(type == DEC)
		sprintf(&m_buf[this->length()],"%d",value);
}

string::string(const char *source):m_buf(NULL),m_bufferSize(0)
{
	operator=(source);
}

string::string(const string &source):m_buf(NULL),m_bufferSize(0)
{
	operator=(source.data());
}

string::string():m_buf(NULL),m_bufferSize(0)
{
	reserve(32);
}

string::~string()
{
	if(m_buf) {
		delete [] m_buf;
		m_buf = NULL;
	}
}



string& string::operator=(const char *source)
{
	unsigned int len;
	if(!source) source = "";
	len = strlen(source)+1;
	reserve(len);
	memcpy(m_buf,source,len);
	m_buf[len-1] = '\0';
	return *this;
}

string& string::operator=(const string &source)
{
	return operator=(source.data());
}

string& string::operator+=(string &source)
{
	return operator+=(source.data());
}


string& string::operator+=(const char *source)
{
	unsigned int len = strlen(m_buf)+strlen(source);
	reserve(len+1);
	strcat(m_buf,source);
	return *this;
}



string& string::operator +=(const int value)
{
	char strv[12];
	itoa(value,strv,10);
	*this+=strv;
	return *this;
}

string& string::operator +=(const char c)
{
	char _c[2];
	_c[0] = c;
	_c[1] = '\0';
	operator+=(_c);
	return *this;
}

bool string::operator ==(const string &source) const
{
	return string_op::match((*this).data(),source.data(),true);
}

bool string::operator !=(const string &source) const
{
	return !string_op::match((*this).data(),source.data(),true);
}

bool string::operator ==(const char *source) const
{
	return string_op::match((*this).data(),source,true);
}

bool string::operator !=(const char *source) const
{
	return !string_op::match((*this).data(),source,true);
}





u32 string::length() const
{
	return string_op::length(m_buf);
}



s32 string::toInteger()
{
	return string_op::integer(this->data());
}


















s32 string_op::copy(char *dest, const char *src, u32 count)
{
	if(!src || count == 0) return 0;
	if(!*src) {
		*dest = '\0';
		return 0;
	}
	if(count == 0xffffffff)
		count = strlen(src)+1;
	strncpy(dest,src,count-1);
	dest[count-1] = '\0';
	return count;
}


s32 string_op::compare(const char *string1, const char *string2, u32 count, bool cs)
{
	if(count == 0) return 0;
	u32 len1 = strlen(string1);
	u32 len2 = strlen(string2);
	u32 max = len1 > len2 ? len1 : len2;
	if(max==0) return 0;
	if(count == 0xffffffff) count = max;
	else if(count > max) count = max;

	int ret = cs ? strncmp(string1,string2,max) : strnicmp(string1,string2,max);

	if(ret == 0) {
		if(len2 > len1) ret |= 1;
		else if(len1 > len2) ret |= 2;
		if(len1 != len2) ret |= 4; //if part of strings contain
	}

	return ret;
}



bool string_op::match(const char *string1, const char *string2, bool cs)
{
	return string_op::compare(string1,string2,0xffffffff,cs)==0;
}

bool string_op::contains(const char *string1, const char *string2, bool cs)
{
	s32 res = string_op::compare(string1,string2,cs);
	return res == 0 || (res&2);
}

u32 string_op::length(const char *string)
{
	return strlen(string);
}

s32 string_op::integer(const char *string)
{
	s32 ret = 0;
	if(string[0] == '0') {
		if(string[1] == 'x')
			sscanf(string,"%x",&ret);
		else {
		}
	}
	else
		sscanf(string,"%d",&ret);
	return ret;
}

float string_op::decimal(const char *string)
{
	return (float)atof(string);
}
