#include "Array2DSubstract.h"

void Usage()
{
  printf("Usage:\n"
	 "  substractpfm <src1> <src2> <destination>\n");
}



int main(int argc, char **argv)
{
  if (argc<4)
    {
      Usage();
      return 0;
    }
  Array2D<float> *src1=new Array2D<float>(argv[1]);
  Array2D<float> *src2=new Array2D<float>(argv[2]);
  Array2D<float> *dst=new Array2DSubstract<float>(*src1,*src2);
  dst->write(argv[3]);
}
