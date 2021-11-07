#pragma once

#include <string>

class ProtocolBuffer
{
public:
	ProtocolBuffer(std::size_t capacity);

	char* GetWritableCursor();

	std::size_t GetRestCapacity();

	void SetReadableSize(std::size_t readableSize);

	bool CanReadOneProtocol();

	std::string_view ReadOneProtocol();

	void Reset();
private:
	bool TryParseHead();
	// 代表从这个位置开始可写
	std::size_t _writeIndex;
	// 代表从这个位置开始可读
	// 同时如果writeIndex 和 startIndex 相同则认为可读长度为0
	std::size_t _startIndex;
	std::string _buffer;

	std::size_t _contentLength;
	std::size_t _bodyStartIndex;
};

