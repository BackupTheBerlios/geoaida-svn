#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QMainWindow>
#include <QMenuBar>
#include <QScrollBar>

#include "ImageWidget.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(QWidget *parent=0);

	protected:

	private slots:
		void LoadFileDialog();
		void QuitApplication();

		void ResetView();
		void ZoomPlus();
		void ZoomMinus();
		void RotatePlus();
		void RotateMinus();

		void RecalculateScrollbarProperties();

	private:
		QWidget *_centralWidget;
		QScrollBar *_horizontalScrollbar;
		QScrollBar *_verticalScrollbar;
		ImageWidget *_imageWidget;

		QMenuBar *createMenuBar();
};

#endif
