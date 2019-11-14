#pragma once
#include <cstring>
#include <algorithm>
#include <array>
#include "UDPSender.h"

const size_t kMaxPacketSize = 8192;

class Transport
{
public:
	size_t send(const void* buffer, size_t size)
	{
		size_t n = std::min(m_buffer.size(), size);
		std::memcpy(m_buffer.data(), buffer, n);
		m_message = n;
		return n;
	}

	size_t recv(void* buffer, size_t size)
	{
		if (m_message > 0) {
			size_t n = std::min(m_message, size);
			std::memcpy(buffer, m_buffer.data(), n);
			m_message = 0;
			return n;
		}
		return 0;
	}

private:
	std::array<char, kMaxPacketSize> m_buffer;
	size_t m_message;
};

Transport* newTransport()
{
	return new Transport;
}

size_t send(Transport* t, const void* buffer, size_t size)
{
	return t->send(buffer, size);
}

size_t recv(Transport* t, void* buffer, size_t size)
{
	return t->recv(buffer, size);
}