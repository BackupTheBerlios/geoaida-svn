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
/// \file		svm_classifier.cpp
/// \brief		Implementation of interface "SVMClassifier"
///
/// \date		2009-07-29
/// \author		Torsten Büschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

#include "svm_classifier.h"

#include <fstream>

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
///////////////////////////////////////////////////////////////////////////////
SVMClassifier::SVMClassifier() : 	m_unNumberOfClasses(SVM_CLASSIFIER_DEFAULT_NUMBER_OF_CLASSES),
									m_bGotFeatures(false),
									m_bGotLabels(false),
									m_bGotModel(false)
{
	METHOD_ENTRY(m_Log, "SVMClassifier::SVMClassifier()");
	CTOR_CALL(m_Log, "SVMClassifier::SVMClassifier()");

	METHOD_EXIT(m_Log, "SVMClassifier::SVMClassifier()");
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Destructor
///
///////////////////////////////////////////////////////////////////////////////
SVMClassifier::~SVMClassifier()
{
	METHOD_ENTRY(m_Log, "SVMClassifier::~SVMClassifier()");
	DTOR_CALL(m_Log, "SVMClassifier::~SVMClassifier()");

// 	m_pClassifier->Delete();
// 	m_pEstimator->Delete();
// 	m_pFeatures->Delete();
// 	m_pLabels->Delete();
// 	m_pModel->Delete();

	METHOD_EXIT(m_Log, "SVMClassifier::~SVMClassifier()");
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Exports the (training) data to given filename
///
/// \param _strFilename Filename
///
/// \return Returns if exporting was succesful
///
///////////////////////////////////////////////////////////////////////////////
bool SVMClassifier::saveData(const std::string& _strFilename) const
{
	METHOD_ENTRY(m_Log, "SVMClassifier::saveData(const std::string&)");

// 	std::ofstream of(_strFilename.c_str());
// 
// 	int i=0; int j=0;
// 	LabelListType::const_iterator cl=m_pFeatures->getLabelList().begin();
// 	for (FeatureListType::const_iterator ci=m_pFeatures->get().begin(); ci!=m_pFeatures->get().end(); ++ci)
// 	{
// 		int k=0;
// 		of << (*cl) << " ";
// 		for (std::vector<double>::const_iterator cj=(*ci).begin(); cj!=(*ci).end(); ++cj)
// 		{
// 			of << k++ << ":" << (*cj) << " ";
// 		}
// 		of << std::endl;
// 		++i; ++j; ++cl;
// 	}
// 
// 	INFO_MSG(m_Log, "SVM Classifier", "Data exported to " << _strFilename, LOG_DOMAIN_NONE);

	METHOD_EXIT(m_Log, "SVMClassifier::saveData(const std::string&)");
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Save SVM model
///
/// \param _strFilename Filename of model to be saved
///
/// \return Returns if saving was succesfull
///
///////////////////////////////////////////////////////////////////////////////
bool SVMClassifier::saveModel(const std::string& _strFilename) const
{
	METHOD_ENTRY(m_Log, "SVMClassifier::saveModel(const std::string&)");

	if (!m_bGotModel)
	{
		WARNING_MSG(m_Log, "SVM Classifier", "No SVM model, nothing is saved. Train or import model first.", LOG_DOMAIN_NONE);
		METHOD_EXIT(m_Log, "SVMClassifier::exportModel(const std::string&)");
		return false;
	}
	else
	{
		INFO_MSG(m_Log, "SVM Classifier", "Saving SVM model to " << _strFilename << ".", LOG_DOMAIN_NONE);
	}

	m_pEstimator->SaveModel(_strFilename.c_str());

	METHOD_EXIT(m_Log, "SVMClassifier::saveModel(const std::string&)");
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Loads SVM model based on training
///
/// \param _strFilename Filename of model to be loaded
///
/// \return Returns if loading was succesfull
///
///////////////////////////////////////////////////////////////////////////////
bool SVMClassifier::loadModel(const std::string& _strFilename)
{
	METHOD_ENTRY(m_Log, "SVMClassifier::loadModel(const std::string&)");

	INFO_MSG(m_Log, "SVM Classifier", "Loading SVM model " << _strFilename << ".", LOG_DOMAIN_NONE);

	if (m_bGotModel)
	{
		NOTICE_MSG(m_Log, "SVM Classifier", "SVM model already exists. Replacing.", LOG_DOMAIN_NONE);
	}
	m_pModel = ModelType::New();

	m_pModel->LoadModel(_strFilename.c_str());
	m_bGotModel = true;

	METHOD_EXIT(m_Log, "SVMClassifier::importModel(const std::string&)");
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Classifies by given features
///
/// \return Returns if classification was succesfull
///
///////////////////////////////////////////////////////////////////////////////
bool SVMClassifier::classify()
{
	METHOD_ENTRY(m_Log, "SVMClassifier::classify()");

	if (!m_bGotFeatures)
	{
		ERROR_MSG(m_Log, "SVM Classifier", "No features. Get features from " <<
					"Image Feature Extractor or load them. ", LOG_DOMAIN_NONE);
		METHOD_EXIT(m_Log, "SVMClassifier::classify()");
		return false;
	}
	if (m_pFeatures->GetNumberOfPoints() == 0)
	{
		ERROR_MSG(m_Log, "SVM Classifier", "Not enough feature points (0).", LOG_DOMAIN_NONE);
		METHOD_EXIT(m_Log, "SVMClassifier::classify()");
		return false;
	}
	if (m_pModel->GetNumberOfSupportVectors() == 0)
	{
		ERROR_MSG(m_Log, "SVM Classifier", "Not enough support vectors (0).", LOG_DOMAIN_NONE);
		METHOD_EXIT(m_Log, "SVMClassifier::classify()");
		return false;
	}

	INFO_MSG(m_Log, "SVM Classifier", "Starting classification.", LOG_DOMAIN_NONE);

	SampleType::Pointer pSample = SampleType::New();
	pSample->SetPointSet(m_pFeatures);

	int nNumberOfClasses = m_pModel->GetNumberOfClasses();

	m_pClassifier = ClassifierType::New();
	m_pClassifier->SetNumberOfClasses(nNumberOfClasses);
	m_pClassifier->SetModel(m_pModel);
	m_pClassifier->SetSample(pSample.GetPointer());

	m_Log.logSeparator();
	m_pClassifier->Update();
	m_Log.logSeparator();

	INFO_MSG(m_Log, "SVM Classifier", "Classification done.", LOG_DOMAIN_NONE);

	METHOD_EXIT(m_Log, "SVMClassifier::classify()");
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Starts training routines of libSVM, converts features accordingly
///
/// \return Returns if training succeded
///
///////////////////////////////////////////////////////////////////////////////
bool SVMClassifier::train()
{
	METHOD_ENTRY(m_Log, "SVMClassifier::train()");

	if (!m_bGotFeatures)
	{
		ERROR_MSG(m_Log, "SVM Classifier", "No features. Get features from " <<
					"Image Feature Extractor or load them. ", LOG_DOMAIN_NONE);
		METHOD_EXIT(m_Log, "SVMClassifier::train()");
		return false;
	}
	if (!m_bGotLabels)
	{
		ERROR_MSG(m_Log, "SVM Classifier", "No labels. Get labels from " <<
					"Image Feature Extractor or load them. ", LOG_DOMAIN_NONE);
		METHOD_EXIT(m_Log, "SVMClassifier::train()");
		return false;
	}
	if (m_pFeatures->GetNumberOfPoints() == 0)
	{
		ERROR_MSG(m_Log, "SVM Classifier", "Not enough feature points (0).", LOG_DOMAIN_NONE);
		METHOD_EXIT(m_Log, "SVMClassifier::train()");
		return false;
	}
	// Zero label points will also be catched with next condition
	if (m_pFeatures->GetNumberOfPoints() != m_pLabels->GetNumberOfPoints())
	{
		ERROR_MSG(m_Log, "SVM Classifier", "Number of labels and feature points differ.", LOG_DOMAIN_NONE);
		METHOD_EXIT(m_Log, "SVMClassifier::train()");
		return false;
	}

	m_pEstimator = EstimatorType::New();

	m_pEstimator->SetInputPointSet(m_pFeatures);
	m_pEstimator->SetTrainingPointSet(m_pLabels);
	m_pEstimator->SetNumberOfClasses(m_unNumberOfClasses);

	m_Log.logSeparator();
	m_pEstimator->Update();
	m_Log.logSeparator();

	if (m_bGotModel)
	{
		NOTICE_MSG(m_Log, "SVM Classifier", "SVM model already exists. Replacing.", LOG_DOMAIN_NONE);
	}
	
	m_pModel = m_pEstimator->GetModel();
	m_bGotModel = true;

	METHOD_EXIT(m_Log, "SVMClassifier::train()");
	return true;
}
