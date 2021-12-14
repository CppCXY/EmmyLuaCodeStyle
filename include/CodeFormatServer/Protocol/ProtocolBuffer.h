#pragma once

#include <string>
#include <list>

class ProtocolBuffer
{
public:
	ProtocolBuffer(std::size_t capacity);

	char* GetWritableCursor();

	std::size_t GetRestCapacity();

	void SetWriteSize(std::size_t size);

	bool CanReadOneProtocol();

	std::string_view ReadOneProtocol();

	void FitCapacity();

	void Reset();
private:
	bool TryParseHead();
	// 代表从这个位置开始可写
	std::size_t _writeIndex;

	std::string _textProtocol;

	std::size_t _contentLength;
	std::size_t _bodyStartIndex;
	std::size_t _normalCapacity;
};

