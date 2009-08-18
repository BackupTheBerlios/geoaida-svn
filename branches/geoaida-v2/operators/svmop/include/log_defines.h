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
/// \file		log_defines.h
/// \brief		Manages some macros for logging class "CLog"
///
/// \date		2009-06-29
/// \author		Torsten Büschenfeld (bfeld@tnt.uni-hannover.de)
///
/// \todo All macros wrapping logSeperator need LogSeverity-parameter
///
////////////////////////////////////////////////////////////////////////////////

#ifndef LOG_DEFINES_H
#define LOG_DEFINES_H

#include "conf_log.h"

////////////////////////////////////////////////////////////////////////////////
///
/// \def DEBUG_MSG(a,b,c,d)
///			Macro wrapping debug output of logging class. One may use streams
///			for message c, for example:
///			DEBUG_MSG(m_Log, "Source", "Message" << "using" << "Streams", LOG_DOMAIN_NONE);
/// \def INFO_MSG(a,b,c,d)
///			Macro wrapping info output of logging class
/// \def NOTICE_MSG(a,b,c,d)
///			Macro wrapping notice output of logging class
/// \def WARNING_MSG(a,b,c,d)
///			Macro wrapping warning output of logging class
/// \def ERROR_MSG(a,b,c,d)
///			Macro wrapping error output of logging class
/// \def DEBUG(a,b)
///			Macro wrapping arbitary debug code
/// \def INFO(a,b)
///			Macro wrapping arbitary info code
/// \def NOTICE(a,b)
///			Macro wrapping arbitary notice code
/// \def WARNING(a,b)
///			Macro wrapping arbitary warning code
/// \def ERROR(a,b)
///			Macro wrapping arbitary error code
/// \def CTOR_CALL(a,b)
///			Macro simplifying log of domain: constructor call
/// \def DTOR_CALL(a,b)
///			Macro simplifying log of domain: destructor call
/// \def METHOD_ENTRY(a,b)
///			Macro simplifying log of domain: method entry
/// \def METHOD_EXIT(a,b)
///			Macro simplifying log of domain: method exit
/// \def MEM_ALLOC(a,b)
///			Macro simplifying log of domain: memory allocated
/// \def MEM_FREED(a,b)
///			Macro simplifying log of domain: memory freed
///
/// \def DOMAIN_MEMORY
///			Special define flag, indicating that "memory alloc" and "mem freed"
///			domains are both active.
/// \def DOMAIN_METHOD_HIERARCHY
///			Special define flag, indicating that "method entry" and "method exit"
///			domains are both active.
///
////////////////////////////////////////////////////////////////////////////////
#ifdef LOGLEVEL_DEBUG
	#define DEBUG_MSG(a,b,c,d)		CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_DEBUG, d)
	#define DEBUG(a,b)				a.logSeparator(LOG_LEVEL_DEBUG); \
									b \
									a.logSeparator(LOG_LEVEL_DEBUG)
	#define INFO_MSG(a,b,c,d)		CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_INFO, d)
	#define INFO(a,b)				a.logSeparator(LOG_LEVEL_INFO); \
									b \
									a.logSeparator(LOG_LEVEL_INFO)
	#define NOTICE_MSG(a,b,c,d)		CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_NOTICE, d)
	#define NOTICE(a,b)				a.logSeparator(LOG_LEVEL_NOTICE); \
									b \
									a.logSeparator(LOG_LEVEL_NOTICE)
	#define WARNING_MSG(a,b,c,d)	CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_WARNING, d)
	#define WARNING(a,b)			a.logSeparator(LOG_LEVEL_WARNING); \
									b \
									a.logSeparator(LOG_LEVEL_WARNING)
	#define ERROR_MSG(a,b,c,d)		CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_ERROR, d)
	#define ERROR(a,b)				a.logSeparator(LOG_LEVEL_ERROR); \
									b \
									a.logSeparator(LOG_LEVEL_ERROR)
	#define CTOR_CALL(a,b)			a.log("Constructor called", b, LOG_LEVEL_DEBUG, LOG_DOMAIN_CONSTRUCTOR)
	#define DTOR_CALL(a,b)			a.log("Destructor called", b, LOG_LEVEL_DEBUG, LOG_DOMAIN_DESTRUCTOR)
	#define METHOD_ENTRY(a,b)		a.log("Method entry", b, LOG_LEVEL_DEBUG, LOG_DOMAIN_METHOD_ENTRY)
	#define METHOD_EXIT(a,b)		a.log("Method exit", b, LOG_LEVEL_DEBUG, LOG_DOMAIN_METHOD_EXIT)
	#define MEM_ALLOC(a,b)			a.log("Memory allocated", b, LOG_LEVEL_DEBUG, LOG_DOMAIN_MEMORY_ALLOCATED)
	#define MEM_FREED(a,b)			a.log("Memory freed", b, LOG_LEVEL_DEBUG, LOG_DOMAIN_MEMORY_FREED)
#endif

#ifdef LOGLEVEL_INFO
	#define DEBUG_MSG(a,b,c,d)
	#define DEBUG(a,b)
	#define INFO_MSG(a,b,c,d)		CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_INFO, d)
	#define INFO(a,b)				a.logSeparator(); \
									b \
									a.logSeparator()
	#define NOTICE_MSG(a,b,c,d)		CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_NOTICE, d)
	#define NOTICE(a,b)				a.logSeparator(); \
									b \
									a.logSeparator()
	#define WARNING_MSG(a,b,c,d)	CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_WARNING, d)
	#define WARNING(a,b)			a.logSeparator(); \
									b \
									a.logSeparator()
	#define ERROR_MSG(a,b,c,d)		CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_ERROR, d)
	#define ERROR(a,b)				a.logSeparator(); \
									b \
									a.logSeparator()

	#define CTOR_CALL(a,b)
	#define DTOR_CALL(a,b)
	#define METHOD_ENTRY(a,b)
	#define METHOD_EXIT(a,b)
	#define MEM_ALLOC(a,b)
	#define MEM_FREED(a,b)
#endif

#ifdef LOGLEVEL_NOTICE
	#define DEBUG_MSG(a,b,c,d)
	#define DEBUG(a,b)
	#define INFO_MSG(a,b,c,d)
	#define INFO(a,b)
	#define NOTICE_MSG(a,b,c,d)		CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_NOTICE, d)
	#define NOTICE(a,b)				a.logSeparator(); \
									b \
									a.logSeparator()
	#define WARNING_MSG(a,b,c,d)	CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_WARNING, d)
	#define WARNING(a,b)			a.logSeparator(); \
									b \
									a.logSeparator()
	#define ERROR_MSG(a,b,c,d)		CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_ERROR, d)
	#define ERROR(a,b)				a.logSeparator(); \
									b \
									a.logSeparator()

	#define CTOR_CALL(a,b)
	#define DTOR_CALL(a,b)
	#define METHOD_ENTRY(a,b)
	#define METHOD_EXIT(a,b)
	#define MEM_ALLOC(a,b)
	#define MEM_FREED(a,b)
#endif

#ifdef LOGLEVEL_WARNING
	#define DEBUG_MSG(a,b,c,d)
	#define DEBUG(a,b)
	#define INFO_MSG(a,b,c,d)
	#define INFO(a,b)
	#define NOTICE_MSG(a,b,c,d)
	#define NOTICE(a,b)
	#define WARNING_MSG(a,b,c,d)	CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_WARNING, d)
	#define WARNING(a,b)			a.logSeparator(); \
									b \
									a.logSeparator()
	#define ERROR_MSG(a,b,c,d)		CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_ERROR, d)
	#define ERROR(a,b)				a.logSeparator(); \
									b \
									a.logSeparator()

	#define CTOR_CALL(a,b)
	#define DTOR_CALL(a,b)
	#define METHOD_ENTRY(a,b)
	#define METHOD_EXIT(a,b)
	#define MEM_ALLOC(a,b)
	#define MEM_FREED(a,b)
#endif

#ifdef LOGLEVEL_ERROR
	#define DEBUG_MSG(a,b,c,d)
	#define DEBUG(a,b)
	#define INFO_MSG(a,b,c,d)
	#define INFO(a,b)
	#define NOTICE_MSG(a,b,c,d)
	#define NOTICE(a,b)
	#define WARNING_MSG(a,b,c,d)
	#define WARNING(a,b)
	#define ERROR_MSG(a,b,c,d)		CLog::s_strStr.str(""); \
									CLog::s_strStr << c; \
									a.log(b, CLog::s_strStr.str(), LOG_LEVEL_ERROR, d)
	#define ERROR(a,b)				a.logSeparator(); \
									b \
									a.logSeparator()

	#define CTOR_CALL(a,b)
	#define DTOR_CALL(a,b)
	#define METHOD_ENTRY(a,b)
	#define METHOD_EXIT(a,b)
	#define MEM_ALLOC(a,b)
	#define MEM_FREED(a,b)
#endif

#ifdef LOGLEVEL_NONE
	#define DEBUG_MSG(a,b,c,d)
	#define DEBUG(a,b)
	#define INFO_MSG(a,b,c,d)
	#define INFO(a,b)
	#define NOTICE_MSG(a,b,c,d)
	#define NOTICE(a,b)
	#define WARNING_MSG(a,b,c,d)
	#define WARNING(a,b)
	#define ERROR_MSG(a,b,c,d)
	#define ERROR(a,b)

	#define CTOR_CALL(a,b)
	#define DTOR_CALL(a,b)
	#define METHOD_ENTRY(a,b)
	#define METHOD_EXIT(a,b)
	#define MEM_ALLOC(a,b)
	#define MEM_FREED(a,b)
#endif

// Be sure not to compile certain domains, although loglevel debug may be active
#ifndef DOMAIN_CONSTRUCTOR
	#undef CTOR_CALL
	#define CTOR_CALL(a,b)
#endif
#ifndef DOMAIN_DESTRUCTOR
	#undef DTOR_CALL
	#define DTOR_CALL(a,b)
#endif
#ifndef DOMAIN_METHOD_ENTRY
	#undef METHOD_ENTRY
	#define METHOD_ENTRY(a,b)
#endif
#ifndef DOMAIN_METHOD_EXIT
	#undef METHOD_EXIT
	#define METHOD_EXIT(a,b)
#endif
#ifndef DOMAIN_MEMORY_ALLOCATED
	#undef MEM_ALLOC
	#define MEM_ALLOC(a,b)
#endif
#ifndef DOMAIN_MEMORY_FREED
	#undef MEM_FREED
	#define MEM_FREED(a,b)
#endif

// Additional macro for memory management
#ifdef DOMAIN_MEMORY_ALLOCATED
	#define DOMAIN_MEMORY
#endif
#ifndef DOMAIN_MEMORY_FREED
	#undef DOMAIN_MEMORY
#endif

// Additional macro for method hierarchy
#ifdef DOMAIN_METHOD_ENTRY
	#define DOMAIN_METHOD_HIERARCHY
#endif
#ifndef DOMAIN_METHOD_EXIT
	#undef DOMAIN_METHOD_HIERARCHY
#endif

#endif
