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

// @configure_input@

/*!
  \class SoAny SoAny.h
  \brief The SoAny class is a collection of common toolkit methods.

  SoAny contains internal (ie not accessible to the application
  programmer) functionality which needs to be "globally" visible to
  all parts of the toolkit.
*/

// includes the FreeCAD configuration
#include "../../../Config.h"

#ifdef HAVE_WINDOWS_H
#include <windows.h> /* GetEnvironmentVariable() */
#endif /* HAVE_WINDOWS_H */

#include <Inventor/Qt/SoAny.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <stdlib.h>
#include <assert.h>

#if SOQT_DEBUG
#include <Inventor/errors/SoDebugError.h>
#endif // COIN_DEBUG

static SoAny * single_instance = NULL;

#ifndef DOXYGEN_SKIP_THIS

class soany_cache_context {
public:
  soany_cache_context(void * display, void * screen)
    : display(display), screen(screen), cacheid(-1) { } 
  ~soany_cache_context() { }

  SbBool isSameCacheContext(void * display, void * screen) const {
    return (this->display == display && this->screen == screen);
  }
  SbBool findContext(void * context) const {
    for (int i = 0; i < this->contextlist.getLength(); i++) {
      if (this->contextlist[i] == context) return TRUE;
    }
    return FALSE;
  }
  void * getFirstContext(void) const {
    assert(this->contextlist.getLength());
    return this->contextlist[0];
  }
  void addContext(void * context) {
    assert(!this->findContext(context));
    this->contextlist.append(context);
  }

  SbBool tryRemoveContext(void * context) {
    for (int i = 0; i < this->contextlist.getLength(); i++) {
      if (this->contextlist[i] == context) {
        this->contextlist.remove(i);
        return TRUE;
      }
    }
    return FALSE;
  }
  SbBool isEmpty(void) const {
    return this->contextlist.getLength() == 0;
  }
  int getCacheContextId(void) {
    if (this->cacheid < 0) {
#ifdef __COIN__
      // Coin has a method for generating an unique cache context id.
      // Unfortunately, this method does not exists for SGI/TGS Inventor.
      this->cacheid = (int) SoGLCacheContextElement::getUniqueCacheContext();
#else // ! __COIN__
      this->cacheid = soany_cache_context::oiv_counter++;
#endif // ! __COIN__
    }
    return this->cacheid;
  }

private:
  void * display;
  void * screen;
  int cacheid;
  SbPList contextlist;
  static int oiv_counter;
};

#if 0 // FIXME: not sure if this is ok, check with pederb. 20010806 mortene.
// A semi-random magic number -- we want to avoid crashing with any
// cache context id values internally used in SGI / TGS OIV as magic
// numbers.
int soany_cache_context::oiv_counter = 4242;
#else
int soany_cache_context::oiv_counter = 0;
#endif

#endif // DOXYGEN_SKIP_THIS



// private constructor
SoAny::SoAny(void) 
  : fatalcb(NULL)
{
}
 
// private destructor
SoAny::~SoAny()
{
  for (int i = 0; i < this->cclist.getLength(); i++) {
    delete (soany_cache_context*) this->cclist[i];
  }
}

// This little hack used to make the OSF1/cxx compiler accept the
// argument to the atexit() call below. Problem reported by Guy
// Barrand.
extern "C" { typedef void(*SoAny_C_func)(void); }

/*!
  Returns pointer to the single instance of this class (implemented
  according to the singleton design pattern).
*/
SoAny * 
SoAny::si(void)
{
  if (single_instance == NULL) {
    single_instance = new SoAny;
    (void)atexit((SoAny_C_func)cleanup_si);
  }
  return single_instance;
}


//  Free memory/resources used by the singleton object. Will be called
//  automatically on exit.
void 
SoAny::cleanup_si(void)
{
  delete single_instance;
  single_instance = NULL;
}

/*!  
  Returns a pointer to any previous context with which GL sharing
  is possible, or \e NULL if none found.  
*/
void * 
SoAny::getSharedGLContext(void * display, void * screen)
{
  for (int i = 0; i < this->cclist.getLength(); i++) {
    soany_cache_context * cc = (soany_cache_context*) this->cclist[i];
    if (cc->isSameCacheContext(display, screen)) {
      return cc->getFirstContext();
    }
  }
  return NULL;
}

/*!
  Adds GL context information. Might be platform dependent what the \a
  display and \a screen will be. Call this method once when the
  context is created. In the Coin GUI toolkits, \a context will be a
  pointer to a SoQtGLWidget.
*/
void 
SoAny::registerGLContext(void * context, void * display, void * screen)
{
#if SOQT_DEBUG && 0 // debug
  SoDebugError::postInfo("SoAny::registerGLContext",
                         "context==%p, display==%p, screen==%p",
                         context, display, screen);
#endif // debug

  int i, n = this->cclist.getLength();
  for (i = 0; i < n; i++) {
    soany_cache_context * cc = (soany_cache_context*) this->cclist[i];
    if (cc->isSameCacheContext(display, screen)) break;
  }
  if (i == n) this->cclist.append(new soany_cache_context(display, screen));
  soany_cache_context * cc = (soany_cache_context*) this->cclist[i];
  cc->addContext(context);
}

/*!
  Removes a GL context from the list of contexts. Should be called
  when the context/widget is destructed.
*/
void 
SoAny::unregisterGLContext(void * context)
{
#if SOQT_DEBUG && 0 // debug
  SoDebugError::postInfo("SoAny::unregisterGLContext", "context==%p", context);
#endif // debug

  for (int i = 0; i < this->cclist.getLength(); i++) {
    soany_cache_context * cc = (soany_cache_context*) this->cclist[i];
    if (cc->tryRemoveContext(context)) {
      if (cc->isEmpty()) {
        delete cc;
        this->cclist.remove(i);
      }
      return;
    }
  }
  assert(FALSE && "couldn't find context in internal list");
}

/*!
  Returns a shared cache context id, to be used as argument
  in SoGLRenderAction::setCacheContext().
*/
int 
SoAny::getSharedCacheContextId(void * context)
{
  for (int i = 0; i < this->cclist.getLength(); i++) {
    soany_cache_context * cc = (soany_cache_context*) this->cclist[i];
    if (cc->findContext(context)) return cc->getCacheContextId();
  }
#if SOQT_DEBUG
  SoDebugError::post("SoAny::getSharedCacheContextId",
                     "could not find shared cache context for context %p "
                     "(shared context handling not implemented for this "
                     "toolkit?)", context);
#endif // SOQT_DEBUG
  return 0;
}

/*!
  Set up a fatal error handler in case of "impossible" errors.  Calls
  to this function are calls forwarded from
  SoQt::setFatalErrorHandler().

  Returns previous fatal error handler callback, or \c NULL if none.

  \sa SoQt::setFatalErrorHandler()
 */
SoQt::FatalErrorCB *
SoAny::setFatalErrorHandler(SoQt::FatalErrorCB * cb, void * userdata)
{
  SoQt::FatalErrorCB * oldcb = this->fatalcb;
  this->fatalcb = cb;
  this->userdata = userdata;
  return oldcb;
}

/*!
  In the case of a fatal error condition in the library, this method
  should be used to invoke the application fatal error handler.

  Returns \c TRUE if an error handler was set up, or \c FALSE if none.

  If no handler was set up, a dialog box with the error string will be
  shown and then the application will exit.

  A very important issue to be aware of for "client code" around in
  the So*-libraries is that this function will not return if the
  application programmer has set up a fatal error handler which throws
  an exception. Because of this, the "as good as possible" clean-up
  code around the points of fatal error failures needs to be executed
  \e before calling this function.

  \sa SoAny::setFatalErrorHandler()
 */
SbBool
SoAny::invokeFatalErrorHandler(SbString errmsg, SoQt::FatalErrors errcode)
{
  // First trigger the internal clean-up routines.
  int internals = this->internalfehandlers.getLength();
  for (int idx = 0; idx < internals; idx++) {
    SoAny::InternalFatalErrorCB * cb =
      (SoAny::InternalFatalErrorCB *)this->internalfehandlers[idx];
    (*cb)(this->internalfedata[idx]);
  }

  // If no app-programmer registered callback, throw up our own
  // message dialog.
  if (this->fatalcb == NULL) {
    SoQt::createSimpleErrorDialog(NULL,
                                     "Fatal application error",
                                     errmsg.getString(),
                                     "Application will exit.");
    return FALSE;
  }

  fatalcb(errmsg, errcode, this->userdata);
  return TRUE;
}


/*!
  Lets us register an internal callback to run when a fatal error is
  detected.

  Internal clean-up functions is sometimes necessary because we set up
  external references to internal resources which may not be valid
  after a fatal error hits.
*/
void
SoAny::addInternalFatalErrorHandler(SoAny::InternalFatalErrorCB * cb,
                                    void * userdata)
{
  this->internalfehandlers.append((void *)cb);
  this->internalfedata.append(userdata);
}


/**************************************************************************/

/*** Singlelinked list for the environment variables. *********************/

/*
  FIXME: should implement a generic (macro-based) singlelinked list
  abstraction in C (the following code could be a good starting
  point). Then axe this code. Then move the various listhandling stuff
  in the Coin library from the SbList<> template to the C-based one to
  aid any future transition to pure C. 20010821 mortene.

  UPDATE: SbList isn't really a linked list, but in fact a growable
  array -- so the code below can not be used as a replacement. Still,
  we should have a generic linked list class. 20011220 mortene.
*/

static struct envvar_data * envlist_head = NULL;
static struct envvar_data * envlist_tail = NULL;

struct envvar_data {
  char * name;
  char * val;
  struct envvar_data * next;
};

static void
envlist_cleanup(void)
{
  struct envvar_data * ptr = envlist_head;
  while (ptr != NULL) {
    struct envvar_data * tmp = ptr;
    free(ptr->name);
    free(ptr->val);
    ptr = ptr->next;
    free(tmp);
  }
}

static void
envlist_append(struct envvar_data * item)
{
  item->next = NULL;
  if (envlist_head == NULL) {
    envlist_head = item;
    envlist_tail = item;
    (void)atexit((SoAny_C_func)envlist_cleanup);
  }
  else {
    envlist_tail->next = item;
    envlist_tail = item;
  }
}

/**************************************************************************/

/*
  When working with MSWindows applications using the library as a DLL,
  setenv() / getenv() will not work as expected, as the application
  and the DLL has different instances of the C library with different
  datastructures on different heaps. That's why we need this
  abstraction around the C-libs getenv() vs the Win32 API
  GetEnvironmentVariable() function.

  Note: do not deallocate the returned string -- they are supposed to
  be permanently available to all callers. Deallocating the resources
  is the responsibility of the application exit cleanup code.
*/
const char *
SoAny::getenv(const char * envname)
{
  // Important note: this code is identical to the code in
  // Coin/src/tidbits.c. If you do bugfixes or whatever -- keep them
  // in sync!

#ifdef HAVE_GETENVIRONMENTVARIABLE
  int neededsize;

  /* Try to find envvar among those requested earlier on. */
  struct envvar_data * ptr = envlist_head;
  while (ptr != NULL) {
    if (strcmp(ptr->name, envname) == 0) { return ptr->val; }
    ptr = ptr->next;
  }

  neededsize = GetEnvironmentVariable(envname, NULL, 0);
  if (neededsize > 1) {
    int resultsize;
    struct envvar_data * item;
    char * tmpbuf = (char *)malloc(neededsize + 1);
    /* Augh. Could we handle this any better? */
    if (tmpbuf == NULL) { return NULL; }
    resultsize = GetEnvironmentVariable(envname, tmpbuf, neededsize);
    /* Augh. Could we handle this any better? */
    if (resultsize != neededsize-1) {
      free(tmpbuf);
      return NULL;
    }

    item = (struct envvar_data *)malloc(sizeof(struct envvar_data));
    /* Augh. Could we handle this any better? */
    if (item == NULL) {
      free(tmpbuf);
      return NULL;
    }
    item->name = strdup(envname);
    /* FIXME: won't work with environment variables that are changed
       "mid-execution". Rather nasty flaw if this is to be considered
       a general replacement for getenv(). 20010821 mortene.*/
    item->val = strdup(tmpbuf);
    /* Augh. Could we handle this any better? */
    if ((item->name == NULL) || (item->val == NULL)) {
      free(tmpbuf);
      if (item->name != NULL) { free(item->name); }
      if (item->val != NULL) { free(item->val); }
      free(item);
      return NULL;
    }
    envlist_append(item);
    return item->val;
  }
  /* FIXME: fall through to the standard C-lib's getenv() if
     GetEnvironmentVariable() fails? If this can cause a crash, it's
     obviously a bad idea, but if not -- it should be
     done. Investigate. 20010821 mortene. */
  return NULL;
#else /* !HAVE_GETENVIRONMENTVARIABLE */
  return ::getenv(envname);
#endif /* !HAVE_GETENVIRONMENTVARIABLE */
}

/**************************************************************************/
