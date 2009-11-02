#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

typedef float RealType;

enum ChannelMappingMode
{
	CM_OneChannelMode,
	CM_ThreeChannelMode
};

enum ColorDepth
{
	CD_UNSUPPORTED,
	CD_INTEGER_8BIT,
	CD_INTEGER_16BIT,
	CD_FLOAT_32BIT
};

#endif
