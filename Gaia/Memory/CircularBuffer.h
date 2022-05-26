#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

import sml;
using namespace sml;

// A circular buffer
template<typename T>
class CircularBuffer
{
public:
	CircularBuffer(){}
	CircularBuffer(std::vector<T> d) : data(d), bufferSize(data.size()) {};
	CircularBuffer(size_t s) : data(std::vector<T>(s)) {};

	// Return the desired element from the array
	inline T ReturnElement(size_t element) {
		element = WrapIterator(element);
		return data[element];
	}

	// Return the next sequential element according to the read pointer
	inline T ReturnNextElement() {
		T temp = this->ReturnElement(readPointer);

		// Iterate and wrap readPointer
		readPointer++;
		readPointer = WrapIterator(readPointer);

		return temp;
	}

	// Insert an entry into the buffer at the next location according to the write pointer
	inline void InsertNextElement(T entry) {
		data[writePointer] = entry;
		writePointer++;
		writePointer = WrapIterator(writePointer);
	}

	// Add an entry to whatever exists in the next buffer location
	inline void InsertAndAddNextElement(T entry) {
		data[writePointer] += entry;
		writePointer++;
		writePointer = WrapIterator(writePointer);
	}

	// Manually overwrite the location of the read pointer
	inline void SetReadPointer(size_t element) {
		element = WrapIterator(element);
		readPointer = element;
	}

	// Manually overwrite the location of the write pointer
	inline void SetWritePointer(size_t element) {
		element = WrapIterator(element);
		writePointer = element;
	}

	// Return the size of the circular buffer
	inline auto size() {
		return data.size();
	}

	// Return the value of the read pointer
	inline size_t ReturnReadPointer() {
		return readPointer;
	}

	// Return the value of the write pointer
	inline size_t ReturnWritePointer() {
		return writePointer;
	}

	// Override the contents of the buffer.
	void OverrideData(std::vector<T> d) {
		data = d;
		bufferSize = data.size();
	}

private:


	// Ensure that the requested element is within bounds
	size_t WrapIterator(size_t element) {
		while (element < 0) {
			element += bufferSize;
		}
		element = element % bufferSize;
		return element;
	}

	size_t bufferSize = 0;
	size_t readPointer = 0;
	size_t writePointer = 0;
	std::vector<T> data;
};

#endif // !CIRCULARBUFFER_H
