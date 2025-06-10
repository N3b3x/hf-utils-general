/**
  * Nebula Tech Corporation
  *
  * Copyright © 2024 Nebula Tech Corporation.   All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
  *
  * Contains the declaration and definition of the windowed buffer class template class.  It provides:
  *        Compile time size definition
  *        Support for any-copyable type.
  *        Ability to iterate forward and reverse thru the values.
  *
  *   Note:  These functions are not thread or interrupt-safe and should be called
  *          called with appropriate guards if used within an ISR or shared between tasks.
  */
#ifndef RingBuffer_h
#define RingBuffer_h

#include <stdint.h>
#include <array>
#include <cstddef>
#include <cstdarg>
#include <iterator>

template <typename Type, uint16_t BufferSize> class RingBuffer
{
	using value_type = Type;
	using reference = Type& ;
	using const_reference = const Type&;
	using size_type = uint16_t;

	public:
		/// <summary>
		/// Constructor allocates an empty buffer of a fixed size.
		/// </summary>

		RingBuffer() noexcept;

		/// <summary>
		/// Copy constructor and assignment operator are disabled.
		/// </summary>

		RingBuffer( const RingBuffer& ) = delete;

		RingBuffer& operator = ( const RingBuffer& ) = delete;

		/// <summary>
		/// Destructor is simple default, since there are no allocations
		/// </summary>

		~RingBuffer() = default;


		/// <summary>
		/// This helper function returns the number of elements in the buffer that are active.
		/// </summary>
		/// <returns>The number of elements in the buffer that are active.</returns>

		uint16_t GetCount() const noexcept;

		/// <summary>
		/// This helper function returns the total number of elements in thebuffer .
		/// </summary>
		/// <returns>The total number of elements in the  buffer.</returns>

		uint16_t GetSize() const noexcept;

		/// <summary>
		/// These helper functions return the state of the buffer.
		/// </summary>
		/// <returns>The appropriate state of the buffer.</returns>

		bool IsFull() const noexcept;

		bool IsEmpty() const noexcept;

		/// <summary>
		/// This function appends a single value to the head of the buffer.
		/// </summary>
		/// <param name="value">Value to append</param>
		/// <returns>True</returns>

		bool Append(const Type& value) noexcept;


		/// <summary>
		/// This function clears the contents of the buffer.
		/// </summary>

		void Erase() noexcept;


		bool empty() const noexcept
		{
			return GetCount() == 0;
		}

		/// <summary>
		/// This function identifies the index that is after the specified index.  It is intended to
		///   be used by forward iterators. If not empty, forward iterators start at the tail and move to
		///   the head.  Note .. should never call next on an empty collection.
		/// </summary>
		///  <param name="index">Current index value</param>
		/// <returns>The index of the "next" entry, or 0xFFFF if the end of the sequence is reached</returns>

		uint16_t Next(uint16_t currentIndex) const noexcept
		{
			uint16_t computedIndex = (++currentIndex) % BufferSize; // Get index to next logical location
			if( computedIndex  == head)
			{
					return 0XFFFF;   // reached the end
			}
			else // List is full
			{
				return computedIndex;
			}
		}

		/// <summary>
		/// This function identifies the index that is after the specified index.  It is intended to
		///   be used by reverse iterators. Note .. should never call next on an empty collection.
		/// </summary>
		///  <param name="index">Current index value</param>
		/// <returns>The index of the "previous" entry, or 0xFFFF if the end of the sequence is reached</returns>

		uint16_t Previous( uint16_t currentIndex) const noexcept
		{
			uint16_t computedIndex = (currentIndex == 0 ) ? (BufferSize - 1) :  (--currentIndex);
			if( full ) // We have wrapped around
			{
				if( computedIndex == tail)   // head == tail at this point
				{
					return 0XFFFF;   // reached the end
				}
			}
			else // Not full, so tail will be 0, and previous value will BufferSize - 1
			{
				if( computedIndex == (BufferSize - 1))
				{
				    return 0XFFFF;
				}
			}

			return computedIndex;
		}


		// Bidirectional iterator must be constructible, copy-constructible, copy-assignable, destructible and swappable.
		//   Instead of keeping a pointer into the std::array that stores the data, The index of the current values will
		//   be used to track position.
		//  Forward iteration will begin with the tail index, and increment to the head (adjusting for wrap-around as needed).
		//  Reverse iteration will begin with the head index and decrement to the tail (adjusting for wrap-around as needed).
		//  An index of 0xFFFF will be used as the "end" indicator.

		class iterator
		{
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using difference_type   = std::ptrdiff_t;
			using value_type        = Type;
			using pointer           = value_type*;
			using const_pointer     = const value_type*;
			using reference         = value_type&;
			using const_reference   = const value_type&;

			iterator(RingBuffer& ringBufferArg, uint16_t index ) noexcept :
				ringBuffer(&ringBufferArg),
			    currentIndex(index)
			{}

			iterator( const iterator& copy ) noexcept = default;

			iterator& operator = (const iterator& copy) noexcept
			{
				ringBuffer = copy.ringBuffer;
				currentIndex = copy.currentIndex;
				return *this;
			}

			~iterator() = default;

			reference operator*() noexcept { return ringBuffer->buffer.at(currentIndex); }

			pointer operator->() noexcept { return ringBuffer->buffer.data() + currentIndex; }

			iterator& operator++() noexcept  //Prefix increment
			{
				currentIndex = ringBuffer->Next(currentIndex);
				return *this;
			}

			iterator operator++(int) noexcept // Postfix increment
			{
				iterator tmp( *this );
				currentIndex = ringBuffer->Next(currentIndex);
				return tmp;
			}

			iterator& operator--() noexcept // Prefix decrement
			{
				currentIndex = ringBuffer->Previous(currentIndex);
			    return *this;
			}

			iterator operator--(int) noexcept  // Postfix decrement
			{
				iterator tmp( *this);
				currentIndex = ringBuffer->Previous(currentIndex);
				return tmp;
			}

			bool operator == (const iterator& other) noexcept { return (ringBuffer == other.ringBuffer) && (currentIndex == other.currentIndex); };

			bool operator!= (const iterator& other) noexcept { return !operator == (other); };

		private:
			RingBuffer* ringBuffer;
			uint16_t currentIndex;

		};

		iterator begin() noexcept
		{
			if( GetCount() > 0 )
			{
				return iterator( *this, tail );  // Oldest item inserted (always)
			}
			else
			{
				return iterator( *this, EndIndex );
			}
		}

		iterator end() noexcept
		{
			return iterator( *this, EndIndex  );
		}

		iterator rbegin() noexcept
		{
			if( GetCount() > 0 )
			{
				uint16_t previousIndex =  Previous(head);
				return iterator( *this, previousIndex );
			}
			else
			{
				return iterator( *this, EndIndex );
			}
		}

		iterator rend() noexcept
		{
			return iterator( *this, EndIndex );
		}


		class const_iterator
		{
			public:
				using iterator_category = std::bidirectional_iterator_tag;
				using difference_type   = std::ptrdiff_t;
				using value_type        = Type;
				using pointer           = value_type*;
				using const_pointer     = const value_type*;
				using reference         = value_type&;
				using const_reference   = const value_type&;

				const_iterator( const RingBuffer& ringBufferArg, uint16_t index ) noexcept :
					ringBuffer(&ringBufferArg),
					currentIndex(index)
			     {}


				const_iterator(const_iterator& copy ) noexcept = default;

				const_iterator( iterator& copy ) noexcept :
					ringBuffer(copy.ringBuffer),
					currentIndex(copy.currentIndex )
				 {}

				const_iterator& operator = (const const_iterator& copy) noexcept
				{
					ringBuffer = copy.ringBuffer;
					currentIndex = copy.currentIndex;
					return *this;
				}

				const_iterator& operator = (iterator& copy ) noexcept
				{
					ringBuffer = copy.ringBuffer;
					currentIndex = copy.currentIndex;
				    return *this;
				}

				~const_iterator() = default;

				const_reference operator*() noexcept { return ringBuffer->buffer.at(currentIndex); }

				const_pointer operator->() noexcept { return ringBuffer->buffer.data() + currentIndex;}

				const_iterator& operator++() noexcept  //Prefix increment
				{
					currentIndex = ringBuffer->Next(currentIndex);
					return *this;
				}

				const_iterator operator++(int) noexcept // Postfix increment
				{
					const_iterator tmp( *this );
					currentIndex = ringBuffer->Next(currentIndex);
					return tmp;
				}

				const_iterator& operator--() noexcept // Prefix decrement
				{
					currentIndex = ringBuffer->Previous(currentIndex);
					return *this;
				}

				const_iterator operator--(int) noexcept  // Postfix decrement
				{
					const_iterator tmp( *this );
					currentIndex = ringBuffer->Previous(currentIndex);
					return tmp;
				}

				bool operator == (const const_iterator& other) noexcept { return (ringBuffer == other.ringBuffer) && (currentIndex == other.currentIndex); };

				bool operator!= (const const_iterator& other) noexcept { return !operator == (other); };
			private:
				const RingBuffer* ringBuffer;
				uint16_t currentIndex;

			};

			const_iterator cbegin() const noexcept
			{
				if( GetCount() > 0 )
				{
					return const_iterator( *this, tail );  // Oldest item inserted (always)
				}
				else
				{
					return const_iterator( *this, EndIndex );
				}
			}

			const_iterator cend() const noexcept
			{
				return const_iterator(  *this, EndIndex  );
			}

			const_iterator crbegin() const noexcept
			{
				if( GetCount() > 0 )
				{
					uint16_t previousIndex = Previous(head);
					return const_iterator( *this, previousIndex );


				}
				else
				{
					return const_iterator( *this, EndIndex );
				}
			}

			const_iterator crend() const noexcept
			{
				return const_iterator( *this, EndIndex );
			}



	private:

		static constexpr uint16_t EndIndex = 0XFFFF;

        /// <summary>
        ///   This function advances the head or tail index .
        /// </summary>

        void Advance(uint16_t& entry) noexcept;


        std::array<Type, BufferSize> buffer;
        uint16_t head;   // Index to next point of insertion
        uint16_t tail;   // Index to next point of insertion
        bool full;
};

/// <summary>
/// Constructor allocates an empty circular buffer.
/// </summary>
template <typename Type, uint16_t BufferSize> RingBuffer<Type, BufferSize>::RingBuffer() noexcept :
   buffer(),
   head(0),   // Index to next point of insertion
   tail(0),  // Index to next point of insertion
   full( false )
{
     // No code at this time
}

/// <summary>
/// This helper function returns the number of elements in the circular buffer that have
/// been used.
/// </summary>
/// <returns>The number of elements in the circular buffer that have been used.</returns>

template <typename Type, uint16_t BufferSize> uint16_t RingBuffer<Type, BufferSize>::GetCount() const noexcept
{
    uint16_t count = BufferSize; // Presume it will be full

    if(!full)
    {
        if(head >= tail)
        {
            count = head - tail;
        }
        else
        {
            count = static_cast<uint16_t>(BufferSize + head - tail);
        }
    }

    return count;
}

/// <summary>
/// This helper function returns the total number of elements in the circular buffer .
/// </summary>
/// <returns>The total number of elements in the circular buffer.</returns>

template <typename Type, uint16_t BufferSize> uint16_t RingBuffer<Type, BufferSize>::GetSize() const noexcept
{
    return BufferSize;
}

/// <summary>
/// These helper functions return the state of the circular buffer .
/// </summary>
/// <returns>The appropriate state of the buffer.</returns>

template <typename Type, uint16_t BufferSize> bool RingBuffer<Type, BufferSize>::IsFull() const noexcept
{
   return full;
}

template <typename Type, uint16_t BufferSize> bool RingBuffer<Type, BufferSize>::IsEmpty() const noexcept
{
   return (!full && (head == tail));
}

/// <summary>
///   This function advances the head or tail pointer .
/// </summary>

template <typename Type, uint16_t BufferSize> void  RingBuffer<Type, BufferSize>::Advance( uint16_t& entry ) noexcept
{
    entry++;
    entry %= BufferSize;
}

/// <summary>
/// This function appends a single value to the buffer.
/// </summary>
/// <param name="value">Valueto append</param>
/// <returns>True</returns>

template <typename Type, uint16_t BufferSize> bool RingBuffer<Type, BufferSize>::Append(const Type& value) noexcept
{
    buffer[head] = value;
    if( full)
    {
      Advance(tail);
    }

    Advance( head);
    full = head == tail;
    return true;
}

/// <summary>
/// This function clears the contents of the buffer.
/// </summary>

template <class Type, uint16_t BufferSize> void RingBuffer<Type, BufferSize>::Erase() noexcept
{
   head = tail = 0;
   full = false;
}

#endif  //RingBuffer_h
