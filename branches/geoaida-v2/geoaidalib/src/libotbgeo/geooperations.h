#ifndef GEOOPERATIONS_INCLUDE
#define GEOOPERATIONS_INCLUDE

#include <inttypes.h>
#include <otbImage.h>  
#include <otbRemoteSensingRegion.h>
#include <otbImageList.h>

#include <otbResampleImageFilter.h>
//#include <itkAffineTransform.h>
#include <itkIdentityTransform.h>
#include <otbBSplineInterpolateImageFunction.h>

namespace otbgeo {
      
  /**
   * Describes size and location of a geographic area.
   */
  typedef otb::RemoteSensingRegion<double> GeoRegion;
  
  /**
   * Determine common GeoRegion
   */
  GeoRegion cropGeoRegion(const GeoRegion& region1, const GeoRegion&  region2);

  /**
   *  Reads geo-coordinates from image
   *
   * @param image Image to read geo-coordinates from
   * @return GeoRegion containing geo-coordinates from image
   */
  template <class TImageType>  GeoRegion createGeoRegion(const typename TImageType::Pointer image){

    typedef TImageType ImageType;
    const typename ImageType::PointType origin = image->GetOrigin();
    const typename ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();
    const typename ImageType::SpacingType spacing = image->GetSpacing();
    
    
    GeoRegion region;
    GeoRegion::SizeType region_size;
    GeoRegion::IndexType region_index;
    
#ifdef DEBUG
    {
      const typename ImageType::IndexType index = image->GetLargestPossibleRegion().GetIndex();
      std::clog << "###################" << std::endl;
      std::clog << "CreateGeoRegion" << std::endl;
      std::clog << "Image Origin: " << origin[0] << ", " << origin[1] << std::endl;
      std::clog << "Image Region Origin: " << index[0] << ", " << index[1] << std::endl;
      std::clog << "Image Region Size: " << size[0] << ", " << size[1] << std::endl;
      std::clog << "Image Region Spacing: " << spacing[0] << ", " << spacing[1] << std::endl;
      std::clog << "###################" << std::endl;
    }
#endif
    
    region_size[0]  = size[0] * abs(spacing[0]);
    region_size[1]  = size[1] * abs(spacing[1]);
    region_index[0] = origin[0] - (spacing[0]<0 ? region_size[0] : 0);
    region_index[1] = origin[1] - (spacing[1]<0 ? region_size[1] : 0);;
    region.SetSize(region_size);
    region.SetOrigin(region_index);
    
    return region;
  };
  
  /**
   * Determines Image Region that lies within the given geoRegion.  
   * 
   * @param geoRegion GeoRegion
   * @param image Image
   * @return Image Region that lies within geoRegion.
   */
  template <class TImageType> typename TImageType::RegionType geocutImage(const GeoRegion& geoRegion, const typename TImageType::Pointer image) {
    typedef TImageType ImageType;
    
    GeoRegion::PointType geoOrigin = geoRegion.GetOrigin();
    typename ImageType::PointType imageOrigin = image->GetOrigin();
    GeoRegion::SizeType geoSize = geoRegion.GetSize();
    typename ImageType::SizeType imageSize = image->GetLargestPossibleRegion().GetSize();
    typename ImageType::SpacingType imageSpacing = image->GetSpacing();
    
    assert((imageSpacing[0] != 0)&&(imageSpacing[1] != 0));
    
    bool equal = ((geoOrigin[0] == imageOrigin[0]) && (geoOrigin[1] == imageOrigin[1])
		  &&
		  (geoSize[0] == imageSize[0]) && (geoSize[1] == imageSize[1]));
    
#ifdef DEBUG
    {
      std::clog << "###################" << std::endl;
      std::clog << "GeocutImage" << std::endl;
      std::clog << "Image Origin" <<  ": " << imageOrigin[0] << ", " << imageOrigin[1] << std::endl;
      std::clog << "Image Size" << ": " << imageSize[0] << ", " << imageSize[1] << std::endl;
      std::clog << "TargetRegion Origin" <<  ": " << geoOrigin[0] << ", " << geoOrigin[1] << std::endl;
      std::clog << "TargetRegion Size" << ": " << geoSize[0] << ", " << geoSize[1] << std::endl;
      if (equal)
	std::clog << "Result: Everything is fine" << std::endl;
      else
	std::clog << "Result: Cut Region" << std::endl;
      std::clog << "--" << std::endl;
    }
#endif
    
    
    typename ImageType::RegionType imageRegion;
    typename ImageType::RegionType::IndexType imageRegionIndex;  
    typename  ImageType::RegionType::SizeType imageRegionSize;
    {
      
      double s0 = abs(geoSize[0]/imageSpacing[0]);
      double s1 = abs(geoSize[1]/imageSpacing[1]);
      imageRegionSize[0]=s0;
      imageRegionSize[1]=s1;
      
      double i0  = abs((geoOrigin[0] - imageOrigin[0])/imageSpacing[0]);
      double i1 = abs((geoOrigin[1] - imageOrigin[1])/imageSpacing[1]) - s1 ;
      imageRegionIndex[0] = i0;
      imageRegionIndex[1] = i1;
      
      
#ifdef DEBUG
      {
	typename ImageType::IndexType mrIndex = image->GetLargestPossibleRegion().GetIndex();
	typename ImageType::SizeType mrSize = image->GetLargestPossibleRegion().GetSize();
	std::clog << "Image Maximum Region Index" << ": " << mrIndex[0] << ", " << mrIndex[1] << std::endl;
	std::clog << "Image Maximum Region Size" << ": " << mrSize[0] << ", " << mrSize[1] << std::endl;
	
	std::clog << "Calculated Index" <<  ": " << i0 << ", " << i1 << std::endl;
	std::clog << "Calculated Size" <<  ": " << s0 << ", " << s1 << std::endl;
	
	std::clog << "RequestedRegion Index" <<  ": " << imageRegionIndex[0] << ", " << imageRegionIndex[1] << std::endl;
	std::clog << "RequestedRegion Size" << ": " << imageRegionSize[0] << ", " << imageRegionSize[1] << std::endl;
	std::clog << "Image Spacing" << ": " << imageSpacing[0] << ", " << imageSpacing[1] << std::endl;
	std::clog << "###################" << std::endl;
      }
#endif
    }
    imageRegion.SetIndex(imageRegionIndex);
    imageRegion.SetSize(imageRegionSize);
    return imageRegion;
  }

  /**
   * Extract image data that lies within the given geoRegion using resampling.
   *
   * @param geoRegion GeoRegion
   * @param image Image
   * @param spacing Spacing of the output image
   * @return Image that contains the image data.
   */
  template <class TLabel> typename otb::Image<TLabel, 2>::RegionType geocutImage(const GeoRegion& geoRegion, const typename otb::Image<TLabel, 2>::Pointer image, double spacing) {
    typedef otb::Image<TLabel> ImageType;
    
    // Calculate output spacing and origin and update the georegion
    ImageType::SpacingType outputImageSpacing;
    outputImageSpacing[0] = spacing * (geoRegion.GetSpacing()[0] < 0 ? -1 : 1);
    outputImageSpacing[1] = spacing * (geoRegion.GetSpacing()[1] < 0 ? -1 : 1);

    ImageType::SizeType outputImageSize;
    outputImageSize[0] = static_cast<long>(geoRegion.GetSize()[0] / spacing);
    outputImageSize[1] = static_cast<long>(geoRegion.GetSize()[1] / spacing);
    
    ImageType::PointType outputImageOrigin;
    outputImageOrigin[0] = geoRegion.GetOrigin()[0];
    outputImageOrigin[1] = geoRegion.GetOrigin()[1];
        
    // Create and execute resample filter
    typedef otb::StreamingResampleImageFilter<ImageType, ImageType> ResamplerType;
    typedef itk::IdentityTransform<double, 2> TransformType;
    typedef otb::BSplineInterpolateImageFunction<ImageType> InterpolatorType;

    typename TransformType::Pointer transform = TransformType::New();
//     typename TransformType::InputPointType transform_center;
//     typename TransformType::OutputVectorType transform_translation;
//     typename TransformType::MatrixType transform_matrix;
//     
//     transform->SetCenter(transform_center);
//     transform->SetTranslation(transform_translation);
//     transform->SetMatrix(transform_matrix);

    typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
    interpolator->SetSplineOrder(2);
    
    typename ResamplerType::Pointer resampler = ResamplerType::New();
    resampler->SetInput(image);
    resampler->SetOutputOrigin(outputOrigin);
    resampler->SetOutputSpacing(outputSpacing);
    resampler->SetSize(outputSize);
    resampler->SetTransform(transform);
    resampler->SetDefaultPixelValue(0);
    resampler->SetInterpolator(interpolator);
    resampler->Update();

    return resampler->GetOutput();
  }

  /**
   * Determines GeoRegion that is common for all given images.
   * @param images List of images to use.
   * @return GeoRegion that is common for all images in images.
   */
  template <class TImageType> GeoRegion findCommonRegion(typename otb::ImageList<TImageType>::Pointer images){
      typedef TImageType ImageType;
      typedef otb::ImageList<ImageType> ImageList;
#ifdef DEBUG
      {
	std::clog << "####################" << std::endl;
	std::clog << "findCommonRegion" << std::endl;
      }
#endif
      int i=0;      
      typename ImageList::ConstIterator iter = images->Begin();
      typename ImageType::Pointer image = iter.Get();
      GeoRegion region = createGeoRegion<TImageType>(image);
      GeoRegion::IndexType index = region.GetOrigin();
      for (++iter; iter != images->End();++iter){    
	typename ImageType::Pointer image = iter.Get();
	GeoRegion currentRegion = createGeoRegion<TImageType>(image);
	
#ifdef DEBUG
	{      
	  GeoRegion::IndexType index = region.GetOrigin();
	  GeoRegion::SizeType size = region.GetSize();
	  GeoRegion::IndexType currentIndex = currentRegion.GetOrigin();
	  GeoRegion::SizeType currentSize = currentRegion.GetSize();
	  
	  std::clog << "Region Check:" << std::endl;
	  std::clog << "Region Index Until Now: " << index[0] << ", " << index[1] << std::endl;
	  std::clog << "Region Size Until Now: " << size[0] << ", " << size[1] << std::endl;
	  std::clog << "Region Index To Check: " << currentIndex[0] << ", " << currentIndex[1] << std::endl;
	  std::clog << "Region Size To Check: " << currentSize[0] << ", " << currentSize[1] << std::endl;
	  
	  std::clog << "--" << std::endl;
	}
#endif
	/*
	  if (!region.Crop(currentRegion)){
	  std::cerr << "Error: Images do not overlap." << std::endl;
	  exit(1);
	  };
	*/
	region = cropGeoRegion(region, currentRegion);
	
#ifdef DEBUG
	{
	  GeoRegion::IndexType index = region.GetOrigin();
	  GeoRegion::SizeType size = region.GetSize();
	  std::clog << "After Crop:" << std::endl;    
	  std::clog << "Common Region Origin:" << index[0] << ", " << index[1] << std::endl;    
	  std::clog << "Common Region Size:" << size[0] << ", " << size[1] << std::endl;    
	  std::clog << "####################" << std::endl;
	}
#endif
	
      }
      
      return region;
  };
    
}



#endif
