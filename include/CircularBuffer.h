/**
 * @file CircularBuffer.h
 * @brief Fixed-size circular buffer template.
 *
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation.   All Rights Reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public
 * License v3.0 or later.
 *
 * Contains the declaration and definition of the CircularBuffer template class.
 * It provides compile-time size definition and support for any copyable type.
 *
 * Note: These functions are not thread or interrupt-safe and should be called
 * with appropriate guards if used within an ISR or shared between tasks.
 */
#ifndef CircularBuffer_h
#define CircularBuffer_h

#include <stdint.h>
#include <array>
#include <cstddef>
#include <cstdarg>

template <typename Type, uint16_t BufferSize> class CircularBuffer
{
   using value_type = Type;
   using reference = Type& ;
   using const_reference = const Type&;
   using size_type = uint16_t;

   public:
      /// <summary>
      /// Constructor allocates an empty circular buffer.
      /// </summary>

      CircularBuffer() noexcept;

      /// <summary>
      /// Copy constructor and assignment operator are disabled.
      /// </summary>

      CircularBuffer( const CircularBuffer& ) = delete;

      CircularBuffer& operator = ( const CircularBuffer& ) = delete;

      /// <summary>
      /// Destructor is simple default, since there are no allocations
      /// </summary>

      ~CircularBuffer() = default;

      /// <summary>
      /// This helper function returns the number of elements in the circular buffer that have
      /// been used.
      /// </summary>
      /// <returns>The number of elements in the circular buffer that have been used.</returns>

      uint16_t GetCount() const noexcept;

      /// <summary>
      /// This helper function returns the total number of elements in the circular buffer .
      /// </summary>
      /// <returns>The total number of elements in the circular buffer.</returns>

      uint16_t GetSize() const noexcept;

      /// <summary>
      /// These helper functions return the state of the circular buffer.
      /// </summary>
      /// <returns>The appropriate state of the buffer.</returns>

      bool IsFull() const noexcept;

      bool IsEmpty() const noexcept;

      /// <summary>
      /// This function appends a set of values to the buffer.
      /// </summary>
      /// <param name="data">Address of the data</param>
      /// <param name="count">Number of elements</param>
      /// <returns>True</returns>

      bool Write(const Type* data, uint16_t count) noexcept;

      /// <summary>
      /// This function appends a single value to the buffer.
      /// </summary>
      /// <param name="value">Value to append</param>
      /// <returns>True</returns>

      bool Write(const Type& value) noexcept;


      /// <summary>
      /// This function extracts the next available value from the buffer.
      /// </summary>
      /// <param name="value">Next available value</param>
      /// <returns>True if non-empty, false if empty.</returns>

      bool Read(Type&value) noexcept;

      /// <summary>
      /// This function extracts the next set of available values from the buffer.
      /// </summary>
      /// <param name="value">Next available values</param>
      /// <returns>Number of elements extracted.</returns>
      uint16_t Read(Type* buffer, uint16_t maxBytes) noexcept;

      /// <summary>
      /// This function clears the contents of the buffer.
      /// </summary>

      void Erase() noexcept;


   private:

        /// <summary>
        ///   This function advances the head or tail index .
        /// </summary>

        void Advance(uint16_t& entry) noexcept;

        std::array<Type, BufferSize> buffer;
        uint16_t head;
        uint16_t tail;
        bool full;
};

/// <summary>
/// Constructor allocates an empty circular buffer.
/// </summary>
template <typename Type, uint16_t BufferSize> CircularBuffer<Type, BufferSize>::CircularBuffer() noexcept :
   buffer(),
   head(0),
   tail(0),
   full( false )
{
     // No code at this time
}

/// <summary>
/// This helper function returns the number of elements in the circular buffer that have
/// been used.
/// </summary>
/// <returns>The number of elements in the circular buffer that have been used.</returns>

template <typename Type, uint16_t BufferSize> uint16_t CircularBuffer<Type, BufferSize>::GetCount() const noexcept
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

template <typename Type, uint16_t BufferSize> uint16_t CircularBuffer<Type, BufferSize>::GetSize() const noexcept
{
    return BufferSize;
}

/// <summary>
/// These helper functions return the state of the circular buffer .
/// </summary>
/// <returns>The appropriate state of the buffer.</returns>

template <typename Type, uint16_t BufferSize> bool CircularBuffer<Type, BufferSize>::IsFull() const noexcept
{
   return full;
}

template <typename Type, uint16_t BufferSize> bool CircularBuffer<Type, BufferSize>::IsEmpty() const noexcept
{
   return (!full && (head == tail));
}


/// <summary>
///   This function advances the head or tail pointer .
/// </summary>

template <typename Type, uint16_t BufferSize> void  CircularBuffer<Type, BufferSize>::Advance( uint16_t& entry ) noexcept
{
    entry++;
    entry %= BufferSize;
}

/// <summary>
/// This function appends a set of values to the buffer.
/// </summary>
/// <param name="data">Address of the data</param>
/// <param name="count">Number of elements</param>
/// <returns>True</returns>

template <typename Type, uint16_t BufferSize> bool CircularBuffer<Type, BufferSize>::Write(const Type* data, uint16_t count) noexcept
{
   // Add the data items one at a time
   for (uint16_t i = 0; i < count; ++i)
   {
      Write( *data++ );
   }
   return true;
}

/// <summary>
/// This function appends a single value to the buffer.
/// </summary>
/// <param name="value">Valueto append</param>
/// <returns>True</returns>

template <typename Type, uint16_t BufferSize> bool CircularBuffer<Type, BufferSize>::Write(const Type& value) noexcept
{
    buffer[head] = value;
    if( full)
    {
  //  tail = (tail + (uint16_t)1) % BufferSize;
      Advance( tail);
    }

    //  head = (head + (uint16_t)1) % BufferSize;
    Advance( head);
    full = head == tail;
    return true;
}
/// <summary>
/// This function extracts the next available value from the buffer.
/// </summary>
/// <param name="value">Next available value</param>
/// <returns>True if non-empty, false if empty.</returns>

template <typename Type, uint16_t BufferSize> bool CircularBuffer<Type, BufferSize>::Read(Type& value) noexcept
{
   if( !IsEmpty() )
    {
      value = buffer[tail];
      full = false;
  //    tail = (tail + (uint16_t)1) % BufferSize;
      Advance( tail);
      return true;
   }
   return false;
}

/// <summary>
/// This function extracts the next set of available values from the buffer.
/// </summary>
/// <param name="value">Next available values</param>
/// <returns>Number of elements extracted.</returns>

template <typename Type, uint16_t BufferSize> uint16_t CircularBuffer<Type, BufferSize>::Read(Type* bufferArg, uint16_t maxBytes) noexcept
{
   uint16_t bytesRead = 0;

   while( !IsEmpty() && (bytesRead < maxBytes ))
   {
      bufferArg[bytesRead] = buffer[tail];
      full = false;
 //     tail = (tail + (uint16_t)1) % BufferSize;
      Advance( tail);
      ++bytesRead;
   }
   return bytesRead;
}

/// <summary>
/// This function clears the contents of the buffer.
/// </summary>

template <class Type, uint16_t BufferSize> void CircularBuffer<Type, BufferSize>::Erase() noexcept
{
   head = tail = 0;
   full = false;
}

#endif  //CircularBuffer_h
