/***************************************************************************
                          ChannelMappingDialog.cpp  -
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

#include "ChannelMappingDialog.h"

#include <cassert>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

ChannelMappingDialog::ChannelMappingDialog(int mode, int mapping1, int mapping2, int mapping3, int channelCount, QWidget *parent) : QDialog(parent)
{
	assert((mode == 0) || (mode == 1));
	assert(channelCount > 0);
	assert((mapping1 >= 0) && (mapping1 < channelCount));
	assert((mapping2 >= 0) && (mapping2 < channelCount));
	assert((mapping3 >= 0) && (mapping3 < channelCount));

	setWindowTitle(tr("Kanal Mapping"));

	// Create main layouts
	QGridLayout *mainLayout = new QGridLayout(this);
	setLayout(mainLayout);

	// Create one channel options
	_oneChannelGroup = new QGroupBox(tr("Einkanal"), this);
	_oneChannelGroup->setCheckable(true);
	_oneChannelGroup->setChecked(mode == 0);
	connect(_oneChannelGroup, SIGNAL(clicked(bool)), this, SLOT(oneChannelGroupClicked(bool)));

	QGridLayout *oneChannelOptionsLayout = new QGridLayout(_oneChannelGroup);
	_oneChannelGroup->setLayout(oneChannelOptionsLayout);

	oneChannelOptionsLayout->addWidget(new QLabel(tr("Kanal 1: "), _oneChannelGroup), 0, 0, 1, 1);

	_oneChannel1Spin = new QSpinBox(_oneChannelGroup);
	_oneChannel1Spin->setRange(1, channelCount);
	_oneChannel1Spin->setValue(mapping1 + 1);
	oneChannelOptionsLayout->addWidget(_oneChannel1Spin, 0, 1, 1, 1);

	mainLayout->addWidget(_oneChannelGroup, 0, 0, 1, 1);

	// Create three channel options
	_threeChannelGroup = new QGroupBox(tr("Mehrkanal"), this);
	_threeChannelGroup->setCheckable(true);
	_threeChannelGroup->setChecked(mode == 1);
	connect(_threeChannelGroup, SIGNAL(clicked(bool)), this, SLOT(threeChannelGroupClicked(bool)));

	QGridLayout *threeChannelOptionsLayout = new QGridLayout(_threeChannelGroup);
	_threeChannelGroup->setLayout(threeChannelOptionsLayout);

	threeChannelOptionsLayout->addWidget(new QLabel(tr("Kanal 1: "), _threeChannelGroup), 0, 0, 1, 1);
	threeChannelOptionsLayout->addWidget(new QLabel(tr("Kanal 2: "), _threeChannelGroup), 1, 0, 1, 1);
	threeChannelOptionsLayout->addWidget(new QLabel(tr("Kanal 3: "), _threeChannelGroup), 2, 0, 1, 1);

	_threeChannel1Spin = new QSpinBox(_threeChannelGroup);
	_threeChannel1Spin->setRange(1, channelCount);
	_threeChannel1Spin->setValue(mapping1 + 1);
	threeChannelOptionsLayout->addWidget(_threeChannel1Spin, 0, 1, 1, 1);

	_threeChannel2Spin = new QSpinBox(_threeChannelGroup);
	_threeChannel2Spin->setRange(1, channelCount);
	_threeChannel2Spin->setValue(mapping2 + 1);
	threeChannelOptionsLayout->addWidget(_threeChannel2Spin, 1, 1, 1, 1);

	_threeChannel3Spin = new QSpinBox(_threeChannelGroup);
	_threeChannel3Spin->setRange(1, channelCount);
	_threeChannel3Spin->setValue(mapping3 + 1);
	threeChannelOptionsLayout->addWidget(_threeChannel3Spin, 2, 1, 1, 1);

	mainLayout->addWidget(_threeChannelGroup, 0, 1, 1, 1);

	// Create buttons
	QPushButton *acceptButton = new QPushButton(tr("&OK"), this);
	mainLayout->addWidget(acceptButton, 1, 0, 1, 1);
	connect(acceptButton, SIGNAL(clicked()), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(tr("&Abbrechen"), this);
	mainLayout->addWidget(cancelButton, 1, 1, 1, 1);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}

int ChannelMappingDialog::mode()
{
	assert(_oneChannelGroup->isChecked() != _threeChannelGroup->isChecked());

	return (_oneChannelGroup->isChecked() ? 0 : 1);
}

int ChannelMappingDialog::mapping(int nr)
{
	if (mode() == 0)
	{
		assert(nr == 0);

		return (_oneChannel1Spin->value() - 1);
	}
	else
	{
		assert((nr >= 0) && (nr < 3));

		switch (nr)
		{
			case 0:
				return (_threeChannel1Spin->value() - 1);

			case 1:
				return (_threeChannel2Spin->value() - 1);

			case 2:
				return (_threeChannel3Spin->value() - 1);
		}
	}
}

void ChannelMappingDialog::oneChannelGroupClicked(bool status)
{
	_threeChannelGroup->setChecked(!status);
}

void ChannelMappingDialog::threeChannelGroupClicked(bool status)
{
	_oneChannelGroup->setChecked(!status);
}
