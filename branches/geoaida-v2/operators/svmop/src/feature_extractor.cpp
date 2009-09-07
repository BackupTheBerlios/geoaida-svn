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
/// \file		feature_extractor.cpp
/// \brief		Implementation of class "FeatureExtractor"
///
/// \date		2009-07-13
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#include "feature_extractor.h"

#include <sstream>

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
///////////////////////////////////////////////////////////////////////////////
FeatureExtractor::FeatureExtractor():	m_pInputChannelList(ImageListType::New()),
										m_bFeaturesExtracted(false),
										m_bLabelsExtracted(false),
										m_bLabelImageLoaded(false),
										m_bLabelSpacingAndBorderSet(false),
										m_nFilterRadius(FEATURE_EXTRACTOR_DEFAULT_FILTER_RADIUS),
										m_nLabelSpacingX(FEATURE_EXTRACTOR_DEFAULT_SPACING_AND_BORDER),
										m_nLabelSpacingY(FEATURE_EXTRACTOR_DEFAULT_SPACING_AND_BORDER),
										m_nLabelBorderX(FEATURE_EXTRACTOR_DEFAULT_SPACING_AND_BORDER),
										m_nLabelBorderY(FEATURE_EXTRACTOR_DEFAULT_SPACING_AND_BORDER)
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::FeatureExtractor()");

	METHOD_EXIT(m_Log, "FeatureExtractor::FeatureExtractor()");
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Returns the feature point set
///
/// \return Returns a pointer to feature point set
///
///////////////////////////////////////////////////////////////////////////////
const FeaturePointSetType::Pointer FeatureExtractor::getFeatures() const
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::getFeatures()");

	if (!m_bFeaturesExtracted)
	{
		WARNING_MSG(m_Log, "Image Feature Extractor", "No features list was loaded or extracted.", LOG_DOMAIN_NONE);
	}
	METHOD_EXIT(m_Log, "FeatureExtractor::getFeatures()");
	return m_pFeatures;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Returns the label label point set
///
/// \return Returns a pointer to label point set
///
///////////////////////////////////////////////////////////////////////////////
const LabelPointSetType::Pointer FeatureExtractor::getLabels() const
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::getLabels()");

	if (!m_bLabelsExtracted)
	{
		WARNING_MSG(m_Log, "Image Feature Extractor", "No label list was loaded or extracted.", LOG_DOMAIN_NONE);
	}

	METHOD_EXIT(m_Log, "FeatureExtractor::getLabels()");
	return m_pLabels;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Returns the size of the label image
///
/// \return Returns label image size
///
///////////////////////////////////////////////////////////////////////////////
const LabelImageType::SizeType FeatureExtractor::getImageSize() const
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::getImageSize()");

	if (m_pInputChannelList->Size() == 0)
	{
		WARNING_MSG(m_Log, "Image Feature Extractor", "No images were loaded. Can't determine size.", LOG_DOMAIN_NONE);
	}

	METHOD_EXIT(m_Log, "FeatureExtractor::getImageSize()");
	return m_ImageSize;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to add another input channel
///
/// \param ImageName Filename of the input image
///
///////////////////////////////////////////////////////////////////////////////
void FeatureExtractor::addInputChannel(const std::string& ImageName)
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::addInputChannel(const std::string&)");

	INFO_MSG(m_Log, "Image Feature Extractor", "Reading input channel " << ImageName, LOG_DOMAIN_NONE);
	
	ReaderType::Pointer pReader = ReaderType::New();

	pReader->SetFileName(ImageName);
	pReader->Update();
	
	m_pInputChannelList->PushBack(pReader->GetOutput());
	m_ImageSize = m_pInputChannelList->Front()->GetLargestPossibleRegion().GetSize();
	DEBUG_MSG(m_Log, "Image Feature Extractor", "Size: " << m_ImageSize, LOG_DOMAIN_VAR);

	INFO_MSG(m_Log, "Image Feature Extractor", "Number of channels: " << m_pInputChannelList->Size(), LOG_DOMAIN_VAR);

	METHOD_EXIT(m_Log, "FeatureExtractor::addInputChannel(const std::string&)");
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to clear all channels from list
///
///////////////////////////////////////////////////////////////////////////////
void FeatureExtractor::clearChannels()
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::clearChannels()");

	INFO_MSG(m_Log, "Image Feature Extractor", "Deleting input channels.", LOG_DOMAIN_NONE);
	
	m_pInputChannelList->Clear();

	METHOD_EXIT(m_Log, "FeatureExtractor::clearChannels()");
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to extract features from image
///
/// In this method, features are extracted from the image. All previously
/// added channels will be used to build a vector image. Filters and feature
/// extractors are applied on this vector image.
///
/// \todo Normalize features!
/// \todo Check every label in neighbourhood when training. For better performance,
///       create a index list for those pixels to be evaluated
///
/// \param _nMode Mode of operation. 
///
/// \return Returns if extraction was successfull
///
///////////////////////////////////////////////////////////////////////////////
bool FeatureExtractor::extract(const uchar& _nMode)
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::extract()");
	
	switch (_nMode)
	{
		//--------------------------------------------------------------------------
		// Extraction with labels
		//--------------------------------------------------------------------------
		case FEATURE_EXTRACTOR_USE_LABELS:
		{
			if (!m_bLabelImageLoaded)
			{
				ERROR_MSG(m_Log, "Image Feature Extractor", "No label image loaded.", LOG_DOMAIN_NONE);
				METHOD_EXIT(m_Log, "FeatureExtractor::extract()");
				return false;
			}
			if (!m_bLabelSpacingAndBorderSet)
			{
				NOTICE_MSG(m_Log, "Image Feature Extractor", "Label spacing and border not set. Using default value " <<
							FEATURE_EXTRACTOR_DEFAULT_SPACING_AND_BORDER << " for all.", LOG_DOMAIN_NONE);
			}
			
			this->extractWithLabels();
			m_bLabelsExtracted = true;
			break;
		}
		//--------------------------------------------------------------------------
		// Extraction without labels
		//--------------------------------------------------------------------------
		case FEATURE_EXTRACTOR_DEFAULT:
		{
			this->extractWithoutLabels();
			break;
		}
	}

// 	// Initialise image pyramid
// 	PyramidFilterType::Pointer Pyramid = PyramidFilterType::New();
// 	Pyramid->SetNumberOfLevels(4);
// 	Pyramid->SetDecimationRatio(1.5);
// 	Pyramid->SetInput(m_pImage);
// 	Pyramid->Update();

// 	INFO_MSG(m_Log, "Image Feature Extractor", "Image pyramid initialised.", LOG_DOMAIN_NONE);

// 	ImageListIterator itAnalyse = Pyramid->GetOutput()->Begin();
	
// 	TextureFilter->Update();

// 	int i=1;
// 
// 	// Writing the results images
// 	while (itAnalyse != Pyramid->GetOutput()->End())
// 	{
// 		INFO_MSG(m_Log, "Image Feature Extractor", "Pyramid Level " << i << ".", LOG_DOMAIN_NONE);
// // 		TextureFilter->SetInput(itAnalyse.Get());
// 		INFO_MSG(m_Log, "Image Feature Extractor", "Initialise filtering.", LOG_DOMAIN_NONE);
// // 		Writer->SetInput(itAnalyse.Get());//TextureFilter->GetOutput());
// // 		Writer->SetFileName(m_ImageNamesList[i]);
// 		INFO_MSG(m_Log, "Image Feature Extractor", "Processing.", LOG_DOMAIN_NONE);
// // 		Writer->Update();
// // 		INFO_MSG(m_Log, "Image Feature Extractor", "Image " << m_ImageNamesList[i] << " written.", LOG_DOMAIN_FILEIO);
// 		++itAnalyse;
// 		++i;
// 	}

// 	m_pInputChannelList->Update();

// 	ImageListToVectorImageFilterType::Pointer pC2VI = ImageListToVectorImageFilterType::New();
// 	
// 	pC2VI->SetInput(m_pInputChannelList);
// 	pC2VI->Update();
// 
// 	TextureFilter->SetInput(pC2VI->GetOutput());
// 	TextureFilter->Update();
// 	m_pWriter->SetInput(TextureFilter->GetOutput());
// 	m_pWriter->SetFileName("Vectorimage.tif");
// 	m_pWriter->Update();
	
// 	ImageListIterator itVariance = m_pInputChannelList->Begin();
// 
// 	uint i=0;
// 	while (itVariance != m_pInputChannelList->End())
// 	{
// 		TextureFilter->SetInput(itVariance.Get());
// 		m_pWriter->SetInput(TextureFilter->GetOutput());
// 
// 
// 		ostringstream Filename("");
// 		Filename << "entropy_" <<
// 							TextureFilter->GetRadius()[0] << "-" <<
// 							TextureFilter->GetOffset()[0] << "-" <<
// 							TextureFilter->GetOffset()[1] << "_" <<
// 							i << ".tif";
// 		m_pWriter->SetFileName(Filename.str());
// 		m_pWriter->Update();
// 		++itVariance;
// 		++i;
// 	}

	m_bFeaturesExtracted = true;
	METHOD_EXIT(m_Log, "FeatureExtractor::extract()");
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Loads a label image.
///
/// \param _ImageName Filename of the input image
///
///////////////////////////////////////////////////////////////////////////////
void FeatureExtractor::loadLabelImage(const std::string& _ImageName)
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::loadLabelImage(const std::string&)");

	if (m_bLabelImageLoaded == true)
	{
		NOTICE_MSG(m_Log, "Image Feature Extractor", "Labelimage already loaded. Reloading.", LOG_DOMAIN_NONE);
	}

	LabelReaderType::Pointer pLabelReader = LabelReaderType::New();

	pLabelReader->SetFileName(_ImageName);
	pLabelReader->Update();

	INFO_MSG(m_Log, "Image Feature Extractor", "Label image loaded.", LOG_DOMAIN_NONE);

	m_pLabelImage = pLabelReader->GetOutput();
	m_bLabelImageLoaded = true;
	
	METHOD_EXIT(m_Log, "FeatureExtractor::loadLabelImage(const std::string&)");
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Configures the size of filters
///
/// \param _nR Size of filters (radius)
///
///////////////////////////////////////////////////////////////////////////////
void FeatureExtractor::setFilterRadius(const int& _nR)
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::setFilterRadius(const int&)");

	m_nFilterRadius = _nR;

	METHOD_EXIT(m_Log, "FeatureExtractor::setFilterRadius(const int&)");
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Configures label image for training
///
/// \param _nSX Spacing of samples in label image, x direction
/// \param _nSX Spacing of samples in label image, y direction
/// \param _nSX Distance of samples to label image border, x direction
/// \param _nSX Distance of samples to label image border, y direction
///
///////////////////////////////////////////////////////////////////////////////
void FeatureExtractor::setLabelSpacingAndBorder(const int& _nSX, const int& _nSY,
											const int& _nBX, const int& _nBY)
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::setLabelSpacingAndBorder(const int&, const int&, const int&, const int&)");

	if (m_bLabelSpacingAndBorderSet == true)
	{
		NOTICE_MSG(m_Log, "Image Feature Extractor", "Already set label image spacing and border. Overwriting.", LOG_DOMAIN_NONE);
	}

	m_nLabelSpacingX = _nSX;
	m_nLabelSpacingY = _nSY;
	m_nLabelBorderX = _nBX;
	m_nLabelBorderY = _nBY;

	m_bLabelSpacingAndBorderSet = true;

	METHOD_EXIT(m_Log, "FeatureExtractor::setLabelSpacingAndBorder(const int&, const int&, const int&, const int&)");
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to extract features from image incorporating label image.
///
/// In this method the label image is taken into consideration. Since it is
/// often not intended to extract features of every pixel, but from certain
/// pixel of a grid with border, other ORFEO methods for efficient extraction
/// are used.
///
/// \todo Normalize features!
/// \todo Check every label in neighbourhood when training. For better performance,
///       create a index list for those pixels to be evaluated
///
/// \return Returns if extraction was successfull
///
///////////////////////////////////////////////////////////////////////////////
bool FeatureExtractor::extractWithLabels()
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::extractWithLabels()");
	
	//--------------------------------------------------------------------------
	// Setup common stuff
	//--------------------------------------------------------------------------
	m_pFeatures = FeaturePointSetType::New();
	m_pLabels = LabelPointSetType::New();

	FeaturePointContainer::Pointer	pFeatCont = FeaturePointContainer::New();
	LabelPointContainer::Pointer	pLabCont = LabelPointContainer::New();
	
	//--- Define image filters ---//
	typedef itk::MeanImageFilter<InputImageType, InputImageType> MeanFilterType;
	MeanFilterType::Pointer pMeanFilter = MeanFilterType::New();
	
	typedef otb::Functor::VarianceTextureFunctor<	InputPixelType,
													InputPixelType>
		FunctorType;

	typedef otb::TextureImageFunction<InputImageType, FunctorType>
		VarianceFilterType;
		
	typedef LabelImageType::RegionType ValidateRegionType;
	typedef itk::ImageRegionConstIterator<LabelImageType> ConstLabelImageIteratorType;

	VarianceFilterType::Pointer		TextureFilter = VarianceFilterType::New();
	VarianceFilterType::SizeType	nRadius;
	InputImageType::IndexType		nFeatureIndex;
	InputImageType::OffsetType		nOffset;
	InputImageType::SizeType		nSize;
	LabelImageType::IndexType		nLabelIndex;

	nRadius[0] = m_nFilterRadius;
	nRadius[1] = m_nFilterRadius;
	nOffset[0] = 5;
	nOffset[1] = 0;
	TextureFilter->SetRadius(nRadius);
	TextureFilter->SetOffset(nOffset);

	pMeanFilter->SetRadius(nRadius);
	
	m_pInputChannelList->Update();
	ImageListType::Iterator it = m_pInputChannelList->Begin();
	nSize = it.Get()->GetLargestPossibleRegion().GetSize();
	uint nChannel = 0u;
	uint nX;
	uint nY;
	std::vector<FeatureVectorType> FeaturePoints;
	std::vector<LabelPixelType> LabelPoints;
	
	INFO_MSG(m_Log, "Image Feature Extractor", "Reading labels",
					LOG_DOMAIN_NONE);
	INFO_MSG(m_Log, "Image Feature Extractor", "Calculating features of channel 0",
					LOG_DOMAIN_NONE);

	// First run to fill LabelPoints vector and first dimension and
	// therefore size of FeaturePoints vector
	TextureFilter->SetInputImage(it.Get());
	pMeanFilter->SetInput(it.Get());
	pMeanFilter->Update();
	
	LabelWriterType::Pointer pWriter = LabelWriterType::New();
	pWriter->SetFileName("mean.tif");
	pWriter->SetInput(pMeanFilter->GetOutput());
	pWriter->Update();
	
	// For debugging draw an image of feature extraction points
	DEBUG(
		LabelImageType::Pointer pValidationMap = LabelImageType::New();
		LabelImageType::IndexType StartIndex;
		LabelImageType::SizeType Size;
		LabelImageType::RegionType Region;

		StartIndex[0]=0;
		StartIndex[1]=0;
		Region.SetIndex(StartIndex);
		Region.SetSize(m_ImageSize);
		pValidationMap->SetRegions(Region);
		pValidationMap->Allocate();
		pValidationMap->FillBuffer(0);
	);
	
	std::list<InputImageType::IndexType> InputIndexList;

	nY = m_nLabelBorderY;
	while (nY < nSize[1]-m_nLabelBorderY+1)
	{
		nX = m_nLabelBorderX;
		nFeatureIndex[1] = nY;
		nLabelIndex[1] = nY;
		while (nX < nSize[0]-m_nLabelBorderX+1)
		{
			nFeatureIndex[0] = nX;
			nLabelIndex[0] = nX;

			////////////////////////////////////////////////////////////////////
			// Validate label index inside of image region. This is done to
			// ensure only training of features that are calculated on a single
			// label, outside of borders. Iteration through image region should
			// be faster than GetPixel methods.
			////////////////////////////////////////////////////////////////////
			ValidateRegionType Region;
			ValidateRegionType::IndexType RgStart;
			ValidateRegionType::SizeType RgSize;
			
			RgStart[0] = nX-m_nLabelBorderX;
			RgStart[1] = nY-m_nLabelBorderY;
			RgSize[0] = m_nLabelBorderX*2+1;
			RgSize[1] = m_nLabelBorderY*2+1;
			Region.SetIndex(RgStart);
			Region.SetSize(RgSize);
			ConstLabelImageIteratorType ciR(m_pLabelImage, Region);
			
			ciR.GoToBegin();
			LabelPixelType nLabel = ciR.Value(); ++ciR;
			bool bValid = true;
			while (!ciR.IsAtEnd())
			{
				if ((ciR.Value() != nLabel) || (ciR.Value() == 0))
				{
					bValid = false;
					ciR.GoToEnd();
				}
				else
				{
					++ciR;
				}
			}
			////////////////////////////////////////////////////////////////////
			// If image region is valid, create training data and save the index
			// for further channels.
			////////////////////////////////////////////////////////////////////
			if (bValid)
			{
				FeatureVectorType vecFeature;
				vecFeature.push_back(TextureFilter->EvaluateAtIndex(nFeatureIndex));
				vecFeature.push_back(pMeanFilter->GetOutput()->GetPixel(nFeatureIndex));
				FeaturePoints.push_back(vecFeature);
			
				LabelPoints.push_back(m_pLabelImage->GetPixel(nLabelIndex));

				// Fill validation map for debugging purposes
				DEBUG(
					pValidationMap->SetPixel(nLabelIndex,255);
				);

				InputIndexList.push_back(nFeatureIndex);
			}
			
			nX += m_nLabelSpacingX;
		}
		nY += m_nLabelSpacingY;
	}
	++it;
	++nChannel;
	
	// Save the validation map
	DEBUG(
		LabelWriterType::Pointer pVMWriter = LabelWriterType::New();
		pVMWriter->SetFileName("DEBUG_validation_map.tif");
		pVMWriter->SetInput(pValidationMap);
		pVMWriter->Update();
	);
	
	while (it != m_pInputChannelList->End())
	{
		INFO_MSG(m_Log, "Image Feature Extractor", "Calculating features of channel " <<
					nChannel, LOG_DOMAIN_NONE);
		
		int i=0;
		
		TextureFilter->SetInputImage(it.Get());
		pMeanFilter->SetInput(it.Get());
		pMeanFilter->Update();

		std::list<InputImageType::IndexType>::const_iterator ciIndex = InputIndexList.begin();
		while (ciIndex != InputIndexList.end())
		{
			FeaturePoints[i].push_back(TextureFilter->EvaluateAtIndex((*ciIndex)));
			FeaturePoints[i].push_back(pMeanFilter->GetOutput()->GetPixel((*ciIndex)));
			++i;
			++ciIndex;
		}
		++it;
		++nChannel;
	}
	
	std::vector<FeatureVectorType>::const_iterator ci;
	std::vector<LabelPixelType>::const_iterator cj = LabelPoints.begin();
	int i=0;
	for (ci = FeaturePoints.begin(); ci != FeaturePoints.end(); ++ci)
	{
		
		FeaturePointType FP;
		LabelPointType LP;

		FP[0] = -i;
		LP[0] = -i;

		pFeatCont->InsertElement(i, FP);
		m_pFeatures->SetPointData(i, (*ci));

		pLabCont->InsertElement(i, LP);
		m_pLabels->SetPointData(i, (*cj));
		++i;
		++cj;
	}

	m_pFeatures->SetPoints(pFeatCont);
	m_pLabels->SetPoints(pLabCont);

	METHOD_EXIT(m_Log, "FeatureExtractor::extractWithLabels()");
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to extract features from image.
///
/// Since it is common for to extract features for every pixel when in
/// classification, ORFEO methods to filter the whole image are used.
///
/// \todo Normalize features!
///
/// \return Returns if extraction was successfull
///
///////////////////////////////////////////////////////////////////////////////
bool FeatureExtractor::extractWithoutLabels()
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::extractWithoutLabels()");
	
	//--------------------------------------------------------------------------
	// Setup common stuff
	//--------------------------------------------------------------------------
	m_pFeatures = FeaturePointSetType::New();

	FeaturePointContainer::Pointer	pFeatCont = FeaturePointContainer::New();
	
	//--- Define image filters ---//
	typedef itk::MeanImageFilter<InputImageType, InputImageType> MeanFilterType;
	MeanFilterType::Pointer pMeanFilter = MeanFilterType::New();
	
	typedef otb::Functor::VarianceTextureFunctor<	InputPixelType,
													InputPixelType>
		FunctorType;
		
	typedef otb::UnaryFunctorNeighborhoodWithOffsetImageFilter<	InputImageType,
															InputImageType,
															FunctorType>
	VarianceFilterType;
	typedef itk::ImageRegionConstIterator<InputImageType> ConstInputImageIteratorType;

	VarianceFilterType::Pointer		TextureFilter = VarianceFilterType::New();
	InputImageType::SizeType		nRadius;
	InputImageType::IndexType		nFeatureIndex;
	InputImageType::OffsetType		nOffset;
	InputImageType::SizeType		nSize;

	nRadius[0] = m_nFilterRadius;
	nRadius[1] = m_nFilterRadius;
	nOffset[0] = 5;
	nOffset[1] = 0;
	TextureFilter->SetRadius(nRadius);
	TextureFilter->SetOffset(nOffset);

	pMeanFilter->SetRadius(nRadius);
	
	m_pInputChannelList->Update();
	ImageListType::Iterator it = m_pInputChannelList->Begin();
	nSize = it.Get()->GetLargestPossibleRegion().GetSize();
	uint nChannel = 0u;
	uint nX;
	uint nY;
	std::vector<FeatureVectorType> FeaturePoints;
	
	INFO_MSG(m_Log, "Image Feature Extractor", "Reading labels",
					LOG_DOMAIN_NONE);
	INFO_MSG(m_Log, "Image Feature Extractor", "Calculating features of channel 0",
					LOG_DOMAIN_NONE);

	// First run to fill LabelPoints vector and first dimension and
	// therefore size of FeaturePoints vector
	TextureFilter->SetInput(it.Get());
	TextureFilter->Update();
	pMeanFilter->SetInput(it.Get());
	pMeanFilter->Update();
	
// 	LabelWriterType::Pointer pWriter = LabelWriterType::New();
// 	pWriter->SetFileName("mean.tif");
// 	pWriter->SetInput(pMeanFilter->GetOutput());
// 	pWriter->Update();

	nY = 0;
	while (nY < nSize[1])
	{
		nX = 0;
		nFeatureIndex[1] = nY;
		while (nX < nSize[0])
		{
			nFeatureIndex[0] = nX;
			
			FeatureVectorType vecFeature;
			vecFeature.push_back(TextureFilter->GetOutput()->GetPixel(nFeatureIndex));
			vecFeature.push_back(pMeanFilter->GetOutput()->GetPixel(nFeatureIndex));
			FeaturePoints.push_back(vecFeature);
			
			++nX;
		}
		++nY;
	}
// 	ConstInputImageIteratorType itImg(pMeanFilter->GetOutput(), pMeanFilter->GetOutput()->GetLargestPossibleRegion());
// 	itImg.GoToBegin();
// 	while (!itImg.IsAtEnd())
// 	{
// 		FeatureVectorType vecFeature;
// 		vecFeature.push_back(itImg.Value());
// 		++itImg;
// 	}
	++it;
	++nChannel;
	
	while (it != m_pInputChannelList->End())
	{
		INFO_MSG(m_Log, "Image Feature Extractor", "Calculating features of channel " <<
					nChannel, LOG_DOMAIN_NONE);
		
		int i=0;
		
		TextureFilter->SetInput(it.Get());
		TextureFilter->Update();
		pMeanFilter->SetInput(it.Get());
		pMeanFilter->Update();
		nY = 0;
		while (nY < nSize[1])
		{
			nX = 0;
			nFeatureIndex[1] = nY;
			while (nX < nSize[0])
			{
				nFeatureIndex[0] = nX;
				
				FeaturePoints[i].push_back(TextureFilter->GetOutput()->GetPixel(nFeatureIndex));
				FeaturePoints[i].push_back(pMeanFilter->GetOutput()->GetPixel(nFeatureIndex));
				++i;
				
				++nX;
			}
			++nY;
		}
		++it;
		++nChannel;
	}
	std::vector<FeatureVectorType>::const_iterator ci;
	int i=0;
	for (ci = FeaturePoints.begin(); ci != FeaturePoints.end(); ++ci)
	{
		
		FeaturePointType FP;

		FP[0] = -i;

		pFeatCont->InsertElement(i, FP);
		m_pFeatures->SetPointData(i, (*ci));

		++i;
	}

	m_pFeatures->SetPoints(pFeatCont);

	METHOD_EXIT(m_Log, "FeatureExtractor::extractWithoutLabels()");
	return true;
}
// ///////////////////////////////////////////////////////////////////////////////
// ///
// /// \brief Transforms the image list to a vector image for further processing
// ///
// ///////////////////////////////////////////////////////////////////////////////
// void FeatureExtractor::channels2VectorImage()
// {
// 	METHOD_ENTRY(m_Log, "FeatureExtractor::channels2VectorImage()");
// 
// 	INFO_MSG(m_Log, "Image Feature Extractor", "Composing image channels.", LOG_DOMAIN_NONE);
// 	ImageListToVectorImageFilterType::Pointer pC2VI = ImageListToVectorImageFilterType::New();
// 	
// 	pC2VI->SetInput(m_pInputChannelList);
// 	pC2VI->Update();
// 	m_pImage = pC2VI->GetOutput();
// 
// 	METHOD_EXIT(m_Log, "FeatureExtractor::channels2VectorImage()");
// }