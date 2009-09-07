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

DEBUG(
	uint SVMClassifier::m_unNoFVFiles = 0;
);

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
///////////////////////////////////////////////////////////////////////////////
SVMClassifier::SVMClassifier() : 	m_unNumberOfClasses(SVM_CLASSIFIER_DEFAULT_NUMBER_OF_CLASSES),
									m_bGotClassificationResult(false),
									m_bGotFeatures(false),
									m_bGotLabels(false),
									m_bGotModel(false),
									m_bGotSize(false)
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

	METHOD_EXIT(m_Log, "SVMClassifier::~SVMClassifier()");
}

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Saves the label image that resulted from classification
///
/// \param _strFilename Filename
///
/// \return Returns if saving was succesful
///
///////////////////////////////////////////////////////////////////////////////
bool SVMClassifier::saveClassificationResult(const std::string& _strFilename) const
{
	METHOD_ENTRY(m_Log, "SVMClassifier::saveClassificationResult(const std::string&)");

	if (!m_bGotClassificationResult)
	{
		WARNING_MSG(m_Log, "SVM Classifier", "No classification done, cannot save result.", LOG_DOMAIN_NONE);
		METHOD_EXIT(m_Log, "SVMClassifier::saveClassificationResult(const std::string&)");
		return false;
	}

	LabelWriterType::Pointer pWriter = LabelWriterType::New();
	pWriter->SetFileName(_strFilename);
	pWriter->SetInput(m_pLabelImage);
	pWriter->Update();
	INFO_MSG(m_Log, "SVM Classifier", "Classification result stored in " << _strFilename, LOG_DOMAIN_NONE);

	METHOD_EXIT(m_Log, "SVMClassifier::saveClassificationResult(const std::string&)");
	return true;
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
/// \brief Scales features to given interval
///
/// \param _fLower Lower interval border
/// \param _fUpper Upper interval border
///
/// \return Returns if scaling was successful
///
///////////////////////////////////////////////////////////////////////////////
bool SVMClassifier::scaleFeatures(const double& _fLower, const double& _fUpper)
{
	METHOD_ENTRY(m_Log, "SVMClassifier::scaleFeatures(const double&, const double&)");

	//--------------------------------------------------------------------------
	// Check if everthings setup correctly
	//--------------------------------------------------------------------------
	if (!m_bGotFeatures)
	{
		ERROR_MSG(m_Log, "SVM Classifier", "No features, can't scale. Get features from " <<
					"Image Feature Extractor or load them. ", LOG_DOMAIN_NONE);
		METHOD_EXIT(m_Log, "SVMClassifier::scaleFeatures(const double&, const double&)");
		return false;
	}
	
	DEBUG_MSG(m_Log, "SVM Classifier", "Determining extrema in feature vector.", LOG_DOMAIN_NONE);
	
	double fMin = 1e300;
	double fMax = 1e-300;
	
	DEBUG(
		#include <fstream>
		#include <sstream>
		std::ostringstream oss("");
		oss << "DEBUG_feature_vectors_" << m_unNoFVFiles << ".txt";
		std::ofstream of(oss.str().c_str());
		++m_unNoFVFiles;
	);
	
	FeaturePointDataContainer::Iterator it = m_pFeatures->GetPointData()->Begin();
	FeatureVectorType::const_iterator ci;
	while (it != m_pFeatures->GetPointData()->End())
	{
 		ci = (it.Value()).begin();
		while (ci != (it.Value()).end())
		{
			if ((*ci) < fMin) fMin = (*ci);
			if ((*ci) > fMax) fMax = (*ci);
			
			DEBUG(of << (*ci) << " ";);
			
			++ci;
		}
		DEBUG(of << std::endl;);
		++it;
	}
	DEBUG(of.close(););
	DEBUG_MSG(m_Log, "SVM Classifier", "Feature vectors written to: " << oss.str(), LOG_DOMAIN_NONE);
	DEBUG_MSG(m_Log, "SVM Classifier", "Min value: " << fMin, LOG_DOMAIN_VAR);
	DEBUG_MSG(m_Log, "SVM Classifier", "Max value: " << fMax, LOG_DOMAIN_VAR);
	
	INFO_MSG(m_Log, "SVM Classifier", "Scaling features to interval [" << _fLower << "," << _fUpper << "]", LOG_DOMAIN_NONE);
	
	FeatureVectorType::iterator jt;
	it = m_pFeatures->GetPointData()->Begin();
	while (it != m_pFeatures->GetPointData()->End())
	{
 		jt = (it.Value()).begin();
		while (jt != (it.Value()).end())
		{
			(*jt) = _fLower + ((*jt) - fMin) * (_fUpper - _fLower)/(fMax-fMin);
			++jt;
		}
		++it;
	}

	METHOD_EXIT(m_Log, "SVMClassifier::scaleFeatures(const double&, const double&)");
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

	//--------------------------------------------------------------------------
	// Check if everthings setup correctly
	//--------------------------------------------------------------------------
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
	if (!m_bGotSize)
	{
		WARNING_MSG(m_Log, "SVM Classifier", "No label image size specified. Resulting label image will be empty.", LOG_DOMAIN_NONE);
		METHOD_EXIT(m_Log, "SVMClassifier::train()");
		return false;
	}
	if (m_bGotClassificationResult)
	{
		NOTICE_MSG(m_Log, "SVM Classifier", "Already got classification result, overwriting.", LOG_DOMAIN_NONE);
	}

	INFO_MSG(m_Log, "SVM Classifier", "Starting classification.", LOG_DOMAIN_NONE);

	//--------------------------------------------------------------------------
	// Start the classification
	//--------------------------------------------------------------------------
	SampleType::Pointer pSample = SampleType::New();
	pSample->SetPointSet(convertPointSet(m_pFeatures));
	
	int nNumberOfClasses = m_pModel->GetNumberOfClasses();
	
	DEBUG_MSG(m_Log, "SVM Classifier", "Number of features to be classified: " << pSample->Size(), LOG_DOMAIN_VAR);
	DEBUG_MSG(m_Log, "SVM Classifier", "Number of classes: " << nNumberOfClasses, LOG_DOMAIN_VAR);

	m_pClassifier = ClassifierType::New();
	m_pClassifier->SetNumberOfClasses(nNumberOfClasses);
	m_pClassifier->SetModel(m_pModel);
	m_pClassifier->SetSample(pSample.GetPointer());

	m_Log.logSeparator();
	m_pClassifier->Update();
	m_Log.logSeparator();
	
	ClassifierType::OutputType::Pointer pClassificationResult =	m_pClassifier->GetOutput();
	
	//--------------------------------------------------------------------------
	// Create a new image for the resulting labels
	//--------------------------------------------------------------------------
	m_pLabelImage = LabelImageType::New();
	LabelImageType::IndexType StartIndex;
	LabelImageType::SizeType Size;
	LabelImageType::RegionType Region;
	
	StartIndex[0]=0;
	StartIndex[1]=0;
	Region.SetIndex(StartIndex);
	Region.SetSize(m_LabelImageSize);
	m_pLabelImage->SetRegions(Region);
	m_pLabelImage->Allocate();
	
	ClassifierType::OutputType::ConstIterator ciIter = pClassificationResult->Begin();
	ClassifierType::OutputType::ConstIterator ciLast = pClassificationResult->End();
	typedef itk::ImageRegionIterator<LabelImageType> OutputIteratorType;
	OutputIteratorType itOut(m_pLabelImage, m_pLabelImage->GetLargestPossibleRegion());
	itOut.GoToBegin();
	while ((ciIter != ciLast) && (!itOut.IsAtEnd()))
	{
		std::cout << ciIter.GetClassLabel();
		itOut.Set(ciIter.GetClassLabel());
		++ciIter;
		++itOut;
	}
	
	INFO_MSG(m_Log, "SVM Classifier", "Classification done.", LOG_DOMAIN_NONE);

	m_bGotClassificationResult = true;
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

	//--------------------------------------------------------------------------
	// Check if everthings setup correctly
	//--------------------------------------------------------------------------
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
	// Zero label points will also be catched with the following condition
	if (m_pFeatures->GetNumberOfPoints() != m_pLabels->GetNumberOfPoints())
	{
		ERROR_MSG(m_Log, "SVM Classifier", "Number of labels and feature points differ.", LOG_DOMAIN_NONE);
		METHOD_EXIT(m_Log, "SVMClassifier::train()");
		return false;
	}
	
	//--------------------------------------------------------------------------
	// Start training process
	//--------------------------------------------------------------------------
	m_pEstimator = EstimatorType::New();

	m_pEstimator->SetInputPointSet(m_pFeatures);
	m_pEstimator->SetTrainingPointSet(m_pLabels);
	m_pEstimator->SetNumberOfClasses(m_unNumberOfClasses);
	m_pEstimator->SetSVMType(C_SVC);
	m_pEstimator->SetKernelType(RBF);
	m_pEstimator->SetKernelGamma(0.0);
	m_pEstimator->SetKernelCoef0(0.0);
	m_pEstimator->SetPolynomialKernelDegree(3);
	m_pEstimator->SetNu(0.5);
	m_pEstimator->SetCacheSize(100);
	m_pEstimator->SetEpsilon(1.0e-3);
	m_pEstimator->SetC(1.0e1);
	m_pEstimator->SetP(0.1);
	m_pEstimator->DoShrinking(true);
	m_pEstimator->DoProbabilityEstimates(true);

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

///////////////////////////////////////////////////////////////////////////////
///
/// \brief Converts a point set to make it compatible to classification
///
/// \return Returns converted point set
///
///////////////////////////////////////////////////////////////////////////////
TestFeaturePointSetType::Pointer SVMClassifier::convertPointSet(
											const FeaturePointSetType::Pointer _pPS)
{
	METHOD_ENTRY(m_Log, "SVMClassifier::convertPointSet(const FeaturePointSetType::Pointer)");
	
	TestFeaturePointSetType::Pointer	pPS = TestFeaturePointSetType::New();
	TestFeaturePointContainer::Pointer	pFC = TestFeaturePointContainer::New();
	
	TestFeaturePointType FP;
	
	FeaturePointDataContainer::Iterator it = _pPS->GetPointData()->Begin();
	FeatureVectorType::const_iterator ci;
	int i=0;
	while (it != _pPS->GetPointData()->End())
	{
		int j=0;
 		ci = (it.Value()).begin();
		while (ci != (it.Value()).end())
		{
			FP[j] = (*ci);
			
			++j;
			++ci;
		}
		pFC->InsertElement(i, FP);
		pPS->SetPointData(i,(it.Value()));
		++i;
		++it;
	}
	DEBUG_MSG(m_Log, "SVM Classifier", "Point set converted to compatible format for classification.", LOG_DOMAIN_NONE);
// 	std::vector<FeatureVectorType>::iterator jt;
// 	int i=0;
// 	for (ci = FeaturePoints.begin(); ci != FeaturePoints.end(); ++ci)
// 	{
// 		
// 		FeaturePointType FP;
// 
// 		FP[0] = -i;
// 
// 		
// 		m_pFeatures->SetPointData(i, (*ci));
// 
// 		++i;
// 	}

	pPS->SetPoints(pFC);
	
	METHOD_EXIT(m_Log, "SVMClassifier::convertPointSet(const FeaturePointSetType::Pointer)");
	return pPS;
}
