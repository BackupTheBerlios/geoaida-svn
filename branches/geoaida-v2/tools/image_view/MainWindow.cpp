#include "MainWindow.h"

#include <QApplication>
#include <QGridLayout>
#include <QFileDialog>

/**************************************
*
*	Constructor / Destructor
*
***************************************/

MainWindow::MainWindow(QWidget *parent)
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

	// Create connections
	connect(_imageWidget, SIGNAL(UpdateScrollBars()), this, SLOT(RecalculateScrollbarProperties()));

	connect(_horizontalScrollbar, SIGNAL(valueChanged(int)), _imageWidget, SLOT(ChangeOffsetX(int)));
	connect(_verticalScrollbar, SIGNAL(valueChanged(int)), _imageWidget, SLOT(ChangeOffsetY(int)));
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
	QString filename = QFileDialog::getOpenFileName(this, tr("Bild öffnen"), QString(), tr("Bilder (*.tif *.tiff *.pfm *.pgm *.pbm); Alle (*.*)"));
	if (filename.isEmpty())
		return;

	_imageWidget->Open(filename);
}

void MainWindow::QuitApplication()
{
	QApplication::exit(0);
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
