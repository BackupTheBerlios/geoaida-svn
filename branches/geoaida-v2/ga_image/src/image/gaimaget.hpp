 /***************************************************************************
                          gaimaget.hpp  -  GeoAIDA template impl.
                             -------------------
    begin                : Thu Jan 11 2001
    copyright            : (C) 2001 TNT, Uni Hannover
    authors              : J¸rgen B¸ckner, Oliver Stahlhut, Martin Pahl,
                           Julian Raschke, Karsten Vogt
    email                : bueckner@tnt.uni-hannover.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>
#include <stdexcept>
#include <stdarg.h>

namespace Ga {
template <class PixTyp>
double ImageT<PixTyp>::getPixelAsDouble(int x, int y, int channel, double neutral) const {
  return getPixel(x, y, channel, neutral);
}

template <class PixTyp>
void ImageT<PixTyp>::setPixelToDouble(int x, int y, double val, int channel, bool clip) {
  setPixel(x, y, val, channel, clip);
}

template <class PixTyp>
double ImageT<PixTyp>::getPixelAsDoubleFast(int x, int y, int channel, double neutral) const {
	assert(channel>=0);
	assert(channel<noChannels());

	if (x>=0 && x<sizeX() && y>=0 && y<sizeY())
	{
		int segX = x / segSizeX_;
		int segY = y / segSizeY_;
		BlockHandle *handle = getBlockHandle(segX, segY, channel);

		// Read pixel data
		handle->lockRW();
		BlockLock lock(*handle);
		PixTyp *outputBuffer = static_cast<PixTyp *>(handle->getData());

		int xo = x % segSizeX_;
		int yo = y % segSizeY_;

		return outputBuffer[yo * segmentSizeX(segX) + xo];
	}
	else
		return neutral;
}

template <class PixTyp>
int ImageT<PixTyp>::segmentsX() const
{
  return static_cast<int>(ceil((double)sizeX() / (double)segSizeX_));
}

template <class PixTyp>
int ImageT<PixTyp>::segmentsY() const
{
	return static_cast<int>(ceil((double)sizeY() / (double)segSizeY_));
}

template <class PixTyp>
LargeSize ImageT<PixTyp>::segmentSizeX(int row) const
{
  if (row == segmentsX() - 1)
  {
    int edgeSize = sizeX() % segSizeX_;
	return (edgeSize > 0 ? edgeSize : segSizeX_);
  }
  else
    return segSizeX_;
}

template <class PixTyp>
LargeSize ImageT<PixTyp>::segmentSizeY(int col) const
{
  if (col == segmentsY() - 1)
  {
    int edgeSize = sizeY() % segSizeY_;
    return (edgeSize > 0 ? edgeSize : segSizeY_);
  }
  else
    return segSizeY_;
}

template <class PixTyp>
BlockHandle *ImageT<PixTyp>::getBlockHandle(int segX, int segY, int channel) const
{
	BlockHandle *handle = &channels.at(channel).segments.at(segY * segmentsX() + segX);

	if (handle->isEmpty())
	{
		// Protection against integer overflows.
		*handle = Cache::get().alloc(segmentSizeX(segX) * segmentSizeY(segY) * sizeof(PixTyp));

		if (source_)
		{
			// File source open: We need to load the file data now.
			int srcSegSizeX = source_->segmentSizeX();
			int srcSegSizeY = source_->segmentSizeY();

			// Lock output buffer
			handle->lockRW();
			BlockLock lock(*handle);

			//Special case: input/output segment sizes are identical
			if (segSizeX_ == srcSegSizeX && segSizeY_ == srcSegSizeY)
			{
				source_->readRect(channel, segX * segmentSizeX(), segY * segmentSizeY(),
					segmentSizeX(segX), segmentSizeY(segY), static_cast<PixTyp*>(handle->getData()));
			}
			else
			{
				// Load all required input segments and copy them into the output buffer
				PixTyp *inputBuffer;// = new PixTyp[srcSegSizeX * srcSegSizeY];
				PixTyp *outputBuffer = static_cast<PixTyp *>(handle->getData());

				int srcSegsPerRow = sizeX() / srcSegSizeX + (sizeX() % srcSegSizeX != 0 ? 1 : 0);
				int srcSegsPerCol = sizeY() / srcSegSizeY + (sizeY() % srcSegSizeY != 0 ? 1 : 0);
				int srcSegSizeXEdge = (sizeX() % srcSegSizeX != 0) ? (sizeX() % srcSegSizeX) : srcSegSizeX;
				int srcSegSizeYEdge = (sizeY() % srcSegSizeY != 0) ? (sizeY() % srcSegSizeY) : srcSegSizeY;

				int srcSegLeft = (segX * segSizeX_) / srcSegSizeX;
				int srcSegRight = (segX * segSizeX_ + (int)segmentSizeX(segX) - 1) / srcSegSizeX;
				int srcSegTop = (segY * segSizeY_) / srcSegSizeY;
				int srcSegBottom = (segY * segSizeY_ + (int)segmentSizeY(segY) - 1) / srcSegSizeY;

				for (int curSegY = srcSegTop; curSegY <= srcSegBottom; curSegY++)
				{
					for (int curSegX = srcSegLeft; curSegX <= srcSegRight; curSegX++)
					{
						int x = curSegX * srcSegSizeX;
						int y = curSegY * srcSegSizeY;
						int segSizeX = (curSegX == srcSegsPerRow - 1) ? srcSegSizeXEdge  : srcSegSizeX;
						int segSizeY = (curSegY == srcSegsPerCol - 1) ? srcSegSizeYEdge  : srcSegSizeY;

						// Check if the input segment is already in the cache
						unsigned int tileID = curSegY * srcSegsPerRow + curSegX;

						typename std::deque<std::pair<unsigned int, PixTyp *> >::iterator iter;
						for (iter = tileCache_[channel].begin(); iter != tileCache_[channel].end(); iter++)
							if (iter->first == tileID)
								break;

						if (iter == tileCache_[channel].end())
						{
							// Read input segment
							inputBuffer = new PixTyp[segSizeX * segSizeY];
							source_->readRect(channel, x, y, segSizeX, segSizeY, inputBuffer);

							// Add tile to cache
							tileCache_[channel].push_front(std::pair<unsigned int, PixTyp *>(tileID, inputBuffer));
						}
						else
						{
							// Get input segment from cache
							inputBuffer = iter->second;

							// Move cache entry to the front
							tileCache_[channel].erase(iter);
							tileCache_[channel].push_front(std::pair<unsigned int, PixTyp *>(tileID, inputBuffer));
						}

						// Trim tile cache
						if (tileCache_[channel].size() > TILE_CACHE_SIZE)
						{
							// Release memory
							for (int i = TILE_CACHE_SIZE; i < tileCache_[channel].size(); i++)
								delete[] tileCache_[channel][i].second;

							// Trim cache
							tileCache_[channel].resize(TILE_CACHE_SIZE);
						}

						// Copy all relevant parts into the output buffer
						int offX = segX * segSizeX_ - curSegX * srcSegSizeX;
						int offY = segY * segSizeY_ - curSegY * srcSegSizeY;

						int inputOffsetX = std::max(0, offX);
						int inputOffsetY = std::max(0, offY);
						int inputLineSkip = segSizeX;

						int outputOffsetX = std::max(0, -offX);
						int outputOffsetY = std::max(0, -offY);
						int outputLineSkip = segmentSizeX(segX);

						int lineLength = segSizeX - inputOffsetX - std::max(0, -offX + segSizeX - (int)segmentSizeX(segX));
						int lineCount = segSizeY - inputOffsetY - std::max(0, -offY + segSizeY - (int)segmentSizeY(segY));
						PixTyp *inputPixel = &inputBuffer[inputOffsetY * inputLineSkip + inputOffsetX];
						PixTyp *outputPixel = &outputBuffer[outputOffsetY * outputLineSkip + outputOffsetX];

						while (lineCount--)
						{
							memcpy(outputPixel, inputPixel, sizeof(PixTyp) * lineLength);
							inputPixel += inputLineSkip;
							outputPixel += outputLineSkip;
						}
					}
				}
			}
		}
	}

	return handle;
}

template <class PixTyp>
template <typename It>
It ImageT<PixTyp>::iteratorForElem(unsigned channel, LargeSize elem) const
{
	BlockHandle* handle;
	LargeSize offset;
	LargeSize rangeBegin, rangeEnd;

	if (elem < noPixels())
	{
		int col = elem % sizeX();
		int row = elem / sizeX();
		int segX = col / segSizeX_;
		int segY = row / segSizeY_;

		handle = getBlockHandle(segX, segY, channel);
		offset = (row % segSizeY_) * segmentSizeX(segX);
		rangeBegin = row * sizeX() + segX * segSizeX_;
		rangeEnd = rangeBegin + segmentSizeX(segX);
	}
	else
	{
		// Special case for end iterators etc.: If the elem is out of range, the
		// iterator is given an empty range.
		handle = 0;
		offset = rangeBegin = rangeEnd = 0;
	}

	return It(handle, elem, offset, rangeBegin, rangeEnd,
		std::tr1::bind(&ImageT<PixTyp>::template iteratorForElem<It>, this, channel, std::tr1::placeholders::_1));
}

template <class PixTyp>
ImageT<PixTyp>::ImageT(int sizeX, int sizeY, int noChannels) : ImageBase()
{
	// Unsigned values would make more sense, but can be dangerous as well.
	// Let's put our fate into good, old assert's hand.
	assert(sizeX >= 0);
	assert(sizeY >= 0);

	sizeX_ = sizeX;
	sizeY_ = sizeY;
	segSizeX_ = SEGMENT_SIZE;
	segSizeY_ = SEGMENT_SIZE;
	channels.resize(noChannels);

	// Reservation for later segments.
	for (int i = 0; i < noChannels; ++i)
		channels[i].segments.resize(segmentsX() * segmentsY());

	// Initialize tile cache
	tileCache_.resize(noChannels);

	// Try to guess the filetype.
	fileMin_ = std::numeric_limits<PixTyp>::min();
	fileMax_ = std::numeric_limits<PixTyp>::max();

	if (typeid(PixTyp) == typeid(unsigned char))
		setFileType(_TIFF_UINT8);
	else if (typeid(PixTyp) == typeid(signed char))
		setFileType(_TIFF_SINT8);
	else if (typeid(PixTyp) == typeid(unsigned short))
		setFileType(_TIFF_UINT16);
	else if (typeid(PixTyp) == typeid(signed short))
		setFileType(_TIFF_SINT16);
	else if (typeid(PixTyp) == typeid(unsigned int))
		setFileType(_TIFF_UINT32);
	else if (typeid(PixTyp) == typeid(signed int))
		setFileType(_TIFF_SINT32);
	else if (typeid(PixTyp) == typeid(float))
	{
		fileMin_ = 0;
		fileMax_ = 1;
		setFileType(_TIFF_FLOAT);
	}
	else if (typeid(PixTyp) == typeid(double))
	{
		fileMin_ = 0;
		fileMax_ = 1;
		setFileType(_TIFF_DOUBLE);
	}
	else if (typeid(PixTyp) == typeid(bool))
	{
		fileMin_ = 0;
		fileMax_ = 1;
		setFileType(_PBM);
	}
	else
		setFileType(_UNKNOWN);
}

template <class PixTyp>
ImageBase* ImageT<PixTyp>::copyObject() {
  return new ImageT(*this);
}

template <class PixTyp>
const std::type_info& ImageT<PixTyp>::typeId() const {
  return typeid(PixTyp);
}

template <class PixTyp>
int ImageT<PixTyp>::noChannels() const {
  return channels.size();
}

template <class PixTyp>
PixTyp ImageT<PixTyp>::getPixel(int x, int y, int channel, PixTyp neutral) const {
  assert(channel>=0);
  assert(channel<noChannels());
  if (x>=0 && x<sizeX() && y>=0 && y<sizeY())
    return constBegin(y,channel)[x];
  else
    return neutral;
}

template <class PixTyp>
void ImageT<PixTyp>::partCopy(const ImageBase &rvalue, int x0, int y0, int width, int height) {
  const ImageT& rval = dynamic_cast<const ImageT&>(rvalue);
  
  assert(width >= 0);
  assert(width < rval.sizeX() - x0);
  assert(height >= 0);
  assert(height < rval.sizeY() - y0);
  assert(x0 >= 0);
  assert(x0 < rval.sizeX());
  assert(y0 >= 0);
  assert(y0 < rval.sizeY());
  
  resize(width, height, rval.noChannels());
  setFileType(rval.fileType());
  for (int c = 0; c < noChannels(); ++c)
    for (int row = 0; row < sizeY(); ++row)
      std::copy(rval.constBegin(row + y0, c) + x0, rval.constEnd(row, c), begin(row, c));
}

template <class PixTyp>
typename ImageT<PixTyp>::Iterator ImageT<PixTyp>::begin(int row, int channel) {
  return iteratorForElem<Iterator>(channel, row * sizeX());
}

template <class PixTyp>
typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::constBegin(int row, int channel) const {
  return iteratorForElem<ConstIterator>(channel, row * sizeX());
}

template <class PixTyp>
typename ImageT<PixTyp>::Iterator ImageT<PixTyp>::end(int row, int channel) {
  return iteratorForElem<Iterator>(channel, (row + 1) * sizeX());
}
  
template <class PixTyp>
typename ImageT<PixTyp>::Iterator ImageT<PixTyp>::end() {
  return iteratorForElem<Iterator>(noChannels() - 1, noPixels());
}
  
template <class PixTyp>
typename ImageT<PixTyp>::Iterator ImageT<PixTyp>::endChannel(int channel) {
  return iteratorForElem<Iterator>(channel, noPixels());
}

template <class PixTyp>
typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::constEnd(int row, int channel) const {
  return iteratorForElem<ConstIterator>(channel, (row + 1) * sizeX());
}

template <class PixTyp>
typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::constEnd() const {
  return iteratorForElem<ConstIterator>(noChannels() - 1, noPixels());
}
  
template <class PixTyp>
typename ImageT<PixTyp>::ConstIterator ImageT<PixTyp>::constEndChannel(int channel) const {
  return iteratorForElem<ConstIterator>(channel, noPixels());
}

template <class PixTyp>
PixTyp& ImageT<PixTyp>::operator() (int x, int y, int channel) {
  return begin(y,channel)[x];
}

template <class PixTyp>
void ImageT<PixTyp>::setPixel(int x, int y, PixTyp val, int channel, bool clip) {
  assert(channel>=0);
  assert(channel<noChannels());
  if (!clip || x >= 0 && x < sizeX() && y >= 0 && y < sizeY())
    begin(y,channel)[x] = val;
}

template <class PixTyp>
void ImageT<PixTyp>::fillRow(int row, int startX, int endX, double val, int channel) {
  Iterator it = begin(row, channel);
  std::fill(it + startX, it + endX, static_cast<PixTyp>(val));
}

template <class PixTyp>
double ImageT<PixTyp>::findMaxValue(int channel) {
  return *std::max_element(begin(0, channel), endChannel(channel));
}

template <class PixTyp>
double ImageT<PixTyp>::findMinValue(int channel) {
  return *std::min_element(begin(0, channel), endChannel(channel));
}

template <class PixTyp>
void ImageT<PixTyp>::resize(int rx, int ry, int noChannels) {
  ImageT(rx, ry, noChannels).swap(*this);
}

template <class PixTyp>
void ImageT<PixTyp>::fill(double value) {
  for (int c = 0; c < noChannels(); ++c)
    std::fill(begin(0, c), end(sizeY(), c), static_cast<PixTyp>(value));
}

template <class PixTyp>
void ImageT<PixTyp>::getChannel(ImageBase& resultImg, int channel)
{
  ImageT<PixTyp>& pic = dynamic_cast<ImageT<PixTyp>&>(resultImg);
  ImageT<PixTyp> channelCopy(sizeX(), sizeY(), 1);
  channelCopy.channels[0] = channels.at(channel);
  pic.swap(channelCopy);
}

template <class PixTyp>
void ImageT<PixTyp>::preload()
{
	if (!source_)
		return;

	// Load all segments (implicitly)
	for (int segY = 0; segY < segmentsY(); ++segY)
		for (int segX = 0; segX < segmentsX(); ++segX)
			for (int c = 0; c < noChannels(); ++c)
				getPixel(segX * segmentSizeX(), segY * segmentSizeY(), c);

	// The source file can now be released
	source_.reset();
}

template <class PixTyp>
void ImageT<PixTyp>::read(ImageIOPtr io)
{
	source_ = io;
	setComment(io->comment());
	setFileMin(io->fileMin());
	setFileMax(io->fileMax());
}

template <class PixTyp>
void ImageT<PixTyp>::write(ImageIOPtr io)
{
	io->setComment(comment());
	io->setFileMin(fileMin());
	io->setFileMax(fileMax());

	// Write all segments back to disk
	for (int segY = 0; segY < segmentsY(); ++segY)
		for (int segX = 0; segX < segmentsX(); ++segX)
			for (int c = 0; c < noChannels(); ++c)
			{
				BlockHandle& handle = channels[c].segments[segY * segmentsX() + segX];
				assert(!handle.isEmpty());
				handle.lockR();
				BlockLock lock(handle);
				io->replaceRect(c, segX * segmentSizeX(), segY * segmentSizeY(),
				segmentSizeX(segX), segmentSizeY(segY),
				static_cast<PixTyp*>(handle.getData()));
				handle.markClean();
			}
}

} // namespace Ga
