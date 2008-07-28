/***************************************************************************
                          HistogramDialog.h  -
                             -------------------
    begin                : Wed Jul 25 2008
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

#ifndef _HISTOGRAMDIALOG_H_
#define _HISTOGRAMDIALOG_H_

#include <QDialog>
#include <QPaintEvent>
#include <QVector>

class HistogramWidget : public QWidget
{
	Q_OBJECT

	public:
		HistogramWidget(QWidget *parent=0);

		void SetHistogram(QVector<double> hist);
		void SetHistogram(QVector<double> hist1, QVector<double> hist2, QVector<double> hist3);

	public slots:
		void SetBucketCount(int count);

	protected:
		void paintEvent(QPaintEvent *event);

	private:
		int _mode, _bucketCount;
		QVector<double> _hist1, _hist2, _hist3;

		QVector<int> CalculateBuckets(QVector<double> histogram, double minValue, double maxValue);
};

class HistogramDialog : public QDialog
{
	Q_OBJECT

	public:
		HistogramDialog(QVector<double> hist, QWidget *parent=0);
		HistogramDialog(QVector<double> hist1, QVector<double> hist2, QVector<double> hist3, QWidget *parent=0);

	private:
		HistogramWidget *_histogramWidget;

		QLayout *CreateLayout();
};

#endif
