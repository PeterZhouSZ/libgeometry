// This file is part of Eigen, a lightweight C++ template library
// for linear algebra.
//
// Copyright (C) 2008 Gael Guennebaud <g.gael@free.fr>
// Copyright (C) 2006-2009 Benoit Jacob <jacob.benoit.1@gmail.com>
//
// Eigen is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Eigen is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// Eigen. If not, see <http://www.gnu.org/licenses/>.

#ifndef EIGEN_MATRIXSTORAGE_H
#define EIGEN_MATRIXSTORAGE_H

struct ei_constructor_without_unaligned_array_assert {};

/** \internal
  * Static array. If the MatrixOptions require auto-alignment, the array will be automatically aligned:
  * to 16 bytes boundary if the total size is a multiple of 16 bytes.
  */
template <typename T, int Size, int MatrixOptions,
          int Alignment = (MatrixOptions&DontAlign) ? 0
                        : (((Size*sizeof(T))%16)==0) ? 16
                        : 0 >
struct ei_matrix_array
{
  T array[Size];
  ei_matrix_array() {}
  ei_matrix_array(ei_constructor_without_unaligned_array_assert) {}
};

#ifdef EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
  #define EIGEN_MAKE_UNALIGNED_ARRAY_ASSERT(sizemask)
#else
  #define EIGEN_MAKE_UNALIGNED_ARRAY_ASSERT(sizemask) \
    ei_assert((reinterpret_cast<size_t>(array) & sizemask) == 0 \
              && "this assertion is explained here: " \
              "http://eigen.tuxfamily.org/dox/UnalignedArrayAssert.html" \
              " **** READ THIS WEB PAGE !!! ****");
#endif

template <typename T, int Size, int MatrixOptions>
struct ei_matrix_array<T, Size, MatrixOptions, 16>
{
  EIGEN_ALIGN16 T array[Size];
  ei_matrix_array() { EIGEN_MAKE_UNALIGNED_ARRAY_ASSERT(0xf) }
  ei_matrix_array(ei_constructor_without_unaligned_array_assert) {}
};

/** \internal
  *
  * \class ei_matrix_storage
  *
  * \brief Stores the data of a matrix
  *
  * This class stores the data of fixed-size, dynamic-size or mixed matrices
  * in a way as compact as possible.
  *
  * \sa Matrix
  */
template<typename T, int Size, int _Rows, int _Cols, int _Options> class ei_matrix_storage;

// purely fixed-size matrix
template<typename T, int Size, int _Rows, int _Cols, int _Options> class ei_matrix_storage
{
    ei_matrix_array<T,Size,_Options> m_data;
  public:
    inline explicit ei_matrix_storage() {}
    inline ei_matrix_storage(ei_constructor_without_unaligned_array_assert)
      : m_data(ei_constructor_without_unaligned_array_assert()) {}
    inline ei_matrix_storage(int,int,int) {}
    inline void swap(ei_matrix_storage& other) { std::swap(m_data,other.m_data); }
    inline static int rows(void) {return _Rows;}
    inline static int cols(void) {return _Cols;}
    inline void resize(int,int,int) {}
    inline const T *data() const { return m_data.array; }
    inline T *data() { return m_data.array; }
};

// null matrix
template<typename T, int _Rows, int _Cols, int _Options> class ei_matrix_storage<T, 0, _Rows, _Cols, _Options>
{
  public:
    inline explicit ei_matrix_storage() {}
    inline ei_matrix_storage(ei_constructor_without_unaligned_array_assert) {}
    inline ei_matrix_storage(int,int,int) {}
    inline void swap(ei_matrix_storage& ) {}
    inline static int rows(void) {return _Rows;}
    inline static int cols(void) {return _Cols;}
    inline void resize(int,int,int) {}
    inline const T *data() const { return 0; }
    inline T *data() { return 0; }
};

// dynamic-size matrix with fixed-size storage
template<typename T, int Size, int _Options> class ei_matrix_storage<T, Size, Dynamic, Dynamic, _Options>
{
    ei_matrix_array<T,Size,_Options> m_data;
    int m_rows;
    int m_cols;
  public:
    inline explicit ei_matrix_storage() : m_rows(0), m_cols(0) {}
    inline ei_matrix_storage(ei_constructor_without_unaligned_array_assert)
      : m_data(ei_constructor_without_unaligned_array_assert()), m_rows(0), m_cols(0) {}
    inline ei_matrix_storage(int, int rows, int cols) : m_rows(rows), m_cols(cols) {}
    inline void swap(ei_matrix_storage& other)
    { std::swap(m_data,other.m_data); std::swap(m_rows,other.m_rows); std::swap(m_cols,other.m_cols); }
    inline int rows(void) const {return m_rows;}
    inline int cols(void) const {return m_cols;}
    inline void resize(int, int rows, int cols)
    {
      m_rows = rows;
      m_cols = cols;
    }
    inline const T *data() const { return m_data.array; }
    inline T *data() { return m_data.array; }
};

// dynamic-size matrix with fixed-size storage and fixed width
template<typename T, int Size, int _Cols, int _Options> class ei_matrix_storage<T, Size, Dynamic, _Cols, _Options>
{
    ei_matrix_array<T,Size,_Options> m_data;
    int m_rows;
  public:
    inline explicit ei_matrix_storage() : m_rows(0) {}
    inline ei_matrix_storage(ei_constructor_without_unaligned_array_assert)
      : m_data(ei_constructor_without_unaligned_array_assert()), m_rows(0) {}
    inline ei_matrix_storage(int, int rows, int) : m_rows(rows) {}
    inline void swap(ei_matrix_storage& other) { std::swap(m_data,other.m_data); std::swap(m_rows,other.m_rows); }
    inline int rows(void) const {return m_rows;}
    inline int cols(void) const {return _Cols;}
    inline void resize(int /*size*/, int rows, int)
    {
      m_rows = rows;
    }
    inline const T *data() const { return m_data.array; }
    inline T *data() { return m_data.array; }
};

// dynamic-size matrix with fixed-size storage and fixed height
template<typename T, int Size, int _Rows, int _Options> class ei_matrix_storage<T, Size, _Rows, Dynamic, _Options>
{
    ei_matrix_array<T,Size,_Options> m_data;
    int m_cols;
  public:
    inline explicit ei_matrix_storage() : m_cols(0) {}
    inline ei_matrix_storage(ei_constructor_without_unaligned_array_assert)
      : m_data(ei_constructor_without_unaligned_array_assert()), m_cols(0) {}
    inline ei_matrix_storage(int, int, int cols) : m_cols(cols) {}
    inline void swap(ei_matrix_storage& other) { std::swap(m_data,other.m_data); std::swap(m_cols,other.m_cols); }
    inline int rows(void) const {return _Rows;}
    inline int cols(void) const {return m_cols;}
    inline void resize(int, int, int cols)
    {
      m_cols = cols;
    }
    inline const T *data() const { return m_data.array; }
    inline T *data() { return m_data.array; }
};

// purely dynamic matrix.
template<typename T, int _Options> class ei_matrix_storage<T, Dynamic, Dynamic, Dynamic, _Options>
{
    T *m_data;
    int m_rows;
    int m_cols;
  public:
    inline explicit ei_matrix_storage() : m_data(0), m_rows(0), m_cols(0) {}
    inline ei_matrix_storage(ei_constructor_without_unaligned_array_assert)
       : m_data(0), m_rows(0), m_cols(0) {}
    inline ei_matrix_storage(int size, int rows, int cols)
      : m_data(ei_conditional_aligned_new<T,(_Options&DontAlign)==0>(size)), m_rows(rows), m_cols(cols) {}
    inline ~ei_matrix_storage() { ei_conditional_aligned_delete<T,(_Options&DontAlign)==0>(m_data, m_rows*m_cols); }
    inline void swap(ei_matrix_storage& other)
    { std::swap(m_data,other.m_data); std::swap(m_rows,other.m_rows); std::swap(m_cols,other.m_cols); }
    inline int rows(void) const {return m_rows;}
    inline int cols(void) const {return m_cols;}
    void resize(int size, int rows, int cols)
    {
      if(size != m_rows*m_cols)
      {
        ei_conditional_aligned_delete<T,(_Options&DontAlign)==0>(m_data, m_rows*m_cols);
        if (size)
          m_data = ei_conditional_aligned_new<T,(_Options&DontAlign)==0>(size);
        else
          m_data = 0;
      }
      m_rows = rows;
      m_cols = cols;
    }
    inline const T *data() const { return m_data; }
    inline T *data() { return m_data; }
};

// matrix with dynamic width and fixed height (so that matrix has dynamic size).
template<typename T, int _Rows, int _Options> class ei_matrix_storage<T, Dynamic, _Rows, Dynamic, _Options>
{
    T *m_data;
    int m_cols;
  public:
    inline explicit ei_matrix_storage() : m_data(0), m_cols(0) {}
    inline ei_matrix_storage(ei_constructor_without_unaligned_array_assert) : m_data(0), m_cols(0) {}
    inline ei_matrix_storage(int size, int, int cols) : m_data(ei_conditional_aligned_new<T,(_Options&DontAlign)==0>(size)), m_cols(cols) {}
    inline ~ei_matrix_storage() { ei_conditional_aligned_delete<T,(_Options&DontAlign)==0>(m_data, _Rows*m_cols); }
    inline void swap(ei_matrix_storage& other) { std::swap(m_data,other.m_data); std::swap(m_cols,other.m_cols); }
    inline static int rows(void) {return _Rows;}
    inline int cols(void) const {return m_cols;}
    void resize(int size, int, int cols)
    {
      if(size != _Rows*m_cols)
      {
        ei_conditional_aligned_delete<T,(_Options&DontAlign)==0>(m_data, _Rows*m_cols);
        if (size)
          m_data = ei_conditional_aligned_new<T,(_Options&DontAlign)==0>(size);
        else
          m_data = 0;
      }
      m_cols = cols;
    }
    inline const T *data() const { return m_data; }
    inline T *data() { return m_data; }
};

// matrix with dynamic height and fixed width (so that matrix has dynamic size).
template<typename T, int _Cols, int _Options> class ei_matrix_storage<T, Dynamic, Dynamic, _Cols, _Options>
{
    T *m_data;
    int m_rows;
  public:
    inline explicit ei_matrix_storage() : m_data(0), m_rows(0) {}
    inline ei_matrix_storage(ei_constructor_without_unaligned_array_assert) : m_data(0), m_rows(0) {}
    inline ei_matrix_storage(int size, int rows, int) : m_data(ei_conditional_aligned_new<T,(_Options&DontAlign)==0>(size)), m_rows(rows) {}
    inline ~ei_matrix_storage() { ei_conditional_aligned_delete<T,(_Options&DontAlign)==0>(m_data, _Cols*m_rows); }
    inline void swap(ei_matrix_storage& other) { std::swap(m_data,other.m_data); std::swap(m_rows,other.m_rows); }
    inline int rows(void) const {return m_rows;}
    inline static int cols(void) {return _Cols;}
    void resize(int size, int rows, int)
    {
      if(size != m_rows*_Cols)
      {
        ei_conditional_aligned_delete<T,(_Options&DontAlign)==0>(m_data, _Cols*m_rows);
        if (size)
          m_data = ei_conditional_aligned_new<T,(_Options&DontAlign)==0>(size);
        else
          m_data = 0;
      }
      m_rows = rows;
    }
    inline const T *data() const { return m_data; }
    inline T *data() { return m_data; }
};

#endif // EIGEN_MATRIX_H
