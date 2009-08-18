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
FeatureExtractor::FeatureExtractor():	m_pWriter(WriterType::New()),
										m_pInputChannelList(ImageListType::New()),
										m_bFeaturesExtracted(false),
										m_bLabelsExtracted(false),
										m_bLabelImageLoaded(false),
										m_bLabelSpacingAndBorderSet(false),
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

	INFO_MSG(m_Log, "Image Feature Extractor", "Number of channels: " << m_pInputChannelList->Size(), LOG_DOMAIN_VAR);

	METHOD_EXIT(m_Log, "FeatureExtractor::addInputChannel(const std::string&)");
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
bool FeatureExtractor::extract(const uchar& _nMode)
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::extract()");

// 	FeaturePointSetType::Pointer pFeatures = FeaturePointSetType::New();
// 	LabelPointSetType::Pointer pLabels = LabelPointSetType::New();
	
	m_pFeatures = FeaturePointSetType::New();
	m_pLabels = LabelPointSetType::New();

	FeaturePointContainer::Pointer	pFeatCont = FeaturePointContainer::New();
	LabelPointContainer::Pointer	pLabCont = LabelPointContainer::New();
	
	FilterType::Pointer			TextureFilter = FilterType::New();
	FilterType::SizeType		nRadius;
	InputImageType::IndexType	nIndex;
	InputImageType::OffsetType	nOffset;
	InputImageType::SizeType	nSize;

	nRadius[0] = 10;
	nRadius[1] = 10;
	nOffset[0] = 7;
	nOffset[1] = 7;
	TextureFilter->SetRadius(nRadius);
	TextureFilter->SetOffset(nOffset);

	switch (_nMode)
	{
		//--- Extraction with labels ---
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
			
			m_pInputChannelList->Update();
			ImageListIterator it = m_pInputChannelList->Begin();
			
			nSize = it.Get()->GetLargestPossibleRegion().GetSize();
			uint nChannel = 0u;
			uint nX;
			uint nY;
			while (it != m_pInputChannelList->End())
			{
				TextureFilter->SetInputImage(it.Get());
				nY = m_nLabelBorderY;
				while (nY < nSize[1]-m_nLabelBorderY+1)
				{
					nX = m_nLabelBorderX;
					nIndex[1] = nY;
					while (nX < nSize[0]-m_nLabelBorderX+1)
					{
						nIndex[0] = nX;
						TextureFilter->EvaluateAtIndex(nIndex);
						std::cout << "Evaluated " << nX << "," << nY << "," << nChannel << std::endl;
						nX += m_nLabelSpacingX;
					}
					nY += m_nLabelSpacingY;
				}
// 				m_pWriter->SetInput(TextureFilter->GetOutput());
// 				ostringstream Filename("");
// 				Filename << "entropy_" <<
// 									TextureFilter->GetRadius()[0] << "-" <<
// 									TextureFilter->GetOffset()[0] << "-" <<
// 									TextureFilter->GetOffset()[1] << "_" <<
// 									i << ".tif";
// 				m_pWriter->SetFileName(Filename.str());
// 				m_pWriter->Update();
				++it;
				++nChannel;
			}
			
			break;
		}
		//--- Extraction without labels ---//
		case FEATURE_EXTRACTOR_DEFAULT:
			break;
	}

// 	this->channels2VectorImage();

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

	for (int i=0; i<1; ++i)
	{
	// Testing
	FeatureVectorType vecFeature;
	LabelType Label;
	vecFeature.push_back(1.0*(i % 4));
	vecFeature.push_back(0.9*(i % 4));
	Label = i % 2;

	FeaturePointType FP;
	LabelPointType LP;

	FP[0] = i;
	LP[0] = i;

	pFeatCont->InsertElement(i, FP);
	m_pFeatures->SetPointData(i, vecFeature);

	pLabCont->InsertElement(i, LP);
	m_pLabels->SetPointData(i, Label);
	}

	m_pFeatures->SetPoints(pFeatCont);
	m_pLabels->SetPoints(pLabCont);

	switch (_nMode)
	{
		case FEATURE_EXTRACTOR_USE_LABELS:
			m_bLabelsExtracted = true;
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
/// \brief Transforms the image list to a vector image for further processing
///
///////////////////////////////////////////////////////////////////////////////
void FeatureExtractor::channels2VectorImage()
{
	METHOD_ENTRY(m_Log, "FeatureExtractor::channels2VectorImage()");

	INFO_MSG(m_Log, "Image Feature Extractor", "Composing image channels.", LOG_DOMAIN_NONE);
	ImageListToVectorImageFilterType::Pointer pC2VI = ImageListToVectorImageFilterType::New();
	
	pC2VI->SetInput(m_pInputChannelList);
	pC2VI->Update();
	m_pImage = pC2VI->GetOutput();

	METHOD_EXIT(m_Log, "FeatureExtractor::channels2VectorImage()");
}