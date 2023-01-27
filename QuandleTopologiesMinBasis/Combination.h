#include <vector>
#pragma once

class Combination
{
private:
	int n = 0, r = 0;
	std::vector<int> index;
	//JAVA TO C++ CONVERTER NOTE: Fields cannot have the same name as methods of the current type:
	bool hasNext_Conflict = true;
	bool getIndex = false;

public:
	Combination(int n, int r, bool getIndex);

	virtual bool hasNext();

	// Based on code from KodersCode:
	// The algorithm is from Applied Combinatorics, by Alan Tucker.
	// Move the index forward a notch. The algorithm finds the rightmost
	// index element that can be incremented, increments it, and then
	// changes the elements to the right to each be 1 plus the element on their left.
	//
	// For example, if an index of 5 things taken 3 at a time is at {0 3 4}, only the 0 can
	// be incremented without running out of room. The next index is {1, 1+1, 1+2) or
	// {1, 2, 3}. This will be followed by {1, 2, 4}, {1, 3, 4}, and {2, 3, 4}.

private:
	void moveIndex();

public:
	virtual std::vector<int> next();

	// return int,the index which can be bumped up.
private:
	int rightmostIndexBelowMax();

};

