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

DEBUG(
	uint FeatureExtractor::m_unNoImageFiles = 0;
);

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
///////////////////////////////////////////////////////////////////////////////
FeatureExtractor::FeatureExtractor():	m_pInputChannelList(ImageListType::New()),
										m_bChannelLoaded(false),
										m_bFeaturesExtracted(false),
										m_bLabelsExtracted(false),
										m_bLabelImageLoaded(false),
										m_bLabelSpacingSet(false),
										m_fPyramidDecimationRate(FEATURE_EXTRACTOR_DEFAULT_PYRAMID_DECIMATION_RATE),
										m_nFilterRadius(FEATURE_EXTRACTOR_DEFAULT_FILTER_RADIUS),
										m_nNumberOfPyramidLevels(FEATURE_EXTRACTOR_DEFAULT_NUMBER_OF_PYRAMID_LEVELS),
										m_nLabelSpacingX(FEATURE_EXTRACTOR_DEFAULT_SPACING),
										m_nLabelSpacingY(FEATURE_EXTRACTOR_DEFAULT_SPACING),
										m_nLabelBorderX(FEATURE_EXTRACTOR_DEFAULT_FILTER_RADIUS),
										m_nLabelBorderY(FEATURE_EXTRACTOR_DEFAULT_FILTER_RADIUS)
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

	if (!m_bChannelLoaded)
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
	m_pInputChannelList->Update();	
	
	m_ImageSize = m_pInputChannelList->Front()->GetLargestPossibleRegion().GetSize();
	DEBUG_MSG(m_Log, "Image Feature Extractor", "Size: " << m_ImageSize, LOG_DOMAIN_VAR);

	INFO_MSG(m_Log, "Image Feature Extractor", "Number of channels: " << m_pInputChannelList->Size(), LOG_DOMAIN_VAR);
	m_bChannelLoaded = true;

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
	m_bChannelLoaded = false;

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
/// \param _nMode Mode of operation. 
///
/// \return Returns if extraction was successfull
///
///////////////////////////////////////////////////////////////////////////////
bool FeatureExtractor::extract(const unsigned char& _nMode)
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::extract()");
	
	m_nLabelBorderX = static_cast<int>(
						static_cast<double>(m_nFilterRadius) * 
						pow(m_fPyramidDecimationRate, m_nNumberOfPyramidLevels)
						+ 1);
	m_nLabelBorderY = m_nLabelBorderX;

	DEBUG_MSG(m_Log, "Image Feature Extractor", "Label border calculated: " <<
				m_nLabelBorderX, LOG_DOMAIN_VAR);
	
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
			if (!m_bLabelSpacingSet)
			{
				NOTICE_MSG(m_Log, "Image Feature Extractor", "Label spacing not set. Using default value " <<
							FEATURE_EXTRACTOR_DEFAULT_SPACING << " for all.", LOG_DOMAIN_NONE);
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
///
///////////////////////////////////////////////////////////////////////////////
void FeatureExtractor::setLabelSpacing(const int& _nSX, const int& _nSY)
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::setLabelSpacing(const int&, const int&)");

	if (m_bLabelSpacingSet == true)
	{
		NOTICE_MSG(m_Log, "Image Feature Extractor", "Already set label image spacing. Overwriting.", LOG_DOMAIN_NONE);
	}

	m_nLabelSpacingX = _nSX;
	m_nLabelSpacingY = _nSY;

	m_bLabelSpacingSet = true;

	METHOD_EXIT(m_Log, "FeatureExtractor::setLabelSpacing(const int&, const int&)");
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Configures image pyramid
///
/// \param _nNoPL Number of levels for image pyramid
///
///////////////////////////////////////////////////////////////////////////////
void FeatureExtractor::setNumberOfPyramidLevels(const int& _nNoPL)
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::setNumberOfPyramidLevels(const int&)");

	m_nNumberOfPyramidLevels = _nNoPL;
	INFO_MSG(m_Log, "Image Feature Extractor", "Number of levels for image "
			"pyramid set to " << _nNoPL, LOG_DOMAIN_VAR);
			
	METHOD_EXIT(m_Log, "FeatureExtractor::setNumberOfPyramidLevels(const int&)");
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Configures image pyramid
///
/// \param _fPDR Rate for subsampling of images within pyramid
///
///////////////////////////////////////////////////////////////////////////////
void FeatureExtractor::setPyramidDecimationRate(const double& _fPDR)
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::setPyramidDecimationRate(const double&)");

	m_fPyramidDecimationRate = _fPDR;
	INFO_MSG(m_Log, "Image Feature Extractor", "Decimation Rate for image pyramid "
			"set to " << _fPDR, LOG_DOMAIN_VAR);

	METHOD_EXIT(m_Log, "FeatureExtractor::setPyramidDecimationRate(const double&)");
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
/// \return Returns if extraction was successfull
///
///////////////////////////////////////////////////////////////////////////////
bool FeatureExtractor::extractWithLabels()
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::extractWithLabels()");
	
	////////////////////////////////////////////////////////////////////////////
	// Setup common stuff
	////////////////////////////////////////////////////////////////////////////
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
	InputImageType::SizeType		nRadius;
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
	ImageListType::ConstIterator it = m_pInputChannelList->Begin();
	nSize = it.Get()->GetLargestPossibleRegion().GetSize();
	nSize = m_ImageSize;
	uint nChannel = 0u;
	uint nX;
	uint nY;
	double fPyramidLevelFactor;
	std::vector<FeatureVectorType> FeaturePoints;
	std::vector<LabelPixelType> LabelPoints;

	INFO_MSG(m_Log, "Image Feature Extractor", "Reading labels",
					LOG_DOMAIN_NONE);
	INFO_MSG(m_Log, "Image Feature Extractor", "Calculating features of channel 0",
					LOG_DOMAIN_NONE);

	////////////////////////////////////////////////////////////////////////////
	// First run to fill LabelPoints vector and first dimension and therfore
	// size of FeaturePoints vector
	////////////////////////////////////////////////////////////////////////////
	
	//--------------------------
	// Initialise image pyramid
	//--------------------------
	PyramidFilterType::Pointer pPyramid = PyramidFilterType::New();
	pPyramid->SetNumberOfLevels(m_nNumberOfPyramidLevels);
	pPyramid->SetDecimationRatio(m_fPyramidDecimationRate);
	pPyramid->SetInput(it.Get());
	pPyramid->Update();
	fPyramidLevelFactor = 1.0;
	ImageListIterator itP = pPyramid->GetOutput()->Begin();
	
	TextureFilter->SetInputImage(it.Get());
	pMeanFilter->SetInput(it.Get());
	pMeanFilter->Update();

	// For debugging draw an image of feature extraction points
	DEBUG(
		Image8BitType::Pointer pValidationMap = Image8BitType::New();
		Image8BitType::IndexType StartIndex;
		Image8BitType::RegionType Region;

		StartIndex[0]=0;
		StartIndex[1]=0;
		Region.SetIndex(StartIndex);
		Region.SetSize(m_ImageSize);
		pValidationMap->SetRegions(Region);
		pValidationMap->Allocate();
		pValidationMap->FillBuffer(0);
	);
	
	
	//--------------------------------------------------------------------------
	// Store first feature vector component, build up list for further channels
	//--------------------------------------------------------------------------
	std::list<InputImageType::IndexType> InputIndexList;
	nY = m_nLabelBorderY;
	while (nY < nSize[1]-m_nLabelBorderY)
	{
		nX = m_nLabelBorderX;
		nFeatureIndex[1] = nY;
		nLabelIndex[1] = nY;
		while (nX < nSize[0]-m_nLabelBorderX)
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
			
			RgStart[0] = nX-int(m_nLabelBorderX);
			RgStart[1] = nY-int(m_nLabelBorderY);
			RgSize[0] = int(m_nLabelBorderX)*2+1;
			RgSize[1] = int(m_nLabelBorderY)*2+1;
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
		saveImage(pValidationMap, "DEBUG_validation_map.tif");
	);

	////////////////////////////////////////////////////////////////////////////
	// Now do the same for all other channels.
	////////////////////////////////////////////////////////////////////////////
	while (!((it == m_pInputChannelList->End()) && (itP == pPyramid->GetOutput()->End())))
	{
		int i=0;
	
		//-----------------------------------------------
		// Create new filters due to changing image size
		//-----------------------------------------------
		TextureFilter = VarianceFilterType::New();
		TextureFilter->SetRadius(nRadius);
		TextureFilter->SetOffset(nOffset);
		
		pMeanFilter = MeanFilterType::New();
		pMeanFilter->SetRadius(nRadius);
		
		//---------------------------------------------------
		// Calculate on sublevels or start new pyramid level
		//---------------------------------------------------
		if (itP != pPyramid->GetOutput()->End())
		{			
			TextureFilter->SetInputImage(itP.Get());
			pMeanFilter->SetInput(itP.Get());
			pMeanFilter->Update();
			fPyramidLevelFactor *= 1/m_fPyramidDecimationRate;
			++itP;
			
			INFO_MSG(m_Log, "Image Feature Extractor", "New pyramid level, size: " <<
					pMeanFilter->GetOutput()->GetLargestPossibleRegion().GetSize(),
					LOG_DOMAIN_VAR);
			DEBUG_MSG(m_Log, "Image Feature Extractor", "Pyramid level factor: " <<
					fPyramidLevelFactor, LOG_DOMAIN_VAR);
					
			DEBUG(
				std::ostringstream oss("");
				oss << "DEBUG_feature_image_" << m_unNoImageFiles << ".tif";
				saveImage(pMeanFilter->GetOutput(), oss.str());
				++m_unNoImageFiles;
			);
		}
		else
		{
			INFO_MSG(m_Log, "Image Feature Extractor", "Calculating features of channel " <<
					nChannel, LOG_DOMAIN_NONE);
			TextureFilter->SetInputImage(it.Get());
			pMeanFilter->SetInput(it.Get());
			pMeanFilter->Update();
			
			pPyramid = PyramidFilterType::New();
			pPyramid->SetNumberOfLevels(m_nNumberOfPyramidLevels);
			pPyramid->SetDecimationRatio(m_fPyramidDecimationRate);
			pPyramid->SetInput(it.Get());
			pPyramid->Update();
			fPyramidLevelFactor = 1.0;
			
			itP = pPyramid->GetOutput()->Begin();
			
			++it;
			++nChannel;
			
			INFO_MSG(m_Log, "Image Feature Extractor", "New pyramid level, size: " <<
					pMeanFilter->GetOutput()->GetLargestPossibleRegion().GetSize(),
					LOG_DOMAIN_VAR);
					
			DEBUG(
				std::ostringstream oss("");
				oss << "DEBUG_feature_image_" << m_unNoImageFiles << ".tif";
				saveImage(pMeanFilter->GetOutput(), oss.str());
				++m_unNoImageFiles;
			);
		}
		//--------------------------------
		// Store feature vector component
		//--------------------------------
		std::list<InputImageType::IndexType>::const_iterator ciIndex = InputIndexList.begin();
		while (ciIndex != InputIndexList.end())
		{
			InputImageType::IndexType Index;
			Index[0] = (*ciIndex)[0]*fPyramidLevelFactor;
			Index[1] = (*ciIndex)[1]*fPyramidLevelFactor;
			FeaturePoints[i].push_back(TextureFilter->EvaluateAtIndex(Index));
			FeaturePoints[i].push_back(pMeanFilter->GetOutput()->GetPixel(Index));
			++i;
			++ciIndex;
		}
	}

	////////////////////////////////////////////////////////////////////////////
	// Move feature points to point set for classification.
	////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Method to extract features from image.
///
/// Since it is common for to extract features for every pixel when in
/// classification, ORFEO methods to filter the whole image are used.
///
/// \return Returns if extraction was successfull
///
////////////////////////////////////////////////////////////////////////////////
bool FeatureExtractor::extractWithoutLabels()
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::extractWithoutLabels()");
	
	////////////////////////////////////////////////////////////////////////////
	// Setup common stuff
	////////////////////////////////////////////////////////////////////////////
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
	double fPyramidLevelFactor;
	std::vector<FeatureVectorType> FeaturePoints;
	
	INFO_MSG(m_Log, "Image Feature Extractor", "Calculating features of channel 0",
					LOG_DOMAIN_NONE);

	////////////////////////////////////////////////////////////////////////////
	// First run to fill LabelPoints vector and first dimension and therfore
	// size of FeaturePoints vector
	////////////////////////////////////////////////////////////////////////////
	
	//--------------------------
	// Initialise image pyramid
	//--------------------------
	PyramidFilterType::Pointer pPyramid = PyramidFilterType::New();
	pPyramid->SetNumberOfLevels(m_nNumberOfPyramidLevels);
	pPyramid->SetDecimationRatio(m_fPyramidDecimationRate);
	pPyramid->SetInput(it.Get());
	pPyramid->Update();
	fPyramidLevelFactor = 1.0;
	ImageListIterator itP = pPyramid->GetOutput()->Begin();

	TextureFilter->SetInput(it.Get());
	TextureFilter->Update();
	pMeanFilter->SetInput(it.Get());
	pMeanFilter->Update();
	
	DEBUG(
	{
		#include <fstream>
		#include <sstream>
		std::ostringstream oss("");
		oss << "DEBUG_feature_image_" << m_unNoImageFiles << ".tif";
		saveImage(pMeanFilter->GetOutput(), oss.str());
		++m_unNoImageFiles;
	});
	
	INFO_MSG(m_Log, "Image Feature Extractor", "New pyramid level, size: " <<
					pMeanFilter->GetOutput()->GetLargestPossibleRegion().GetSize(),
					LOG_DOMAIN_VAR);

	//--------------------------------------
	// Store first feature vector component
	//--------------------------------------
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
	++it;
	++nChannel;
	
	////////////////////////////////////////////////////////////////////////////
	// Now do the same for all other channels.
	////////////////////////////////////////////////////////////////////////////
	while (!((it == m_pInputChannelList->End()) && (itP == pPyramid->GetOutput()->End())))
	{
		int i=0;
		
		//-----------------------------------------------
		// Create new filters due to changing image size
		//-----------------------------------------------
		TextureFilter = VarianceFilterType::New();
		TextureFilter->SetRadius(nRadius);
		TextureFilter->SetOffset(nOffset);
		
		pMeanFilter = MeanFilterType::New();
		pMeanFilter->SetRadius(nRadius);
		
		//---------------------------------------------------
		// Calculate on sublevels or start new pyramid level
		//---------------------------------------------------
		if (itP != pPyramid->GetOutput()->End())
		{			
			TextureFilter->SetInput(itP.Get());
			TextureFilter->Update();
			pMeanFilter->SetInput(itP.Get());
			pMeanFilter->Update();
			fPyramidLevelFactor *= 1/m_fPyramidDecimationRate;
			++itP;
			
			INFO_MSG(m_Log, "Image Feature Extractor", "New pyramid level, size: " <<
					pMeanFilter->GetOutput()->GetLargestPossibleRegion().GetSize(),
					LOG_DOMAIN_VAR);
			DEBUG_MSG(m_Log, "Image Feature Extractor", "Pyramid level factor: " <<
					fPyramidLevelFactor, LOG_DOMAIN_VAR);
					
			DEBUG(
				std::ostringstream oss("");
				oss << "DEBUG_feature_image_" << m_unNoImageFiles << ".tif";
				saveImage(pMeanFilter->GetOutput(), oss.str());
				++m_unNoImageFiles;
			);
		}
		else
		{
			INFO_MSG(m_Log, "Image Feature Extractor", "Calculating features of channel " <<
					nChannel, LOG_DOMAIN_NONE);
			TextureFilter->SetInput(it.Get());
			TextureFilter->Update();
			pMeanFilter->SetInput(it.Get());
			pMeanFilter->Update();
			
			pPyramid = PyramidFilterType::New();
			pPyramid->SetNumberOfLevels(m_nNumberOfPyramidLevels);
			pPyramid->SetDecimationRatio(m_fPyramidDecimationRate);
			pPyramid->SetInput(it.Get());
			pPyramid->Update();
			fPyramidLevelFactor = 1.0;
			
			itP = pPyramid->GetOutput()->Begin();
			
			++it;
			++nChannel;
			
			INFO_MSG(m_Log, "Image Feature Extractor", "New pyramid level, size: " <<
					pMeanFilter->GetOutput()->GetLargestPossibleRegion().GetSize(),
					LOG_DOMAIN_VAR);
					
			DEBUG(
				std::ostringstream oss("");
				oss << "DEBUG_feature_image_" << m_unNoImageFiles << ".tif";
				saveImage(pMeanFilter->GetOutput(), oss.str());
				++m_unNoImageFiles;
			);
		}
		//--------------------------------
		// Store feature vector component
		//--------------------------------
		nY = 0;
		while (nY < nSize[1])
		{
			nX = 0;
			nFeatureIndex[1] = double(nY)*fPyramidLevelFactor;
			while (nX < nSize[0])
			{
				nFeatureIndex[0] = double(nX)*fPyramidLevelFactor;
				
				FeaturePoints[i].push_back(TextureFilter->GetOutput()->GetPixel(nFeatureIndex));
				FeaturePoints[i].push_back(pMeanFilter->GetOutput()->GetPixel(nFeatureIndex));
				++i;
				
				++nX;
			}
			++nY;
		}
	}
	
	////////////////////////////////////////////////////////////////////////////
	// Move feature points to point set for classification.
	////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Stores a given 8 bit image to disk
///
/// \param _pImg Pointer to image
/// \param _Filename File to write to
///
////////////////////////////////////////////////////////////////////////////////
void FeatureExtractor::saveImage(const Image8BitType::Pointer _pImg,
								 const std::string& _Filename) const
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::saveImage(const Image8BitType::Pointer, const std::string&");
	
	INFO_MSG(m_Log, "Image Feature Extractor", "Writing image " << _Filename, LOG_DOMAIN_NONE);
	Writer8BitType::Pointer pWriter = Writer8BitType::New();
	pWriter->SetFileName(_Filename);
	pWriter->SetInput(_pImg);
	pWriter->Update();
	
	METHOD_EXIT(m_Log, "FeatureExtractor::saveImage(const Image8BitType::Pointer, const std::string&");
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Stores a given 16 bit image to disk
///
/// \param _pImg Pointer to image
/// \param _Filename File to write to
///
////////////////////////////////////////////////////////////////////////////////
void FeatureExtractor::saveImage(const Image16BitType::Pointer _pImg,
								 const std::string& _Filename) const
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::saveImage(const Image16BitType::Pointer, const std::string&");
	
	INFO_MSG(m_Log, "Image Feature Extractor", "Writing image " << _Filename, LOG_DOMAIN_NONE);
	Writer16BitType::Pointer pWriter = Writer16BitType::New();
	pWriter->SetFileName(_Filename);
	pWriter->SetInput(_pImg);
	pWriter->Update();
	
	METHOD_EXIT(m_Log, "FeatureExtractor::saveImage(const Image16BitType::Pointer, const std::string&");
}