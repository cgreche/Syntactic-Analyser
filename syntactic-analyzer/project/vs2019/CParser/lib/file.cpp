
//Last edit: 05/10/2016 15:22

#include <time.h>
#include "file.h"

File::File()
{
	m_OpenMode = openmode::notopen;

	m_FileSize = 0;
	m_Buffer = NULL;
	m_BufferSize = 0;
	m_Pos = 0;

	m_CurrLine = 0;
	m_CurrCol = 0;
}

File::~File()
{
	if(m_Buffer)
	{
		delete [] m_Buffer;
		m_Buffer = NULL;
	}
	if(m_file)
		m_file->Close();
}

File *File::Open(const char *filename, File::openmode mode, FILEERR *err)
{
	File *f;
	FILEERR currerr;

	f = new File;
	if(!f)
	{
		if(err)
			*err = FILEERR_OUT_OF_MEMORY;
		return NULL;
	}

	strncpy(f->m_Filename,filename,MAX_PATH);

	currerr = f->OpenInternal(mode);
	if(currerr != FILEERR_NONE)
	{
		delete f;
		f = NULL;
	}

	if(err)
		*err = currerr;

	return f;
}

FILEERR File::OpenInternal(File::openmode mode)
{
	DWORD flags;
	FILEERR currerr;

	if(mode == openmode::write)
		flags = OPENFLAG_WRITE;
	else if(mode == openmode::read)
		flags = OPENFLAG_READ;
	else if(mode == openmode::readwrite)
		flags = OPENFLAG_WRITE | OPENFLAG_READ;
	else if(mode == openmode::create)
		flags = OPENFLAG_WRITE|OPENFLAG_CREATE;
	else
		return FILEERR_ACCESS_DENIED;

	m_OpenMode = mode;

	m_file = OSDFile::Open(m_Filename,flags,&currerr);
	if(currerr == FILEERR_FILE_NOT_FOUND && mode == openmode::write)
	{
		m_OpenMode = openmode::create;
		flags = OPENFLAG_WRITE|OPENFLAG_CREATE;
		m_file = OSDFile::Open(m_Filename,flags,&currerr);
	}

	//pos open
	if(currerr == FILEERR_NONE)
	{
		if(flags & OPENFLAG_READ)
		{
			m_FileSize = m_file->GetFileSize();
			m_BufferSize = m_FileSize;
			
			if(!Reserve((unsigned int)m_BufferSize+1)) //file size + null
				return FILEERR_OUT_OF_MEMORY;

			m_file->Read(m_Buffer,(unsigned int)m_FileSize);
			m_Pos = 0;
			m_CurrCol = 0; m_CurrLine = 0;
		}

		if(flags & OPENFLAG_WRITE)
			Reserve(MIN_BUFFER_SIZE);

		m_creationtime = m_file->GetFileCreationTime();
		if(m_creationtime == 0)
			m_creationtime = time(NULL);
		m_modifytime = m_file->GetFileModifyTime();
		if(m_creationtime == 0)
			m_modifytime = m_creationtime;
	}

	return currerr;
}

void File::Close()
{
	if(m_OpenMode == openmode::write || m_OpenMode == openmode::create)
	{
		m_file->Write(m_Buffer,(DWORD)m_FileSize);
		if(m_OpenMode == openmode::create)
		{
			m_file->SetCreationTime(m_creationtime);
			m_file->SetModifyTime(m_modifytime);
		}
	}
	delete this;
}

char File::getch()
{
	if(eof())
		return (char)EOF;

	char ch = m_Buffer[m_Pos++];

	if(ch == '\n'){
		++m_CurrLine; m_CurrCol = 0;
	}
	else
		++m_CurrCol;

	return ch;
}

//todo: to fix
unsigned int File::forward() {
	if(eof()) return m_Pos;
	++m_CurrCol;
	if(m_Buffer[m_Pos++] == '\n')
	{
		++m_CurrLine;
		m_CurrCol = 0;
	}
	return m_Pos;
}

//todo: to fix
unsigned int File::backward(unsigned int n) { 
	if(m_Pos > 0 && m_Pos >= n){
		m_Pos -= n;
	}
	else
	{
		m_Pos = 0;
		m_CurrLine = 0;
		m_CurrCol = 0;
	}

	return m_Pos;
}

void File::rewind() //uhu
{
	m_Pos = 0;
	m_CurrCol = 0; m_CurrLine = 0;
}

bool File::eof() { return m_Pos == m_FileSize; }

bool File::Reserve(unsigned int size)
{
	if(m_BufferSize > size) return true;

	DWORD addsize = size > MIN_BUFFER_SIZE ? size : MIN_BUFFER_SIZE;
	DWORD buffersize = addsize;

	char *newbuf = new char[buffersize];
	if(!newbuf)
		return false;

	memset(newbuf,0,(DWORD)buffersize);
	if(m_Buffer)
	{
		memcpy(newbuf,m_Buffer,(DWORD)m_BufferSize);
		delete [] m_Buffer;
	}

	m_Buffer = newbuf;
	m_BufferSize = buffersize;
	return true;
}

void File::SetPos(unsigned int pos)
{
	if((m_Pos+1) > m_BufferSize)
		pos -= ((pos+1)-(DWORD)m_BufferSize);

	m_Pos = pos;

}

void File::Write(void *buffer, unsigned int length)
{
	if(!buffer || length == 0) return;
	if(!Reserve(m_Pos + length)) return;

	memcpy(&m_Buffer[m_Pos],buffer,length);
	m_Pos += length;

	if(m_Pos > m_FileSize)
		m_FileSize = m_Pos;
}

void File::Read(void *buffer, unsigned int length)
{
	if(!buffer || length == 0 || m_Pos == m_FileSize) return;
	if((m_Pos+length) > m_FileSize)
		length = (DWORD)m_FileSize-m_Pos;

	memcpy(buffer,&m_Buffer[m_Pos],length);
	m_Pos += length;
}

unsigned int File::Insert(const char *string)
{
	unsigned int len;

	DWORD pos = m_Pos;
	if(!string) return pos;

	len = strlen(string);
	if(len == 0) return pos;

	if(!Reserve(pos + len)) return pos;

	if((pos+1) > m_FileSize)
		memcpy(&m_Buffer[pos],string,len);
	else
	{
		char *curr = &m_Buffer[pos];
		char *endcurr = &m_Buffer[m_FileSize-1];
		char *endbuf = &m_Buffer[len + m_FileSize-1];

		do{
			*endbuf-- = *endcurr;
		}while(endcurr-- != curr);

		memcpy(curr,string,len);
	}

	pos += len;
	m_FileSize += len;
	m_Pos = pos;

	return pos;
}

unsigned int File::Delete(unsigned int count)
{
	DWORD pos = m_Pos;
	
	if((pos+1+count) > m_FileSize)
		count = m_FileSize-pos;

	if(count == 0) return pos;

	char *curr = &m_Buffer[pos];
	char *remainbuf = &m_Buffer[pos+count];
	char *endbuf = &m_Buffer[m_FileSize-count];

	unsigned int remaincount = m_FileSize - (pos+count);

	if(remaincount > 0) {
		memcpy(curr,remainbuf,remaincount);
		memset(endbuf,0,count);
	}
	else {
		memset(curr,0,count);
	}

	m_FileSize -= count;

	return pos;
}

unsigned int File::Replace(const char *string, unsigned int count)
{
	Delete(count);
	return Insert(string);
}

void File::Print(const char *string)
{
	m_file->Write(string,strlen(string));
}