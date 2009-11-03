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

#include <algorithm>

/**************************************
*
*	Constructor / Destructor
*
***************************************/

ImageWidget::ImageWidget(QWidget *parent)
	: QWidget(parent), _currentTimestamp(0), _contrast(1.0), _brightness(0.0)
{
	_cmMode = CM_OneChannelMode;
	_channelMapping[0] = _channelMapping[1] = _channelMapping[2] = 0;
	_randomMapping = false;

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
	_scaleFactor = 1.0f;

	_contrast = 1.0;
	_brightness = 0.0;
	_bitdepth = 8;

	_selection = QRect();

	_images.clear();
	_channels.clear();
}

void ImageWidget::Open(QString filename)
{
	Clear();

	// Load image
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(filename.toStdString().c_str());
	reader->UpdateOutputInformation();
	_images.append(reader);

	// Split into channels
	for (int i = 0; i < reader->GetOutput()->GetNumberOfComponentsPerPixel(); i++)
	{
		ChannelExtractType::Pointer extractor = ChannelExtractType::New();
		extractor->SetExtractionRegion(reader->GetOutput()->GetLargestPossibleRegion());
		extractor->SetChannel(i + 1);
		extractor->SetInput(reader->GetOutput());
		extractor->UpdateOutputInformation();
		_channels.append(extractor);
	}

	// Initialize channel mapping
	if (channelCount() < 3)
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

void ImageWidget::AddChannels(QString filename)
{
	if (!isValidImage())
	{
		Open(filename);
		return;
	}
	
	// Load image
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(filename.toStdString().c_str());
	reader->UpdateOutputInformation();
	
	if ((imageWidth() != reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0]) || (imageHeight() != reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1]))
		emit ShowWarning(tr("Das neue Bild muss dieselbe Größe haben wie das bereits geladene!"));
	else
	{
		_images.append(reader);

		// Split into channels
		for (int i = 0; i < reader->GetOutput()->GetNumberOfComponentsPerPixel(); i++)
		{
			ChannelExtractType::Pointer extractor = ChannelExtractType::New();
			extractor->SetExtractionRegion(reader->GetOutput()->GetLargestPossibleRegion());
			extractor->SetChannel(i + 1);
			extractor->SetInput(reader->GetOutput());
			extractor->UpdateOutputInformation();
			_channels.append(extractor);
		}
	}
}

void ImageWidget::SaveSelection(QString filename, QVector<bool> channels, ColorDepth colordepth, bool applycontrastbrightness)
{
	if (!isValidImage())
		return;

	// Create list of channels to be exported
	typedef otb::ImageList<ChannelType> cilTempType;
	cilTempType::Pointer channelList = cilTempType::New();
	
	for (int i = 0; i < std::min(channelCount(), channels.size()); i++)
		if (channels[i])
			channelList->PushBack(_channels[i]->GetOutput());

	// Convert image-list to vector-image
	typedef otb::ImageListToVectorImageFilter<cilTempType, ImageType> il2viTempType;
	il2viTempType::Pointer converter = il2viTempType::New();
	converter->SetInput(channelList);
	
	// Setup cropping region
	typedef itk::ExtractImageFilter<ImageType, ImageType> eiTempType;
	eiTempType::Pointer extractregion = eiTempType::New();

	ImageType::RegionType::IndexType region_topleft;
	region_topleft[0] = _selection.isEmpty() ? 0 : _selection.left();
	region_topleft[1] = _selection.isEmpty() ? 0 : _selection.top();
	ImageType::RegionType::SizeType region_size;
	region_size[0] = _selection.isEmpty() ? imageWidth() : _selection.width();
	region_size[1] = _selection.isEmpty() ? imageHeight() : _selection.height();
	extractregion->SetExtractionRegion(ImageType::RegionType(region_topleft, region_size));
	extractregion->SetInput(converter->GetOutput());

	// Write image
	RealType fMin = 0;
	RealType fMax = (1 << bitdepth()) - 1;
	RealType fScale = 1 / (fMax - fMin);
	RealType fAdd = fMin;

	switch (colordepth)
	{
		case CD_INTEGER_8BIT:
		{
			typedef otb::VectorImage<quint8, 2> ExportImageTempType;
			typedef itk::ShiftScaleImageFilter<otb::Image<RealType, 2>, otb::Image<quint8, 2> > cbTempType;
			typedef otb::PerBandVectorImageFilter<ImageType, ExportImageTempType, cbTempType> pbcbTempType;
			pbcbTempType::Pointer caster = pbcbTempType::New();
			caster->GetFilter()->SetShift(fAdd + (applycontrastbrightness ? brightness() : 0));
			caster->GetFilter()->SetScale(((1 << 8) - 1) * fScale * (applycontrastbrightness ? contrast() : 1.0));
			caster->SetInput(extractregion->GetOutput());
			
			typedef otb::ImageFileWriter<ExportImageTempType> wviTempType;
			wviTempType::Pointer writer = wviTempType::New();
			writer->SetFileName(filename.toStdString().c_str());
			writer->SetInput(caster->GetOutput());
			
			writer->Update();
		}
		break;
		
		case CD_INTEGER_16BIT:
		{
			typedef otb::VectorImage<quint16, 2> ExportImageTempType;
			typedef itk::ShiftScaleImageFilter<otb::Image<RealType, 2>, otb::Image<quint16, 2> > cbTempType;
			typedef otb::PerBandVectorImageFilter<ImageType, ExportImageTempType, cbTempType> pbcbTempType;
			pbcbTempType::Pointer caster = pbcbTempType::New();
			caster->GetFilter()->SetShift(fAdd + (applycontrastbrightness ? brightness() : 0));
			caster->GetFilter()->SetScale(((1 << 16) - 1) * fScale * (applycontrastbrightness ? contrast() : 1.0));
			caster->SetInput(extractregion->GetOutput());
			
			typedef otb::ImageFileWriter<ExportImageTempType> wviTempType;
			wviTempType::Pointer writer = wviTempType::New();
			writer->SetFileName(filename.toStdString().c_str());
			writer->SetInput(caster->GetOutput());
			
			writer->Update();
		}
		break;
		
		case CD_FLOAT_32BIT:
		{
			typedef itk::ShiftScaleImageFilter<otb::Image<RealType, 2>, otb::Image<RealType, 2> > cbTempType;
			typedef otb::PerBandVectorImageFilter<ImageType, ImageType, cbTempType> pbcbTempType;
			pbcbTempType::Pointer caster = pbcbTempType::New();
			caster->GetFilter()->SetShift(fAdd + (applycontrastbrightness ? brightness() : 0));
			caster->GetFilter()->SetScale(fScale * (applycontrastbrightness ? contrast() : 1.0));
			caster->SetInput(extractregion->GetOutput());
			
			typedef otb::ImageFileWriter<ImageType> wviTempType;
			wviTempType::Pointer writer = wviTempType::New();
			writer->SetFileName(filename.toStdString().c_str());
			writer->SetInput(caster->GetOutput());
			
			writer->Update();
		}
		break;
		
		default:
			emit ShowWarning(tr("Die gewählte Farbtiefe ist ungültig!"));
	}
}

void ImageWidget::SaveSelectionSeparatedChannels(QString filenametemplate, QVector<bool> channels, ColorDepth colordepth, bool applycontrastbrightness)
{
	if (!isValidImage())
		return;

	// Create filename template
	QString path = QFileInfo(filenametemplate).canonicalPath();
	QString base = QFileInfo(filenametemplate).baseName();
	QString suffix = QFileInfo(filenametemplate).completeSuffix();

	if (suffix.isEmpty())
		suffix = "tif";

	// Export each selected channel
	for (int i = 0; i < std::min(channelCount(), channels.size()); i++)
	{
		if (!channels[i])
			continue;

		QString filename = QString("%1/%2_%3.%4").arg(path).arg(base).arg(i).arg(suffix);

		// Setup cropping region
		typedef itk::ExtractImageFilter<ChannelType, ChannelType> eiTempType;
		eiTempType::Pointer extractregion = eiTempType::New();

		ImageType::RegionType::IndexType region_topleft;
		region_topleft[0] = _selection.isEmpty() ? 0 : _selection.left();
		region_topleft[1] = _selection.isEmpty() ? 0 : _selection.top();
		ImageType::RegionType::SizeType region_size;
		region_size[0] = _selection.isEmpty() ? imageWidth() : _selection.width();
		region_size[1] = _selection.isEmpty() ? imageHeight() : _selection.height();
		extractregion->SetExtractionRegion(ImageType::RegionType(region_topleft, region_size));
		extractregion->SetInput(_channels[i]->GetOutput());

		// Write image
		RealType fMin = 0;
		RealType fMax = (1 << bitdepth()) - 1;
		RealType fScale = 1 / (fMax - fMin);
		RealType fAdd = fMin;

		switch (colordepth)
		{
			case CD_INTEGER_8BIT:
			{
				typedef otb::Image<quint8, 2> ExportImageTempType;
				typedef itk::ShiftScaleImageFilter<otb::Image<RealType, 2>, otb::Image<quint8, 2> > cbTempType;
				cbTempType::Pointer caster = cbTempType::New();
				caster->SetShift(fAdd + (applycontrastbrightness ? brightness() : 0));
				caster->SetScale(((1 << 8) - 1) * fScale * (applycontrastbrightness ? contrast() : 1.0));
				caster->SetInput(extractregion->GetOutput());
				
				typedef otb::ImageFileWriter<ExportImageTempType> wviTempType;
				wviTempType::Pointer writer = wviTempType::New();
				writer->SetFileName(filename.toStdString().c_str());
				writer->SetInput(caster->GetOutput());
				
				writer->Update();
			}
			break;
			
			case CD_INTEGER_16BIT:
			{
				typedef otb::Image<quint16, 2> ExportImageTempType;
				typedef itk::ShiftScaleImageFilter<otb::Image<RealType, 2>, otb::Image<quint16, 2> > cbTempType;
				cbTempType::Pointer caster = cbTempType::New();
				caster->SetShift(fAdd + (applycontrastbrightness ? brightness() : 0));
				caster->SetScale(((1 << 16) - 1) * fScale * (applycontrastbrightness ? contrast() : 1.0));
				caster->SetInput(extractregion->GetOutput());
				
				typedef otb::ImageFileWriter<ExportImageTempType> wviTempType;
				wviTempType::Pointer writer = wviTempType::New();
				writer->SetFileName(filename.toStdString().c_str());
				writer->SetInput(caster->GetOutput());
				
				writer->Update();
			}
			break;
			
			case CD_FLOAT_32BIT:
			{
				typedef itk::ShiftScaleImageFilter<otb::Image<RealType, 2>, otb::Image<RealType, 2> > cbTempType;
				cbTempType::Pointer caster = cbTempType::New();
				caster->SetShift(fAdd + (applycontrastbrightness ? brightness() : 0));
				caster->SetScale(fScale * (applycontrastbrightness ? contrast() : 1.0));
				caster->SetInput(extractregion->GetOutput());
				
				typedef otb::ImageFileWriter<ChannelType> wviTempType;
				wviTempType::Pointer writer = wviTempType::New();
				writer->SetFileName(filename.toStdString().c_str());
				writer->SetInput(caster->GetOutput());
				
				writer->Update();
			}
			break;
			
			default:
				emit ShowWarning(tr("Die gewählte Farbtiefe ist ungültig!"));
		}
	}
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
	if (!isValidImage())
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
				_loadingTiles.push_front(id);
		}
	}

	// Draw the selection rectangle
	QPointF points[4];
	points[0] = QPointF(
			_selection.left() * _scaleFactor - _offset.x(),
			_selection.top() * _scaleFactor - _offset.y()
	);
	points[1] = QPointF(
			_selection.right() * _scaleFactor - _offset.x(),
			_selection.top() * _scaleFactor - _offset.y()
	);
	points[2] = QPointF(
			_selection.right() * _scaleFactor - _offset.x(),
			_selection.bottom() * _scaleFactor - _offset.y()
	);
	points[3] = QPointF(
			_selection.left() * _scaleFactor - _offset.x(),
			_selection.bottom() * _scaleFactor - _offset.y()
	);
	
	painter.setPen(QPen(QBrush(QColor(255, 0, 0)), 2, Qt::DashDotLine));
	painter.drawConvexPolygon(points, 4);

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
		QPoint pos = event->pos() + _offset;
		QPoint transformedPoint(
				pos.x() / _scaleFactor + 0.5,
				pos.y() / _scaleFactor + 0.5
		);

		_selection.setTopLeft(transformedPoint);
		_selection.setBottomRight(transformedPoint);

		update();
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event)
{
	// Selection rectangle
	if (event->buttons() & Qt::LeftButton)
	{
		QPoint pos = event->pos() + _offset;
		QPoint transformedPoint(
				pos.x() / _scaleFactor + 0.5,
				pos.y() / _scaleFactor + 0.5
		);

		_selection.setBottomRight(transformedPoint);

		update();
	}

	// Current pixel value
	/*
	if (isValidImage())
	{
		int px = event->x() + _offset.x();
		int py = event->y() + _offset.y();

		ChannelType::RegionType::IndexType index;
		index[0] = static_cast<int>(px / _scaleFactor);
		index[1] = static_cast<int>(py / _scaleFactor);

		ChannelType::RegionType::SizeType size;
		size[0] = 1;
		size[1] = 1;
		
		switch (_cmMode)
		{
			case CM_OneChannelMode:
				{
					_channels[_channelMapping[0]]->GetOutput()->SetRequestedRegion(ChannelType::RegionType(index, size));
					_channels[_channelMapping[0]]->Update();
					ChannelType::PixelType pixel = _channels[_channelMapping[0]]->GetOutput()->GetPixel(index);
					emit ShowMessage(tr("Pixel an Position (%1, %2) = %3").arg(index[0]).arg(index[1]).arg(pixel));
				}
				break;

			case CM_ThreeChannelMode:
				{
					for (int i = 0; i < 3; i++)
					{
						_channels[_channelMapping[i]]->GetOutput()->SetRequestedRegion(ChannelType::RegionType(index, size));
						_channels[_channelMapping[i]]->Update();
					}
					ChannelType::PixelType pixelR = _channels[_channelMapping[0]]->GetOutput()->GetPixel(index);
					ChannelType::PixelType pixelG = _channels[_channelMapping[1]]->GetOutput()->GetPixel(index);
					ChannelType::PixelType pixelB = _channels[_channelMapping[2]]->GetOutput()->GetPixel(index);
					emit ShowMessage(tr("Pixel an Position (%1, %2) = (%3, %4, %5)").arg(index[0]).arg(index[1]).arg(pixelR).arg(pixelG).arg(pixelB));
				}
				break;
		}
	}
	*/
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
	// Selection rectangle
	QPoint pos = event->pos() + _offset;
	QPoint transformedPoint(
			pos.x() / _scaleFactor + 0.5,
			pos.y() / _scaleFactor + 0.5
	);

	_selection.setBottomRight(transformedPoint);
	_selection = _selection.normalized();
	_selection = _selection & QRect(0, 0, imageWidth(), imageHeight());

	if (_selection.width() * _selection.height() < 4)
		_selection = QRect();

	update();
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
}

void ImageWidget::LoadNextTile()
{
	if (!isValidImage())
		return;

	// Visible region of the image
	QRect visibleRect = this->rect().translated(_offset) & _bounds;

retry:
	// Return if no tiles need to be loaded
	if (_loadingTiles.empty())
		return;

	// Get next tile-id from list and check if it needs to be loaded
	TileID id = _loadingTiles.takeFirst();

	QPoint tilePosition = PositionFromId(id) * TileSize;
	QRect tileRect(tilePosition, QSize(TileSize, TileSize));
	if (!visibleRect.intersects(tileRect))
		goto retry;

	// Load tile data and generate pixmap
	int offx = tilePosition.x();
	int offy = tilePosition.y();

	ChannelType::RegionType::IndexType otbTileIndex;
	otbTileIndex[0] = offx / _scaleFactor;
	otbTileIndex[1] = offy / _scaleFactor;

	if ((imageWidth() < otbTileIndex[0]) || (imageHeight() < otbTileIndex[1]))
		goto retry;

	ChannelType::RegionType::SizeType otbTileSize, otbRequestSize;
	otbRequestSize[0] = std::min(imageWidth() - otbTileIndex[0], (long)(TileSize / _scaleFactor));
	otbRequestSize[1] = std::min(imageHeight() - otbTileIndex[1], (long)(TileSize / _scaleFactor));
	otbTileSize[0] = otbRequestSize[0] * _scaleFactor;
	otbTileSize[1] = otbRequestSize[1] * _scaleFactor;

	QImage tmpImage(TileSize, TileSize, QImage::Format_RGB32);
	if ((otbTileSize[0] != TileSize) || (otbTileSize[1] != TileSize))
		tmpImage.fill(qRgb(0, 0, 0));

	int sx_dx = otbTileSize[0];	// Use bresenham line drawing algorithm to interpolate pixel coordinates for image zoom
	int sx_dy = otbRequestSize[0];
	int sx_error = sx_dx - 1;
	int sy_dx = otbTileSize[1];
	int sy_dy = otbRequestSize[1];
	int sy_error = sy_dx - 1;

	RealType fMin = 0;
	RealType fMax = (1 << bitdepth()) - 1;
	RealType fScale = (255 / (fMax - fMin)) * _contrast;
	RealType fAdd = fMin + _brightness;

	int x = 0, y = 0;
	quint8 r, g, b;
	int color;

	switch (_cmMode)
	{
		case CM_OneChannelMode:
		{
			ChannelExtractType::Pointer channel = _channels[_channelMapping[0]];
			channel->GetOutput()->SetRequestedRegion(ChannelType::RegionType(otbTileIndex, otbRequestSize));
			channel->Update();
			
			for (int y = 0; y < otbTileSize[1]; y++)
			{
				for (int x = 0; x < otbTileSize[0]; x++)
				{
					ChannelType::PixelType value = channel->GetOutput()->GetPixel(otbTileIndex);
					if (_randomMapping)
					{
						if (_randomMap.contains(value))
							color = _randomMap[value];
						else
						{
							color = qRgb(rand() % 256, rand() % 256, rand() % 256);
							_randomMap[value] = color;
						}
					}
					else
					{
						g = static_cast<quint8>(std::max(static_cast<RealType>(0), std::min((value + fAdd) * fScale, static_cast<RealType>(255))));
						color = qRgb(g, g, g);
					}
					tmpImage.setPixel(x, y, color);
					
					sx_error -= sx_dy;
					while (sx_error < 0)
					{
						otbTileIndex[0]++;
						sx_error += sx_dx;
					}
				}
				
				otbTileIndex[0] -= otbRequestSize[0];
				
				sy_error -= sy_dy;
				while (sy_error < 0)
				{
					otbTileIndex[1]++;
					sy_error += sy_dx;
				}
			}
		}
		break;

		case CM_ThreeChannelMode:
		{
			ChannelExtractType::Pointer channel[] = {_channels[_channelMapping[0]], _channels[_channelMapping[1]], _channels[_channelMapping[2]]};
			for (int i = 0; i < 3; i++)
			{
				channel[i]->GetOutput()->SetRequestedRegion(ChannelType::RegionType(otbTileIndex, otbRequestSize));
				channel[i]->Update();
			}
			
			for (int y = 0; y < otbTileSize[1]; y++)
			{
				for (int x = 0; x < otbTileSize[0]; x++)
				{
					r = static_cast<quint8>(std::max(static_cast<RealType>(0), std::min((channel[0]->GetOutput()->GetPixel(otbTileIndex) + fAdd) * fScale, static_cast<RealType>(255))));
					g = static_cast<quint8>(std::max(static_cast<RealType>(0), std::min((channel[1]->GetOutput()->GetPixel(otbTileIndex) + fAdd) * fScale, static_cast<RealType>(255))));
					b = static_cast<quint8>(std::max(static_cast<RealType>(0), std::min((channel[2]->GetOutput()->GetPixel(otbTileIndex) + fAdd) * fScale, static_cast<RealType>(255))));
					
					color = qRgb(r, g, b);
					tmpImage.setPixel(x, y, color);
					
					sx_error -= sx_dy;
					while (sx_error < 0)
					{
						otbTileIndex[0]++;
						sx_error += sx_dx;
					}
				}
				
				otbTileIndex[0] -= otbRequestSize[0];
				
				sy_error -= sy_dy;
				while (sy_error < 0)
				{
					otbTileIndex[1]++;
					sy_error += sy_dx;
				}
			}
		}
		break;
	}

	_currentTiles.insert(id, TileInfo(id, _currentTimestamp, tmpImage));

	QRect updateRect = tileRect.translated(-_offset) & this->rect();
	update(updateRect);
}

void ImageWidget::RemoveOldTiles()
{
	if (!isValidImage())
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

void ImageWidget::ResetView()
{
	_scaleFactor = 1.0;
	
	RecalculateBounds();
	Redraw();
}

void ImageWidget::ZoomView(RealType zoomfactor)
{
	if ((_scaleFactor * zoomfactor > 128.0) || (_scaleFactor * zoomfactor < 1.0/128.0))
		return;
	
	_scaleFactor *= zoomfactor;
	
	RecalculateBounds();
	Redraw();
}

void ImageWidget::RecalculateBounds()
{
	_bounds = QRect(0, 0, imageWidth() * _scaleFactor, imageHeight() * _scaleFactor);

	// Redraw and update scrollbars
	ClearTileCache();
	emit UpdateScrollBars();
}

/**************************************
*
*	Histogram and selection
*
***************************************/

/*
QVector<double> ImageWidget::CalculateHistogram(float coverage, int channel)
{
	assert(_image);
	assert((channel >= 0) && (channel < _image->noChannels()));
	assert((coverage > 0.0f) && (coverage <= 1.0f));

	float skip = 1.0f / coverage;
	QVector<double> returnList;

	QRect selection = _selection.normalized();
	if (!selection.isValid() || selection.isNull() || selection.isEmpty())
	{
		// Read image block-wise (faster)
		int expectedPixelsX = _image->sizeX() * coverage;
		int tileCountX = (_image->sizeX() + TileSize - 1) / TileSize;
		int expectedTilesX = (expectedPixelsX + TileSize - 1) / TileSize;

		int expectedPixelsY = _image->sizeY() * coverage;
		int tileCountY = (_image->sizeY() + TileSize - 1) / TileSize;
		int expectedTilesY = (expectedPixelsY + TileSize - 1) / TileSize;

		int tileWidth, tileHeight;
		Ga::ImageBase *pImage = _image->pImage();
		returnList.reserve(expectedTilesX * expectedTilesY * TileSize * TileSize);

		for (float ty = 0; ty < tileCountY; ty += skip)
		{
			tileHeight = ((int)ty == (tileCountY - 1)) ? (_image->sizeY() % TileSize) : TileSize;
			tileHeight = (tileHeight == 0) ? TileSize : tileHeight;

			for (float tx = 0; tx < tileCountX; tx += skip)
			{
				tileWidth = ((int)tx == (tileCountX - 1)) ? (_image->sizeX() % TileSize) : TileSize;
				tileWidth = (tileWidth == 0) ? TileSize : tileWidth;

				for (int y = (int)ty * TileSize; y < (int)ty * TileSize + tileHeight; y++)
					for (int x = (int)tx * TileSize; x < (int)tx * TileSize + tileWidth; x++)
						returnList.push_back(pImage->getPixelAsDoubleFast(x, y, channel));
			}
		}
	}
	else
	{
		// Read image pixel-wise (more accurate, only for small selections)
		float left = selection.x();
		float right = selection.x() + selection.width();
		float top = selection.y();
		float bottom = selection.y() + selection.height();

		Ga::ImageBase *pImage = _image->pImage();
		returnList.reserve((selection.width() + 1) * (selection.height() + 1) * coverage * coverage);

		for (float y = top; y <= bottom; y += skip)
		{
			for (float x = left; x <= right; x += skip)
			{
				double g = pImage->getPixelAsDoubleFast(x, y, channel);
				returnList.push_back(g);
			}
		}
	}

	qSort(returnList);
	return returnList;
}
*/

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

void ImageWidget::CalculateAutoCB()
{
	if (!isValidImage())
		return;

	// Calculate min, max pixel intensities
	typedef itk::MinimumMaximumImageCalculator<ChannelType> mmcTempType;

	RealType fMin = 0;
	RealType fMax = (1 << bitdepth()) - 1;
	RealType minvalue = std::numeric_limits<RealType>::infinity();
	RealType maxvalue = -std::numeric_limits<RealType>::infinity();

	ChannelType::RegionType::IndexType region_topleft;
	region_topleft[0] = _selection.isEmpty() ? 0 : _selection.left();
	region_topleft[1] = _selection.isEmpty() ? 0 : _selection.top();
	ChannelType::RegionType::SizeType region_size;
	region_size[0] = _selection.isEmpty() ? imageWidth() : _selection.width();
	region_size[1] = _selection.isEmpty() ? imageHeight() : _selection.height();

	switch (_cmMode)
	{
		case CM_OneChannelMode:
		{
			_channels[_channelMapping[0]]->GetOutput()->SetRequestedRegion(ChannelType::RegionType(region_topleft, region_size));
			_channels[_channelMapping[0]]->Update();
			
			mmcTempType::Pointer mmcalc = mmcTempType::New();
			mmcalc->SetRegion(ChannelType::RegionType(region_topleft, region_size));
			mmcalc->SetImage(_channels[_channelMapping[0]]->GetOutput());
			mmcalc->Compute();

			minvalue = std::min(minvalue, mmcalc->GetMinimum());
			maxvalue = std::max(maxvalue, mmcalc->GetMaximum());
		}
		break;
	
		case CM_ThreeChannelMode:
		{
			for (int i = 0; i < 3; i++)
			{
				_channels[_channelMapping[i]]->GetOutput()->SetRequestedRegion(ChannelType::RegionType(region_topleft, region_size));
				_channels[_channelMapping[i]]->Update();
				
				mmcTempType::Pointer mmcalc = mmcTempType::New();
				mmcalc->SetRegion(ChannelType::RegionType(region_topleft, region_size));
				mmcalc->SetImage(_channels[_channelMapping[i]]->GetOutput());
				mmcalc->Compute();

				minvalue = std::min(minvalue, mmcalc->GetMinimum());
				maxvalue = std::max(maxvalue, mmcalc->GetMaximum());
			}
		}
		break;
	}

	_brightness = -minvalue / (fMax - fMin);
	_contrast = (fMax - fMin) / (maxvalue - minvalue);

	Redraw();
}

void ImageWidget::SetRandomMapping(bool activate)
{
	_randomMapping = activate;
	_randomMap.clear();

	if (_cmMode == CM_OneChannelMode)
		Redraw();
}

void ImageWidget::SetContrast(double contrast)
{
	_contrast = contrast;
	Redraw();
}

void ImageWidget::SetBrightness(double brightness)
{
	_brightness = brightness;
	Redraw();
}

void ImageWidget::SetBitdepth(int bitdepth)
{
	_bitdepth = bitdepth;
	Redraw();
}

void ImageWidget::Redraw()
{
	ClearTileCache();
	update();
}
