/***************************************************************************
                          SaveSelectionDialog.cpp  -
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

#include "SaveSelectionDialog.h"

#include <cassert>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QFileDialog>

SaveSelectionDialog::SaveSelectionDialog(int channelCount, QString currentDirectory, QWidget *parent) : QDialog(parent), _currentDirectory(currentDirectory)
{
	assert(channelCount > 0);

	setWindowTitle(tr("Selektion Speichern"));

	// Create main layouts
	QGridLayout *mainLayout = new QGridLayout(this);
	setLayout(mainLayout);

	// Create file selection
	QLabel *filenameLabel = new QLabel(tr("Dateiname:"), this);
	mainLayout->addWidget(filenameLabel, 0, 0, 1, 1);

	_filenameEdit = new QLineEdit(this);
	_filenameEdit->setReadOnly(true);
	mainLayout->addWidget(_filenameEdit, 0, 1, 1, 2);

	QPushButton *selectFilenameButton = new QPushButton(tr("..."), this);
	selectFilenameButton->setMaximumWidth(30);
	mainLayout->addWidget(selectFilenameButton, 0, 3, 1, 1);
	connect(selectFilenameButton, SIGNAL(clicked()), this, SLOT(FilenameRequested()));

	// Create channel selection
	QGroupBox *channelGroup = new QGroupBox(tr("Farbkanäle"), this);
	mainLayout->addWidget(channelGroup, 1, 0, 1, 2);

	QVBoxLayout *vboxchannels = new QVBoxLayout;
	for (int i = 0; i < channelCount; i++)
	{
		QCheckBox *checkbox = new QCheckBox(tr("Kanal %1").arg(i+1));
		checkbox->toggle();
		_channelCheckBoxes.append(checkbox);
		vboxchannels->addWidget(checkbox);
	}
	vboxchannels->addStretch(1);
	channelGroup->setLayout(vboxchannels);

	// Create color depth selection
	QGroupBox *colordepthGroup = new QGroupBox(tr("Farbtiefe / Kanal"), this);
	mainLayout->addWidget(colordepthGroup, 1, 2, 1, 2);

	_8bitButton = new QRadioButton(tr("8 Bit Integer"));
	_16bitButton = new QRadioButton(tr("16 Bit Integer"));
	_floatButton = new QRadioButton(tr("32 Bit Float"));

	QVBoxLayout *vboxcolor = new QVBoxLayout;
	vboxcolor->addWidget(_8bitButton);
	vboxcolor->addWidget(_16bitButton);
	vboxcolor->addWidget(_floatButton);
	vboxcolor->addStretch(1);
	colordepthGroup->setLayout(vboxcolor);

	_8bitButton->toggle();

	// Create apply option checkboxes
	_separatechannelsCheckBox = new QCheckBox(tr("Farbkanäle separieren"), this);
	mainLayout->addWidget(_separatechannelsCheckBox, 2, 0, 1, 4);

	_applycbCheckBox = new QCheckBox(tr("Kontrast / Helligkeit anwenden"), this);
	mainLayout->addWidget(_applycbCheckBox, 3, 0, 1, 4);

	// Create buttons
	QPushButton *acceptButton = new QPushButton(tr("&OK"), this);
	mainLayout->addWidget(acceptButton, 4, 0, 1, 2);
	connect(acceptButton, SIGNAL(clicked()), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(tr("&Abbrechen"), this);
	mainLayout->addWidget(cancelButton, 4, 2, 1, 2);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	acceptButton->setDefault(true);
}

QString SaveSelectionDialog::filename()
{
	return _filenameEdit->text();
}

QVector<bool> SaveSelectionDialog::channels()
{
	QVector<bool> result;
	for (int i = 0; i < _channelCheckBoxes.size(); i++)
		result.append(_channelCheckBoxes[i]->isChecked());

	return result;
}

ColorDepth SaveSelectionDialog::colordepth()
{
	if (_8bitButton->isChecked())
		return CD_INTEGER_8BIT;
	else if (_16bitButton->isChecked())
		return CD_INTEGER_16BIT;
	else if (_floatButton->isChecked())
		return CD_FLOAT_32BIT;
	else
		return CD_UNSUPPORTED;
}

bool SaveSelectionDialog::separatechannels()
{
	return _separatechannelsCheckBox->isChecked();
}

bool SaveSelectionDialog::applycontrastbrightness()
{
	return _applycbCheckBox->isChecked();
}

void SaveSelectionDialog::FilenameRequested()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Selektion speichern"), _currentDirectory, tr("Bilder ( *.tif *.tiff ) ;; Alle ( *.* )"));
	if (filename.isEmpty())
		return;

	_filenameEdit->setText(filename);
}
