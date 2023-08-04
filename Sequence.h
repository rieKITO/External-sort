#pragma once
#include <fstream>
#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>

struct Element
{
	int key;
	char value;
	Element() {}
	Element(int key, char value) : key(key), value(value) {}
};

struct Sequence
{
private:
	void copy_to_vec(Sequence& sequence, size_t maxElemInGroup, std::vector<Element>& vec);
	void copy_sort_group_to_file(Sequence& sequence, size_t maxElemInGroup);
	void copy_group_to_file(Sequence& sequence, size_t maxElemInGroup);
public:
	std::fstream file;
	std::string fileName;
	Element element;
	size_t totalElementsInGroup;
	bool eof, eor;

	// methods
	void read_next(size_t maxElemInGroup);
	void start_read(std::string fileName, size_t maxElemInGroup);
	void start_write(std::string fileName);
	void Close();
	void copy_elem_to_file(Sequence& sequence, size_t maxElemInGroup);
	void copy_run(Sequence& sequence, bool sort, size_t maxElemInGroup);
};