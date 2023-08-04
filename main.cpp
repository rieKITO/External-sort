/*
простое
двухфазное
многопутевое
с внутренней сортировкой
несбалансированное
слияние
*/

#include "Sequence.h"

const size_t FILES_COUNT = 5;
size_t maxElemInGroup = 10;

struct IndexMap
{
	std::vector<int> indexVector;
	size_t countNotEofFiles;
	size_t countNotEorFiles;

	IndexMap(size_t totalNotEofFiles) : countNotEofFiles(totalNotEofFiles) {}
};

void print_file(std::string fileName)
{
	std::ifstream file;
	Element element;

	file.open(fileName, std::ios::in | std::ios::binary);

	while (file.read((char*)&element, sizeof(Element)))
		std::cout << element.key << " - " << element.value << '\n';
}

void distribute(Sequence& f0, std::string fileName, Sequence* filesArray, bool sort, size_t maxElemInGroup)
{
	f0.start_read(fileName, maxElemInGroup);

	for (size_t i = 0; i < FILES_COUNT; ++i) {
		filesArray[i].fileName = "f" + std::to_string(i) + ".txt";
		filesArray[i].start_write(filesArray[i].fileName);
	}

	while (!f0.eof) {
		for (size_t i = 0; i < FILES_COUNT && !f0.eof; ++i) {
			f0.totalElementsInGroup = 0;
			filesArray[i].copy_run(f0, sort, maxElemInGroup);
		}
	}
	for (size_t i = 0; i < FILES_COUNT; ++i)
		filesArray[i].Close();
	f0.Close();
}

size_t merge(Sequence& f0, std::string fileName, Sequence* filesArray, size_t maxElemInGroup)
{
	size_t mergeCount = 0;
	IndexMap indexMap(FILES_COUNT);

	for (size_t i = 0; i < FILES_COUNT; ++i) {

		filesArray[i].start_read(filesArray[i].fileName, maxElemInGroup);
		filesArray[i].totalElementsInGroup = 0;
		indexMap.indexVector.push_back(i);
	}
	f0.start_write(fileName);

	for (size_t i = 0; i < FILES_COUNT; ++i) {

		if (filesArray[i].eof) {
			--indexMap.countNotEofFiles;
			--indexMap.countNotEorFiles;
			std::swap(indexMap.indexVector[i], indexMap.indexVector[indexMap.countNotEofFiles]);
		}
	}

	while (indexMap.countNotEofFiles > 1) {

		indexMap.countNotEorFiles = indexMap.countNotEofFiles;

		while (indexMap.countNotEorFiles > 1) {

			int minElem = filesArray[indexMap.indexVector[0]].element.key;
			size_t fileIndexWithMinElement = indexMap.indexVector[0];
			size_t fileIndexInIndexMap = 0;

			for (size_t i = 1; i < indexMap.countNotEorFiles; ++i) {
				if (filesArray[indexMap.indexVector[i]].element.key < minElem) {
					fileIndexWithMinElement = indexMap.indexVector[i];
					fileIndexInIndexMap = i;
					minElem = filesArray[fileIndexWithMinElement].element.key;
				}
			}

			f0.copy_elem_to_file(filesArray[fileIndexWithMinElement], maxElemInGroup);

			if (filesArray[fileIndexWithMinElement].eor) {
				--indexMap.countNotEorFiles;
				std::swap(indexMap.indexVector[fileIndexInIndexMap], indexMap.indexVector[indexMap.countNotEorFiles]);
			}
		}

		if (!filesArray[indexMap.indexVector[0]].eor)
			f0.copy_run(filesArray[indexMap.indexVector[0]], false, maxElemInGroup);

		for (size_t i = 0; i < indexMap.countNotEofFiles; ++i) {

			if (filesArray[indexMap.indexVector[i]].eof) {
				--indexMap.countNotEofFiles;
				std::swap(indexMap.indexVector[i], indexMap.indexVector[indexMap.countNotEofFiles]);
				--i;
			}

			else {
				filesArray[indexMap.indexVector[i]].eor = filesArray[indexMap.indexVector[i]].eof;
				filesArray[indexMap.indexVector[i]].totalElementsInGroup = 0;
			}
		}

		++mergeCount;
	}

	for (size_t i = 0; i < indexMap.countNotEofFiles; ++i) {

		while (!filesArray[indexMap.indexVector[i]].eof) {
			f0.copy_run(filesArray[indexMap.indexVector[i]], false, maxElemInGroup);
			++mergeCount;
		}
	}

	for (size_t i = 0; i < FILES_COUNT; ++i)
		filesArray[i].Close();
	f0.Close();

	return mergeCount;
}

void Sorting(std::string fileName, size_t maxElemInGroup)
{
	Sequence filesArray[FILES_COUNT];
	Sequence f0;
	size_t mergeCount = 0;

	distribute(f0, fileName, filesArray, true, maxElemInGroup);
	mergeCount = merge(f0, fileName, filesArray, maxElemInGroup);

	while (mergeCount > 1) {
		maxElemInGroup *= FILES_COUNT;
		distribute(f0, fileName, filesArray, false, maxElemInGroup);
		mergeCount = merge(f0, fileName, filesArray, maxElemInGroup);
	}

	for (size_t i = 0; i < FILES_COUNT; ++i)
		remove(filesArray[i].fileName.c_str());
}

void create_file(std::string fileName, int countElements)
{
	srand(GetTickCount64());

	std::ofstream file;
	Element element;

	file.open(fileName, std::ios::out | std::ios::binary);

	for (int i = 0; i < countElements; ++i) {
		element.key = rand() % 100;
		element.value = 'a' + rand() % 26;
		file.write((char*)&element, sizeof(Element));
	}

	file.close();
}

bool check_file(std::string fileName)
{
	std::ifstream file;
	Element first, second;
	bool check = true;

	file.open(fileName, std::ios::in | std::ios::binary);
	file.read((char*)&second, sizeof(Element));

	while (!file.eof() && check) {
		first = second;
		file.read((char*)&second, sizeof(Element));
		check = first.key <= second.key;
	}

	file.close();

	return check;
}

int main()
{
	create_file("data.txt", 100);
	std::cout << "Original File:\n";
	print_file("data.txt");

	Sorting("data.txt", maxElemInGroup);

	if (check_file("data.txt")) {
		std::cout << "\nFile is ordered!\n"
			<< "\nOrdered File:\n";
		print_file("data.txt");
	}

	else {
		std::cout << "\nFile is not ordered\n"
			<< "\nNot ordered File:\n";;
		print_file("data.txt");
	}

	std::cin.get();
	return 0;
}