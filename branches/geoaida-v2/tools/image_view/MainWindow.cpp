/***************************************************************************
                          MainWindow.cpp  -
                             -------------------
    begin                : Mon Jul 07 2008
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

#include "MainWindow.h"

#include <QApplication>
#include <QGridLayout>
#include <QFileDialog>
#include <QStatusBar>

#include "ChannelMappingDialog.h"

/**************************************
*
*	Constructor / Destructor
*
***************************************/

MainWindow::MainWindow(const QString &filename, QWidget *parent)
	: QMainWindow(parent)
{
	// Create central widget
	_centralWidget = new QWidget();
	QGridLayout *centralLayout = new QGridLayout(_centralWidget);

	// Create image widget
	_imageWidget = new ImageWidget(_centralWidget);
	centralLayout->addWidget(_imageWidget, 0, 0);

	// Create scrollbars
	_horizontalScrollbar = new QScrollBar(Qt::Horizontal, _centralWidget);
	centralLayout->addWidget(_horizontalScrollbar, 1, 0);

	_verticalScrollbar = new QScrollBar(Qt::Vertical, _centralWidget);
	centralLayout->addWidget(_verticalScrollbar, 0, 1);

	// Set central widget
	setCentralWidget(_centralWidget);

	// Create menu bar
	setMenuBar(createMenuBar());

	// Create status bar
	setStatusBar(new QStatusBar(this));

	// Create connections
	connect(_imageWidget, SIGNAL(UpdateScrollBars()), this, SLOT(RecalculateScrollbarProperties()));
	connect(_imageWidget, SIGNAL(ShowMessage(const QString &, int)), this->statusBar(), SLOT(showMessage(const QString &, int)));

	connect(_horizontalScrollbar, SIGNAL(valueChanged(int)), _imageWidget, SLOT(ChangeOffsetX(int)));
	connect(_verticalScrollbar, SIGNAL(valueChanged(int)), _imageWidget, SLOT(ChangeOffsetY(int)));

	// Load image from command-line
	if (!filename.isEmpty())
		_imageWidget->Open(filename);
}

QMenuBar *MainWindow::createMenuBar()
{
	QMenuBar *menuBar = new QMenuBar();

	// File menu
	QMenu *fileMenu = menuBar->addMenu(tr("&Datei"));

	QAction *fileLoadAction = fileMenu->addAction(tr("Bild &Laden..."));
	connect(fileLoadAction, SIGNAL(triggered()), this, SLOT(LoadFileDialog()));

	fileMenu->addSeparator();

	QAction *fileQuitAction = fileMenu->addAction(tr("&Beenden"));
	connect(fileQuitAction, SIGNAL(triggered()), this, SLOT(QuitApplication()));

	// Channel menu
	QMenu *channelMenu = menuBar->addMenu(tr("&Farbkan�le"));

	QAction *channelMappingAction = channelMenu->addAction(tr("Mapping &�ndern"));
	connect(channelMappingAction, SIGNAL(triggered()), this, SLOT(ChangeChannelMapping()));

	// View menu
	QMenu *viewMenu = menuBar->addMenu(tr("&Ansicht"));

	QAction *viewResetAction = viewMenu->addAction(tr("&Zur�cksetzen"));
	connect(viewResetAction, SIGNAL(triggered()), this, SLOT(ResetView()));

	viewMenu->addSeparator();

	QAction *viewZoomPAction = viewMenu->addAction(tr("Zoom +"));
	connect(viewZoomPAction, SIGNAL(triggered()), this, SLOT(ZoomPlus()));

	QAction *viewZoomMAction = viewMenu->addAction(tr("Zoom -"));
	connect(viewZoomMAction, SIGNAL(triggered()), this, SLOT(ZoomMinus()));

	viewMenu->addSeparator();

	QAction *viewRotatePAction = viewMenu->addAction(tr("Rotate +"));
	connect(viewRotatePAction, SIGNAL(triggered()), this, SLOT(RotatePlus()));

	QAction *viewRotateMAction = viewMenu->addAction(tr("Rotate -"));
	connect(viewRotateMAction, SIGNAL(triggered()), this, SLOT(RotateMinus()));

	return menuBar;
}

/**************************************
*
*	Slots
*
***************************************/

void MainWindow::LoadFileDialog()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Bild �ffnen"), QString(), tr("Bilder (*.tif *.tiff *.pfm *.pgm *.pbm); Alle (*.*)"));
	if (filename.isEmpty())
		return;

	_imageWidget->Open(filename);
}

void MainWindow::QuitApplication()
{
	QApplication::exit(0);
}

void MainWindow::ChangeChannelMapping()
{
	if (_imageWidget->channelCount() == 0)
		return;

	ChannelMappingDialog dialog((int)_imageWidget->channelMappingMode(), _imageWidget->channelMapping(0), _imageWidget->channelMapping(1), _imageWidget->channelMapping(2), _imageWidget->channelCount(), this);
	int result = dialog.exec();

	if (result == QDialog::Accepted)
	{
		if (dialog.mode() == 0)
			_imageWidget->setChannelMapping((ChannelMappingMode)dialog.mode(), dialog.mapping(0), 0, 0);
		else
			_imageWidget->setChannelMapping((ChannelMappingMode)dialog.mode(), dialog.mapping(0), dialog.mapping(1), dialog.mapping(2));
	}
}

void MainWindow::ResetView()
{
	_imageWidget->ResetView();
	_imageWidget->update();
}

void MainWindow::ZoomPlus()
{
	_imageWidget->ZoomView(2.0f, 2.0f);
	_imageWidget->update();
}

void MainWindow::ZoomMinus()
{
	_imageWidget->ZoomView(0.5f, 0.5f);
	_imageWidget->update();
}

void MainWindow::RotatePlus()
{
	_imageWidget->RotateView(-3.14159265f / 4.0f);
	_imageWidget->update();
}

void MainWindow::RotateMinus()
{
	_imageWidget->RotateView(3.14159265f / 4.0f);
	_imageWidget->update();
}

void MainWindow::RecalculateScrollbarProperties()
{
	int width = _imageWidget->width();
	int imageWidth = _imageWidget->imageWidth();
	_horizontalScrollbar->setMinimum(0);
	_horizontalScrollbar->setMaximum(std::max(0, imageWidth - width));
	_horizontalScrollbar->setPageStep(width);
	_horizontalScrollbar->setValue(_imageWidget->offsetX());

	int height = _imageWidget->height();
	int imageHeight = _imageWidget->imageHeight();
	_verticalScrollbar->setMinimum(0);
	_verticalScrollbar->setMaximum(std::max(0, imageHeight - height));
	_verticalScrollbar->setPageStep(height);
	_verticalScrollbar->setValue(_imageWidget->offsetY());
}
