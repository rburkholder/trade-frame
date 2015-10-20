/*
 * Copyright (C) 2013 Advanced Software Engineering Limited.
 *
 * This file is part of the ChartDirector software. Usage of this file is
 * subjected to the ChartDirector license agreement. See the LICENSE.TXT
 * file that comes with the ChartDirector software release, or visit
 * Advanced Software Engineering's web site at www.advsofteng.com or 
 * email to support@advsofteng.com for information.
 *
 */
 
#ifndef MEMBLOCK_HDR
#define MEMBLOCK_HDR

class MemBlock
{
public :
	int len;
	const char *data;
	MemBlock() : len(0), data(0) {}
	MemBlock(const char *_data, int _len) : len(_len), data(_data) {}
	char operator[](int i) const { return data[i]; }
};

class StringArray
{
public :
	int len;
	const char * const *data;
	StringArray() : len(0), data(0) {}
	StringArray(const char * const *_data, int _len) : len(_len), data(_data) {}
	const char *operator[](int i) const { return data[i]; }
};

class DoubleArray
{
public :
	int len;
	const double *data;
	DoubleArray() : len(0), data(0) {}
	DoubleArray(const double *_data, int _len) : len(_len), data(_data) {}
	double operator[](int i) const { return data[i]; }
};

class IntArray
{
public :
	int len;
	const int *data;
	IntArray() : len(0), data(0) {}
	IntArray(const int *_data, int _len) : len(_len), data(_data) {}
	int operator[](int i) const { return data[i]; }
};

#endif
