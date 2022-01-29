#ifndef NNPAGEDARRAY_H
#define NNPAGEDARRAY_H

#include <stdlib.h>
#include "nn_global.h"

//this class stores data in number of separate continuous blocks of memory
template <class DataType> class NNPagedArray
{
public:
	inline NNPagedArray(size_t pageSize = 0, size_t pageCount = 0)
	{
		this->tData = 0;

		if (pageSize != 0 && pageCount != 0)
		{
			this->alloc(pageSize, pageCount);
		}
	}

	inline ~NNPagedArray()
	{
		this->free();
	}

	DataType * operator [](size_t page) const
	{
		return tData[page];
	}

	DataType * & operator [](size_t page)
	{
		return tData[page];
	}

	inline bool isAllocated()
	{
		return this->tData != 0;
	}

	inline DataType *getPage(size_t page) const
	{
		return tData[page];
	}

	inline size_t getPageCount()
	{
		return pageCount;
	}

	inline size_t getPageSize()
	{
		return pageSize;
	}

	inline void free()
	{
		if (this->isAllocated())
		{
			for (size_t i = 0; i < this->getPageCount(); i++)
			{
				delete[] tData[i];
			}
			delete[] tData;

			this->tData = 0;
			this->pageCount = 0;
			this->pageSize = 0;
		}
	}

	inline void alloc(size_t pageSize, size_t pageCount)
	{
		free();

		this->pageSize = pageSize;
		this->pageCount = pageCount;

		tData = new DataType *[pageCount];
		for (size_t i = 0; i < pageCount; i++)
		{
			tData[i] = new DataType[pageSize];
			memset(tData[i], 0, pageSize);
		}
	}

private:
	size_t pageCount;
	size_t pageSize;

	DataType **tData;
};

#endif // NNPAGEDARRAY_H
