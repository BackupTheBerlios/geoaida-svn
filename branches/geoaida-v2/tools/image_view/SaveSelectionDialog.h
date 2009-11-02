/***************************************************************************
                          SaveSelectionDialog.h  -
                             -------------------
    begin                : Mon Nov 2 2009
    copyright            : (C) 2009 TNT, Uni Hannover
    authors              : Karsten Vogt

    email                : vogt@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _SAVESELECTIONDIALOG_H_
#define _SAVESELECTIONDIALOG_H_

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>

#include "ImageWidget.h"

class SaveSelectionDialog : public QDialog
{
	Q_OBJECT

	public:
		SaveSelectionDialog(int channelCount, QString currentDirectoy, QWidget *parent=0);
		
		QString filename();
		QVector<bool> channels();
		ColorDepth colordepth();
		
	private slots:
		void FilenameRequested();
		
	private:
		QString					_currentDirectory;
		QLineEdit				*_filenameEdit;
		
		QVector<QCheckBox *>	_channelCheckBoxes;
		
		QRadioButton			*_8bitButton;
		QRadioButton			*_16bitButton;
		QRadioButton			*_floatButton;
};

#endif
