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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QPushButton>

CBDialog::CBDialog(RealType startContrast, RealType startBrightness, QWidget *parent)
	: QDialog(parent), _startContrast(startContrast), _startBrightness(startBrightness)
{
	setWindowTitle(tr("Kontrast / Helligkeit Ändern"));

	// Create main layout
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	setLayout(mainLayout);

	// Create contrast / brightness controls
	QGridLayout *cbControlsLayout = new QGridLayout();

	cbControlsLayout->addWidget(new QLabel(tr("Helligkeit: "), this), 0, 0, 1, 1);

	_brightnessSpin = new QDoubleSpinBox(this);
	_brightnessSpin->setRange(-10000.0, 10000.0);
	_brightnessSpin->setValue(startBrightness);
	cbControlsLayout->addWidget(_brightnessSpin, 0, 1, 1, 1);
	connect(_brightnessSpin, SIGNAL(valueChanged(double)), this, SIGNAL(brightnessChanged(double)));

	cbControlsLayout->addWidget(new QLabel(tr("Kontrast: "), this), 1, 0, 1, 1);

	_contrastSpin = new QDoubleSpinBox(this);
	_contrastSpin->setRange(-10000.0, 10000.0);
	_contrastSpin->setValue(startContrast);
	cbControlsLayout->addWidget(_contrastSpin, 1, 1, 1, 1);
	connect(_contrastSpin, SIGNAL(valueChanged(double)), this, SIGNAL(contrastChanged(double)));

	mainLayout->addLayout(cbControlsLayout);

	// Buttons
	QHBoxLayout *buttonsLayout = new QHBoxLayout();

	QPushButton *resetButton = new QPushButton(tr("&Zurücksetzen"));
	buttonsLayout->addWidget(resetButton);
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));

	QPushButton *closeButton = new QPushButton(tr("&Schliessen"));
	buttonsLayout->addWidget(closeButton);
	connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));

	closeButton->setDefault(true);

	mainLayout->addLayout(buttonsLayout);
}

void CBDialog::resetValues()
{
	_contrastSpin->setValue(_startContrast);
	_brightnessSpin->setValue(_startBrightness);
}
