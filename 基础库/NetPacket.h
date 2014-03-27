#pragma once

#include "Logger.h"
#include "NetPacketPool.h"

#include <string>

namespace Answer
{
	enum PackType
	{
		PACK_UNKNOWN = (uint8_t)0x00,
		PACK_PROC = (uint8_t)0x01,
		PACK_PROCREPLY = (uint8_t)0x02,
		PACK_DISPATCH = (uint8_t)0x03,
	};

	class NetPacket
	{
	public:
		NetPacket(unsigned int bufferSize, NetPacketPool *pool, bool scalable = true, bool policy = false)
		{
			if (bufferSize < NET_PACKET_HEAD_LEN)
			{
				bufferSize = NET_PACKET_HEAD_LEN;
			}

			m_buffer = new char[bufferSize];
			m_buffer += NET_PACKET_HEAD_LEN;
			m_bufferSize = bufferSize - NET_PACKET_HEAD_LEN;

			m_pool = pool;
			m_scalable = scalable;
			m_policy = policy;

			reset();
		}

		~NetPacket()
		{
			reset();

			m_bufferSize = 0;
			m_buffer -= NET_PACKET_HEAD_LEN;
			delete[] m_buffer;
			m_buffer = NULL;
		}

	public:
		void reset()
		{
			m_rOffset = 0;
			m_wOffset = 0;
			m_rOverFlow = false;
			m_wOverFlow = false;
			type = PACK_UNKNOWN;
			m_proc = 0;
			m_size = 0;
			m_refCount = 1;
			m_scalable = true;
		}

		char* getBuffer()
		{
			return m_buffer;
		}

		size_t getBufferSize()
		{
			return m_bufferSize;
		}

		int8_t readInt8() 
		{	
			return read<int8_t>(); 
		}
		
		uint8_t readUInt8()	
		{
			return read<uint8_t>(); 
		}
		
		int16_t readInt16()	
		{
			return read<int16_t>(); 
		}
		
		uint16_t readUInt16() 
		{
			return read<uint16_t>(); 
		}
		
		int32_t readInt32()	
		{
			return read<int32_t>(); 
		}
		
		uint32_t readUInt32() 
		{
			return read<uint32_t>(); 
		}
		
		int64_t readInt64()
		{ 
			return read<int64_t>(); 
		}
		
		uint64_t readUInt64() 
		{
			return read<uint64_t>(); 
		}
		
		std::string readUTF8(bool checkQuote)
		{
			uint16_t len = readInt16();

			if (m_rOverFlow)
			{
				return "";
			}

			if (m_rOffset + len > m_wOffset)
			{
				LOG_ERROR("netpacket type = %d, proc = %d read overflow\n", (int)type, (int)m_proc);
				m_rOverFlow = true;
				return "";
			}

			if (checkQuote)
			{
				for (size_t i = 0; i < len; ++i)
				{
					if (*(m_buffer+m_rOffset+i) == '\'')
					{
						*(m_buffer+m_rOffset+i) = '\?';
					}
				}
			}

			m_rOffset += len;

			return std::string(m_buffer+m_rOffset-len, len);
		}

		void writeInt8(int8_t val) 
		{
			write<int8_t>(val); 
		}
		
		void writeUInt8(uint8_t val) 
		{
			write<uint8_t>(val); 
		}
		
		void writeInt16(int16_t val) 
		{
			write<int16_t>(val); 
		}
		
		void writeUInt16(uint16_t val) 
		{
			write<uint16_t>(val); 
		}
		
		void writeInt32(int32_t val) 
		{
			write<int32_t>(val); 
		}
		
		void writeUInt32(uint32_t val) 
		{
			write<uint32_t>(val); 
		}
		
		void writeInt64(int64_t val) 
		{
			write<int64_t>(val); 
		}
		
		void writeUInt64(uint64_t val) 
		{
			write<uint64_t>(val); 
		}
		
		void writeUTF8(const std::string &val)
		{
			writeInt16(static_cast<int16_t>(val.length()));
			write(val.c_str(), static_cast<uint32_t>(val.length()));
		}

		template<typename T>
		T read()
		{
			if (m_rOverFlow)
			{
				return T(0);
			}

			if (m_rOffset + sizeof(T) > m_wOffset)
			{
				LOG_ERROR("netpacket type = %d, proc = %d read overflow\n", (int)type, (int)m_proc);
				m_rOverFlow = true;
				return T(0);
			}

			m_rOffset += sizeof(T);

			return *((T*)(m_buffer+m_rOffset-sizeof(T)));
		}

		template<typename T>
		void write(T val)
		{
			if (m_wOverFlow)
			{
				return;
			}

			if (m_wOffset + sizeof(T) > m_bufferSize)
			{
				if (m_scalable)
				{
					reallocate();
				}

				if (m_wOffset + sizeof(T) > m_bufferSize)
				{
					LOG_ERROR("netpacket type = %d, proc = %d write overflow\n", (int)type, (int)m_proc);
					m_wOverFlow = true;
					return;
				}
			}

			*((T*)(m_buffer+m_wOffset)) = val;
			m_wOffset += sizeof(T);
		}

		void write(const char *buf, uint32_t size)
		{
			if (m_wOverFlow || buf == NULL)
			{
				return;
			}

			if (m_wOffset+size > m_bufferSize)
			{
				if (m_scalable)
				{
					reallocate();
				}

				if (m_wOffset+size > m_bufferSize)
				{
					LOG_ERROR("netpacket type = %d, proc = %d write overflow\n", (int)type, (int)m_proc);
					m_wOverFlow = true;
					return;
				}
			}

			memcpy(m_buffer+m_wOffset, buf, size);
			m_wOffset += size;
		}

		uint32_t getWOffset()
		{
			return m_wOffset;
		}

		uint32_t getROffset()
		{
			return m_rOffset;
		}

		uint32_t setWOffset(uint32_t offset)
		{
			uint32_t old = m_wOffset;

			if (offset <= m_bufferSize)
			{
				m_wOffset = offset;
			}

			return old;
		}

		bool isOverflow()
		{
			return m_wOverFlow;
		}

		PackType getType()
		{
			return type;
		}

		void setType(PackType packtype)
		{
			type = packtype;
		}

		uint16_t getProc()
		{
			return m_proc;
		}

		void setProc(uint16_t proc)
		{
			m_proc = proc;
		}

		uint32_t getSize()
		{
			return m_size;
		}

		void setSize(uint32_t packSize)
		{
			m_size = packSize;
		}

		bool isPolicy()
		{
			return m_policy;
		}

		void encodeHeadInfo()
		{
			char *buffer = m_buffer-NET_PACKET_HEAD_LEN;
			*((uint8_t*)buffer) = (uint8_t)type;
			*((uint16_t*)(buffer+1)) = m_proc;
			*((uint32_t*)(buffer+3)) = m_size;
			setSize(getSize()+NET_PACKET_HEAD_LEN);
		}

		bool rightShift(uint32_t size)
		{
			if (m_bufferSize-m_wOffset < size)
			{
				return false;
			}

			memmove(m_buffer+size, m_buffer, m_wOffset);
			m_wOffset = 0;
			return true;
		}

		long getRefCount()
		{
			return m_refCount;
		}

		void setRefCount(long count)
		{
			m_refCount = count;
		}

		long subRefCount()
		{
#ifdef LIBANSWER_WINDOWS
			return InterlockedDecrement(&m_refCount);
#endif

#ifdef LIBANSWER_LINUX
			return __sync_sub_and_fetch(&m_refCount, 1);
#endif
		}

		void destroy()
		{
			if (m_pool != NULL)
			{
				m_pool->push(this);
			}
			else
			{
				LOG_DEBUG("NetPacket %p with m_pool == NULL, delete this\n", this);
				delete this;
			}
		}

		void reallocate()
		{
			char *buffer = new char[m_bufferSize*2 + NET_PACKET_HEAD_LEN];
			memcpy(buffer, m_buffer-NET_PACKET_HEAD_LEN, m_bufferSize+NET_PACKET_HEAD_LEN);
			m_buffer -= NET_PACKET_HEAD_LEN;
			delete[] m_buffer;
			m_buffer = buffer + NET_PACKET_HEAD_LEN;
			m_bufferSize = m_bufferSize*2;
		}

	private:
		char *m_buffer;
		uint32_t m_bufferSize;
		uint32_t m_rOffset;
		uint32_t m_wOffset;
		bool m_rOverFlow;
		bool m_wOverFlow;
		bool m_scalable;
		bool m_policy;

		PackType type;
		uint16_t m_proc;
		uint32_t m_size;

		volatile long m_refCount;

		NetPacketPool *m_pool;
	};
}

