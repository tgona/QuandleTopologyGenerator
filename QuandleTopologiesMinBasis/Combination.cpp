#include "Combination.h"

Combination::Combination(int n, int r, bool getIndex)
{
	this->n = n;
	this->r = r;
	this->getIndex = getIndex;
	index = std::vector<int>(r);
	for (int i = 0; i < r; i++)
	{
		index[i] = i;
	}
}

bool Combination::hasNext()
{
	return hasNext_Conflict;
}

void Combination::moveIndex()
{
	int i = rightmostIndexBelowMax();
	if (i >= 0)
	{
		index[i] = index[i] + 1;
		for (int j = i + 1; j < r; j++)
		{
			index[j] = index[j - 1] + 1;
		}
	}
	else
	{
		hasNext_Conflict = false;
	}
}

std::vector<int> Combination::next()
{
	if (!hasNext_Conflict)
	{
		return std::vector<int>();
	}
	std::vector<int> result(r);
	for (int i = 0; i < r; i++)
	{
		if (!this->getIndex)
		{
			result[i] = index[i] + 1;
		}
		else
		{
			result[i] = index[i];
		}
	}
	moveIndex();
	return result;
}

int Combination::rightmostIndexBelowMax()
{
	for (int i = r - 1; i >= 0; i--)
	{
		if (index[i] < n - r + i)
		{
			return i;
		}
	}
	return -1;
}
