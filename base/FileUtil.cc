#include <algorithm>
#include <cstdio>
#include <string>
#include <fcntl.h>
#include <cerrno>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <cstring>

#include "FileUtil.h"
#include "Logging.h"

namespace mini_muduo
{

namespace FileUtil
{

ReadSmallFile::ReadSmallFile(StringArg filename):
	fd_(::open(filename.c_str(), O_RDONLY | O_CLOEXEC)),
	err_(0)
{
	buf_[0] = '\0';
	if(fd_ < 0)
	{
		err_ = errno;	
	}
}

ReadSmallFile::~ReadSmallFile()
{
	if(fd_ > 0)
	{
		::close(fd_);
	}
}

template<typename String>
int ReadSmallFile::readToString(
		int maxSize,
		String *content,
		int64_t *fileSize,
		int64_t *modifyTime,
		int64_t *createTime
		)
{
	assert(content != NULL);
	int err = err_;

	if(fd_ >= 0)
	{
		content->clear();

		if(fileSize)
		{
			struct stat st;
			if(::fstat(fd_, &st) == 0)
			{
				if(S_ISREG(st.st_mode))
				{
					*fileSize = st.st_size;
					content->reserve(
							std::min(maxSize,
								static_cast<int>(*fileSize)
								)
							);
				}
				else if(S_ISDIR(st.st_mode))
				{
					err = EISDIR;
				}
				if(modifyTime)
				{
					*modifyTime = st.st_mtime;
				}
				if(createTime)
				{
					*createTime = st.st_ctime;
				}
			}
			else
			{
				err = errno;
			}
		}
	}

	while(content->size() < static_cast<size_t>(maxSize))
	{
		int toRead = std::min(maxSize - content->size(),
				sizeof(buf_));
		ssize_t n = ::read(fd_, buf_, toRead);
		if(n > 0)
		{
			content->append(buf_, n);
		}
		else
		{
			if(n < 0)
			{
				err = errno;
			}
			break;
		}
	}

	return err;
}

int ReadSmallFile::readToBuffer(int *size)
{
	int err = err_;

	if(fd_ > 0)
	{
		ssize_t n = ::pread(fd_, buf_, sizeof(buf_) - 1, 0);
		
		if(n >= 0)
		{
			if(size)
			{
				*size = static_cast<int>(n);
			}
			buf_[n] = '\0';
		}
		else
		{
			err = errno;
		}
	}

	return err;
}

template int readFile(
		StringArg filename,
		int maxSize,
		string *content,
		int64_t *fileSize,
		int64_t *modifyTime,
		int64_t * createTime);


template int ReadSmallFile::readToString(
		int maxSize,
		string *content,
		int64_t *fileSize,
		int64_t *modifyTime,
		int64_t * createTime);

AppendFile::AppendFile(StringArg filename):
	fp_(::fopen(filename.c_str(), "ae"))
{
	assert(fp_);
	::setbuffer(fp_, buf_, sizeof(buf_));
}

AppendFile::~AppendFile()
{
	::fclose(fp_);
}

void AppendFile::append(const char* logline, size_t len)
{
	size_t written = 0;

	while(written != len)
	{
		size_t remain = len - written;
		size_t n = write(logline, remain);
		if(n != remain)
		{
			int err = ferror(fp_);
			if(err)
			{
				char err_buf[512];
				//Fix: use strerror_tl
				::fprintf(stderr, "error in AppendFile::append:\
						%s\n", strerror_tl(err));
				break;
			}
		}

		written += n;
	}

	writenBytes_ += written;
}

void AppendFile::flush()
{
	::fflush(fp_);
}

size_t AppendFile::write(const char* logline, size_t len)
{
	return ::fwrite_unlocked(logline, 1, len, fp_);
}

}//FileUti

}//mini_muduo
