#ifndef MMAP_PROTECT
#define MMAP_PROTECT

#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <algorithm>
#include <errno.h>
#include <vector>
#include <time.h>

using namespace std;

int* getMappedFile(int fd, off_t offset , size_t length )
{
	return (int *) mmap (0, length*((size_t)sizeof(int)) ,
						 PROT_READ | PROT_WRITE , MAP_SHARED, fd ,
						 offset*((off_t)sizeof(int)));
}

void removeMap(int* beginAddress, size_t len)
{
	munmap (beginAddress, len*((size_t)sizeof(int)));
}

size_t getFileSizeInIntegers(string fileName)
{
	struct stat stat_buf;
	int rc = stat(fileName.c_str(), &stat_buf);
	return stat_buf.st_size/4;
}

struct fileBlock{
	off_t begin;
	size_t length;
	off_t readTillNow;
	int fd;
};

class mappedVector
{
	public :
	mappedVector(fileBlock fileBlock,size_t max) : f(fileBlock) , maxSize(max)
	{
		loadNextChunck();
	}
	
	void loadNextChunck()
	{
		size_t remainingToBeRead = (off_t)f.length - f.readTillNow;
		if ( remainingToBeRead > maxSize )
		{
			remainingToBeRead = maxSize;
		}
		mapStart = getMappedFile(f.fd, f.begin + f.readTillNow , remainingToBeRead);
		numberOFReadIntegers = remainingToBeRead;
		
		currentPos = mapStart;
	}
	int peek()
	{
		return *currentPos;
	}
	int pull(){
		int retValue = *currentPos;
		f.readTillNow ++;
		if (f.readTillNow % maxSize == 0 && hasRemaining() && f.readTillNow != 0)
		{
			removeMap(mapStart, numberOFReadIntegers);
			loadNextChunck();
		}else {
			currentPos ++;
		}
		return retValue;
	}
	bool hasRemaining()
	{
		return f.readTillNow != f.length ;
	}
	void close()
	{
		removeMap(mapStart, numberOFReadIntegers);
	}
	
	private :
	fileBlock f;
	size_t maxSize;
	int* currentPos;
	
	int* mapStart;
	size_t numberOFReadIntegers;
	
};



fileBlock mergeBlocks(vector<fileBlock> fileBlocks , int resNumber ,size_t sizeToRead)
{
	string x = "IntRes" + to_string(resNumber);
	FILE* res;
	res = fopen(x.c_str(), "wb");
	
	vector<mappedVector*> streams;
	size_t totalSize = 0;
	for (fileBlock f: fileBlocks)
	{
		streams.push_back(new mappedVector(f,sizeToRead));
		totalSize += f.length;
	}
	
	vector<int> outputBuffer ;
	for (int i = 0 ; i < totalSize ; i++)
	{
		int minim = INT_MAX;
		int minStream;
		for (int k = 0 ; k < streams.size() ; k++)
		{
			if ( streams[k]->peek() < minim )
			{
				minim = streams[k]->peek();
				minStream = k;
				
			}
		}
		
		outputBuffer.push_back(streams[minStream]->pull());
		if (outputBuffer.size() == sizeToRead)
		{
			fwrite(outputBuffer.data() , sizeof(int), outputBuffer.size() , res);
			outputBuffer.clear();
		}
		
		if (! (streams[minStream]->hasRemaining()))
		{
			streams[minStream]->close();
			streams.erase(streams.begin()+minStream);
		}
	}
	fwrite(outputBuffer.data() , sizeof(int), outputBuffer.size() , res);

	fclose(res);
	
	int fd = open(x.c_str(), O_RDWR);
	
	return (fileBlock){ 0,totalSize , 0 , fd  };
}

#endif