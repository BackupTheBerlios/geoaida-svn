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
#include "CBDialog.h"
#include "AutoCBDialog.h"
#include "HistogramDialog.h"

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
	{
		_imageWidget->Open(filename);
		setWindowTitle(QFileInfo(filename).fileName() + tr(" (%1 x %2)").arg(_imageWidget->imageWidth()).arg(_imageWidget->imageHeight()));
	}

	resize(800, 600);
}

QMenuBar *MainWindow::createMenuBar()
{
	QMenuBar *menuBar = new QMenuBar();

	// File menu
	QMenu *fileMenu = menuBar->addMenu(tr("&Datei"));

	QAction *fileLoadAction = fileMenu->addAction(tr("Bild &Laden..."));
	connect(fileLoadAction, SIGNAL(triggered()), this, SLOT(LoadFileDialog()));

	QAction *fileAddChannelsAction = fileMenu->addAction(tr("Kanäle &Hinzufügen..."));
	connect(fileAddChannelsAction, SIGNAL(triggered()), this, SLOT(AddChannelsDialog()));

	fileMenu->addSeparator();

	QAction *fileQuitAction = fileMenu->addAction(tr("&Beenden"));
	connect(fileQuitAction, SIGNAL(triggered()), this, SLOT(QuitApplication()));

	// Channel menu
	QMenu *channelMenu = menuBar->addMenu(tr("&Farbkanäle"));

	QAction *channelMappingAction = channelMenu->addAction(tr("&Mapping Ändern..."));
	connect(channelMappingAction, SIGNAL(triggered()), this, SLOT(ChangeChannelMapping()));

	QAction *channelLabelMappingAction = channelMenu->addAction(tr("&Labelbild darstellen"));
	channelLabelMappingAction->setCheckable(true);
	connect(channelLabelMappingAction, SIGNAL(triggered(bool)), _imageWidget, SLOT(SetRandomMapping(bool)));

	channelMenu->addSeparator();

	QAction *channelResetContrastBrightnessAction = channelMenu->addAction(tr("Kontrast / Helligkeit &Zurücksetzen"));
	connect(channelResetContrastBrightnessAction, SIGNAL(triggered()), this, SLOT(ResetContrastBrightness()));

	QAction *channelContrastBrightnessAction = channelMenu->addAction(tr("Kontrast / Helligkeit &Ändern..."));
	connect(channelContrastBrightnessAction, SIGNAL(triggered()), this, SLOT(ChangeContrastBrightness()));

	QAction *channelAutoContrastBrightnessAction = channelMenu->addAction(tr("&Auto Kontrast / Helligkeit..."));
	connect(channelAutoContrastBrightnessAction, SIGNAL(triggered()), this, SLOT(CalculateAutoContrastBrightness()));

	channelMenu->addSeparator();

	QAction *channelShowHistogramAction = channelMenu->addAction(tr("&Histogramm Anzeigen..."));
	connect(channelShowHistogramAction, SIGNAL(triggered()), this, SLOT(ShowHistogram()));

	// View menu
	QMenu *viewMenu = menuBar->addMenu(tr("&Ansicht"));

	QAction *viewResetAction = viewMenu->addAction(tr("&Zurücksetzen"));
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
	static QString directory;

	QString filename = QFileDialog::getOpenFileName(this, tr("Bild öffnen"), directory, tr("Bilder ( *.tif *.tiff *.ppm *.pgm *.pfm *.pbm ) ;; Alle ( *.* )"));
	if (filename.isEmpty())
		return;

	directory = QFileInfo(filename).absolutePath();
	_imageWidget->Open(filename);

	setWindowTitle(QFileInfo(filename).fileName() + tr(" (%1 x %2)").arg(_imageWidget->imageWidth()).arg(_imageWidget->imageHeight()));
}

void MainWindow::AddChannelsDialog()
{
	static QString directory;

	QString filename = QFileDialog::getOpenFileName(this, tr("Bild öffnen"), directory, tr("Bilder ( *.tif *.tiff *.ppm *.pgm *.pfm *.pbm ) ;; Alle ( *.* )"));
	if (filename.isEmpty())
		return;

	directory = QFileInfo(filename).absolutePath();
	_imageWidget->AddChannels(filename);
}

void MainWindow::QuitApplication()
{
	QApplication::exit(0);
}

void MainWindow::ChangeChannelMapping()
{
	if (!_imageWidget->isValidImage())
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

void MainWindow::ResetContrastBrightness()
{
	_imageWidget->SetContrast(1.0);
	_imageWidget->SetBrightness(0.0);
}

void MainWindow::ChangeContrastBrightness()
{
	if (!_imageWidget->isValidImage())
		return;

	CBDialog *dialog = new CBDialog(_imageWidget->contrast(), _imageWidget->brightness(), this);
	dialog->setModal(true);

	connect(dialog, SIGNAL(contrastChanged(double)), _imageWidget, SLOT(SetContrast(double)));
	connect(dialog, SIGNAL(brightnessChanged(double)), _imageWidget, SLOT(SetBrightness(double)));

	dialog->show();
}

void MainWindow::CalculateAutoContrastBrightness()
{
	if (!_imageWidget->isValidImage())
		return;

	// Calculate start coverage value
	double expectedPixelCount = 100000.0;
	double startCoverage;

	QRect selection = _imageWidget->selection();
	if (!selection.isValid() || selection.isEmpty() || selection.isNull())
		startCoverage = expectedPixelCount / static_cast<double>(_imageWidget->imageWidth() * _imageWidget->imageHeight());
	else
		startCoverage = expectedPixelCount / static_cast<double>(selection.width() * selection.height());

	startCoverage = std::max(0.25, std::min(startCoverage, 1.0));

	// Show dialog
	AutoCBDialog dialog(startCoverage * 100.0, this);
	int result = dialog.exec();

	if (result == QDialog::Accepted)
		_imageWidget->CalculateAutoCB(dialog.coverage());
}

void MainWindow::ShowHistogram()
{
	if (!_imageWidget->isValidImage())
		return;

	// Calculate start coverage value
	double expectedPixelCount = 100000.0;
	double startCoverage;

	QRect selection = _imageWidget->selection();
	if (!selection.isValid() || selection.isEmpty() || selection.isNull())
		startCoverage = expectedPixelCount / static_cast<double>(_imageWidget->imageWidth() * _imageWidget->imageHeight());
	else
		startCoverage = expectedPixelCount / static_cast<double>(selection.width() * selection.height());

	startCoverage = std::max(0.25, std::min(startCoverage, 1.0));

	if (_imageWidget->channelMappingMode() == 0)
	{
		// Get histogram
		QVector<double> histogram1 = _imageWidget->GetHistogram(0, startCoverage);

		// Show dialog
		HistogramDialog *dialog = new HistogramDialog(histogram1, this);
		dialog->setModal(true);

		dialog->show();
	}
	else if (_imageWidget->channelMappingMode() == 1)
	{
		// Get histograms
		QVector<double> histogram1 = _imageWidget->GetHistogram(0, startCoverage);
		QVector<double> histogram2 = _imageWidget->GetHistogram(1, startCoverage);
		QVector<double> histogram3 = _imageWidget->GetHistogram(2, startCoverage);

		// Show dialog
		HistogramDialog *dialog = new HistogramDialog(histogram1, histogram2, histogram3, this);
		dialog->setModal(true);

		dialog->show();
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
	int height = _imageWidget->height();

	int currentOffsetX = _imageWidget->offsetX();
	int currentOffsetY = _imageWidget->offsetY();

	int boundsWidth = _imageWidget->boundsWidth();
	int boundsHeight = _imageWidget->boundsHeight();
	
	_horizontalScrollbar->setRange(0, std::max(0, boundsWidth - width));
	_horizontalScrollbar->setPageStep(width);
	_horizontalScrollbar->setValue(currentOffsetX);
	_imageWidget->ChangeOffsetX(_horizontalScrollbar->value());

	_verticalScrollbar->setRange(0, std::max(0, boundsHeight - height));
	_verticalScrollbar->setPageStep(height);
	_verticalScrollbar->setValue(currentOffsetY);
	_imageWidget->ChangeOffsetY(_verticalScrollbar->value());
}
