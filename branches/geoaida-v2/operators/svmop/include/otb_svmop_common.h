/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

////////////////////////////////////////////////////////////////////////////////
///
/// \file		otb_svmop_common.h
/// \brief		Commonly used libraries and type definitions
///
/// \date		2009-07-13
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#ifndef OTB_SVMOP_COMMON_H
#define OTB_SVMOP_COMMON_H

#include "itkBinaryBallStructuringElement.h"
#include "itkListSample.h"
#include "itkPointSet.h"
#include "itkPointSetToListAdaptor.h"

#include "otbImage.h"
#include "otbImageList.h"
#include "otbImageFileReader.h"
#include "otbImageListToVectorImageFilter.h"
#include "otbMorphologicalPyramidAnalysisFilter.h"
#include "otbOpeningClosingMorphologicalFilter.h"
#include "otbStreamingImageFileWriter.h"
#include "otbSVMClassifier.h"
#include "otbSVMPointSetModelEstimator.h"
#include "otbTextureFunctors.h"
// #include "otbUnaryFunctorNeighborhoodWithOffsetImageFilter.h"
#include "otbTextureImageFunction.h"
#include "otbVectorImage.h"

// Define input and output images
const unsigned int Dimension = 2;
typedef unsigned short	InputPixelType;
typedef unsigned short	OutputPixelType;
typedef unsigned char	LabelPixelType;
typedef otb::Image<InputPixelType, Dimension> InputImageType;
typedef otb::Image<OutputPixelType, Dimension> OutputImageType;
typedef otb::Image<LabelPixelType, Dimension> LabelImageType;
typedef otb::VectorImage<OutputPixelType, Dimension> VectorImageType;

typedef otb::ImageList<InputImageType> ImageListType;

typedef otb::ImageListToVectorImageFilter<ImageListType, VectorImageType>
		ImageListToVectorImageFilterType;

// Define image filters
typedef otb::Functor::ContrastTextureFunctor<	InputPixelType,
												InputPixelType>
		FunctorType;
// typedef otb::UnaryFunctorNeighborhoodWithOffsetImageFilter<	InputImageType,
// 															InputImageType,
// 															FunctorType>
typedef otb::TextureImageFunction<InputImageType, FunctorType>
		FilterType;

// Define pyramid filtering types
typedef itk::BinaryBallStructuringElement<InputPixelType,Dimension> StructuringElementType;
typedef otb::OpeningClosingMorphologicalFilter<	InputImageType,
												InputImageType,
												StructuringElementType>
		OpeningClosingFilterType;
typedef otb::MorphologicalPyramidAnalysisFilter<InputImageType,
												InputImageType,
												OpeningClosingFilterType>
		PyramidFilterType;
typedef PyramidFilterType::OutputImageListType::Iterator ImageListIterator;

// Define Pipes
typedef otb::ImageFileReader<InputImageType> ReaderType;
typedef otb::ImageFileReader<LabelImageType> LabelReaderType;
typedef otb::StreamingImageFileWriter<OutputImageType> WriterType;

//--- Define types for svm learning and classification ---//
const unsigned int FeatureSpaceDimension = 1;
typedef int														LabelType;
typedef itk::PointSet<LabelType, 1>								LabelPointSetType;
typedef LabelPointSetType::PointType							LabelPointType;
typedef LabelPointSetType::PointsContainer						LabelPointContainer;

typedef float													FeatureType;
typedef std::vector<FeatureType>								FeatureVectorType;
typedef itk::PointSet<FeatureVectorType,FeatureSpaceDimension>	FeaturePointSetType;
typedef FeaturePointSetType::PointType							FeaturePointType;
typedef FeaturePointSetType::PointsContainer					FeaturePointContainer;

typedef itk::Statistics::PointSetToListAdaptor<FeaturePointSetType>				SampleType;
typedef otb::SVMModel<SampleType::MeasurementVectorType::ValueType,LabelType>	ModelType;

typedef otb::SVMPointSetModelEstimator<FeaturePointSetType,LabelPointSetType>	EstimatorType;
typedef otb::SVMClassifier<SampleType,LabelType>								ClassifierType;


#endif