#ifndef FILEUTIL_H
#define FILEUTIL_H

//#include <inttypes.h>

#include "StringPiece.h"
#include "noncopyable.h"

namespace mini_muduo
{

namespace FileUtil
{

class ReadSmallFile: noncopyable
{

public:
	ReadSmallFile(StringArg filename);
	~ReadSmallFile();

	template<typename String>
	int readToString(
			int maxSize,
			String *content,
			int64_t *fileSize,
			int64_t *modifyTime,
			int64_t *createTime
			);

	int readToBuffer(int *size);

	const char* buffer()
	{
		return buf_;
	}

	static const int kBufferSize = 6 * 1024;

private:
	int fd_;
	int err_;
	char buf_[kBufferSize];

};//ReadSmallFile

template<typename String>
int readFile(
		StringArg filename,
		int maxSize,
		String *content,
		int64_t *fileSize = NULL,
		int64_t *modifyTime = NULL,
		int64_t *createTime = NULL)
{
	ReadSmallFile file(filename);

	return file.readToString(
			maxSize,
			content,
			fileSize,
			modifyTime,
			createTime
			);
}

class AppendFile: noncopyable
{

public:
	AppendFile(StringArg filename);
	~AppendFile();

	void append(const char* logline, size_t len);

	void flush();

	size_t writenBytes()
	{
		return writenBytes_;
	}

	static const int kBufferSize = 6 * 1024;

private:
	size_t write(const char* logline, size_t len);

	FILE *fp_;
	char buf_[kBufferSize];
	size_t writenBytes_;
};//AppendFile

}//FileUtil

}//mini_muduo

#endif
