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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
#endif

#include "Stream.h"
#include "Swap.h"

using namespace Base;

/** Initializes the stream data. This method should be called before 
 * Open() and after Close() calls.
 */
void DataStream::Init (void)
{
  bSwap = false;
  bFormatError = false;
  ulFlag = 0;
}

/** Construction. */
DataStream::DataStream (void)
{
  Init ();
}

/** Returns the user-stream flag. */
unsigned long DataStream::GetFlag (void)
{
  return ulFlag;
}

/** Sets the user-stream flag. */
void DataStream::SetFlag (unsigned long ulNewFlag)
{
  ulFlag = ulNewFlag;
}

/** Returns the swap mode. */
bool DataStream::GetSwap (void)
{
  return bSwap;
}

/** Sets the swap mode. */
void DataStream::SetSwap (bool bNewSwap)
{
  bSwap = bNewSwap;
}

/** Sets format error flag. */
void  DataStream::SetFormatError (bool bErr)
{
  bFormatError = bErr;
}

/** Returns format error flag. */
bool DataStream::IsFormatError (void)
{
  return bFormatError;
}

/** Stream manipulator that reads an id from the current stream. */
DataStream& Base::readSwapOrder (DataStream& rcs)
{
  unsigned short usOrder;
  rcs >> usOrder;
  rcs.bSwap = usOrder != SwapOrder ();
  return rcs;
}

/** Stream manipulator that writes an id to the current stream. */
DataStream& Base::writeSwapOrder (DataStream& rcs)
{
  unsigned short usOrder = SwapOrder ();
  rcs << usOrder;
  return rcs;
}

/** void operator */
DataStream::operator void* (void)
{
  return (void*) (IsGood () && (!IsFormatError ()));
}

/** ! operator */
bool DataStream::operator! (void)
{
  return !this->operator void* ();
}

/** input operator */
DataStream& DataStream::operator>> (DataStream& (*fct) (DataStream&))
{
  return (*fct) (*this);
}

/** input operator */
DataStream& DataStream::operator>> (char &ch)
{
  return Read (ch);
}

/** input operator */
DataStream& DataStream::operator>> (unsigned char &uch)
{
  return *this >> (char&) uch;
}

/** input operator */
DataStream& DataStream::operator>> (short &s)
{
  Read ((char*) &s, sizeof (short));
  if (bSwap) SwapVar (s);
  return *this;
}

/** input operator */
DataStream& DataStream::operator>> (unsigned short &us)
{
  *this >> (short&) us;
  return *this;
}

/** input operator */
DataStream& DataStream::operator>> (long &l)
{
  Read ((char*) &l, sizeof (long));
  if (bSwap) SwapVar (l);
  return *this;
}

/** input operator */
DataStream& DataStream::operator>> (unsigned long &ul)
{
  *this >> (long&) ul;
  return *this;
}

/** input operator */
DataStream& DataStream::operator>> (float &f)
{
  Read ((char*) &f, sizeof (float));
  if (bSwap) SwapVar (f);
  return *this;
}

/** input operator */
DataStream& DataStream::operator>> (double &d)
{
  Read ((char*) &d, sizeof (double));
  if (bSwap) SwapVar (d);
  return *this;
}

/** output operator */
DataStream& DataStream::operator<< (DataStream& (*fct) (DataStream&))
{
  return (*fct) (*this);
}

/** output operator */
DataStream& DataStream::operator<< (char ch)
{
  return Write (ch);
}

/** output operator */
DataStream& DataStream::operator<< (unsigned char uch)
{
  return *this << (char) uch;
}

/** output operator */
DataStream& DataStream::operator<< (short s)
{
  if (bSwap) SwapVar (s);
  return Write ((char*) &s, sizeof (short));
}

/** output operator */
DataStream& DataStream::operator<< (unsigned short us)
{
  return *this << (short) us;
}

/** output operator */
DataStream& DataStream::operator<< (long l)
{
  if (bSwap) SwapVar (l);
  return Write ((char*) &l, sizeof (long));
}

/** output operator */
DataStream& DataStream::operator<< (unsigned long ul)
{
  return *this << (long) ul;
}

/** output operator */
DataStream& DataStream::operator<< (float f)
{
  if (bSwap) SwapVar ((float&)f);
  return Write ((char*) &f, sizeof (float));
}

/** output operator */
DataStream& DataStream::operator<< (double d)
{
  if (bSwap) SwapVar ((double&) d);
  return Write ((char*) &d, sizeof (double));
}

/** Reads a line from the stream. */
DataStream& DataStream::ReadLine (char* pData, int nSize, char cDelim)
{
  unsigned long i;
  char  cCh;
  
  i   = 0;
  cCh = 0; 
  while ((i < (unsigned long)nSize) && (cCh != cDelim))
  {
    Read(cCh);
    pData[i] = cCh;
    i++;
  }
   
  return *this;
}

//--------- CLASS FileStream -------------------------*/

/** Construction. */
FileStream::FileStream (void)
  : DataStream ()
{
  pfs = NULL;
}

/** Construction. */
FileStream::FileStream (const char *pszName, std::ios::openmode om)
  : DataStream ()
{
  pfs = NULL;
  Open (pszName, om);
}

/** Denstruction. Closes an open file. */
FileStream::~FileStream (void)
{
  Close();
  if (pfs)
    delete pfs;
}

/** Opens a file. */
bool FileStream::Open (const char* pszName, std::ios::openmode om)
{
  // Close open stream
  if (IsOpen ())
    Close ();

  // Init base-stream
  Init ();

  // open new stream
  pfs = new std::fstream;
  if (pfs)
  {
#ifdef FC_OS_WIN32  
    om = (std::ios::open_mode)(om | std::ios::binary);
#endif
    bSwap = false;
    pfs->open (pszName, om);
    return pfs->good ();
  }
  else
    return false;
}

/** Closes an open file. */
void FileStream::Close (void)
{ 
  if (pfs != 0)
  {
    pfs->flush();
    pfs->close ();
  }
  
  pfs = NULL;

  // Init base-stream
  Init ();
}

/** Clears the stream. */
void FileStream::Flush (void)
{ 
  if (!pfs)
    return;
    
  pfs->flush();
}

FileStream::operator void* (void)
{
  return DataStream::operator void* ();
}

bool FileStream::operator! (void)
{
  return DataStream::operator! ();
}

/** Returns true if the file is open, otherwise false is returned. */
bool FileStream::IsOpen (void)
{
  return pfs != NULL;
}

/** Returns the state of the stream. */
bool FileStream::IsGood (void)
{
  return pfs ? pfs->good () : false;
}

/** Returns the EOF state of the stream. */
bool FileStream::IsEof (void)
{
  return pfs ? pfs->eof () : false;
}

/** Retruns the error state of the stream. */
bool FileStream::IsFail (void)
{
  return pfs ? pfs->fail () : true;
}

/** Retruns the error state of the stream. */
bool FileStream::IsBad (void)
{
  return pfs ? pfs->bad () : true;
}

/** Returns the size of the open file. */
unsigned long FileStream::FileSize (void)
{
  FileBuffer  *pfsbuf;
 
  pfsbuf = (FileBuffer*)pfs->rdbuf();

  if (pfsbuf == 0)
    return 0;

  return pfsbuf->FileSize();  
}

/** Returns the current read position. */
unsigned long FileStream::Position (void)
{
  FileBuffer  *pfsbuf;
 
  pfsbuf = (FileBuffer*)pfs->rdbuf();

  if (pfsbuf == 0)
    return 0;
  
  return pfsbuf->Position();
}

/** Sets the position in the stream. */
void FileStream::SetPosition (unsigned long ulPos)
{
  FileBuffer  *pfsbuf;
 
  pfsbuf = (FileBuffer*)pfs->rdbuf();

  if (pfsbuf == 0)
    return;

  pfsbuf->SetPosition(ulPos);  
}

/** Reads a character from the stream. */
DataStream& FileStream::Read (char &ch)
{
  if (pfs == NULL) return *this;
  pfs->read (&ch, 1);
  return *this;
}

/** Reads a block of size \a nSize from the stream. */
DataStream& FileStream::Read (char* pData, int nSize)
{
  if (pfs == NULL) return *this;
  pfs->read (pData, nSize);
  return *this;
}

/** Reads a line from the stream. */
DataStream& FileStream::ReadLine (char* pData, int nSize, char cDelim)
{
  pfs->getline(pData, nSize, cDelim);    
  return *this;
}

/** Writes a character into the stream. */
DataStream& FileStream::Write (char ch)
{
  if (pfs == NULL) return *this;
  pfs->write (&ch, 1);
  return *this;
}

/** Writes a block of size \a nSize into the stream. */
DataStream& FileStream::Write (const char* pData, int nSize)
{
  if (pfs == NULL) return *this;
  pfs->write (pData, nSize);
  return *this;
}

void FileBuffer::SetPosition (unsigned long ulPos)
{
  seekoff(ulPos, std::ios::beg, std::ios::in);
}

unsigned long FileBuffer::Position (void)
{
  return seekoff(0, std::ios::cur, std::ios::in);
}

unsigned long FileBuffer::FileSize (void)
{
  unsigned long    ulAktpos, ulEndpos;

  ulAktpos = seekoff(0, std::ios::cur, std::ios::in);
  
  ulEndpos = seekoff(0, std::ios::end, std::ios::in);
   
  seekoff(ulAktpos, std::ios::beg, std::ios::in);
  
  return ulEndpos;
}

//--------- CLASS BlobStream -------------------------*/

#define BLOB_DEFSIZE    10000
#define BLOB_DEFNAME    "noname"

/** Construction of an empty stream. */
BlobStream::BlobStream (void)
{
  pBuf = (char*) malloc (BLOB_DEFSIZE);
  pszName = strdup (BLOB_DEFNAME);
  ulBufSize = pBuf ? BLOB_DEFSIZE : 0;
  ulCursor = 0;
}

/** Destructs the stream, all data in the stream will be lost. */              
BlobStream::~BlobStream (void)
{
  if (pBuf) free (pBuf);
  if (pszName) free (pszName);
}

/** Sets a name to the stream. */
void BlobStream::SetName (const char *pszBlobName)
{
  if (pszName) free (pszName);
  pszName = strdup (pszBlobName);
}

/** Returns the name of the stream. */
char *BlobStream::GetName (void)
{
  return pszName;
}

/** Returns the current position of the stream pointer. */
unsigned long BlobStream::GetPosition (void)
{
  return ulCursor;
}

/** Returns the pointer to the internal stream buffer. This could
 * be used to make a copy of the data block or to convert the data.
 * The valid size of the buffer can be determined with GetPosition().
 */
char *BlobStream::GetBuffer (void)
{
  return pBuf;
}

/**  Sets the new internal stream buffer. The data of current stream#
 * will be lost.
 */
void BlobStream::SetBuffer (char *pBuffer, unsigned long ulSize)
{
  if (pBuffer && ulSize)
  {
    if (pBuf) free (pBuf);
    pBuf = pBuffer;
    ulBufSize = ulSize;
  }
  Rewind ();
}

BlobStream::operator void* (void)
{
  return DataStream::operator void* ();
}

bool BlobStream::operator! (void)
{
  return DataStream::operator! ();
}

bool BlobStream::IsOpen (void)
{
  return true;
}

bool BlobStream::IsGood (void)
{
  return pBuf != NULL;
}

bool BlobStream::IsEof (void)
{
  return ulCursor >= ulBufSize;
}
              
bool BlobStream::IsFail (void)
{
  return !IsGood ();
}
              
bool BlobStream::IsBad (void)
{
  return !IsGood ();
}
              
void BlobStream::Flush (void)
{
  ulCursor = 0;
}

/** Resets the stream pointer to the beginning. */
void BlobStream::Rewind (void)
{
  ulCursor = 0;
}

DataStream& BlobStream::Read (char &ch)
{
  if (pBuf && !IsEof ()) 
    ch = pBuf[ulCursor++];
  return *this;
}

DataStream& BlobStream::Read (char* pData, int nSize)
{
  int n;
  if (pBuf)
    for (n = 0; (n < nSize) && !IsEof (); n++)
      pData[n] = pBuf[ulCursor++];
  return *this;
}

DataStream& BlobStream::Write (char ch)
{
  if (pBuf)
  {
    if (ulCursor + 1 >= ulBufSize)
      pBuf = (char*) realloc (pBuf, ulBufSize += BLOB_DEFSIZE);
    if (pBuf) pBuf[ulCursor++] = ch;
  }
  return *this;  
}

DataStream& BlobStream::Write (const char* pData, int nSize)
{
  int n;
  if (pBuf)
  {
    if (ulCursor + nSize >= ulBufSize)
      pBuf = (char*) realloc (pBuf, ulBufSize += nSize + BLOB_DEFSIZE);
    if (pBuf)
      for (n = 0; n < nSize; n++)
        pBuf[ulCursor++] = pData[n];
  }
  return *this;
}
