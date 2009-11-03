/***************************************************************************
                          CBDialog.cpp  -
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

#include "CBDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

CBDialog::CBDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Kontrast / Helligkeit Ändern"));

	// Create main layout
	QGridLayout *mainLayout = new QGridLayout(this);
	setLayout(mainLayout);

	// Create contrast / brightness controls
	mainLayout->addWidget(new QLabel(tr("Helligkeit: "), this), 0, 0, 1, 1);

	_brightnessSpin = new QDoubleSpinBox(this);
	_brightnessSpin->setRange(-10000.0, 10000.0);
	_brightnessSpin->setDecimals(4);
	_brightnessSpin->setSingleStep(10.0);
	mainLayout->addWidget(_brightnessSpin, 0, 1, 1, 1);
	connect(_brightnessSpin, SIGNAL(valueChanged(double)), this, SIGNAL(brightnessChanged(double)));

	mainLayout->addWidget(new QLabel(tr("Kontrast: "), this), 1, 0, 1, 1);

	_contrastSpin = new QDoubleSpinBox(this);
	_contrastSpin->setRange(-10000.0, 10000.0);
	_contrastSpin->setDecimals(4);
	_contrastSpin->setSingleStep(0.1);
	mainLayout->addWidget(_contrastSpin, 1, 1, 1, 1);
	connect(_contrastSpin, SIGNAL(valueChanged(double)), this, SIGNAL(contrastChanged(double)));

	// Buttons
	QPushButton *resetButton = new QPushButton(tr("&Zurücksetzen"));
	mainLayout->addWidget(resetButton, 3, 0, 1, 1);
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));

	QPushButton *closeButton = new QPushButton(tr("&Schliessen"));
	mainLayout->addWidget(closeButton, 3, 1, 1, 1);
	connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));

	closeButton->setDefault(true);
}

void CBDialog::setContrast(double contrast)
{
	_contrastSpin->setValue(contrast);
}

void CBDialog::setBrightness(double brightness)
{
	_brightnessSpin->setValue(brightness);
}

void CBDialog::resetValues()
{
	_contrastSpin->setValue(1.0);
	_brightnessSpin->setValue(0.0);
}
