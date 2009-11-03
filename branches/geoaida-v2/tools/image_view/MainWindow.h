/***************************************************************************
                          MainWindow.h  -  
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

#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QMainWindow>
#include <QMenuBar>
#include <QScrollBar>

#include "Definitions.h"
#include "ImageWidget.h"
#include "CBDialog.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(const QString &filename=QString(), QWidget *parent=0);

	protected:

	private slots:
		void LoadFileDialog();
		void AddChannelsDialog();
		void SaveFileDialog();
		void QuitApplication();

		void ChangeChannelMapping();
		void ResetContrastBrightness();
		void ChangeContrastBrightness();
		void CalculateAutoContrastBrightness();
		void ShowHistogram();

		void ResetView();
		void ZoomPlus();
		void ZoomMinus();

		void ScrollUp();
		void ScrollDown();
		void ScrollLeft();
		void ScrollRight();

		void RecalculateScrollbarProperties();
		void ShowWarningBox(const QString &message);

	private:
		QWidget *_centralWidget;
		QScrollBar *_horizontalScrollbar;
		QScrollBar *_verticalScrollbar;
		ImageWidget *_imageWidget;
		QString _currentDirectory;

		CBDialog *_cbdialog;

		QMenuBar *createMenuBar();
};

#endif
