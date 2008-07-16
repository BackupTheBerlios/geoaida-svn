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
#include <QMap>
#include <QLinkedList>

#include <Image>

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

enum ChannelMappingMode
{
	CM_OneChannelMode,
	CM_ThreeChannelMode
};

class ImageWidget : public QWidget
{
	Q_OBJECT

	public:
		ImageWidget(QWidget *parent=0);
		virtual ~ImageWidget();

		void Clear();
		void Open(QString filename);

		int imageWidth()	{ return _bounds.width(); }
		int imageHeight()	{ return _bounds.height(); }
		int offsetX()		{ return _offset.x(); }
		int offsetY()		{ return _offset.y(); }

		int channelCount()						{ return (_image ? _image->noChannels() : 0); }
		ChannelMappingMode channelMappingMode()	{ return _cmMode; }
		int channelMapping(int nr)				{ return _channelMapping[nr]; }
		void setChannelMapping(ChannelMappingMode mode, int mapping1, int mapping2, int mapping3)
		{
			_cmMode = (ChannelMappingMode)mode;
			_channelMapping[0] = mapping1;
			_channelMapping[1] = mapping2;
			_channelMapping[2] = mapping3;

			ClearTileCache();
		}

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

	public slots:
		void ChangeOffsetX(int offsetX);
		void ChangeOffsetY(int offsetY);

		void ResetView(bool recalc=true);
		void ZoomView(float zoomX, float zoomY, bool recalc=true);
		void RotateView(float angle, bool recalc=true);
		void TranslateView(float transX, float transY, bool recalc=true);

	private slots:
		void ClearTileCache();
		void LoadNextTile();
		void RemoveOldTiles();

		void RecalculateBounds();

	private:
		Ga::Image *_image;

		QRect _bounds;
		QPoint _offset;
		float *_affineTransformation;
		float *_invAffineTransformation;

		TimeStamp _currentTimestamp;
		int _maxTilesCached;
		QMap<TileID, TileInfo> _currentTiles;
		QLinkedList<TileID> _loadingTiles;

		ChannelMappingMode _cmMode;
		int _channelMapping[3];

		QRect _selection;

		TileID IdFromPosition(int x, int y) const;
		TileID IdFromPosition(const QPoint &point) const;
		QPoint PositionFromId(const TileID id) const;

		void MultiplyMatrixRight(float *matrix, bool target);
		void MultiplyMatrixLeft(float *matrix, bool target);
};

#endif
