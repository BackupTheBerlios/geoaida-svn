/***************************************************************************
                          ImageWidget.h  -  
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

#ifndef _IMAGEWIDGET_H_
#define _IMAGEWIDGET_H_

#include <QWidget>
#include <QVector>
#include <QMap>
#include <QLinkedList>

#include <otbImage.h>
#include <otbVectorImage.h>
#include <otbImageList.h>
#include <otbPerBandVectorImageFilter.h>

#include <otbImageFileReader.h>
#include <otbImageFileWriter.h>

#include <otbMultiToMonoChannelExtractROI.h>
#include <itkExtractImageFilter.h>
#include <otbImageListToVectorImageFilter.h>

#include <itkImageRegion.h>
#include <itkImageRegionConstIterator.h>

#include <itkMinimumMaximumImageCalculator.h>
#include <itkShiftScaleImageFilter.h>

#include "Definitions.h"

typedef RealType PixelType;

typedef otb::Image<PixelType, 2> ChannelType;
typedef otb::VectorImage<PixelType, 2> ImageType;
typedef otb::ImageFileReader<ImageType> ReaderType;
typedef otb::MultiToMonoChannelExtractROI<PixelType, PixelType> ChannelExtractType;

typedef itk::ImageRegionConstIterator<ChannelType> ConstChannelIteratorType;
typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;

typedef quint32 TileID;
typedef quint32 TimeStamp;

const int TileSize = 128;

struct TileInfo
{
	TileID id;
	TimeStamp timestamp;
	QPixmap pixmap;

	TileInfo()
		: id(-1), timestamp(0), pixmap()
	{
	}

	TileInfo(TileID id, TimeStamp timestamp, const QImage &image)
		: id(id), timestamp(timestamp), pixmap(QPixmap::fromImage(image))
	{
	}
};

class ImageWidget : public QWidget
{
	Q_OBJECT

	public:
		ImageWidget(QWidget *parent=0);
		virtual ~ImageWidget();

		void Clear();
		void Open(QString filename);
		void AddChannels(QString filename);
		void SaveSelection(QString filename, QVector<bool> channels, ColorDepth colordepth, bool applycontrastbrightness);
		void SaveSelectionSeparatedChannels(QString filenametemplate, QVector<bool> channels, ColorDepth colordepth, bool applycontrastbrightness);

		bool isValidImage()	{ return !_images.isEmpty(); }
		int imageWidth()	{ return (isValidImage() ? _images[0]->GetOutput()->GetLargestPossibleRegion().GetSize()[0] : 0); }
		int imageHeight()	{ return (isValidImage() ? _images[0]->GetOutput()->GetLargestPossibleRegion().GetSize()[1] : 0); }
		int boundsWidth()	{ return _bounds.width(); }
		int boundsHeight()	{ return _bounds.height(); }
		int offsetX()		{ return _offset.x(); }
		int offsetY()		{ return _offset.y(); }
		QRect selection()	{ return _selection; }

		int channelCount()						{ return (isValidImage() ? _channels.size() : 0); }
		ChannelMappingMode channelMappingMode()	{ return _cmMode; }
		int channelMapping(int nr)				{ return _channelMapping[nr]; }
		void setChannelMapping(ChannelMappingMode mode, int mapping1, int mapping2, int mapping3)
		{
			_cmMode = (ChannelMappingMode)mode;
			_channelMapping[0] = mapping1;
			_channelMapping[1] = mapping2;
			_channelMapping[2] = mapping3;

			Redraw();
		}

		RealType contrast()						{ return _contrast; }
		RealType brightness()					{ return _brightness; }
		int bitdepth()							{ return _bitdepth; }

		/*QVector<double> GetHistogram(int nr, RealType coverage=0.1)
		{
			return CalculateHistogram(coverage, _channelMapping[nr]);
		}*/

	protected:
		void resizeEvent(QResizeEvent *event);
		void paintEvent(QPaintEvent *event);

		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);

	signals:
		void UpdateTileCacheExpand();
		void UpdateTileCacheShrink();
		void UpdateScrollBars();
		void ShowMessage(const QString &message, int timeout=0);
		void ShowWarning(const QString &message);

	public slots:
		void ChangeOffsetX(int offsetX);
		void ChangeOffsetY(int offsetY);

		void CalculateAutoCB();
		void SetRandomMapping(bool activate);
		void SetContrast(double contrast);
		void SetBrightness(double brightness);
		void SetBitdepth(int bitdepth);

		void ResetView();
		void ZoomView(RealType zoomfactor);

		void Redraw();

	private slots:
		void ClearTileCache();
		void LoadNextTile();
		void RemoveOldTiles();

		void RecalculateBounds();

	private:
		QVector<ReaderType::Pointer> _images;
		QVector<ChannelExtractType::Pointer> _channels;

		QRect _bounds;
		QPoint _offset;
		RealType _scaleFactor;

		TimeStamp _currentTimestamp;
		int _maxTilesCached;
		QMap<TileID, TileInfo> _currentTiles;
		QLinkedList<TileID> _loadingTiles;

		ChannelMappingMode _cmMode;
		int _channelMapping[3];
		bool _randomMapping;
		QMap<PixelType, int> _randomMap;

		RealType _contrast;
		RealType _brightness;
		int _bitdepth;

		QRect _selection;

		TileID IdFromPosition(int x, int y) const;
		TileID IdFromPosition(const QPoint &point) const;
		QPoint PositionFromId(const TileID id) const;

		//QVector<double> CalculateHistogram(float coverage, int channel=0);
		//void CalculateHistogram3(float coverage, int channel1, QVector<double> &histogram1, int channel2, QVector<double> &histogram2, int channel3, QVector<double> &histogram3);
};

#endif
