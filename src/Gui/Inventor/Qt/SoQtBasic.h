/**************************************************************************\
 *
 *  This file is part of the Coin GUI toolkit libraries.
 *  Copyright (C) 1998-2002 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  version 2.1 as published by the Free Software Foundation.  See the
 *  file LICENSE.LGPL at the root directory of this source distribution
 *  for more details.
 *
 *  If you want to use this library with software that is incompatible
 *  licensewise with the LGPL, and / or you would like to take
 *  advantage of the additional benefits with regard to our support
 *  services, please contact Systems in Motion about acquiring a Coin
 *  Professional Edition License.  See <URL:http://www.coin3d.org> for
 *  more information.
 *
 *  Systems in Motion, Prof Brochs gate 6, 7030 Trondheim, NORWAY
 *  <URL:http://www.sim.no>, <mailto:support@sim.no>
 *
\**************************************************************************/


#ifndef SOQT_BASIC_H
#define SOQT_BASIC_H

// *************************************************************************

/* Some useful inline template functions:
 *   SoQtMin(Val1, Val2)       - returns minimum value
 *   SoQtMax(Val1, Val2)       - returns maximum value
 *   SoQtClamp(Val, Min, Max)  - returns clamped value
 *   SoQtSwap(Val1, Val2)      - swaps the two values (no return value)
 */

template <class Type>
inline Type SoQtAbs(Type Val) {
  return (Val < 0) ? -Val : Val;
}

template <class Type>
inline Type SoQtMin(Type a, Type b) {
  return (b < a) ? b : a;
}

template <class Type>
inline Type SoQtMax(Type a, Type b) {
  return (b > a) ? b : a;
}

template <class Type>
inline Type SoQtClamp(Type val, Type min, Type max) {
  return SoQtMax(min, SoQtMin(max, val));
}

template <class Type>
inline void SoQtSwap(Type & a, Type & b) {
  Type t = a; a = b; b = t;
}

// *************************************************************************

#define __COIN_SOQT__

#if ! defined(SOQT_MAJOR_VERSION)
#undef SOQT_MAJOR_VERSION
#endif /* ! SOQT_MAJOR_VERSION */
#if ! defined(SOQT_MINOR_VERSION)
#undef SOQT_MINOR_VERSION
#endif /* ! SOQT_MINOR_VERSION */
#if ! defined(SOQT_MICRO_VERSION)
#undef SOQT_MICRO_VERSION
#endif /* ! SOQT_MICRO_VERSION */
#if ! defined(SOQT_VERSION)
#undef SOQT_VERSION
#endif /* ! SOQT_VERSION */

// *************************************************************************

/* Precaution to avoid an error easily made by the application programmer. */
#ifdef SOQT_DLL_API
# error Leave the internal SOQT_DLL_API define alone.
#endif /* SOQT_DLL_API */

/*
  On MSWindows platforms, one of these defines must always be set when
  building application programs:

   - "SOQT_DLL", when the application programmer is using the
     library in the form of a dynamic link library (DLL)

   - "SOQT_NOT_DLL", when the application programmer is using the
     library in the form of a static object library (LIB)

  Note that either SOQT_DLL or SOQT_NOT_DLL _must_ be defined by
  the application programmer on MSWindows platforms, or else the
  #error statement will hit. Set up one or the other of these two
  defines in your compiler environment according to how the library
  was built -- as a DLL (use "SOQT_DLL") or as a LIB (use
  "SOQT_NOT_DLL").

  (Setting up defines for the compiler is typically done by either
  adding something like "/DSOQT_DLL" to the compiler's argument
  line (for command-line build processes), or by adding the define to
  the list of preprocessor symbols in your IDE GUI (in the MSVC IDE,
  this is done from the "Project"->"Settings" menu, choose the "C/C++"
  tab, then "Preprocessor" from the dropdown box and add the
  appropriate define)).

  It is extremely important that the application programmer uses the
  correct define, as using "SOQT_NOT_DLL" when "SOQT_DLL" is
  correct is likely to cause mysterious crashes.
 */
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
# ifdef SOQT_INTERNAL
#  ifdef SOQT_MAKE_DLL
#   define SOQT_DLL_API __declspec(dllexport)
#  endif /* SOQT_MAKE_DLL */
# else /* !SOQT_INTERNAL */
#  ifdef SOQT_DLL
#   define SOQT_DLL_API __declspec(dllimport)
#  else /* !SOQT_DLL */
#   ifndef SOQT_NOT_DLL
#    error Define either SOQT_DLL or SOQT_NOT_DLL as appropriate for your linkage! See Inventor/Qt/SoQtBasic.h for further instructions.
#   endif /* SOQT_NOT_DLL */
#  endif /* !SOQT_DLL */
# endif /* !SOQT_MAKE_DLL */
#endif /* Microsoft Windows */

/* Empty define to avoid errors when _not_ compiling an MSWindows DLL. */
#ifndef SOQT_DLL_API
# define SOQT_DLL_API
#endif /* !SOQT_DLL_API */

// *************************************************************************

#endif // ! SOQT_BASIC_H
