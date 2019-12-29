#ifndef OSD_FILE_H
#define OSD_FILE_H

#include <windows.h>
#include <time.h>

enum FILE_MODES
{
	FILE_MODE_NOTOPEN = 0,
	FILE_MODE_READ = 1,
	FILE_MODE_WRITE = 2,
	FILE_MODE_READWRITE = FILE_MODE_READ | FILE_MODE_WRITE
};

#define OPENFLAG_READ 1
#define OPENFLAG_WRITE 2
#define OPENFLAG_CREATE 4

enum FILEERR
{
	FILEERR_NONE = 0,
	FILEERR_OUT_OF_MEMORY,
	FILEERR_FILE_NOT_FOUND,
	FILEERR_ACCESS_DENIED,
	FILEERR_ALREADY_OPEN,
	FILEERR_UNKNOWN = -1
};



class OSDFile
{
	OSDFile();
	virtual ~OSDFile();

	public:
		//OSDFile methods
		static OSDFile *Open(const char *filename, unsigned long flags, FILEERR *pErr);
		void Close();

		unsigned __int64 GetFileSize();
		time_t GetFileCreationTime();
		time_t GetFileModifyTime();
		void SetCreationTime(time_t time);
		void SetModifyTime(time_t time);

		FILEERR Read(const char *buffer, unsigned int length);
		FILEERR Write(const char *buffer, unsigned int length);
		FILEERR SetPointer(unsigned __int64 offset);

	protected:
		HANDLE m_handle;
		bool is_directory;
};



#endif