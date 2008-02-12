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

class   DataStream;
typedef DataStream *PDataStream;
typedef DataStream &RDataStream;

/** \deprecated */
class BaseExport DataStream
{
public:
  DataStream (void);
  virtual ~DataStream (void) {}

  unsigned long GetFlag (void);
  void SetFlag (unsigned long ulNewFlag);

  operator void* (void);
  bool operator! (void);

  virtual bool IsOpen (void) = 0;
  virtual bool IsGood (void) = 0;
  virtual bool IsEof (void)  = 0;
  virtual bool IsFail (void) = 0;
  virtual bool IsBad (void)  = 0;

  virtual void Flush (void) = 0;
  
  void SetFormatError (bool bErr);
  bool IsFormatError (void);

  bool GetSwap (void);
  void SetSwap (bool bNewSwab);

  friend DataStream& readSwapOrder (DataStream& rcs);
  friend DataStream& writeSwapOrder (DataStream& rcs);

  // Import-Methods
  DataStream& operator>> (DataStream& (*fct) (DataStream&));
  DataStream& operator>> (char &ch);
  DataStream& operator>> (unsigned char &uch);
  DataStream& operator>> (short &s);
  DataStream& operator>> (unsigned short &us);
  DataStream& operator>> (long &l);
  DataStream& operator>> (unsigned long &ul);
  DataStream& operator>> (float &f);
  DataStream& operator>> (double &d);
//  DataStream& operator>> (bool &b);


  virtual DataStream& Read (char &ch) = 0;
  virtual DataStream& Read (char* pData, int nSize) = 0;
  virtual DataStream& ReadLine (char* pData, int nSize,
                                           char cDelim = '\n');                                              
  // Export-Methods
  DataStream& operator<< (DataStream& (*fct) (DataStream&));
  DataStream& operator<< (char ch);
  DataStream& operator<< (unsigned char uch);
  DataStream& operator<< (short s);
  DataStream& operator<< (unsigned short us);
  DataStream& operator<< (long l);
  DataStream& operator<< (unsigned long ul);
  DataStream& operator<< (float f);
  DataStream& operator<< (double d);
 // DataStream& operator<< (bool b);

  virtual DataStream& Write (char ch) = 0;
  virtual DataStream& Write (const char* pData, int nSize) = 0;


protected:
  /// Init-Stream-Data
  void Init (void);
  /// User-format-error occured
  bool bFormatError;
  /// automatic byte-swapping
  bool bSwap;
  /// user-reserved flag
  unsigned long ulFlag;
};

//--------- CLASS FileStream -------------------------*/

class   FileStream;
typedef FileStream *PFileStream;
typedef FileStream &RFileStream;

/** \deprecated */
class BaseExport FileStream : public DataStream
{
protected:
  std::fstream *pfs;

public:
  //commented default parameter because of VC++ internel compiler error
  FileStream (const char *pszName,
      std::ios::openmode om /*= reinterpret_cast <std::ios::open_mode>(std::ios::in  | std::ios::out)*/);
  FileStream (void);
  virtual ~FileStream (void);

  bool Open (const char* pszName, std::ios::openmode om = (std::ios::openmode)(std::ios::in | std::ios::out));
  void  Close (void);

  void Flush (void);

  // Status-Methods
  operator void* (void);
  bool operator! (void);

  bool IsOpen (void);
  bool IsGood (void);
  bool IsEof (void);
  bool IsFail (void);
  bool IsBad (void);

  unsigned long FileSize (void);
  unsigned long Position (void);
  void SetPosition (unsigned long ulPos);

  // I/O-Methods
  DataStream& Read (char &ch);
  DataStream& Read (char* pData, int nSize);
  DataStream& ReadLine (char* pData, int nSize, char cDelim = '\n');
  DataStream& Write (char ch);
  DataStream& Write (const char* pData, int nSize);
};

//--------- CLASS FileBuffer -------------------------*/

/** \deprecated */
class BaseExport FileBuffer : public std::filebuf
{
public:
  void   SetPosition (unsigned long ulPos);
  unsigned long Position (void);
  unsigned long FileSize (void);
};

//--------- CLASS BlobStream -------------------------*/

class   BlobStream;
typedef BlobStream *PBlobStream;
typedef BlobStream &RBlobStream;

/** \deprecated */
class BaseExport BlobStream : public DataStream
{
protected:
  char *pBuf, *pszName;
  unsigned long ulBufSize, ulCursor;
  
public:
  BlobStream (void);
  virtual ~BlobStream (void);

  // Stream-Name
  void SetName (const char *pszBlobName);
  char *GetName (void);

  // Blob-Interface
  unsigned long GetPosition (void);
  char *GetBuffer (void);
  void SetBuffer (char *pBuffer, unsigned long ulSize);

  // Status-Methods
  operator void* (void);
  bool operator! (void);

  bool IsOpen (void);
  bool IsGood (void);
  bool IsEof (void);
  bool IsFail (void);
  bool IsBad (void);

  void Flush (void);

  // I/O-Methods
  void Rewind (void);
  DataStream& Read (char &ch);
  DataStream& Read (char* pData, int nSize);
  DataStream& Write (char ch);
  DataStream& Write (const char* pData, int nSize);
};

} // namespace Base

#endif // BASE_STREAM_H
