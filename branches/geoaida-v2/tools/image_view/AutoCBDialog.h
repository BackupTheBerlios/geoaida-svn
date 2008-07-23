/***************************************************************************
                          AutoCBDialog.h  -
                             -------------------
    begin                : Wed Jul 23 2008
    copyright            : (C) 2008 TNT, Uni Hannover
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

#ifndef _AUTOCBDIALOG_H_
#define _AUTOCBDIALOG_H_

#include <QDialog>
#include <QDoubleSpinBox>

class AutoCBDialog : public QDialog
{
	Q_OBJECT

	public:
		AutoCBDialog(double startCoverage, QWidget *parent=0);

		double coverage() { return (_coverageSpin->value() / 100.0); }

	signals:

	private slots:

	private:
		QDoubleSpinBox *_coverageSpin;
};

#endif
