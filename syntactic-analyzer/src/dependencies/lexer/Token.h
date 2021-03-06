//Created in: 29/12/2019 03:19
//Last edit: 12/01/2020 19:11

#pragma once

#ifndef __LEXER_TOKEN_H__
#define __LEXER_TOKEN_H__

namespace lexer {

	class Token
	{
	public:
		virtual int id() const = 0;
		virtual const char* rawValue() const = 0;

		virtual char asChar() = 0;
		virtual short asShort() = 0;
		virtual long asLong() = 0;
		virtual int asInt() = 0;
		virtual const char* asString() = 0;
		virtual unsigned char asUChar() = 0;
		virtual unsigned short asUShort() = 0;
		virtual unsigned long asULong() = 0;
		virtual unsigned int asUInt() = 0;
		virtual bool asBool() = 0;
		virtual float asFloat() = 0;
		virtual double asDouble() = 0;
		virtual void* asPointer() = 0;
	};

}

#endif
