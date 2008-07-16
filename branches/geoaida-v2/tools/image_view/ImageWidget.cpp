/***************************************************************************
                          ImageWidget.cpp  -
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

#include "ImageWidget.h"

#include <QtGui>

/**************************************
*
*	Constructor / Destructor
*
***************************************/

ImageWidget::ImageWidget(QWidget *parent)
	: QWidget(parent), _image(0), _affineTransformation(new float[9]), _invAffineTransformation(new float[9]), _currentTimestamp(0)
{
	_cmMode = CM_OneChannelMode;
	_channelMapping[0] = _channelMapping[1] = _channelMapping[2] = 0;

	QPalette palette;
	palette.setColor(QPalette::Background, Qt::black);
	setPalette(palette);
	setAutoFillBackground(true);
	setMouseTracking(true);

	connect(this, SIGNAL(UpdateTileCacheExpand()), this, SLOT(LoadNextTile()));
	connect(this, SIGNAL(UpdateTileCacheShrink()), this, SLOT(RemoveOldTiles()));
}

ImageWidget::~ImageWidget()
{
	Clear();

	delete[] _affineTransformation;
	delete[] _invAffineTransformation;
}

/**************************************
*
*	File input / output
*
***************************************/

void ImageWidget::Clear()
{
	ClearTileCache();

	_bounds = QRect();
	_offset = QPoint();

	_selection = QRect();

	if (_image)
	{
		delete _image;
		_image = 0;
	}
}

void ImageWidget::Open(QString filename)
{
	Clear();
	_image = new Ga::Image(filename.toStdString());

	if (_image->noChannels() < 3)
	{
		_cmMode = CM_OneChannelMode;
		_channelMapping[0] = _channelMapping[1] = _channelMapping[2] = 0;
	}
	else
	{
		_cmMode = CM_ThreeChannelMode;
		_channelMapping[0] = 0;
		_channelMapping[1] = 1;
		_channelMapping[2] = 2;
	}

	ResetView();
	update();
}

/**************************************
*
*	Tile ID calculation
*
***************************************/

TileID ImageWidget::IdFromPosition(int x, int y) const
{
	x &= (1 << 16) - 1;
	y &= (1 << 16) - 1;

	return ((y << 16) | x);
}

TileID ImageWidget::IdFromPosition(const QPoint &point) const
{
	return IdFromPosition(point.x(), point.y());
}

QPoint ImageWidget::PositionFromId(const TileID id) const
{
	int x = id & ((1 << 16) - 1);
	int y = (id >> 16) & ((1 << 16) - 1);

	return QPoint(x, y);
}

/**************************************
*
*	Events
*
***************************************/

void ImageWidget::resizeEvent(QResizeEvent *event)
{
	int columns = event->size().width() / TileSize + 2;
	int rows = event->size().height() / TileSize + 2;
	_maxTilesCached = 2 * std::max(4, columns * rows);

	emit UpdateTileCacheShrink();
	emit UpdateScrollBars();
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
	if (!_image)
		return;

	QPainter painter;
	painter.begin(this);

	// Calculate visible rect and tile indices
	QRect rect = event->rect().translated(_offset) & _bounds;
	int left = rect.x() / TileSize;
	int right = (rect.x() + rect.width() - 1) / TileSize;
	int top = rect.y() / TileSize;
	int bottom = (rect.y() + rect.height() - 1) / TileSize;

	// Try to draw all visible tiles
	// If a tile is not currently in the cache, put it on the "to load" list
	for (int y = top; y <= bottom; y++)
	{
		for (int x = left; x <= right; x++)
		{
			TileID id = IdFromPosition(x, y);
			if (_currentTiles.contains(id))
			{
				TileInfo &tinfo = _currentTiles[id];
				tinfo.timestamp = _currentTimestamp;
				painter.drawPixmap(x * TileSize - _offset.x(), y * TileSize - _offset.y(), tinfo.pixmap);
			}
			else if (!_loadingTiles.contains(id))
				_loadingTiles.push_back(id);
		}
	}

	// Draw the selection rectangle
	painter.setPen(QPen(QBrush(QColor(255, 0, 0)), 2, Qt::DashDotLine));
	painter.drawRect(_selection.translated(-_offset));

	painter.end();
	_currentTimestamp++;

	emit UpdateTileCacheExpand();
	emit UpdateTileCacheShrink();
}

void ImageWidget::mousePressEvent(QMouseEvent *event)
{
	// Selection rectangle
	if (event->buttons() & Qt::LeftButton)
	{
		_selection.setTopLeft(event->pos() + _offset);
		_selection.setBottomRight(event->pos() + _offset);

		update();
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
	// Selection rectangle
	if (event->buttons() & Qt::LeftButton)
	{
		_selection.setBottomRight(event->pos() + _offset);

		update();
	}

	// Current pixel value
	if (_image)
	{
		int px = event->x() + _offset.x();
		int py = event->y() + _offset.y();
		int posx = static_cast<int>(px * _affineTransformation[0] + py * _affineTransformation[1] + _affineTransformation[2]);
		int posy = static_cast<int>(px * _affineTransformation[3] + py * _affineTransformation[4] + _affineTransformation[5]);

		switch (_cmMode)
		{
			case CM_OneChannelMode:
				emit ShowMessage(tr("Pixel an Position (%1, %2) = %3").arg(posx).arg(posy).arg(_image->getPixel(posx, posy, _channelMapping[0])));
				break;

			case CM_ThreeChannelMode:
				emit ShowMessage(tr("Pixel an Position (%1, %2) = (%3, %4, %5)").arg(posx).arg(posy).arg(_image->getPixel(posx, posy, _channelMapping[0])).arg(_image->getPixel(posx, posy, _channelMapping[1])).arg(_image->getPixel(posx, posy, _channelMapping[2])));
				break;
		}
	}
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
	// Selection rectangle
	if (event->buttons() & Qt::LeftButton)
	{
		_selection.setBottomRight(event->pos() + _offset);

		update();
	}
}

/**************************************
*
*	Tile caching
*
***************************************/

void ImageWidget::ClearTileCache()
{
	_currentTimestamp = 0;
	_currentTiles.clear();
	_loadingTiles.clear();

	update();
}

void ImageWidget::LoadNextTile()
{
	if (!_image)
		return;

	// Visible region of the image
	QRect visibleRect = this->rect().translated(_offset) & _bounds;

retry:
	// Return if no tiles need to be loaded
	if (_loadingTiles.empty())
		return;

	// Get next tile-id from list and check if it needs to be loaded
	TileID id = _loadingTiles.takeFirst();

	if (_currentTiles.contains(id))
		goto retry;

	QPoint tilePosition = PositionFromId(id) * TileSize;
	QRect tileRect(tilePosition, QSize(TileSize, TileSize));
	if (!visibleRect.intersects(tileRect))
		goto retry;

	// Load tile data and generate pixmap
	QImage tmpImage(TileSize, TileSize, QImage::Format_RGB32);
	int offx = tilePosition.x(), offy = tilePosition.y();
	quint8 r, g, b;

	Ga::ImageBase *pImage = _image->pImage();
	double fMin = pImage->fileMin();
	double fMax = pImage->fileMax();
	double fScale = 255.0 / (fMax - fMin);

	switch (_cmMode)
	{
		case CM_OneChannelMode:
		{
			for (int y = 0; y < TileSize; y++)
			{
				for (int x = 0; x < TileSize; x++)
				{
					int px = x + offx;
					int py = y + offy;
					int posx = static_cast<int>(px * _affineTransformation[0] + py * _affineTransformation[1] + _affineTransformation[2]);
					int posy = static_cast<int>(px * _affineTransformation[3] + py * _affineTransformation[4] + _affineTransformation[5]);

					g = static_cast<quint8>(std::max(0.0, std::min((pImage->getPixelAsDouble(posx, posy, _channelMapping[0]) + fMin) * fScale, 255.0)));

					tmpImage.setPixel(x, y, qRgb(g, g, g));
				}
			}
			break;
		}

		case CM_ThreeChannelMode:
		{
			for (int y = 0; y < TileSize; y++)
			{
				for (int x = 0; x < TileSize; x++)
				{
					int px = x + offx;
					int py = y + offy;
					int posx = static_cast<int>(px * _affineTransformation[0] + py * _affineTransformation[1] + _affineTransformation[2]);
					int posy = static_cast<int>(px * _affineTransformation[3] + py * _affineTransformation[4] + _affineTransformation[5]);

					r = static_cast<quint8>(std::max(0.0, std::min((pImage->getPixelAsDouble(posx, posy, _channelMapping[0]) + fMin) * fScale, 255.0)));
					g = static_cast<quint8>(std::max(0.0, std::min((pImage->getPixelAsDouble(posx, posy, _channelMapping[1]) + fMin) * fScale, 255.0)));
					b = static_cast<quint8>(std::max(0.0, std::min((pImage->getPixelAsDouble(posx, posy, _channelMapping[2]) + fMin) * fScale, 255.0)));

					tmpImage.setPixel(x, y, qRgb(r, g, b));
				}
			}
			break;
		}
	}

	_currentTiles.insert(id, TileInfo(id, _currentTimestamp, tmpImage));

	QRect updateRect = tileRect.translated(-_offset) & this->rect();
	update(updateRect);
}

void ImageWidget::RemoveOldTiles()
{
	if (!_image)
		return;

	while (_currentTiles.size() > _maxTilesCached)
	{
		// Visible region of the image
		QRect visibleRect = this->rect().translated(_offset) & _bounds;
	
		// Find the tile with the oldest timestamp
		TileID oldestTile = static_cast<TileID>(-1);
		TimeStamp oldestTimestamp = std::numeric_limits<TimeStamp>::max();
	
		for (QMap<TileID, TileInfo>::iterator iter = _currentTiles.begin(); iter != _currentTiles.end(); iter++)
		{
			if (iter.value().timestamp < oldestTimestamp)
			{
				QPoint tilePosition = PositionFromId(iter.key()) * TileSize;
				QRect tileRect(tilePosition, QSize(TileSize, TileSize));
				if (!visibleRect.intersects(tileRect))
				{
					oldestTile = iter.key();
					oldestTimestamp = iter.value().timestamp;
				}
			}
		}
	
		// Remove the tile
		if ((oldestTile != static_cast<TileID>(-1)) && _currentTiles.contains(oldestTile))
			_currentTiles.remove(oldestTile);
	}
}

/**************************************
*
*	View / Affine Transformation
*
***************************************/

void ImageWidget::ResetView(bool recalc)
{
	_affineTransformation[0] = 1.0f;
	_affineTransformation[1] = 0.0f;
	_affineTransformation[2] = 0.0f;
	_affineTransformation[3] = 0.0f;
	_affineTransformation[4] = 1.0f;
	_affineTransformation[5] = 0.0f;
	_affineTransformation[6] = 0.0f;
	_affineTransformation[7] = 0.0f;
	_affineTransformation[8] = 1.0f;

	memcpy(_invAffineTransformation, _affineTransformation, sizeof(float) * 9);

	if (recalc)
		RecalculateBounds();
}

void ImageWidget::ZoomView(float zoomX, float zoomY, bool recalc)
{
	float matrix[9] = {
		1.0f / zoomX, 0.0f, 0.0f,
		0.0f, 1.0f / zoomY, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	float invmatrix[9] = {
		zoomX, 0.0f, 0.0f,
		0.0f, zoomY, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	MultiplyMatrixRight(matrix, true);
	MultiplyMatrixLeft(invmatrix, false);

	if (recalc)
		RecalculateBounds();
}

void ImageWidget::RotateView(float angle, bool recalc)
{
	float sina = sin(angle);
	float cosa = cos(angle);

	float matrix[9] = {
		+cosa, -sina, 0.0f,
		+sina, +cosa, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	float invmatrix[9] = {
		+cosa, +sina, 0.0f,
		-sina, +cosa, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	MultiplyMatrixRight(matrix, true);
	MultiplyMatrixLeft(invmatrix, false);

	if (recalc)
		RecalculateBounds();
}

void ImageWidget::TranslateView(float transX, float transY, bool recalc)
{
	float matrix[9] = {
		1.0f, 0.0f, -transX,
		0.0f, 1.0f, -transY,
		0.0f, 0.0f, 1.0f
	};

	float invmatrix[9] = {
		1.0f, 0.0f, transX,
		0.0f, 1.0f, transY,
		0.0f, 0.0f, 1.0f
	};

	MultiplyMatrixRight(matrix, true);
	MultiplyMatrixLeft(invmatrix, false);

	if (recalc)
		RecalculateBounds();
}

void ImageWidget::RecalculateBounds()
{
	if (!_image)
		return;

	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::min();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::min();

	for (int i = 0; i < 4; i++)
	{
		float x = (i & 1) ? _image->sizeX() : 0.0f;
		float y = (i & 2) ? _image->sizeY() : 0.0f;

		float px = x * _invAffineTransformation[0] + y * _invAffineTransformation[1] + _invAffineTransformation[2];
		float py = x * _invAffineTransformation[3] + y * _invAffineTransformation[4] + _invAffineTransformation[5];

		minX = std::min(minX, px);
		maxX = std::max(maxX, px);
		minY = std::min(minY, py);
		maxY = std::max(maxY, py);
	}

	TranslateView(-minX, -minY, false);
	_bounds = QRect(0, 0, maxX - minX, maxY - minY);

	ClearTileCache();

	emit UpdateScrollBars();
}

void ImageWidget::MultiplyMatrixRight(float *matrix, bool target)
{
	float tmpMatrix[9];
	float *targetMatrix = target ? _affineTransformation : _invAffineTransformation;
	memcpy(tmpMatrix, targetMatrix, sizeof(float) * 9);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			targetMatrix[j*3 + i] = 0.0f;
			for (int k = 0; k < 3; k++)
				targetMatrix[j*3 + i] += tmpMatrix[j*3 + k] * matrix[k*3 + i];
		}
	}
}

void ImageWidget::MultiplyMatrixLeft(float *matrix, bool target)
{
	float tmpMatrix[9];
	float *targetMatrix = target ? _affineTransformation : _invAffineTransformation;
	memcpy(tmpMatrix, targetMatrix, sizeof(float) * 9);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			targetMatrix[j*3 + i] = 0.0f;
			for (int k = 0; k < 3; k++)
				targetMatrix[j*3 + i] += matrix[j*3 + k] * tmpMatrix[k*3 + i];
		}
	}
}

/**************************************
*
*	Misc
*
***************************************/

void ImageWidget::ChangeOffsetX(int offsetX)
{
	_offset.setX(offsetX);
	update();
}

void ImageWidget::ChangeOffsetY(int offsetY)
{
	_offset.setY(offsetY);
	update();
}
