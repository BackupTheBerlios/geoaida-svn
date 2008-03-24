/***************************************************************************
                          gaimaget.hpp  -  GeoAIDA template impl.
                             -------------------
    begin                : Thu Jan 11 2001
    copyright            : (C) 2001 TNT, Uni Hannover
    authors              : J¸rgen B¸ckner, Oliver Stahlhut, Martin Pahl,
                           Julian Raschke
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
int ImageT<PixTyp>::segmentsX() const
{
  return static_cast<int>(ceil(sizeX() / segSizeX_));
}

template <class PixTyp>
int ImageT<PixTyp>::segmentsY() const
{
  return static_cast<int>(ceil(sizeY() / segSizeY_));
}

template <class PixTyp>
template <typename It>
It ImageT<PixTyp>::iteratorForElem(unsigned channel, unsigned elem) const
{
  BlockHandle* handle;
  unsigned rangeBegin, rangeEnd;
  
  if (elem < noPixels())
  {
    // TODO: LargeSize again?
    unsigned col = elem % sizeX(), row = elem / sizeX();
    unsigned segX = col / segSizeX_, segY = row / segSizeY_;

    handle = &channels.at(channel).segments.at(segY * segmentsX() + segX);
    
    rangeBegin = row * sizeX() + segX * segSizeX_;
    rangeEnd = rangeBegin + segSizeX_;
    if (segX == segmentsX() - 1 && sizeX() % segSizeX_ != 0)
      rangeEnd -= (segSizeX_ - (sizeX() % segSizeX_));
  }
  else
  {
    // Special case for end iterators etc.: If the elem is out of range, the
    // iterator is given an empty range.
    handle = 0;
    rangeBegin = rangeEnd = 0;
  }
  
  return It(handle, elem, rangeBegin, rangeEnd,
    std::tr1::bind(&ImageT<PixTyp>::template iteratorForElem<It>, this, channel, std::tr1::placeholders::_1));
}

template <class PixTyp>
ImageT<PixTyp>::ImageT(int sizeX, int sizeY, int noChannels, int segSizeX, int segSizeY) : ImageBase() {
  // Unsigned values would make more sense, but can be dangerous as well.
  // Let's put our fate into good, old assert's hand.
  assert(sizeX >= 0);
  assert(sizeY >= 0);
  assert(segSizeX >= 0);
  assert(segSizeY >= 0);
  
  sizeX_ = sizeX;
  sizeY_ = sizeY;
  segSizeX_ = segSizeX ? segSizeX : sizeX;
  segSizeY_ = segSizeY ? segSizeY : sizeY;
  channels.resize(noChannels);
  
  // Protection against integer overflows.
  LargeSize size = segSizeX_;
  size *= segSizeY_;
  size *= sizeof(PixTyp);
  // Actual allocation.
  for (int i = 0; i < noChannels; ++i)
    for (int j = 0; j < segmentsX() * segmentsY(); ++j)
      channels[i].segments.push_back(Cache::get().alloc(size));
  
  // Try to guess the filetype.
  
  fileMin_ = 0;
  fileMax_ = 255;
  
  setFileType(_UNKNOWN);
  if (noChannels == 3 && typeid(PixTyp) == typeid(unsigned char))
    setFileType(_PPM);
  if (noChannels != 1)
    return;
  if (typeid(PixTyp) == typeid(unsigned char))
    setFileType(_PFM_FLOAT);
  else if (typeid(PixTyp) == typeid(signed))
    setFileType(_PFM_SINT);
  else if (typeid(PixTyp) == typeid(unsigned))
    setFileType(_PFM_UINT);
  else if (typeid(PixTyp) == typeid(signed short))
    setFileType(_PFM_SINT16);
  else if (typeid(PixTyp) == typeid(unsigned short))
    setFileType(_PFM_UINT16);
  else if (typeid(PixTyp) == typeid(float))
    setFileType(_PGM);
  else if (typeid(PixTyp) == typeid(bool))
    setFileType(_PBM);
}

template <class PixTyp>
ImageBase* ImageT<PixTyp>::copyObject() {
  return new ImageT(*this);
}

template <class PixTyp>
const class std::type_info& ImageT<PixTyp>::typeId() const {
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

template<typename PixTyp>
struct AvoidVectorBool { typedef PixTyp Type; };
template<>
struct AvoidVectorBool<bool> { typedef unsigned char Type; };

template <class PixTyp>
void ImageT<PixTyp>::read(ImageIO& io) {
  for (int c = 0; c < noChannels(); ++c)
  {
    std::vector<typename AvoidVectorBool<PixTyp>::Type>
        buffer(io.sizeX() * io.sizeY());
    io.readRect(c, 0, 0, io.sizeX(), io.sizeY(), &buffer[0]);
    std::copy(buffer.begin(), buffer.end(), begin(0, c));
  }
  setComment(io.comment());
  setFileMin(io.fileMin());
  setFileMax(io.fileMax());
}

template <class PixTyp>
void ImageT<PixTyp>::write(ImageIO& io, int channel) {
  io.setComment(comment());
  io.setFileMin(fileMin());
  io.setFileMax(fileMax());
  for (int c = 0; c < noChannels(); ++c)
  {
    std::vector<typename AvoidVectorBool<PixTyp>::Type>
        buffer(sizeX() * sizeY());
    std::copy(begin(0, c), begin(0, c) + sizeX() * sizeY(), buffer.begin());
    io.replaceRect(c, 0, 0, sizeX(), sizeY(), &buffer[0]);
  }
}

} // namespace Ga
