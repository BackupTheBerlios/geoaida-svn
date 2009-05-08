/***************************************************************************
                          HistogramDialog.cpp  -
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

#include "HistogramDialog.h"

#include <QPainter>

#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>

#include <iostream>

// HistogramWidget
HistogramWidget::HistogramWidget(QWidget *parent) : QWidget(parent), _mode(0), _bucketCount(64)
{
	setMinimumSize(QSize(300, 100));
}

void HistogramWidget::SetHistogram(QVector<double> hist)
{
	_mode = 1;
	_hist1 = hist;
}

void HistogramWidget::SetHistogram(QVector<double> hist1, QVector<double> hist2, QVector<double> hist3)
{
	_mode = 3;
	_hist1 = hist1;
	_hist2 = hist2;
	_hist3 = hist3;
}

void HistogramWidget::SetBucketCount(int count)
{
	_bucketCount = count;
	update();
}

void HistogramWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	// Compute min/max values
	double minValue;
	double maxValue;

	if (_mode == 1)
	{
		minValue = _hist1.first();
		maxValue = _hist1.last();
	}
	else
	{
		minValue = std::min(_hist1.first(), std::min(_hist2.first(), _hist3.first()));
		maxValue = std::max(_hist1.last(), std::max(_hist2.last(), _hist3.last()));
	}

	// Fill background
	painter.fillRect(event->rect(), QBrush(QColor(255, 255, 255)));

	// Draw frame
	painter.setPen(QColor(0, 0, 0));

	painter.drawLine(0, this->height() - 30, this->width(), this->height() - 30);
	painter.drawLine(0, this->height() - 30, 0, this->height() - 25);
	painter.drawLine(this->width()-1, this->height() - 30, this->width()-1, this->height() - 25);
	painter.drawLine(this->width()/2, this->height() - 30, this->width()/2, this->height() - 25);

	painter.drawText(QRect(0, this->height() - 20, this->width() / 2 , 20), Qt::AlignLeft, QString("%1").arg(minValue));
	painter.drawText(QRect(this->width()/2, this->height() - 20, this->width() / 2 , 20), Qt::AlignRight, QString("%1").arg(maxValue));
	painter.drawText(QRect(0, this->height() - 20, this->width(), 20), Qt::AlignHCenter, QString("%1").arg((minValue + maxValue) / 2));

	switch (_mode)
	{
		case 1:
		{
			// Calculate buckets
			QVector<int> buckets = CalculateBuckets(_hist1, minValue, maxValue);

			// Calculate scaling values
			int maxBucketContents = 0;
			for (int i=0; i<buckets.size(); i++)
				maxBucketContents = std::max(maxBucketContents, buckets[i]);

			double vScale = (double)(this->height() - 30) / (double)maxBucketContents;
			double hScale = (double)this->width() / (double)(_bucketCount - 1);

			// Generate histogram line from buckets
			QVector<QPointF> line;

			for (int i=0; i<buckets.size(); i++)
				line.push_back(QPointF((double)i * hScale, this->height() - 30 - (double)buckets[i] * vScale));

			// Draw histogram line
			painter.setPen(QColor(0, 0, 0));
			painter.drawPolyline(&line.first(), line.size());

			break;
		}

		case 3:
		{
			// Calculate buckets
			QVector<int> buckets[3] = {
				CalculateBuckets(_hist1, minValue, maxValue),
				CalculateBuckets(_hist2, minValue, maxValue),
				CalculateBuckets(_hist3, minValue, maxValue)
			};

			// Calculate scaling values
			int maxBucketContents[3] = {0, 0, 0};
			for (int j=0; j<3; j++)
				for (int i=0; i<buckets[j].size(); i++)
					maxBucketContents[j] = std::max(maxBucketContents[j], buckets[j][i]);

			double vScale = (double)(this->height() - 30) / (double)(std::max(maxBucketContents[0], std::max(maxBucketContents[1], maxBucketContents[2])));
			double hScale = (double)this->width() / (double)(_bucketCount - 1);

			// Generate histogram line from buckets
			QVector<QPointF> line[3];

			for (int j=0; j<3; j++)
				for (int i=0; i<buckets[j].size(); i++)
					line[j].push_back(QPointF((double)i * hScale, this->height() - 30 - (double)buckets[j][i] * vScale));

			// Draw histogram line
			painter.setPen(QColor(255, 0, 0));
			painter.drawPolyline(&line[0].first(), line[0].size());
			painter.setPen(QColor(0, 255, 0));
			painter.drawPolyline(&line[1].first(), line[1].size());
			painter.setPen(QColor(0, 0, 255));
			painter.drawPolyline(&line[2].first(), line[2].size());

			break;
		}
	}
}

QVector<int> HistogramWidget::CalculateBuckets(QVector<double> histogram, double minValue, double maxValue)
{
	QVector<int> buckets;

	double bucketStep = (maxValue - minValue) / (double)_bucketCount;
	double compValue = minValue + bucketStep;
	int bucketContents = 0;

	for (int i=0; i<histogram.size(); i++)
	{
		if (histogram[i] > compValue)
		{
			// Add bucket contents
			buckets.push_back(bucketContents);

			// Prepare computing next bucket contents
			compValue += bucketStep;
			bucketContents = 0;
		}
		else
			bucketContents++;
	}

	// Add final bucket contents
	buckets.push_back(bucketContents);

	return buckets;
}

// HistogramDialog
HistogramDialog::HistogramDialog(QVector<double> hist, QWidget *parent) : QDialog(parent)
{
	setWindowTitle(tr("Histogramm"));
	setLayout(CreateLayout());

	_histogramWidget->SetHistogram(hist);
}

HistogramDialog::HistogramDialog(QVector<double> hist1, QVector<double> hist2, QVector<double> hist3, QWidget *parent) : QDialog(parent)
{
	setWindowTitle(tr("Histogramm"));
	setLayout(CreateLayout());

	_histogramWidget->SetHistogram(hist1, hist2, hist3);
}

QLayout *HistogramDialog::CreateLayout()
{
	QGridLayout *mainLayout = new QGridLayout(this);

	// Create histogram widget
	_histogramWidget = new HistogramWidget(this);
	mainLayout->addWidget(_histogramWidget, 0, 0, 1, 4);

	// Create bucket count selector
	mainLayout->addWidget(new QLabel("Buckets: "), 1, 0, 1, 1);

	QSpinBox *bucketSpin = new QSpinBox(this);
	bucketSpin->setRange(1, 256);
	bucketSpin->setValue(64);
	mainLayout->addWidget(bucketSpin, 1, 1, 1, 1);
	connect(bucketSpin, SIGNAL(valueChanged(int)), _histogramWidget, SLOT(SetBucketCount(int)));

	// Create stretch
	mainLayout->setColumnStretch(2, 100);

	// Create buttons
	QPushButton *acceptButton = new QPushButton(tr("&OK"), this);
	mainLayout->addWidget(acceptButton, 1, 3, 1, 1);
	connect(acceptButton, SIGNAL(clicked()), this, SLOT(accept()));

	return mainLayout;
}
