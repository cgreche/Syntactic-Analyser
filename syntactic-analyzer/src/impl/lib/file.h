#ifndef __FILE_H__
#define __FILE_H__

#include <list>

#include "osd_file.h"


#define MIN_BUFFER_SIZE 1024

class File
{
	public:
		enum openmode
		{
			notopen = -1,
			read = 0,
			write,
			readwrite,
			create //used like write, but setting timestamps
		};

		FILEERR OpenInternal(openmode mode);

		File();
		~File();

	public:
		static File *Open(const char *filename, openmode mode, FILEERR *err);
		void Close();

		char getch();

		unsigned int forward();
		unsigned int backward(unsigned int n = 1);
		void rewind();
		bool eof();

	protected:
		OSDFile *m_file;

		openmode m_OpenMode;

		char m_Filename[256];
		UINT64 m_FileSize;
		UINT64 m_BufferSize;

		char *m_Buffer;
		DWORD m_Pos;

		//
		time_t m_creationtime;
		time_t m_modifytime;
	
		unsigned int m_CurrLine;
		unsigned int m_CurrCol;

	public:
		//acessors
		char *FileName() { return m_Filename; }
		UINT64 FileSize() { return m_FileSize;}
		time_t CreationTime() { return m_creationtime; }
		time_t ModifyTime() { return m_modifytime; }
		void SetCreationTime(time_t time) { m_creationtime = time; }
		void SetModifyTime(time_t time) { m_modifytime = time; }

		bool Reserve(unsigned int size);
		void SetPos(unsigned int pos);
		unsigned int GetPos() { return m_Pos; }
		char *Buffer() const { return m_Buffer; }

		void Write(void *buffer, unsigned int length);
		void Read(void *buffer, unsigned int length = 1);

		unsigned int Insert(const char *string); 
		unsigned int Delete(unsigned int count = 1);
		unsigned int Replace(const char *string, unsigned int count);
	public:
		//File Tools
		//returns new pos {
		void Print(const char *string);
		DWORD WriteNumber(DWORD number);


		//returns new pos }

};


#endif