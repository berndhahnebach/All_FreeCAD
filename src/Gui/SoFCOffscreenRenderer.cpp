/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

//gcc
# include <iomanip>
# include <ios>
# include <sstream>

#include <Base/FileInfo.h>
#include <Base/Exception.h>
#include <Base/Console.h>
#include <App/Application.h>

#include "SoFCOffscreenRenderer.h"
#include "BitmapFactory.h"

using namespace Gui;
using namespace std;


#define MAX_EXT_LEN 255

void writeJPEGComment(const char* InFile, const char* OutFile, const char* Comment);

// ---------------------------------------------------------------

SoFCOffscreenRenderer::SoFCOffscreenRenderer (const SbViewportRegion &viewportregion)
  : SoOffscreenRenderer(viewportregion)
{
}

SoFCOffscreenRenderer::SoFCOffscreenRenderer (SoGLRenderAction *action)
  : SoOffscreenRenderer(action)
{
}

SoFCOffscreenRenderer::~SoFCOffscreenRenderer()
{
}

void SoFCOffscreenRenderer::writeToImage (QImage& img) const
{
    const unsigned char * bytes = getBuffer();
    SbVec2s size = getViewportRegion().getViewportSizePixels();
    int numcomponents = (int) this->getComponents();

    SoSFImage image;
    image.setValue(size, numcomponents, bytes, SoSFImage::NO_COPY);
    BitmapFactory().convert(image, img);
}

SbBool SoFCOffscreenRenderer::writeToImageFile (const SbString&  filename, const SbName& filetypeextension) const
{
  if ( isWriteSupported( filetypeextension ) )
  {
    return writeToFile( filename, filetypeextension );
  }
  else if ( strcmp(filetypeextension.getString(), "EPS") == 0 || strcmp(filetypeextension.getString(), "PS") == 0 )
  {
    return writeToPostScript( filename.getString() );
  }
  else if ( strcmp(filetypeextension.getString(), "RGB") == 0 || strcmp(filetypeextension.getString(), "SGI") == 0 )
  {
    return writeToRGB( filename.getString() );
  }
  else // try to convert into a QImage and save then
  {
    QImage img;
    writeToImage(img);
    return img.save(filename.getString(), filetypeextension.getString());
  }

  return false;
}

void SoFCOffscreenRenderer::writeToImageFile (const char *filename, const char* comment) const
{
  Base::FileInfo file(filename);
  if (file.hasExtension("JPG") || file.hasExtension("JPEG")) {
    QImage img;
    writeToImage(img);

    const char* format = "JPEG";;
    if (!img.save((file.filePath()+"_temp").c_str(), format))
      throw Base::Exception();

    // writing comment in case of jpeg (Qt ignores setText() in case of jpeg)
    if (strcmp(comment,"")==0)
      writeJPEGComment( (file.filePath()+"_temp").c_str(),file.filePath().c_str(), "Screenshot created by FreeCAD");   
    else if (strcmp(comment,"$MIBA")==0)
      writeJPEGComment((file.filePath()+"_temp").c_str(),file.filePath().c_str(), createMIBA().c_str());   
    else 
      writeJPEGComment((file.filePath()+"_temp").c_str(),file.filePath().c_str(), comment);

    // delete temporary file
    Base::FileInfo tmp((file.filePath()+"_temp").c_str());
    tmp.deleteFile();
  } else {
    // check for all QImage formats
    bool supported = false;
    QByteArray format;
    QList<QByteArray> qtformats = QImageWriter::supportedImageFormats();
    for (QList<QByteArray>::Iterator it = qtformats.begin(); it != qtformats.end(); ++it) {
      if (file.hasExtension((*it).data())) {
        format = *it;
        supported = true;
        break;
      }
    }
    
    // Supported by Qt
    if (supported) {
      QImage img;
      writeToImage(img);
      // set keywords for PNG format
      if (file.hasExtension("PNG")) {
        img.setText("Title", filename);
        img.setText("Author", "FreeCAD (http://free-cad.sourceforge.net)");
        if(strcmp(comment,"")==0)
          img.setText("Description", "Screenshot created by FreeCAD");   
        else if(strcmp(comment,"$MIBA")==0)
          img.setText("Description", createMIBA().c_str());   
        else 
          img.setText("Description", comment);
        img.setText("Creation Time", QDateTime::currentDateTime().toString());
        img.setText("Software", App::Application::Config()["ExeName"].c_str());
      }

      QFile f(QString::fromUtf8(filename));
      if (f.open(QFile::WriteOnly)) {
        if (img.save(&f, format.data())) {
          f.close();
        } else {
          f.close();
          std::stringstream str;
          str << "Cannot save image to file '" << filename << "'.";
          throw Base::Exception(str.str());
        }
      } else {
        std::stringstream str;
        str << "Cannot open file '" << filename << "' for writing.";
        throw Base::Exception(str.str());
      }
    } else if (isWriteSupported(file.extension().c_str())) {
      // Any format which is supported by Coin only
      if (!writeToFile( filename, file.extension().c_str()))
        throw Base::Exception("SoFCOffscreenRenderer::writeToImageFile(): Error writing file with SoOffscreenRenerer...");
    } else if (file.hasExtension("EPS") || file.hasExtension("PS")) {
      // Any format which is supported by Coin only
      if(! writeToPostScript( filename ))
        throw Base::Exception("SoFCOffscreenRenderer::writeToImageFile(): Error writing file with SoOffscreenRenerer...");
    } else if (file.hasExtension("RGB") || file.hasExtension("SGI")) {
      // Any format which is supported by Coin only
      if(!writeToRGB(filename))
        throw Base::Exception("SoFCOffscreenRenderer::writeToImageFile(): Error writing file with SoOffscreenRenerer...");
    }
  }
}

QStringList SoFCOffscreenRenderer::getWriteImageFiletypeInfo()
{
  QStringList formats;

  // get all supported formats by Coin3D
  int num = getNumWriteFiletypes();
  for (int i=0; i < num; i++)
  {
#if   (COIN_MAJOR_VERSION < 2) // Coin3D <= 1.x
    SbList<SbName> extlist;
#elif (COIN_MAJOR_VERSION < 3) // Coin3D <= 2.x
# if  (COIN_MINOR_VERSION < 3) // Coin3D <= 2.2.x
    SbList<SbName> extlist;
# else                         // Coin3D >= 2.3.x
    SbPList extlist;
# endif                        
#else                          // Coin3D >= 3.x
    SbPList extlist;
#endif

    SbString fullname, description;
    getWriteFiletypeInfo(i, extlist, fullname, description);

    for (int j=0; j < extlist.getLength(); j++)
    {
      QString ext = (const char*) extlist[j];
      if ( formats.indexOf( ext.toUpper() ) == -1 )
        formats << ext.toUpper();
    }
  }

  // add now all further QImage formats
  QList<QByteArray> qtformats = QImageWriter::supportedImageFormats();
  for ( QList<QByteArray>::Iterator it = qtformats.begin(); it != qtformats.end(); ++it )
  {
    // not supported? then append
    if ( isWriteSupported( (*it).data() ) == false && formats.indexOf(*it) == -1 )
      formats << *it;
  }

  // now add PostScript and SGI RGB
  if ( formats.indexOf("EPS") == -1 )
    formats << "EPS";
  else if ( formats.indexOf("SGI") == -1 )
    formats << "SGI";

  formats.sort();

  return formats;
}

std::string SoFCOffscreenRenderer::createMIBA() const
{
  std::stringstream com;

  com << setw(7) << setfill(' ') << fixed;
  com << "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n" ;
  com << "<MIBA xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://juergen-riegel.net/Miba/Miba2.xsd\" Version=\"2\"> \n" ;
  com << " <View>\n"; 
  com << "  <Matrix \n"; 
  com << "     a11=\"" << _Matrix[0][0] <<"\" a12=\"" << _Matrix[1][0] <<"\" a13=\"" << _Matrix[2][0] <<"\" a14=\"" << _Matrix[3][0] << "\"\n";
  com << "     a21=\"" << _Matrix[0][1] <<"\" a22=\"" << _Matrix[1][1] <<"\" a23=\"" << _Matrix[2][1] <<"\" a24=\"" << _Matrix[3][1] << "\"\n";
  com << "     a31=\"" << _Matrix[0][2] <<"\" a32=\"" << _Matrix[1][2] <<"\" a33=\"" << _Matrix[2][2] <<"\" a34=\"" << _Matrix[3][2] << "\"\n";
  com << "     a41=\"" << _Matrix[0][3] <<"\" a42=\"" << _Matrix[1][3] <<"\" a43=\"" << _Matrix[2][3] <<"\" a44=\"" << _Matrix[3][3] << "\"\n";
  com << "   />\n" ; 
  com << " </View>\n" ; 
  com << " <Source>\n" ; 
  com << "  <Creator>Unknown</Creator>\n" ;  
  com << "  <CreationDate>2006-07-05T01:11:00</CreationDate>\n" ;  
  com << "  <CreatingSystem>FreeCAD 0.5</CreatingSystem>\n" ;
  com << "  <PartNumber>Unknown</PartNumber>\n";
  com << "  <Revision>1.0</Revision>\n";
  com << " </Source>\n" ;
  com << "</MIBA>\n" ;

  return com.str();
}

//===========================================================================
// helper from wrjpgcom.c
//===========================================================================

/*
 * wrjpgcom.c
 *
 * Copyright (C) 1994, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains a very simple stand-alone application that inserts
 * user-supplied text as a COM (comment) marker in a JFIF file.
 * This may be useful as an example of the minimum logic needed to parse
 * JPEG markers.
 */

//#define JPEG_CJPEG_DJPEG	/* to get the command-line config symbols */
//#include "jinclude.h"		/* get auto-config symbols, <stdio.h> */

#ifndef HAVE_STDLIB_H		/* <stdlib.h> should declare malloc() */
extern void * malloc ();
#endif
#include <ctype.h>		/* to declare isupper(), tolower() */
#ifdef USE_SETMODE
#include <fcntl.h>		/* to declare setmode()'s parameter macros */
/* If you have setmode() but not <io.h>, just delete this line: */
#include <io.h>			/* to declare setmode() */
#endif

#ifdef USE_CCOMMAND		/* command-line reader for Macintosh */
#ifdef __MWERKS__
#include <SIOUX.h>              /* Metrowerks declares it here */
#endif
#ifdef THINK_C
#include <console.h>		/* Think declares it here */
#endif
#endif

#ifdef DONT_USE_B_MODE		/* define mode parameters for fopen() */
#define READ_BINARY	"r"
#define WRITE_BINARY	"w"
#else
#define READ_BINARY	"rb"
#define WRITE_BINARY	"wb"
#endif

#ifndef EXIT_FAILURE		/* define exit() codes if not provided */
#define EXIT_FAILURE  1
#endif
#ifndef EXIT_SUCCESS
#ifdef VMS
#define EXIT_SUCCESS  1		/* VMS is very nonstandard */
#else
#define EXIT_SUCCESS  0
#endif
#endif

/* Reduce this value if your malloc() can't allocate blocks up to 64K.
 * On DOS, compiling in large model is usually a better solution.
 */

#ifndef MAX_COM_LENGTH
#define MAX_COM_LENGTH 65000	/* must be < 65534 in any case */
#endif


/*
 * These macros are used to read the input file and write the output file.
 * To reuse this code in another application, you might need to change these.
 */

static FILE * infile;		/* input JPEG file */

/* Return next input byte, or EOF if no more */
#define NEXTBYTE()  getc(infile)

static FILE * outfile;		/* output JPEG file */

/* Emit an output byte */
#define PUTBYTE(x)  putc((x), outfile)


/* Error exit handler */
#define ERREXIT(msg)  (throw Base::Exception(msg))


/* Read one byte, testing for EOF */
static int
read_1_byte (void)
{
  int c;

  c = NEXTBYTE();
  if (c == EOF)
    ERREXIT("Premature EOF in JPEG file");
  return c;
}

/* Read 2 bytes, convert to unsigned int */
/* All 2-byte quantities in JPEG markers are MSB first */
static unsigned int
read_2_bytes (void)
{
  int c1, c2;

  c1 = NEXTBYTE();
  if (c1 == EOF)
    ERREXIT("Premature EOF in JPEG file");
  c2 = NEXTBYTE();
  if (c2 == EOF)
    ERREXIT("Premature EOF in JPEG file");
  return (((unsigned int) c1) << 8) + ((unsigned int) c2);
}


/* Routines to write data to output file */

static void
write_1_byte (int c)
{
  PUTBYTE(c);
}

static void
write_2_bytes (unsigned int val)
{
  PUTBYTE((val >> 8) & 0xFF);
  PUTBYTE(val & 0xFF);
}

static void
write_marker (int marker)
{
  PUTBYTE(0xFF);
  PUTBYTE(marker);
}

static void
copy_rest_of_file (void)
{
  int c;

  while ((c = NEXTBYTE()) != EOF)
    PUTBYTE(c);
}


/*
 * JPEG markers consist of one or more 0xFF bytes, followed by a marker
 * code byte (which is not an FF).  Here are the marker codes of interest
 * in this program.  (See jdmarker.c for a more complete list.)
 */

#define M_SOF0  0xC0		/* Start Of Frame N */
#define M_SOF1  0xC1		/* N indicates which compression process */
#define M_SOF2  0xC2		/* Only SOF0 and SOF1 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8		/* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9		/* End Of Image (end of datastream) */
#define M_SOS   0xDA		/* Start Of Scan (begins compressed data) */
#define M_COM   0xFE		/* COMment */


/*
 * Find the next JPEG marker and return its marker code.
 * We expect at least one FF byte, possibly more if the compressor used FFs
 * to pad the file.  (Padding FFs will NOT be replicated in the output file.)
 * There could also be non-FF garbage between markers.  The treatment of such
 * garbage is unspecified; we choose to skip over it but emit a warning msg.
 * NB: this routine must not be used after seeing SOS marker, since it will
 * not deal correctly with FF/00 sequences in the compressed image data...
 */

static int
next_marker (void)
{
  int c;
  int discarded_bytes = 0;

  /* Find 0xFF byte; count and skip any non-FFs. */
  c = read_1_byte();
  while (c != 0xFF) {
    discarded_bytes++;
    c = read_1_byte();
  }
  /* Get marker code byte, swallowing any duplicate FF bytes.  Extra FFs
   * are legal as pad bytes, so don't count them in discarded_bytes.
   */
  do {
    c = read_1_byte();
  } while (c == 0xFF);

  if (discarded_bytes != 0) {
    fprintf(stderr, "Warning: garbage data found in JPEG file\n");
  }

  return c;
}


/*
 * Read the initial marker, which should be SOI.
 * For a JFIF file, the first two bytes of the file should be literally
 * 0xFF M_SOI.  To be more general, we could use next_marker, but if the
 * input file weren't actually JPEG at all, next_marker might read the whole
 * file and then return a misleading error message...
 */

static int
first_marker (void)
{
  int c1, c2;

  c1 = NEXTBYTE();
  c2 = NEXTBYTE();
  if (c1 != 0xFF || c2 != M_SOI)
    ERREXIT("Not a JPEG file");
  return c2;
}


/*
 * Most types of marker are followed by a variable-length parameter segment.
 * This routine skips over the parameters for any marker we don't otherwise
 * want to process.
 * Note that we MUST skip the parameter segment explicitly in order not to
 * be fooled by 0xFF bytes that might appear within the parameter segment;
 * such bytes do NOT introduce new markers.
 */

static void
copy_variable (void)
/* Copy an unknown or uninteresting variable-length marker */
{
  unsigned int length;

  /* Get the marker parameter length count */
  length = read_2_bytes();
  write_2_bytes(length);
  /* Length includes itself, so must be at least 2 */
  if (length < 2)
    ERREXIT("Erroneous JPEG marker length");
  length -= 2;
  /* Skip over the remaining bytes */
  while (length > 0) {
    write_1_byte(read_1_byte());
    length--;
  }
}

static void
skip_variable (void)
/* Skip over an unknown or uninteresting variable-length marker */
{
  unsigned int length;

  /* Get the marker parameter length count */
  length = read_2_bytes();
  /* Length includes itself, so must be at least 2 */
  if (length < 2)
    ERREXIT("Erroneous JPEG marker length");
  length -= 2;
  /* Skip over the remaining bytes */
  while (length > 0) {
    (void) read_1_byte();
    length--;
  }
}


/*
 * Parse the marker stream until SOFn or EOI is seen;
 * copy data to output, but discard COM markers unless keep_COM is true.
 */

static int
scan_JPEG_header (int keep_COM)
{
  int marker;

  /* Expect SOI at start of file */
  if (first_marker() != M_SOI)
    ERREXIT("Expected SOI marker first");
  write_marker(M_SOI);

  /* Scan miscellaneous markers until we reach SOFn. */
  for (;;) {
    marker = next_marker();
    switch (marker) {
    case M_SOF0:		/* Baseline */
    case M_SOF1:		/* Extended sequential, Huffman */
    case M_SOF2:		/* Progressive, Huffman */
    case M_SOF3:		/* Lossless, Huffman */
    case M_SOF5:		/* Differential sequential, Huffman */
    case M_SOF6:		/* Differential progressive, Huffman */
    case M_SOF7:		/* Differential lossless, Huffman */
    case M_SOF9:		/* Extended sequential, arithmetic */
    case M_SOF10:		/* Progressive, arithmetic */
    case M_SOF11:		/* Lossless, arithmetic */
    case M_SOF13:		/* Differential sequential, arithmetic */
    case M_SOF14:		/* Differential progressive, arithmetic */
    case M_SOF15:		/* Differential lossless, arithmetic */
      return marker;

    case M_SOS:			/* should not see compressed data before SOF */
      ERREXIT("SOS without prior SOFn");
      break;

    case M_EOI:			/* in case it's a tables-only JPEG stream */
      return marker;

    case M_COM:			/* Existing COM: conditionally discard */
      if (keep_COM) {
	write_marker(marker);
	copy_variable();
      } else {
	skip_variable();
      }
      break;

    default:			/* Anything else just gets copied */
      write_marker(marker);
      copy_variable();		/* we assume it has a parameter count... */
      break;
    }
  } /* end loop */
}


#ifdef _MSC_VER
/* Command line parsing code */
static const char * progname;	/* program name for error messages */

static int
keymatch (char * arg, const char * keyword, int minchars)
/* Case-insensitive matching of (possibly abbreviated) keyword switches. */
/* keyword is the constant keyword (must be lower case already), */
/* minchars is length of minimum legal abbreviation. */
{
  register int ca, ck;
  register int nmatched = 0;

  while ((ca = *arg++) != '\0') {
    if ((ck = *keyword++) == '\0')
      return 0;			/* arg longer than keyword, no good */
    if (isupper(ca))		/* force arg to lcase (assume ck is already) */
      ca = tolower(ca);
    if (ca != ck)
      return 0;			/* no good */
    nmatched++;			/* count matched characters */
  }
  /* reached end of argument; fail if it's too short for unique abbrev */
  if (nmatched < minchars)
    return 0;
  return 1;			/* A-OK */
}
#endif

void writeJPEGComment(const char* InFile, const char* OutFile, const char* Comment)
{
    char * comment_arg;
    unsigned int comment_length;
    int marker;

    comment_arg = (char *) malloc((size_t) std::strlen(Comment)+2);
    strcpy(comment_arg, Comment);
    comment_length = (unsigned int)std::strlen(comment_arg);


    if ((infile = fopen(InFile, READ_BINARY)) == NULL) {
        //fprintf(stderr, "%s: can't open %s\n", progname, argv[argn]);
        return;
    }

    if ((outfile = fopen(OutFile, WRITE_BINARY)) == NULL) {
        //fprintf(stderr, "%s: can't open %s\n", progname, argv[argn+1]);
        return;
    }

    /* Copy JPEG headers until SOFn marker;
    * we will insert the new comment marker just before SOFn.
    * This (a) causes the new comment to appear after, rather than before,
    * existing comments; and (b) ensures that comments come after any JFIF
    * or JFXX markers, as required by the JFIF specification.
    */
    marker = scan_JPEG_header(0);
    /* Insert the new COM marker, but only if nonempty text has been supplied */
    if (comment_length > 0) {
        write_marker(M_COM);
        write_2_bytes(comment_length + 2);
        while (comment_length > 0) {
            write_1_byte(*comment_arg++);
            comment_length--;
        }
    }
    /* Duplicate the remainder of the source file.
     * Note that any COM markers occuring after SOF will not be touched.
     */
    write_marker(marker);
    copy_rest_of_file();


    fclose(infile);
    fclose(outfile);
}
