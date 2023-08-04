#pragma once
#include "Sequence.h"

void Sequence::read_next(size_t maxElemInGroup)
{
	eof = !file.read((char*)&element, sizeof(Element));
	++totalElementsInGroup;
	eor = eof || totalElementsInGroup >= maxElemInGroup;
}

void Sequence::start_read(std::string fileName, size_t maxElemInGroup)
{
	file.open(fileName, std::ios::in | std::ios::binary);
	read_next(maxElemInGroup);
	eor = eof;
}

void Sequence::start_write(std::string fileName)
{
	file.open(fileName, std::ios::out | std::ios::binary);
}

void Sequence::Close()
{
	file.close();
}

void Sequence::copy_to_vec(Sequence& sequence, size_t maxElemInGroup, std::vector<Element>& vec)
{
	element = sequence.element;
	vec.push_back(element);
	sequence.read_next(maxElemInGroup);
}

void Sequence::copy_sort_group_to_file(Sequence& sequence, size_t maxElemInGroup)
{
	std::vector<Element> vec;

	do {
		copy_to_vec(sequence, maxElemInGroup, vec);

	} while (!sequence.eor);

	if (!vec.empty()) {
		size_t size = vec.size();

		// sort
		for (size_t i = 0; i < size; ++i)
			for (size_t j = size - 1; j > i; --j)
				if (vec[j - 1].key > vec[j].key)
					std::swap(vec[j - 1], vec[j]);

		// vector to file
		for (auto& iter : vec)
			file.write((char*)&iter, sizeof(Element));

		vec.clear();
	}
}

void Sequence::copy_elem_to_file(Sequence& sequence, size_t maxElemInGroup)
{
	element = sequence.element;
	file.write((char*)&element, sizeof(Element));
	sequence.read_next(maxElemInGroup);
}

void Sequence::copy_group_to_file(Sequence& sequence, size_t maxElemInGroup)
{
	do {
		copy_elem_to_file(sequence, maxElemInGroup);
	} while (!sequence.eor);
}

void Sequence::copy_run(Sequence& sequence, bool sort, size_t maxElemInGroup)
{
	if (sort)
		copy_sort_group_to_file(sequence, maxElemInGroup);
	else
		copy_group_to_file(sequence, maxElemInGroup);
}