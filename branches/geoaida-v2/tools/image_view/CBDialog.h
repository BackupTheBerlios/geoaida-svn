/***************************************************************************
                          CBDialog.h  -
                             -------------------
    begin                : Wed Jul 16 2008
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

#ifndef _CBDIALOG_H_
#define _CBDIALOG_H_

#include <QDialog>
#include <QDoubleSpinBox>

#include "Definitions.h"

class CBDialog : public QDialog
{
	Q_OBJECT

	public:
		CBDialog(QWidget *parent=0);

		void setContrast(double contrast=1.0);
		void setBrightness(double brightness=0.0);

	signals:
		void contrastChanged(double contrast=1.0);
		void brightnessChanged(double brightness=0.0);

	private slots:
		void resetValues();

	private:
		QDoubleSpinBox	*_contrastSpin;
		QDoubleSpinBox	*_brightnessSpin;
};

#endif
