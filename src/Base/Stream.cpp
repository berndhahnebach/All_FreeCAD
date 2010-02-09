/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <QDataStream>
# include <QIODevice>
# include <cstdlib>
# include <string>
# include <cstdio>
#ifdef __GNUC__
# include <stdint.h>
#endif
#endif

#include "Stream.h"
#include "Swap.h"
#include "FileInfo.h"

using namespace Base;

Stream::Stream() : _swap(false)
{
}

Stream::~Stream()
{
}

Stream::ByteOrder Stream::byteOrder() const
{
    return _swap ? BigEndian : LittleEndian;
}

void Stream::setByteOrder(ByteOrder bo)
{
    _swap = (bo == BigEndian);
}

OutputStream::OutputStream(std::ostream &rout) : _out(rout)
{
}

OutputStream::~OutputStream()
{
}

OutputStream& OutputStream::operator << (bool b)
{
    _out.write((const char*)&b, sizeof(bool));
    return *this;
}

OutputStream& OutputStream::operator << (int8_t ch)
{
    _out.write((const char*)&ch, sizeof(int8_t));
    return *this;
}

OutputStream& OutputStream::operator << (uint8_t uch)
{
    _out.write((const char*)&uch, sizeof(uint8_t));
    return *this;
}

OutputStream& OutputStream::operator << (int16_t s)
{
    if (_swap) SwapEndian<int16_t>(s);
    _out.write((const char*)&s, sizeof(int16_t));
    return *this;
}

OutputStream& OutputStream::operator << (uint16_t us)
{
    if (_swap) SwapEndian<uint16_t>(us);
    _out.write((const char*)&us, sizeof(uint16_t));
    return *this;
}

OutputStream& OutputStream::operator << (int32_t i)
{
    if (_swap) SwapEndian<int32_t>(i);
    _out.write((const char*)&i, sizeof(int32_t));
    return *this;
}

OutputStream& OutputStream::operator << (uint32_t ui)
{
    if (_swap) SwapEndian<uint32_t>(ui);
    _out.write((const char*)&ui, sizeof(uint32_t));
    return *this;
}

OutputStream& OutputStream::operator << (int64_t l)
{
    if (_swap) SwapEndian<int64_t>(l);
    _out.write((const char*)&l, sizeof(int64_t));
    return *this;
}

OutputStream& OutputStream::operator << (uint64_t ul)
{
    if (_swap) SwapEndian<uint64_t>(ul);
    _out.write((const char*)&ul, sizeof(uint64_t));
    return *this;
}

OutputStream& OutputStream::operator << (float f)
{
    if (_swap) SwapEndian<float>(f);
    _out.write((const char*)&f, sizeof(float));
    return *this;
}

OutputStream& OutputStream::operator << (double d)
{
    if (_swap) SwapEndian<double>(d);
    _out.write((const char*)&d, sizeof(double));
    return *this;
}

InputStream::InputStream(std::istream &rin) : _in(rin)
{
}

InputStream::~InputStream()
{
}

InputStream& InputStream::operator >> (bool& b)
{
    _in.read((char*)&b, sizeof(bool));
    return *this;
}

InputStream& InputStream::operator >> (int8_t& ch)
{
    _in.read((char*)&ch, sizeof(int8_t));
    return *this;
}

InputStream& InputStream::operator >> (uint8_t& uch)
{
    _in.read((char*)&uch, sizeof(uint8_t));
    return *this;
}

InputStream& InputStream::operator >> (int16_t& s)
{
    _in.read((char*)&s, sizeof(int16_t));
    if (_swap) SwapEndian<int16_t>(s);
    return *this;
}

InputStream& InputStream::operator >> (uint16_t& us)
{
    _in.read((char*)&us, sizeof(uint16_t));
    if (_swap) SwapEndian<uint16_t>(us);
    return *this;
}

InputStream& InputStream::operator >> (int32_t& i)
{
    _in.read((char*)&i, sizeof(int32_t));
    if (_swap) SwapEndian<int32_t>(i);
    return *this;
}

InputStream& InputStream::operator >> (uint32_t& ui)
{
    _in.read((char*)&ui, sizeof(uint32_t));
    if (_swap) SwapEndian<uint32_t>(ui);
    return *this;
}

InputStream& InputStream::operator >> (int64_t& l)
{
    _in.read((char*)&l, sizeof(int64_t));
    if (_swap) SwapEndian<int64_t>(l);
    return *this;
}

InputStream& InputStream::operator >> (uint64_t& ul)
{
    _in.read((char*)&ul, sizeof(uint64_t));
    if (_swap) SwapEndian<uint64_t>(ul);
    return *this;
}

InputStream& InputStream::operator >> (float& f)
{
    _in.read((char*)&f, sizeof(float));
    if (_swap) SwapEndian<float>(f);
    return *this;
}

InputStream& InputStream::operator >> (double& d)
{
    _in.read((char*)&d, sizeof(double));
    if (_swap) SwapEndian<double>(d);
    return *this;
}

// ----------------------------------------------------------------------

IODeviceOStreambuf::IODeviceOStreambuf(QIODevice* dev) : device(dev)
{
}

IODeviceOStreambuf::~IODeviceOStreambuf()
{
}

std::streambuf::int_type
IODeviceOStreambuf::overflow(std::streambuf::int_type c)
{
    if (c != EOF) {
        char z = c;
        if (device->write (&z, 1) != 1) {
            return EOF;
        }
    }
    return c;
}

std::streamsize IODeviceOStreambuf::xsputn (const char* s, std::streamsize num)
{
    return device->write(s,num);
}

// ----------------------------------------------------------------------

IODeviceIStreambuf::IODeviceIStreambuf(QIODevice* dev) : device(dev)
{
    setg (buffer+pbSize,     // beginning of putback area
          buffer+pbSize,     // read position
          buffer+pbSize);    // end position
}

IODeviceIStreambuf::~IODeviceIStreambuf()
{
}

std::streambuf::int_type
IODeviceIStreambuf::underflow()
{
#ifndef _MSC_VER
using std::memcpy;
#endif

    // is read position before end of buffer?
    if (gptr() < egptr()) {
        return *gptr();
    }

    /* process size of putback area
     * - use number of characters read
     * - but at most size of putback area
     */
    int numPutback;
    numPutback = gptr() - eback();
    if (numPutback > pbSize) {
        numPutback = pbSize;
    }

    /* copy up to pbSize characters previously read into
     * the putback area
     */
    memcpy (buffer+(pbSize-numPutback), gptr()-numPutback,
            numPutback);

    // read at most bufSize new characters
    int num;
    num = device->read(buffer+pbSize, bufSize);
    if (num <= 0) {
        // ERROR or EOF
        return EOF;
    }

    // reset buffer pointers
    setg (buffer+(pbSize-numPutback),   // beginning of putback area
          buffer+pbSize,                // read position
          buffer+pbSize+num);           // end of buffer

    // return next character
    return *gptr();
}

// ---------------------------------------------------------

Base::ofstream::ofstream(const FileInfo& fi, ios_base::openmode mode)
#ifdef _MSC_VER
: std::ofstream(fi.toStdWString().c_str(), mode)
#else
: std::ofstream(fi.filePath().c_str(), mode)
#endif
{
}

Base::ofstream::~ofstream()
{
}

Base::ifstream::ifstream(const FileInfo& fi, ios_base::openmode mode)
#ifdef _MSC_VER
: std::ifstream(fi.toStdWString().c_str(), mode)
#else
: std::ifstream(fi.filePath().c_str(), mode)
#endif
{
}

Base::ifstream::~ifstream()
{
}

