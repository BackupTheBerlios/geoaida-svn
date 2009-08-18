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
/// \mainpage	Project SVM_Operator
/// \author		Torsten Büschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///
/// \file		main.cpp
/// \brief		Main program
///
/// \date		2009-06-29
/// \author		Torsten Büschenfeld (bfeld@tnt.uni-hannover.de)
///
////////////////////////////////////////////////////////////////////////////////

//--- Standard header --------------------------------------------------------//
#include <cstdlib>
#include <string>
#include <dirent.h>

//--- Program header ---------------------------------------------------------//
#include "feature_extractor.h"
#include "log.h"
#include "otb_svmop_common.h"
#include "svm_classifier.h"

//--- Misc header ------------------------------------------------------------//

using namespace std;

//--- Major glocal instances -------------------------------------------------//
std::string			g_strWorkingDir;			///< Specifies working directory
CLog&				Log=CLog::getInstance();	///< Logging instance

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Prints out usage message if command was called incorrectly
///
////////////////////////////////////////////////////////////////////////////////
void usage()
{
	METHOD_ENTRY(Log, "usage()");
	INFO(Log,

	std::cout << "Usage: svmop [PROJECT_DIRECTORY]" << std::endl;
	std::cout << "Starts svm operation using given parameter string as working directory."
				<< std::endl;

	);
	METHOD_EXIT(Log, "usage()");
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Prepares svmop by evaluating working directory
///
/// \return Indicates if setup was successful
///
////////////////////////////////////////////////////////////////////////////////
bool setup(const char* _strDir)
{
	METHOD_ENTRY(Log, "setup()");

	g_strWorkingDir = _strDir;
	
	DIR* pDir;
	pDir = opendir(g_strWorkingDir.c_str());

	if (pDir == 0)
	{
		ERROR_MSG(Log, "main", "Directory " << g_strWorkingDir << " does not exist.", LOG_DOMAIN_NONE);
		METHOD_EXIT(Log, "setup()");
		return false;
	}
	else
	{
		INFO_MSG(Log, "main", "Using working directory " << _strDir, LOG_DOMAIN_NONE);
	}

	METHOD_EXIT(Log, "setup()");
	return true;
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Clean up, free memory of global instances
///
////////////////////////////////////////////////////////////////////////////////
void cleanUp()
{
	METHOD_ENTRY(Log, "cleanUp()");
// 	if (g_pLibSvm != 0)
// 	{
// 		{
// // 			delete g_pLibSvm;
// // 			g_pLibSvm = 0;
// 			MEM_FREED(Log, "g_pLibSvm");
// 		}
// 		if (g_pFeatures != 0)
// 		{
// 			delete g_pFeatures;
// 			g_pFeatures = 0;
// 			MEM_FREED(Log, "g_pFeatures");
// 		}
// 	}
	METHOD_EXIT(Log, "cleanUp()");
}

////////////////////////////////////////////////////////////////////////////////
///
/// \brief Main function
///
/// This is the entrance point for program startup.
///
/// \param	argc number of given arguments
/// \param	argv array, storing the arguments
/// \return	exit code
///
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	METHOD_ENTRY(Log, "main");

	Log.setBreak(120u);

	FeatureExtractor	Extractor;
	SVMClassifier		Classifier;

	if (argc < 4)
	{
		ERROR_MSG(Log, "Main", "Wrong number of parameters.", LOG_DOMAIN_NONE);
		usage();
		METHOD_EXIT(Log, "Main");
		return EXIT_FAILURE;
	}
	else
	{
		std::vector<std::string> ArgvList;

		for (int i=1; i<argc-1; ++i)
		{
			ArgvList.push_back(argv[i]);
			Extractor.addInputChannel(argv[i]);
		}

		Extractor.loadLabelImage(argv[3]);
		Extractor.extract(FEATURE_EXTRACTOR_USE_LABELS);
		Classifier.setFeatures(Extractor.getFeatures());
		Classifier.setLabels(Extractor.getLabels());
		Classifier.setNumberOfClasses(2);
		Classifier.loadModel("svm_model");
		Classifier.train();
		Classifier.saveModel("svm_model");

		Extractor.extract();
		Classifier.loadModel("svm_model");
		Classifier.setFeatures(Extractor.getFeatures());
		Classifier.classify();

// 		if (!setup(argv[1]))
// 		{
// 			METHOD_EXIT(Log, "Main");
// 			return EXIT_FAILURE;
// 		};
	}

	//--- Start the training process -----------------------------------------//

	//--- Reclassification ---------------------------------------------------//

	//--- Start the classification process -----------------------------------//

	//--- Clean up -----------------------------------------------------------//
	cleanUp();

	METHOD_EXIT(Log, "main");
	return EXIT_SUCCESS;
}
