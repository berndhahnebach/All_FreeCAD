/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef PROPERTYEDITORINPUT_H
#define PROPERTYEDITORINPUT_H


#include <qspinbox.h>
#include "Widgets.h"
#include "kexipropertysubeditor.h"

class QLineEdit;
class QComboBox;
class KexiProperty;
class QToolButton;
class QEvent; 

class GuiExport PropertyEditorInput : public KexiPropertySubEditor
{
	Q_OBJECT

	public:
		PropertyEditorInput(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorInput() {;}

		virtual QVariant	value();
		virtual	void 		setValue(const QVariant &value);

	protected slots:
		void			slotTextChanged(const QString &text);

	protected:
		QLineEdit		*m_lineedit;
};

//INT

class GuiExport PropIntSpinBox : public QSpinBox
{
	Q_OBJECT
	
	public:
	PropIntSpinBox(int lower, int upper, int step, int value, int base, QWidget *parent, const char *name);
	virtual bool eventFilter(QObject *o, QEvent *e);
	~PropIntSpinBox() {;}
};

class GuiExport PropertyEditorSpin : public KexiPropertySubEditor
{
	Q_OBJECT

	public:
		PropertyEditorSpin(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorSpin() {;}

		virtual QVariant	value();
		virtual	void 		setValue(const QVariant &value);

	protected slots:
		void			valueChange(int);
		
	protected:
		QSpinBox		*m_spinBox;
};


//DOUBLE
class GuiExport PropDoubleSpinBox : public FCFloatSpinBox
{
	Q_OBJECT
	
	public:
	PropDoubleSpinBox(QWidget *parent);
	virtual bool eventFilter(QObject *o, QEvent *e);
	~PropDoubleSpinBox() {;}
};

class GuiExport PropertyEditorDblSpin : public KexiPropertySubEditor
{
	Q_OBJECT

	public:
		PropertyEditorDblSpin(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorDblSpin() {;}

		virtual QVariant	value();
		virtual	void 		setValue(const QVariant &value);
	
	protected slots:
		void			valueChange(int);

	protected:
		FCFloatSpinBox		*m_spinBox;
};

//BOOL EDITOR

class GuiExport PropertyEditorBool : public KexiPropertySubEditor
{
	Q_OBJECT

	public:
		PropertyEditorBool(QWidget *parent, KexiProperty *property, const char *name=0);
		~PropertyEditorBool() {;}

		virtual QVariant	value();
		virtual	void 		setValue(const QVariant &value);
		
	protected slots:
		void			setState(int state);
		
	protected:
		QComboBox		*m_toggle;
};

#endif
