#include "mapping.h"
#include <cstdlib>

int main(int argc, char *argv[])
{
	string fileName(argv[1]);
	cout << "number of integers in the file : " << getFileSizeInIntegers(fileName) << endl;
	int fd = open(fileName.c_str(), O_RDWR);
	
	size_t N = getFileSizeInIntegers(fileName); //size of input file
	size_t M = atoi(argv[2]);			//1000000; //buffer size (in tems of integers)
	size_t D = atoi(argv[3]);			//10     ; //number of files to merge at the same time
	size_t pageSize = getpagesize()/4;
	
	size_t prevM = M;
	//calculating the effective map size that can be used
	M = ((int)(M/pageSize))*pageSize;
	
	vector<fileBlock> blocks;
	size_t numberOFLoops = (size_t)floor((((double)N) / ((double)M)));
	cout << "number of cuts : " << numberOFLoops << endl;
	for (int i = 0 ; i < numberOFLoops ; i++)
	{
		blocks.push_back((fileBlock){((off_t)i)*(off_t)M,M,0,fd});
		int* buffer = getMappedFile( fd , i*M , M );
		sort(buffer, buffer + M);
		munmap (buffer, M);
	}
	
	size_t numberOFRemainingIntegers = ((double)N-(numberOFLoops*M));
	if (numberOFRemainingIntegers > 0)
	{
		blocks.push_back((fileBlock){((off_t)numberOFLoops)*((off_t)M),numberOFRemainingIntegers,0,fd});
		int* buffer = getMappedFile( fd , numberOFLoops*M , numberOFRemainingIntegers );
		sort(buffer, buffer + numberOFRemainingIntegers);
		
		removeMap(buffer, numberOFRemainingIntegers);
		
	}
	
	int resNumber = 1;	
	while (blocks.size() > 1)
	{
		vector<fileBlock> toBeMerged;
		int numberOfBlocksToMerge = blocks.size();
		if (numberOfBlocksToMerge > D)
		{
			numberOfBlocksToMerge = D;
		}
		
		size_t sizeToRead = ((size_t)(((size_t)(prevM/(numberOfBlocksToMerge+1))) / pageSize))*pageSize ;
		if (sizeToRead == 0) 
		{
			sizeToRead = pageSize;
		}
		
		for (int i = 0 ; i < numberOfBlocksToMerge ; i++)
		{
			
			toBeMerged.push_back(blocks[i]);
		}
		
		blocks.erase(blocks.begin(),blocks.begin()+numberOfBlocksToMerge);
		blocks.push_back(mergeBlocks(toBeMerged,resNumber,sizeToRead));
		resNumber ++;
	}
	
	return 0;
}
