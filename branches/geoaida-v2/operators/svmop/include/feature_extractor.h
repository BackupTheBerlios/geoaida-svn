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
/// \file		feature_extractor.h
/// \brief		Prototype of class "FeatureExtractor"
///
/// \date		2009-07-13
/// \author		Torsten Bueschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#ifndef FEATURE_EXTRACTOR_H
#define FEATURE_EXTRACTOR_H

//--- Standard header --------------------------------------------------------//
#include "list"
#include "string"
#include "vector"

//--- Program header ---------------------------------------------------------//
#include "log.h"
#include "otb_svmop_common.h"

//--- Misc header ------------------------------------------------------------//

const uchar FEATURE_EXTRACTOR_DEFAULT = 0;
const uchar FEATURE_EXTRACTOR_USE_LABELS = 10;

const int FEATURE_EXTRACTOR_DEFAULT_SPACING_AND_BORDER = 5; ///< Default value that fits 5x5 filters

///< \todo Normalize features.
///< \todo Extract features according to borders and spacing

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Class for extraction of features from images
/// 
////////////////////////////////////////////////////////////////////////////////
class FeatureExtractor : virtual public CLogBase
{

	public:

		//--- Constructor ----------------------------------------------------//
		FeatureExtractor();

		//--- Const methods --------------------------------------------------//
		const FeaturePointSetType::Pointer	getFeatures() const;
		const LabelPointSetType::Pointer	getLabels() const;

		//--- Methods --------------------------------------------------------//
		void addInputChannel(const std::string&);
		bool extract(const uchar& = FEATURE_EXTRACTOR_DEFAULT);
		void loadLabelImage(const std::string&);
		void setLabelSpacingAndBorder(const int&, const int&, const int&, const int&);

	private:

		WriterType::Pointer				m_pWriter;						///< Pointer to writer pipe
		ImageListType::Pointer			m_pInputChannelList;			///< Contains a list of input channel images
		VectorImageType::Pointer		m_pImage;						///< Contains the composed image
		LabelImageType::Pointer			m_pLabelImage;					///< Contains the label image

		FeaturePointSetType::Pointer	m_pFeatures;					///< Contains the extracted features
		LabelPointSetType::Pointer		m_pLabels;						///< Contains the extracted features

		bool							m_bFeaturesExtracted;			///< Flags if features were extracted
		bool							m_bLabelsExtracted;				///< Flags if labels were extracted
		bool							m_bLabelImageLoaded;			///< Flags if label image was loaded
		bool							m_bLabelSpacingAndBorderSet;	///< Flags if label image settings are configured

		int								m_nLabelSpacingX;				///< Distance between samples for training
		int								m_nLabelSpacingY;				///< Distance between samples for training
		int								m_nLabelBorderX;				///< Distance of border when training
		int								m_nLabelBorderY;				///< Distance of border when training

		//--- Methods [private] ----------------------------------------------//
		void channels2VectorImage();

};

#endif
