/***************************************************************************
 *   Copyright (c) 2005 Imetric 3D GmbH                                    *
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


#ifndef BASE_STREAM_H
#define BASE_STREAM_H


#ifndef _PreComp_
# include <fstream>
# include <ios>
# include <iostream>
# include <sstream>
# include <string>
# include <vector>
#endif

namespace Base {

class BaseExport Stream
{
public:
    enum ByteOrder { BigEndian, LittleEndian };
    
    ByteOrder byteOrder() const;
    void setByteOrder(ByteOrder);

protected:
    Stream();
    virtual ~Stream(); 

    bool _swap;
};

/**
 * The OutputStream class provides writing of binary data to an ostream.
 * @author Werner Mayer
 */
class BaseExport OutputStream : public Stream
{
public:
    OutputStream(std::ostream &rout);
    ~OutputStream();

    OutputStream& operator << (bool b);
    OutputStream& operator << (int8_t ch);
    OutputStream& operator << (uint8_t uch);
    OutputStream& operator << (int16_t s);
    OutputStream& operator << (uint16_t us);
    OutputStream& operator << (int32_t i);
    OutputStream& operator << (uint32_t ui);
    OutputStream& operator << (int64_t l);
    OutputStream& operator << (uint64_t ul);
    OutputStream& operator << (float f);
    OutputStream& operator << (double d);

private:
    OutputStream (const OutputStream&);
    void operator = (const OutputStream&);

private:
    std::ostream& _out;
};

/**
 * The InputStream class provides reading of binary data from an istream.
 * @author Werner Mayer
 */
class BaseExport InputStream : public Stream
{
public:
    InputStream(std::istream &rin);
    ~InputStream();

    InputStream& operator >> (bool& b);
    InputStream& operator >> (int8_t& ch);
    InputStream& operator >> (uint8_t& uch);
    InputStream& operator >> (int16_t& s);
    InputStream& operator >> (uint16_t& us);
    InputStream& operator >> (int32_t& i);
    InputStream& operator >> (uint32_t& ui);
    InputStream& operator >> (int64_t& l);
    InputStream& operator >> (uint64_t& ul);
    InputStream& operator >> (float& f);
    InputStream& operator >> (double& d);

private:
    InputStream (const InputStream&);
    void operator = (const InputStream&);

private:
    std::istream& _in;
};

} // namespace Base

#endif // BASE_STREAM_H
