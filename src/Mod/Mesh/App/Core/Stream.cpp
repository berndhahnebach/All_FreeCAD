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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Stream.h"
#include "Swap.h"

using namespace Mesh;

void DataStream::Init (void)
{
  bSwap = FALSE;
  bFormatError = FALSE;
  ulFlag = 0;
}

DataStream::DataStream (void)
{
  Init ();
}

unsigned long DataStream::GetFlag (void)
{
  return ulFlag;
}

void DataStream::SetFlag (unsigned long ulNewFlag)
{
  ulFlag = ulNewFlag;
}

bool DataStream::GetSwap (void)
{
  return bSwap;
}

void DataStream::SetSwap (bool bNewSwap)
{
  bSwap = bNewSwap;
}

void  DataStream::SetFormatError (bool bErr)
{
  bFormatError = bErr;
}

bool DataStream::IsFormatError (void)
{
  return bFormatError;
}

DataStream& Mesh::readSwapOrder (DataStream& rcs)
{
  unsigned short usOrder;
  rcs >> usOrder;
  rcs.bSwap = usOrder != SwapOrder ();
  return rcs;
}

DataStream& Mesh::writeSwapOrder (DataStream& rcs)
{
  unsigned short usOrder = SwapOrder ();
  rcs << usOrder;
  return rcs;
}

DataStream::operator void* (void)
{
  return (void*) (IsGood () && (!IsFormatError ()));
}

bool DataStream::operator! (void)
{
  return !this->operator void* ();
}

DataStream& DataStream::operator>> (DataStream& (*fct) (DataStream&))
{
  return (*fct) (*this);
}

DataStream& DataStream::operator>> (char &ch)
{
  return Read (ch);
}

DataStream& DataStream::operator>> (unsigned char &uch)
{
  return *this >> (char&) uch;
}

DataStream& DataStream::operator>> (short &s)
{
  Read ((char*) &s, sizeof (short));
  if (bSwap) SwapVar (s);
  return *this;
}

DataStream& DataStream::operator>> (unsigned short &us)
{
  *this >> (short&) us;
  return *this;
}

DataStream& DataStream::operator>> (long &l)
{
  Read ((char*) &l, sizeof (long));
  if (bSwap) SwapVar (l);
  return *this;
}

DataStream& DataStream::operator>> (unsigned long &ul)
{
  *this >> (long&) ul;
  return *this;
}

DataStream& DataStream::operator>> (float &f)
{
  Read ((char*) &f, sizeof (float));
  if (bSwap) SwapVar (f);
  return *this;
}

DataStream& DataStream::operator>> (double &d)
{
  Read ((char*) &d, sizeof (double));
  if (bSwap) SwapVar (d);
  return *this;
}

DataStream& DataStream::operator<< (DataStream& (*fct) (DataStream&))
{
  return (*fct) (*this);
}

DataStream& DataStream::operator<< (char ch)
{
  return Write (ch);
}

DataStream& DataStream::operator<< (unsigned char uch)
{
  return *this << (char) uch;
}

DataStream& DataStream::operator<< (short s)
{
  if (bSwap) SwapVar (s);
  return Write ((char*) &s, sizeof (short));
}

DataStream& DataStream::operator<< (unsigned short us)
{
  return *this << (short) us;
}

DataStream& DataStream::operator<< (long l)
{
  if (bSwap) SwapVar (l);
  return Write ((char*) &l, sizeof (long));
}

DataStream& DataStream::operator<< (unsigned long ul)
{
  return *this << (long) ul;
}

DataStream& DataStream::operator<< (float f)
{
  if (bSwap) SwapVar ((float&)f);
  return Write ((char*) &f, sizeof (float));
}

DataStream& DataStream::operator<< (double d)
{
  if (bSwap) SwapVar ((double&) d);
  return Write ((char*) &d, sizeof (double));
}

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

FileStream::FileStream (void)
	: DataStream ()
{
  pfs = NULL;
}

FileStream::FileStream (const char *pszName, std::ios::openmode om)
	: DataStream ()
{
  pfs = NULL;
  Open (pszName, om);
}

/*$$$*/
FileStream::~FileStream (void)
/*------------------------------------------------------
 Beschreibung : Destruktor
		Schliesst gegebenenfalls eine geoffnete
		Datei.
 Parameter    : void
 Rueckgabe    : void
 -------------------------------------------------------*/
{
  Close();
  if (pfs)
    delete pfs;
}

/*$$$*/
bool FileStream::Open (const char* pszName, std::ios::openmode om)
/*------------------------------------------------------
 Beschreibung : Oeffnet eine Datei
 Parameter    : pszName : Dateiname
		om : Oeffnungsmodus (std::ios-Flag-Kombination)
 Rueckgabe    : TRUE/FALSE OK / Fehler
 -------------------------------------------------------*/
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
	bSwap = FALSE;
    pfs->open (pszName, om);
    return pfs->good ();
  }
  else
    return FALSE;
}

/*$$$*/
void FileStream::Close (void)
/*------------------------------------------------------
 Beschreibung : Schliesst eine geoeffnete Datei.
 Parameter    : void
 Rueckgabe    : void
 -------------------------------------------------------*/
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

/*$$$*/
void FileStream::Flush (void)
/*------------------------------------------------------
 Beschreibung : loescht den Stream (Datei)
 Parameter    : void
 Rueckgabe    : void
 -------------------------------------------------------*/
{ 
  if (!pfs)
    return;
    
  pfs->flush();
}

/*$$$*/
FileStream::operator void* (void)
/*------------------------------------------------------
 Beschreibung : void-Operator zur positiven Statusabfrage
 Parameter    : void
 Rueckgabe    : void
 -------------------------------------------------------*/
{
  return DataStream::operator void* ();
}

/*$$$*/
bool FileStream::operator! (void)
/*------------------------------------------------------
 Beschreibung : !-Operator zur negativen Statusabfrage
 Parameter    : void
 Rueckgabe    : TRUE/FALSE Fehler / OK
 -------------------------------------------------------*/
{
  return DataStream::operator! ();
}

/*$$$*/
bool FileStream::IsOpen (void)
/*------------------------------------------------------
 Beschreibung : Liefert Oeffnungszustand des Streams
 Parameter    : void
 Rueckgabe    : TRUE/FALSE File open / closed
 -------------------------------------------------------*/
{
  return pfs != NULL;
}

/*$$$*/
bool FileStream::IsGood (void)
/*------------------------------------------------------
 Beschreibung : Liefert positiven Zustand des Streams.
 Parameter    : void
 Rueckgabe    : TRUE/FALSE OK / Fehler
 -------------------------------------------------------*/
{
  return pfs ? pfs->good () : FALSE;
}

/*$$$*/
bool FileStream::IsEof (void)
/*------------------------------------------------------
 Beschreibung : Liefert EOF-Zustand des Streams.
 Parameter    : void
 Rueckgabe    : TRUE/FALSE EOF / nicht EOF
 -------------------------------------------------------*/
{
  return pfs ? pfs->eof () : FALSE;
}

/*$$$*/
bool FileStream::IsFail (void)
/*------------------------------------------------------
 Beschreibung : Liefert Fehlerzustand des Streams
 Parameter    : void
 Rueckgabe    : TRUE/FALSE Fehler / kein Fehler
 -------------------------------------------------------*/
{
  return pfs ? pfs->fail () : TRUE;
}

/*$$$*/
bool FileStream::IsBad (void)
/*------------------------------------------------------
 Beschreibung : Liefert Fehlerzustand des Streams
 Parameter    : void
 Rueckgabe    : TRUE/FALSE Fehler / kein Fehler
 -------------------------------------------------------*/
{
  return pfs ? pfs->bad () : TRUE;
}

/*$$$*/
unsigned long FileStream::FileSize (void)
/*------------------------------------------------------
 Beschreibung : Liefert die Dateigroesse des geoeffneten
   Streams
 Parameter    : void
 Rueckgabe    : unsigned long : Dateigroesse in Bytes bzw. 0 (Zero)
   wenn die Methode fehl geht.
 -------------------------------------------------------*/
{
  FileBuffer  *pfsbuf;
 
  pfsbuf = (FileBuffer*)pfs->rdbuf();

  if (pfsbuf == 0)
    return 0;

  return pfsbuf->FileSize();  
}

/*$$$*/
unsigned long FileStream::Position (void)
/*------------------------------------------------------
 Beschreibung : gibt die aktuelle (Lese-)Position im File an.
 Parameter    : void
 Rueckgabe    : unsigned long : aktuelle Position, 0 wenn nicht 
  ermittelbar.
 -------------------------------------------------------*/
{
  FileBuffer  *pfsbuf;
 
  pfsbuf = (FileBuffer*)pfs->rdbuf();

  if (pfsbuf == 0)
    return 0;
  
  return pfsbuf->Position();
}


void FileStream::SetPosition (unsigned long ulPos)
/*------------------------------------------------------
 Beschreibung : setzt eine neue Position im Stream
 Parameter    : ulPos:  Position
 Rueckgabe    : void
 -------------------------------------------------------*/
{
  FileBuffer  *pfsbuf;
 
  pfsbuf = (FileBuffer*)pfs->rdbuf();

  if (pfsbuf == 0)
    return;

  pfsbuf->SetPosition(ulPos);  
}

/*$$$*/
DataStream& FileStream::Read (char &ch)
/*------------------------------------------------------
 Beschreibung : Liest ein Zeichen aus aktuellem
		geoeffneten Stream.
 Parameter    : ch : gelesenes Zeichen
 Rueckgabe    : Stream-Referenz
 -------------------------------------------------------*/
{
  if (pfs == NULL) return *this;
  pfs->read (&ch, 1);
  return *this;
}

/*$$$*/
DataStream& FileStream::Read (char* pData, int nSize)
/*------------------------------------------------------
 Beschreibung : Liest einen Datenblock aus aktuellem
		geoeffneten Stream.
 Parameter    : pData : Zeiger auf Zielpuffer
		nSize : Anzahl geforderter Bytes
 Rueckgabe    : Stream-Referenz
 -------------------------------------------------------*/
{
  if (pfs == NULL) return *this;
  pfs->read (pData, nSize);
  return *this;
}

/*$$$*/
DataStream& FileStream::ReadLine (char* pData, int nSize,
                                              char cDelim)
/*------------------------------------------------------
 Beschreibung : Liest eine Zeile aus Stream.
 Parameter    : pData : Lesedatenpuffer
                nSize : max. Anz. zu lesender Zeichen
                cDelim: Zeilenende-Zeichen (Default: '\n')
 Rueckgabe    : Stream&
 -------------------------------------------------------*/
{
  pfs->getline(pData, nSize, cDelim);    
  return *this;
}

/*$$$*/
DataStream& FileStream::Write (char ch)
/*------------------------------------------------------
 Beschreibung : Schreibt ein Zeichen in aktuellen
		geoeffneten Stream.
 Parameter    : ch : zu schreibendes Zeichen
 Rueckgabe    : Stream-Referenz
 -------------------------------------------------------*/
{
  if (pfs == NULL) return *this;
  pfs->write (&ch, 1);
  return *this;
}

/*$$$*/
DataStream& FileStream::Write (const char* pData, int nSize)
/*------------------------------------------------------
 Beschreibung : Schreibt einen Datenblock in aktuellen
		geoeffneten Stream.
 Parameter    : pData : Zeiger auf zu schreibende Daten
		nSize : Laenge des Datenblocks in Bytes
 Rueckgabe    : Stream-Referenz
 -------------------------------------------------------*/
{
  if (pfs == NULL) return *this;
  pfs->write (pData, nSize);
  return *this;
}


/********************************************************/
/** IMPLEMENTATION OF BlobStream **********************/

#define BLOB_DEFSIZE    10000
#define BLOB_DEFNAME    "noname"

/*$$$*/
BlobStream::BlobStream (void)
/*------------------------------------------------------
 Beschreibung : Konstruktor.
         Erzeugt leeren Blobstream.
 Parameter    : void
 Rueckgabe    : void
 -------------------------------------------------------*/
{
  pBuf = (char*) malloc (BLOB_DEFSIZE);
  pszName = strdup (BLOB_DEFNAME);
  ulBufSize = pBuf ? BLOB_DEFSIZE : 0;
  ulCursor = 0;
}
              
/*$$$*/
BlobStream::~BlobStream (void)
/*------------------------------------------------------
 Beschreibung : Destruktor. 
         Gibt Streamdaten frei. Vorher in den Stream
         geschriebene Daten gehen verloren.
 Parameter    : void
 Rueckgabe    : void
 -------------------------------------------------------*/
{
  if (pBuf) free (pBuf);
  if (pszName) free (pszName);
}

/*$$$*/
void BlobStream::SetName (const char *pszBlobName)
/*------------------------------------------------------
 Beschreibung : Setzt den Streamnamen.
     Kann zur Identifikation mehrerer Streamobjekte
     dienen.
 Parameter    : pszBlobName : neuer Name
 Rueckgabe    : void
 -------------------------------------------------------*/
{
  if (pszName) free (pszName);
  pszName = strdup (pszBlobName);
}

/*$$$*/
char *BlobStream::GetName (void)
/*------------------------------------------------------
 Beschreibung : Gibt den aktuellen Namen des 
           Streamobjekts zurueck.
 Parameter    : void
 Rueckgabe    : Ptr. Streamname
 -------------------------------------------------------*/
{
  return pszName;
}

/*$$$*/
unsigned long BlobStream::GetPosition (void)
/*------------------------------------------------------
 Beschreibung : Gibt die aktuelle Position des 
         Stream-Zeigers zurueck. Jede Read/Write-
         Operation manipuliert diese Position.
 Parameter    : void
 Rueckgabe    : Akt. Stream-Zeiger-Position
 -------------------------------------------------------*/
{
  return ulCursor;
}
              
/*$$$*/
char *BlobStream::GetBuffer (void)
/*------------------------------------------------------
 Beschreibung : Gibt Zeiger auf internen Streampuffer
         zurueck. Kann z.B. zur Kopie des Datenblocks
         und zu einer Konvertierung verwendet werden.
         Die gueltige Groesse dieses Buffers kann 
         durch GetPosition() ermittelt werden.
 Parameter    : void
 Rueckgabe    : Ptr .auf Streambuffer
 -------------------------------------------------------*/
{
  return pBuf;
}
              
/*$$$*/
void BlobStream::SetBuffer (char *pBuffer, unsigned long ulSize)
/*------------------------------------------------------
 Beschreibung : Setzt neuen internen Streambuffer.
         Koennen z.B. durch IPC empfangene Daten sein.
         Aktueller Streambuffer wird freigegeben,
         Daten gehen verloren. Die Daten in pBuffer 
         werden NICHT kopiert, sie sollten von aussen
         durch malloc() bereitgestellt werden.
         Stream-Zeiger wird auf Anfang zurueckgesetzt.
         
 Parameter    : pBuffer: Ptr. auf Buffer
                 ulSize : Groesse des Datenblocks in Byte
 Rueckgabe    : void
 -------------------------------------------------------*/
{
  if (pBuffer && ulSize)
  {
    if (pBuf) free (pBuf);
    pBuf = pBuffer;
    ulBufSize = ulSize;
  }
  Rewind ();
}
              
/*$$$*/
BlobStream::operator void* (void)
/*------------------------------------------------------
 Beschreibung : Valid-Operator
     z.B. : if (blob_stream) puts ("OK");
 Parameter    : void
 Rueckgabe    : TRUE/FALSE OK / Fehler
 -------------------------------------------------------*/
{
  return DataStream::operator void* ();
}
              
/*$$$*/
bool BlobStream::operator! (void)
/*------------------------------------------------------
 Beschreibung : Invalid-Operator
     z.B. : if (!blob_stream) puts ("error");
 Parameter    : void
 Rueckgabe    : TRUE/FALSE Fehler / OK
 -------------------------------------------------------*/
{
  return DataStream::operator! ();
}
              
/*$$$*/
bool BlobStream::IsOpen (void)
/*------------------------------------------------------
 Beschreibung : Blob-Stream ist grundsaetzlich 
       schreibend/lesend geoeffnet.
 Parameter    : void
 Rueckgabe    : TRUE
 -------------------------------------------------------*/
{
  return TRUE;
}
              
/*$$$*/
bool BlobStream::IsGood (void)
/*------------------------------------------------------
 Beschreibung : Liefert Status des Streams zurueck.
 Parameter    : void
 Rueckgabe    : TRUE/FALSE OK / Fehler
 -------------------------------------------------------*/
{
  return pBuf != NULL;
}
              
/*$$$*/
bool BlobStream::IsEof (void)
/*------------------------------------------------------
 Beschreibung : Liefert EOF-Status des Streams zurueck.
         EOF wird dann erreicht, wenn der Stream-Zeiger
         bei Leseoperationen die Gesamtpuffergroesse
         erreicht.
 Parameter    : void
 Rueckgabe    : TRUE/FALSE EOF / not EOF
 -------------------------------------------------------*/
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

//*********************************************************//
//******** TEST *******************************************//


#ifdef __TEST__


#include <stlall.h>

void main (void)
{
  unsigned long ulValue = 5;
  FileStream clFile;

  clFile.Open ("hallo", std::ios::out);
  if (clFile)
    clFile << writeSwapOrder << ulValue;
  clFile.Close ();

  ulValue = 1;
  clFile.Open ("hallo", std::ios::in);
  if (clFile)
    clFile >> readSwapOrder >> ulValue;
  clFile.Close ();


  cout << "Read value : " << ulValue << endl;

}




#endif


