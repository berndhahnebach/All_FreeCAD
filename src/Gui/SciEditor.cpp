/***************************************************************************
                          SciEditor.h  -  description
                             -------------------
    begin                : 2002/12/20 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file SciEditor.h
 *  \brief A Qt port to the scintilla library
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/



#include "PreCompiled.h"
#ifndef _PreComp_
#	include <qaction.h>
#	include <qapplication.h>
#	include <qscrollbar.h>
#	include <qpopupmenu.h>
#	include <qstring.h>
#	include <qtimer.h>
#	include <qclipboard.h>
#	include <qdragobject.h>
#	include <qpainter.h>
#	include <qscrollview.h>
#	include <qthread.h>
#endif

#include "SciEditor.h"
#include "Application.h"
#include "scintilla/SciLexer.h"
#include "../Base/Interpreter.h"

#include "DlgEditorImp.h"

static bool lexersLinked = FALSE;

SciTEBase::SciTEBase(QWidget *parent,const char *name,WFlags f)
	: QScrollView(parent,name,f), sciQt(0L), _bControlButton(false)
{
	setFocusPolicy(WheelFocus);

	sciQt = new ScintillaQt(this);

	SendScintilla(SCI_SETCARETPERIOD,QApplication::cursorFlashTime() / 2);

	if (!lexersLinked)
	{
		Scintilla_LinkLexers();

		lexersLinked = TRUE;
	}
}

void SciTEBase::DefineMarker(int marker, int markerType, long fore, long back) 
{
	SendScintilla(SCI_MARKERDEFINE, marker, markerType);
	SendScintilla(SCI_MARKERSETFORE, marker, fore);
	SendScintilla(SCI_MARKERSETBACK, marker, back);
}

SciTEBase::~SciTEBase()
{
	delete sciQt;
}

long SciTEBase::SendScintilla(unsigned int msg,unsigned long wParam, long lParam)
{
	return sciQt->WndProc(msg,wParam,lParam);
}

void SciTEBase::focusInEvent(QFocusEvent * e)
{
	SendScintilla(SCI_SETFOCUS, 1, 0);
}

void SciTEBase::focusOutEvent(QFocusEvent * e)
{
	SendScintilla(SCI_SETFOCUS, 0, 0);
}

bool SciTEBase::focusNextPrevChild (bool next)
{
  // if CTRL is pressed
  if (_bControlButton)
    return QWidget::focusNextPrevChild(next);
  return false;
}

void SciTEBase::keyPressEvent(QKeyEvent * e)
{
  _bControlButton = (e->state() &  ControlButton);

	// function keys
	switch (e->key())
	{
		case Key_F9:
			if (e->state() & ControlButton)
			{
				if (e->state() & ShiftButton)
					clearAllBreakpoints();
				else
					GetConsole().Message("Disable breakpoint: not yet implemented\n");
			}
			else
				toggleBreakpoint(getCurrentLine());

		case Key_F2:
			if (e->state() & ControlButton)
			{
				if (e->state() & ShiftButton)
					clearAllBookmarks();
				else
					toggleBookmark(getCurrentLine());
			}
			else if (e->state() & ShiftButton)
				nextBookmark(false);
			else
				nextBookmark(true);

		default:
			QApplication::sendEvent(sciQt, e);
			break;
	}
}

void SciTEBase::keyReleaseEvent ( QKeyEvent * e )
{
  _bControlButton = (e->state() &  ControlButton);
  QWidget::keyReleaseEvent (e);
}

QSize SciTEBase::sizeHint() const
{
	return sciQt->sizeHint();
}

ScintillaQt* SciTEBase::getTextEditor() const
{
  return sciQt;
}

bool SciTEBase::toggleBreakpoint(int nLine)
{
	if ( SendScintilla (SCI_MARKERGET, nLine,0) & 1 )
	{
		SendScintilla(SCI_MARKERDELETE, nLine, 0);
		return false;
	}
	else
  {
  	SendScintilla(SCI_MARKERADD, nLine, 0);
		return true;
  }
}

void SciTEBase::clearAllBreakpoints()
{
	SendScintilla(SCI_MARKERDELETEALL, SciQt_MARKER_BREAKPNT, 0);
}

void SciTEBase::setBreakpoint(int nLine)
{
	SendScintilla(SCI_MARKERADD, nLine, 0);
}

void SciTEBase::gotoLine(int nLine)
{
	SendScintilla(SCI_GOTOLINE, nLine, 0);

	SendScintilla(SCI_MARKERDELETEALL, 1, 0);
	SendScintilla(SCI_MARKERADD, nLine, 1);
}

void SciTEBase::grabFocus()
{
	SendScintilla(SCI_GRABFOCUS, 0, 0);
}

void SciTEBase::setReadOnly(bool bReadOnly)
{
	SendScintilla(SCI_SETREADONLY, bReadOnly, 0);
}

QString SciTEBase::getLine(int nLine)
{
	QString strLine;
	int nLineLength = SendScintilla(SCI_LINELENGTH, nLine, 0);
  char* szBuf = new char[nLineLength];
	if ( nLineLength>0 )
	{
		SendScintilla(SCI_GETLINE, nLine, (int)szBuf);
		strLine = szBuf;
	}

	return strLine;
}

void SciTEBase::gotoLastLine()
{
	int nLine = SendScintilla(SCI_GETLINECOUNT, 0, 0);
	SendScintilla(SCI_GOTOLINE, nLine-1, 0);
}

int SciTEBase::getCurrentLine()
{
	return SendScintilla(SCI_LINEFROMPOSITION, SendScintilla(SCI_GETCURRENTPOS, 0, 0), 0);
}

void SciTEBase::setStackTraceLevel(int nLevel)
{
	SendScintilla(SCI_MARKERDELETEALL, 0, 0);
	SendScintilla(SCI_MARKERADD, nLevel, 0);
}

CharacterRange SciTEBase::getSelection() 
{
	CharacterRange crange;
	crange.cpMin = SendScintilla(SCI_GETSELECTIONSTART, 0, 0);
	crange.cpMax = SendScintilla(SCI_GETSELECTIONEND, 0, 0);
	return crange;
}

void SciTEBase::assignKey(int key, int mods, int cmd) 
{
	SendScintilla(SCI_ASSIGNCMDKEY,
	           Platform::LongFromTwoShorts(static_cast<short>(key),
	                                       static_cast<short>(mods)), cmd);
}

void SciTEBase::clearAllBookmarks()
{
	SendScintilla(SCI_MARKERDELETEALL, SciQt_MARKER_BOOKMARK, 0);
}

bool SciTEBase::presentBookmark(int nLine) 
{
	if (nLine == -1)
		nLine = getCurrentLine();
	int state = SendScintilla(SCI_MARKERGET, nLine, 0);
	return state & (1 << SciQt_MARKER_BOOKMARK);
}

bool SciTEBase::toggleBookmark(int nLine) 
{
	if ( SendScintilla (SCI_MARKERGET, nLine,1) & 2 )
  {
		SendScintilla(SCI_MARKERDELETE, nLine, 1);
    return false;
  }
	else
  {
  	SendScintilla(SCI_MARKERADD, nLine, 1);
    return true;
  }
}

void SciTEBase::nextBookmark(bool forwardScan) 
{
	int nLine = getCurrentLine();
	int sci_marker = SCI_MARKERNEXT;
	int lineStart = nLine + 1;
	int lineRetry = 0;
	if (!forwardScan) 
  {
		lineStart = nLine - 1;
		lineRetry = SendScintilla(SCI_GETLINECOUNT, 0, 0L);
		sci_marker = SCI_MARKERPREVIOUS;
	}
	int nextLine = SendScintilla(sci_marker, lineStart, 1 << SciQt_MARKER_BOOKMARK);
	if (nextLine < 0)
		nextLine = SendScintilla(sci_marker, lineRetry, 1 << SciQt_MARKER_BOOKMARK);
	if (nextLine < 0 || nextLine == nLine)
		GetConsole().Warning("No other bookmark\n");
	else
		gotoLineEnsureVisible(nextLine);
}

void SciTEBase::gotoLineEnsureVisible(int nLine) 
{
	SendScintilla(SCI_ENSUREVISIBLEENFORCEPOLICY, nLine, 0);
	SendScintilla(SCI_GOTOLINE, nLine, 0);
}

void SciTEBase::foldChanged(int line, int levelNow, int levelPrev) 
{
	if (levelNow & SC_FOLDLEVELHEADERFLAG) 
  {
		if (!(levelPrev & SC_FOLDLEVELHEADERFLAG)) 
    {
			SendScintilla(SCI_SETFOLDEXPANDED, line, 1);
		}
	} 
  else if (levelPrev & SC_FOLDLEVELHEADERFLAG) 
  {
		if (!SendScintilla(SCI_GETFOLDEXPANDED, line, 0)) 
    {
			expand(line, true, false, 0, levelPrev);
		}
	}
}

void SciTEBase::toggleFold(int nLine)
{
	SendScintilla(SCI_TOGGLEFOLD,nLine);
}

void SciTEBase::expand(int &line, bool doExpand, bool force, int visLevels, int level) 
{
	int lineMaxSubord = SendScintilla(SCI_GETLASTCHILD, line, level & SC_FOLDLEVELNUMBERMASK);
	line++;
	while (line <= lineMaxSubord) 
  {
		if (force) 
    {
			if (visLevels > 0)
				SendScintilla(SCI_SHOWLINES, line, line);
			else
				SendScintilla(SCI_HIDELINES, line, line);
		}
    else 
    {
			if (doExpand)
				SendScintilla(SCI_SHOWLINES, line, line);
		}
		int levelLine = level;
		if (levelLine == -1)
			levelLine = SendScintilla(SCI_GETFOLDLEVEL, line, 0);
		if (levelLine & SC_FOLDLEVELHEADERFLAG) {
			if (force) 
      {
				if (visLevels > 1)
					SendScintilla(SCI_SETFOLDEXPANDED, line, 1);
				else
					SendScintilla(SCI_SETFOLDEXPANDED, line, 0);
				expand(line, doExpand, force, visLevels - 1);
			} 
      else 
      {
				if (doExpand) 
        {
					if (!SendScintilla(SCI_GETFOLDEXPANDED, line, 0))
						SendScintilla(SCI_SETFOLDEXPANDED, line, 1);
					expand(line, true, force, visLevels - 1);
				}
        else 
        {
					expand(line, false, force, visLevels - 1);
				}
			}
		} 
    else 
    {
			line++;
		}
	}
}

void SciTEBase::foldAll() 
{
	SendScintilla(SCI_COLOURISE, 0, -1);
	int maxLine = SendScintilla(SCI_GETLINECOUNT, 0, 0);
	bool expanding = true;
	for (int lineSeek = 0; lineSeek < maxLine; lineSeek++) 
  {
		if (SendScintilla(SCI_GETFOLDLEVEL, lineSeek, 0) & SC_FOLDLEVELHEADERFLAG) 
    {
			expanding = !SendScintilla(SCI_GETFOLDEXPANDED, lineSeek, 0);
			break;
		}
	}
	for (int line = 0; line < maxLine; line++) 
  {
		int level = SendScintilla(SCI_GETFOLDLEVEL, line, 0);
		if ((level & SC_FOLDLEVELHEADERFLAG) && (SC_FOLDLEVELBASE == (level & SC_FOLDLEVELNUMBERMASK))) 
    {
			if (expanding) 
      {
				SendScintilla(SCI_SETFOLDEXPANDED, line, 1);
				expand(line, true, false, 0, level);
				line--;
			}
      else 
      {
				int lineMaxSubord = SendScintilla(SCI_GETLASTCHILD, line, -1);
				SendScintilla(SCI_SETFOLDEXPANDED, line, 0);
				if (lineMaxSubord > line)
					SendScintilla(SCI_HIDELINES, line + 1, lineMaxSubord);
			}
		}
	}
}

bool SciTEBase::clickMargin(int position, int modifiers) 
{
	int lineClick = SendScintilla(SCI_LINEFROMPOSITION, position, 0);
	if ((modifiers & SCMOD_SHIFT) && (modifiers & SCMOD_CTRL)) 
  {
		foldAll();
	} 
  else 
  {
		int levelClick = SendScintilla(SCI_GETFOLDLEVEL, lineClick, 0);
		if (levelClick & SC_FOLDLEVELHEADERFLAG) 
    {
			if (modifiers & SCMOD_SHIFT) 
      {
				SendScintilla(SCI_SETFOLDEXPANDED, lineClick, 1);
				expand(lineClick, true, true, 100, levelClick);
			} 
      else if (modifiers & SCMOD_CTRL) 
      {
				if (SendScintilla(SCI_GETFOLDEXPANDED, lineClick, 0)) 
        {
					SendScintilla(SCI_SETFOLDEXPANDED, lineClick, 0);
					expand(lineClick, false, true, 0, levelClick);
				} 
        else 
        {
					SendScintilla(SCI_SETFOLDEXPANDED, lineClick, 1);
					expand(lineClick, true, true, 100, levelClick);
				}
			} 
      else 
      {
				SendScintilla(SCI_TOGGLEFOLD, lineClick, 0);
			}
		}
	}

	return true;
}

// ------------------------------------------------------------------------

FCMacroEditor::FCMacroEditor(QWidget *parent,const char *name, WFlags f)
	: SciTEBase(parent, name, f)
{
  GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Windows/Editor")->Attach(this);
  loadSettings();
}

FCMacroEditor::~FCMacroEditor()
{
  GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Windows/Editor")->Detach(this);
}

void FCMacroEditor::OnChange(FCSubject<const char*> &rCaller,const char* rcReason)
{
  FCParameterGrp& rclGrp = ((FCParameterGrp&)rCaller);
  
  const char font[] = "Verdana";
  const char monospace[] = "Courier";
  const short fontsize = rclGrp.GetGroup("FontSize")->GetInt("currentItem", 3) + 6;

  // style 32: default
  SendScintilla(SCI_STYLESETFONT,32, (sptr_t) font);
  SendScintilla(SCI_STYLESETSIZE,32, fontsize);
  
  // set up basic features (iguides on, tab=3, tabs-to-spaces, EOL=CRLF)
  SendScintilla(SCI_SETINDENTATIONGUIDES,1,0);
  SendScintilla(SCI_SETTABWIDTH,3,0);
  SendScintilla(SCI_SETUSETABS,0,0);
  SendScintilla(SCI_SETEOLMODE,SC_EOL_CRLF,0);

  if (strcmp(rcReason, "Lexer") == 0)
  {
    int l = rclGrp.GetInt("Lexer");

    if (l == 0)
    {
      const char keywords[] = "and assert break class continue def del elif else except exec "
	                            "finally for from global if import in is lambda None not or "
	                            "pass print raise return try while yield";
      // set style bits, choose the lexer and set the keywords list
      SendScintilla(SCI_SETSTYLEBITS,5,0);
      SendScintilla(SCI_SETLEXER,SCLEX_PYTHON,0);
      SendScintilla(SCI_SETKEYWORDS,0,(sptr_t)keywords);
    }
    else if (l == 1)
    {
      const char keywords[] = "XML";
      SendScintilla(SCI_SETSTYLEBITS,5,0);
      SendScintilla(SCI_SETLEXER,SCLEX_XML,0);
      SendScintilla(SCI_SETKEYWORDS,0,(sptr_t)keywords);
    }
    else
    {
      SendScintilla(SCI_SETLEXER,SCLEX_NULL,0);
    }
  }
  else if (strcmp(rcReason, "Text") == 0)
  {
    // text color
    SendScintilla(SCI_STYLESETFORE,SciQt_TEXT, rclGrp.GetInt("Text", GetDefCol().GetColor("Text")));
  }
  else if (strcmp(rcReason, "Comment") == 0)
  {
    // comment color
    SendScintilla(SCI_STYLESETFONT,SciQt_COMMENT, (int)monospace);
    SendScintilla(SCI_STYLESETSIZE,SciQt_COMMENT, fontsize);
    SendScintilla(SCI_STYLESETFORE,SciQt_COMMENT, rclGrp.GetInt("Comment", GetDefCol().GetColor("Comment")));
  }
  else if (strcmp(rcReason, "Block comment") == 0)
  {
    // block comment color
    SendScintilla(SCI_STYLESETFONT,SciQt_COMMENTBLOCK, (int)monospace);
    SendScintilla(SCI_STYLESETSIZE,SciQt_COMMENTBLOCK, fontsize);
    SendScintilla(SCI_STYLESETFORE,SciQt_COMMENTBLOCK, rclGrp.GetInt("Block comment", GetDefCol().GetColor("Block comment")));
  }
  else if (strcmp(rcReason, "Number") == 0)
  {
    // number color
    SendScintilla(SCI_STYLESETFORE,SciQt_NUMBER, rclGrp.GetInt("Number", GetDefCol().GetColor("Number")));
  }
  else if (strcmp(rcReason, "String") == 0)
  {
	  // string color
    SendScintilla(SCI_STYLESETFORE,SciQt_STRING, rclGrp.GetInt("String", GetDefCol().GetColor("String")));      
  }
  else if (strcmp(rcReason, "Keyword") == 0)
  {
	  // keyword color
    SendScintilla(SCI_STYLESETFONT,SciQt_KEYWORDS, (int)font);
    SendScintilla(SCI_STYLESETSIZE,SciQt_KEYWORDS, (int)fontsize);
    SendScintilla(SCI_STYLESETFORE,SciQt_KEYWORDS, rclGrp.GetInt("Keyword", GetDefCol().GetColor("Keyword")));
    SendScintilla(SCI_STYLESETBOLD,SciQt_KEYWORDS, 1);
  }
  else if (strcmp(rcReason, "Breakpoint") == 0)
  {
	  // breakpoint color
	  SendScintilla(SCI_MARKERDEFINE,  SciQt_MARKER_BREAKPNT, SC_MARK_CIRCLE);
	  SendScintilla(SCI_MARKERSETFORE, SciQt_MARKER_BREAKPNT, rclGrp.GetInt("Breakpoint", GetDefCol().GetColor("Breakpoint")));
	  SendScintilla(SCI_MARKERSETBACK, SciQt_MARKER_BREAKPNT, rclGrp.GetInt("Breakpoint", GetDefCol().GetColor("Breakpoint")));
  }
  else if (strcmp(rcReason, "Bookmark") == 0)
  {
	  // bookmark color
	  SendScintilla(SCI_MARKERDEFINE,  SciQt_MARKER_BOOKMARK, SC_MARK_ARROW);
	  SendScintilla(SCI_MARKERSETFORE, SciQt_MARKER_BOOKMARK, rclGrp.GetInt("Bookmark", GetDefCol().GetColor("Bookmark")));
	  SendScintilla(SCI_MARKERSETBACK, SciQt_MARKER_BOOKMARK, rclGrp.GetInt("Bookmark", GetDefCol().GetColor("Bookmark")));
  }
  else if (strcmp(rcReason, "Character") == 0)
  {
    // character
    SendScintilla(SCI_STYLESETFORE,SciQt_CHARACTER, rclGrp.GetInt("Character", GetDefCol().GetColor("Character")));
    // triple
    SendScintilla(SCI_STYLESETFORE,SciQt_TRIPLE, rclGrp.GetInt("Character", GetDefCol().GetColor("Character")));
    // triple double
    SendScintilla(SCI_STYLESETFORE,SciQt_TRIPLEDOUBLE, rclGrp.GetInt("Character", GetDefCol().GetColor("Character")));
    // end of line
    SendScintilla(SCI_STYLESETFORE,SciQt_STRINGEOL, rclGrp.GetInt("Character", GetDefCol().GetColor("Character")));
  }
  else if (strcmp(rcReason, "Class name") == 0)
  {
    // class name
    SendScintilla(SCI_STYLESETFORE,SciQt_CLASSNAME, rclGrp.GetInt("Class name", GetDefCol().GetColor("Class name")));
  }
  else if (strcmp(rcReason, "Define name") == 0)
  {
    // define name
    SendScintilla(SCI_STYLESETFORE,SciQt_DEFNAME, rclGrp.GetInt("Define name", GetDefCol().GetColor("Define name")));
  }
  else if (strcmp(rcReason, "Operator") == 0)
  {
    // operator
    SendScintilla(SCI_STYLESETFORE,SciQt_OPERATOR, rclGrp.GetInt("Operator", GetDefCol().GetColor("Operator")));
  }
  else if (strcmp(rcReason, "Text Selection") == 0)
  {
  }
  else if (strcmp(rcReason, "EnableFolding") == 0)
  {
	  if (rclGrp.GetBool("EnableFolding", true))
    {
      SendScintilla(SCI_SETPROPERTY, (uptr_t) "fold", (sptr_t) "1");
	    SendScintilla(SCI_SETPROPERTY, (uptr_t) "fold.compact", (sptr_t) "1");
	    SendScintilla(SCI_SETPROPERTY, (uptr_t) "fold.symbols", (sptr_t) "1");
	    SendScintilla(SCI_SETPROPERTY, (uptr_t) "styling.within.preprocessor", (sptr_t) "1");
	    // set styles.
	    long fore = 0;
	    long back = 0x00ffffff;

	    SendScintilla(SCI_SETFOLDFLAGS, 16, 0);
	    // to put the folder markers in the line number region
	    //SendScintilla(SCI_SETMARGINMASKN, 0, SC_MASK_FOLDERS);
	    SendScintilla(SCI_SETMODEVENTMASK, SC_MOD_CHANGEFOLD, 0);

	    // create a margin column for the folding symbols
	    SendScintilla(SCI_SETMARGINTYPEN, 2, SC_MARGIN_SYMBOL);
	    SendScintilla(SCI_SETMARGINWIDTHN, 2, /*foldMargin ? foldMarginWidth :*/ 16);
	    SendScintilla(SCI_SETMARGINMASKN, 2, SC_MASK_FOLDERS);
	    SendScintilla(SCI_SETMARGINSENSITIVEN, 2, 1);
	    DefineMarker(SC_MARKNUM_FOLDEROPEN, SC_MARK_MINUS, back, fore);
	    DefineMarker(SC_MARKNUM_FOLDER, SC_MARK_PLUS, back, fore);
	    DefineMarker(SC_MARKNUM_FOLDERSUB, SC_MARK_EMPTY, back, fore);
	    DefineMarker(SC_MARKNUM_FOLDERTAIL, SC_MARK_EMPTY, back, fore);
	    DefineMarker(SC_MARKNUM_FOLDEREND, SC_MARK_EMPTY, back, fore);
	    DefineMarker(SC_MARKNUM_FOLDEROPENMID, SC_MARK_EMPTY, back, fore);
	    DefineMarker(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_EMPTY, back, fore);
    }
  }
  else if (strcmp(rcReason, "EnableLineNumber") == 0)
  {
	  // line number
	  if (rclGrp.GetBool("EnableLineNumber", true))
	  {
		  SendScintilla(SCI_SETMARGINTYPEN, 0, SC_MARGIN_NUMBER);
		  int pixelWidth = 3 * SendScintilla(SCI_TEXTWIDTH, STYLE_LINENUMBER, (int)"9");
		  SendScintilla(SCI_SETMARGINWIDTHN, 0, pixelWidth);
    }
    else
    {
		  SendScintilla(SCI_SETMARGINTYPEN, 0, 0);
    }
  }
  else
  {
	  // margin stuff
	  SendScintilla(SCI_SETMARGINTYPEN,  1, SC_MARGIN_SYMBOL);
	  SendScintilla(SCI_SETMARGINWIDTHN, 1, 10);
	  SendScintilla(SCI_SETMARGINSENSITIVEN, 1, TRUE);
  }
}

void FCMacroEditor::loadSettings()
{
  FCParameterGrp::handle hGrp = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Windows/Editor");
  hGrp->Notify("");
  hGrp->Notify("Lexer");
  hGrp->Notify("Text");
  hGrp->Notify("Comment");
  hGrp->Notify("Block comment");
  hGrp->Notify("Number");
  hGrp->Notify("String");
  hGrp->Notify("Keyword");
  hGrp->Notify("Breakpoint");
  hGrp->Notify("Bookmark");
  hGrp->Notify("Character");
  hGrp->Notify("Class name");
  hGrp->Notify("Define name");
  hGrp->Notify("Operator");
  hGrp->Notify("EnableLineNumber");
  hGrp->Notify("EnableFolding");
}

// ------------------------------------------------------------------------

FCPythonConsole::FCPythonConsole(QWidget *parent,const char *name, WFlags f)
	: SciTEBase(parent, name, f)
{
  const char font[] = "Verdana";
  const char monospace[] = "Courier";
  const short fontsize = 9;

  // style 32: default
  SendScintilla(SCI_STYLESETFONT,32, (sptr_t) font);
  SendScintilla(SCI_STYLESETSIZE,32, fontsize);
  
  // set up basic features (iguides on, tab=3, tabs-to-spaces, EOL=CRLF)
  SendScintilla(SCI_SETINDENTATIONGUIDES,1,0);
  SendScintilla(SCI_SETTABWIDTH,3,0);
  SendScintilla(SCI_SETUSETABS,0,0);
  SendScintilla(SCI_SETEOLMODE,SC_EOL_CRLF,0);

  const char keywords[] = "and assert break class continue def del elif else except exec "
	                        "finally for from global if import in is lambda None not or "
	                        "pass print raise return try while yield";
  // set style bits, choose the lexer and set the keywords list
  SendScintilla(SCI_SETSTYLEBITS,5,0);
  SendScintilla(SCI_SETLEXER,SCLEX_PYTHON,0);
  SendScintilla(SCI_SETKEYWORDS,0,(sptr_t)keywords);

  // text color
  SendScintilla(SCI_STYLESETFORE,SciQt_TEXT, 0);

  // comment color
  SendScintilla(SCI_STYLESETFONT,SciQt_COMMENT, (int)monospace);
  SendScintilla(SCI_STYLESETSIZE,SciQt_COMMENT, fontsize);
  SendScintilla(SCI_STYLESETFORE,SciQt_COMMENT, 43520);

  // number color
  SendScintilla(SCI_STYLESETFORE,SciQt_NUMBER, 16711680);

	// string color
  SendScintilla(SCI_STYLESETFORE,SciQt_STRING, 255);      

	// keyword color
  SendScintilla(SCI_STYLESETFONT,SciQt_KEYWORDS, (int)font);
  SendScintilla(SCI_STYLESETSIZE,SciQt_KEYWORDS, (int)fontsize);
  SendScintilla(SCI_STYLESETFORE,SciQt_KEYWORDS, 16711680);
  SendScintilla(SCI_STYLESETBOLD,SciQt_KEYWORDS, 1);

	nInsertTabs = 0;
}

FCPythonConsole::~FCPythonConsole()
{
}

void FCPythonConsole::keyPressEvent(QKeyEvent * e)
{
	SciTEBase::keyPressEvent(e);

	switch (e->key())
	{
	  case Key_Colon:
			nInsertTabs++;
			break;

	  case Key_Return:
	  case Key_Enter:
			if (nInsertTabs > 0)
			{
				QKeyEvent* ke = new QKeyEvent(QKeyEvent::KeyPress, Key_Tab, Key_Tab, Qt::NoButton);
				for (int i=0; i<nInsertTabs;i++)
				{
					QApplication::sendEvent(sciQt, ke);
				}
			}
			else
			{
				try{
//					GetInterpreter().Launch(sciTE->CopyRange(0,100));
//					GetInterpreter().Launch("print i\n");
				}
				catch(...)
				{
				}
				nInsertTabs=0;
			}

		  break;
	}
}

// ------------------------------------------------------------------------

FCScintillaView::FCScintillaView( QWidget* parent, const char* name)
: FCView(0,parent, name)  
{
	setCaption("Editor");
	setTabCaption("Editor");
	resize( 400, 300 );

	view = new FCMacroEditor(this);
}

FCScintillaView::~FCScintillaView()
{
}

void FCScintillaView::resizeEvent(QResizeEvent* e)
{
	view->resize(e->size().width(),e->size().height());
	FCView::resizeEvent( e);
}

bool FCScintillaView::OnMsg(const char* pMsg)
{
	if (strcmp(pMsg,"Save")==0){
		Save();
		return true;
	}
	if (strcmp(pMsg,"SaveAs")==0){
		SaveAs();
		return true;
	}
	if (strcmp(pMsg,"Cut")==0){
		Cut();
		return true;
	}
	if (strcmp(pMsg,"Copy")==0){
		Copy();
		return true;
	}
	if (strcmp(pMsg,"Paste")==0){
		Paste();
		return true;
	}
	if (strcmp(pMsg,"Undo")==0){
		Undo();
		return true;
	}
	if (strcmp(pMsg,"Redo")==0){
		Redo();
		return true;
	}

	GetConsole().Log("FCScintillaView::OnMsg() unhandled \"%s\"\n",pMsg);
	return false;
}

// Message handler test
bool FCScintillaView::OnHasMsg(const char* pMsg)
{/*
	if (strcmp(pMsg,"Save")==0)  return true;
	if (strcmp(pMsg,"Print")==0) return true;
	if (strcmp(pMsg,"Cut")==0)
  {
		bool writable = !view->getTextEditor()->WndProc(SCI_GETREADONLY, 0, 0);
    return (writable && (view->getTextEditor()->SelectionEmpty() == false));
  }
	if (strcmp(pMsg,"Copy")==0)
  {
    return !(view->getTextEditor()->SelectionEmpty());
  }
	if (strcmp(pMsg,"Paste")==0)
  {
    QClipboard *cb = QApplication::clipboard();
    QString text;

    // Copy text from the clipboard (paste)
    text = cb->text();

    return (text && view->getTextEditor()->CanPaste());
  }
	if (strcmp(pMsg,"Undo")==0)
  {
    return view->SendScintilla(SCI_CANUNDO);
  }
	if (strcmp(pMsg,"Redo")==0)
  {
    return view->SendScintilla(SCI_CANREDO);
  }*/
	return false;
}

bool FCScintillaView::CanClose(void)
{
  if (!view->SendScintilla(SCI_GETMODIFY))
    return true;

  switch(QMessageBox::warning( this, "Unsaved document","Save file before close?","Yes","No","Cancel",0,2))
	{
		case 0:
			return Save();
		case 1:
			return true;
		case 2:
			return false;
		default:
			return false;
	}
}

bool FCScintillaView::Save(void)
{
  if (!view->SendScintilla(SCI_GETMODIFY))
    return true;

  // check if saved ever before
	if (isAlreadySavedBefore())
  {
    saveFile();
    return true;
  }
	else
  {
		return SaveAs();
  }
}

bool FCScintillaView::SaveAs(void)
{
	QString fn = QFileDialog::getSaveFileName(QString::null, "FreeCAD macro (*.FCMacro);;Python (*.py)", this);
	if (!fn.isEmpty())
	{
    _fileName = fn;
    saveFile();
    return true;
	}
	else
	{
    return false;
	}
}

bool FCScintillaView::Open(void)
{
	QString file = QFileDialog::getOpenFileName(QString::null, "Macro files (*.py *.FCMacro);;Python (*.py);;FreeCAD macro (*.FCMacro)", this);
	if ( file.isEmpty() ) 
		return false;

	OpenFile(file);
	setCaption(file);
	QString name = file.left(file.findRev('.'));
	setTabCaption(name);

	return true;
}

void FCScintillaView::OpenFile (const QString& fileName)
{
	view->getTextEditor()->WndProc(SCI_CLEARALL, 0, 0);
	view->getTextEditor()->WndProc(SCI_SETSAVEPOINT, 0, 0);
  view->getTextEditor()->WndProc(SCI_CANCEL, 0, 0);
  view->getTextEditor()->WndProc(SCI_SETUNDOCOLLECTION, 0, 0);

  QString line;
  _fileName = fileName;
  QFile file(fileName);
   
  if( !file.open(IO_ReadOnly))
     return;

  QTextStream in(&file);
  QString text;

  while( !in.atEnd() ){
    line = in.readLine();
    text = line + "\n";
    view->getTextEditor()->WndProc(SCI_ADDTEXT, text.length(), reinterpret_cast<sptr_t>(static_cast<const char *>(text.latin1())));
  }

  file.close();

  view->getTextEditor()->WndProc(SCI_SETUNDOCOLLECTION, 1, 0);
	view->getTextEditor()->WndProc(SCI_SETSAVEPOINT, 0, 0);
	view->getTextEditor()->WndProc(SCI_GOTOPOS, 0, 0);
}

void FCScintillaView::Cut(void)
{
  view->SendScintilla(SCI_CUT);
}

void FCScintillaView::Copy(void)
{
  view->SendScintilla(SCI_COPY);
}

void FCScintillaView::Paste(void)
{
  view->SendScintilla(SCI_PASTE);
}

void FCScintillaView::Undo(void)
{
  view->SendScintilla(SCI_UNDO);
}

void FCScintillaView::Redo(void)
{
  view->SendScintilla(SCI_REDO);
}

void FCScintillaView::Print(QPainter& cPrinter)
{
	// no printing yet ;-)
	assert(0);
}

void FCScintillaView::saveFile()
{
	QFile file(_fileName);
  if( !file.open(IO_WriteOnly))
    return;

  const int blockSize = 131072;

	TextRange tr;
	char data[blockSize + 1];
	int lengthDoc = view->getTextEditor()->WndProc(SCI_GETLENGTH, 0, 0);
	
  QTextStream out(&file);
  for (int i = 0; i < lengthDoc; i += blockSize) 
	{
		int grabSize = lengthDoc - i;
		if (grabSize > blockSize)
			grabSize = blockSize;

		tr.chrg.cpMin = i;
		tr.chrg.cpMax = i + grabSize;
		tr.lpstrText = data;
		view->getTextEditor()->WndProc(SCI_GETTEXTRANGE, 0, long(&tr));

    out << data;
	}

	view->getTextEditor()->WndProc(SCI_SETSAVEPOINT, 0,0);

  file.close();
  view->SendScintilla(SCI_SETSAVEPOINT);
  
  return;
}

bool FCScintillaView::isAlreadySavedBefore()
{
  return (!_fileName.isEmpty());
}

#include "moc_SciEditor.cpp"
