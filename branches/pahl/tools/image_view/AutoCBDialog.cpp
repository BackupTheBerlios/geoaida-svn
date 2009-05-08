/***************************************************************************
                          AutoCBDialog.cpp  -
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

#include "AutoCBDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>

AutoCBDialog::AutoCBDialog(double startCoverage, QWidget *parent) : QDialog(parent)
{
	setWindowTitle(tr("Auto Kontrast / Helligkeit"));

	// Create main layout
	QGridLayout *mainLayout = new QGridLayout(this);
	setLayout(mainLayout);

	// Create coverage controls
	mainLayout->addWidget(new QLabel(tr("Pixelabdeckung (in %): "), this), 0, 0, 1, 1);

	_coverageSpin = new QDoubleSpinBox(this);
	_coverageSpin->setRange(0.0, 100.0);
	_coverageSpin->setValue(startCoverage);
	mainLayout->addWidget(_coverageSpin, 0, 1, 1, 1);

	// Buttons
	QPushButton *acceptButton = new QPushButton(tr("&OK"));
	mainLayout->addWidget(acceptButton, 1, 0, 1, 1);
	connect(acceptButton, SIGNAL(clicked()), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(tr("&Abbrechen"));
	mainLayout->addWidget(cancelButton, 1, 1, 1, 1);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	acceptButton->setDefault(true);
}
