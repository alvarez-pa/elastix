#ifndef __elxMetricBase_h
#define __elxMetricBase_h

/** Needed for the macros */
#include "elxMacro.h"

#include "elxBaseComponentSE.h"
#include "itkImageToImageMetric.h"

#include "elxTimer.h"

/** Mask support. */
#include "itkImageFileReader.h"
#include "itkImageMaskSpatialObject.h"

/** For easy changing the pixel type of the mask images: */
#define __MaskFilePixelType unsigned char

namespace elastix
{
using namespace itk;

	/**
	 * \class MetricBase
	 * \brief This class is the base for all Metrics
	 *
	 * This class contains all the common functionality for Metrics ...
	 *
	 * The command line argumemts used by this class are:
	 * \commandlinearg -fMask: Optional argument with the file name of a mask for
	 * the fixed image. The mask image should contain of zeros and ones, zeros indicating 
	 * pixels that are not used for the registration. \n
	 *   example: <tt> -fMask fixedmask.mhd </tt>
	 * \commandlinearg -mMask: Optional argument with the file name of a mask for
	 * the moving image. The mask image should contain of zeros and ones, zeros indicating 
	 * pixels that are not used for the registration. \n
	 *   example: <tt> -mMask movingmask.mhd </tt>
	 *
	 * \ingroup Metrics
	 * \ingroup ComponentBaseClasses
	 */

	template <class TElastix>
		class MetricBase : public BaseComponentSE<TElastix>
	{
	public:

		/** Standard ITK stuff. */
		typedef MetricBase									Self;
		typedef BaseComponentSE<TElastix>		Superclass;

		/** Run-time type information (and related methods). */
		itkTypeMacro( MetricBase, BaseComponentSE );

		/** Typedef's inherited from Elastix. */
		typedef typename Superclass::ElastixType						ElastixType;
		typedef typename Superclass::ElastixPointer					ElastixPointer;
		typedef typename Superclass::ConfigurationType			ConfigurationType;
		typedef typename Superclass::ConfigurationPointer		ConfigurationPointer;
		typedef typename Superclass::RegistrationType				RegistrationType;
		typedef typename Superclass::RegistrationPointer		RegistrationPointer;

		/** Other typedef's. */
		typedef typename ElastixType::FixedInternalImageType		FixedImageType;
		typedef typename ElastixType::MovingInternalImageType		MovingImageType;
		
		/** ITKBaseType. */
		typedef ImageToImageMetric<
			FixedImageType, MovingImageType >				ITKBaseType;

		/** Cast to ITKBaseType. */
		virtual ITKBaseType * GetAsITKBaseType(void)
		{
			return dynamic_cast<ITKBaseType *>(this);
		}

		/** Get	the dimension of the fixed image. */
		itkStaticConstMacro( FixedImageDimension, unsigned int, FixedImageType::ImageDimension );
		/** Get	the dimension of the moving image. */
		itkStaticConstMacro( MovingImageDimension, unsigned int, MovingImageType::ImageDimension );

		/** Typedef for timer.*/
		typedef tmr::Timer					TimerType;
		/** Typedef for timer.*/
		typedef TimerType::Pointer	TimerPointer;

		/** Typedef's for fixed mask support. */
		typedef ImageMaskSpatialObject<
			itkGetStaticConstMacro( FixedImageDimension ) >			FixedImageMaskSpatialObjectType;
		/** Typedef's for moving mask support. */
		typedef ImageMaskSpatialObject<
			itkGetStaticConstMacro( MovingImageDimension ) >		MovingImageMaskSpatialObjectType;
		/** Typedef's for fixed mask support. */
		typedef typename FixedImageMaskSpatialObjectType::Pointer
			FixedImageMaskSpatialObjectPointer;
		/** Typedef's for moving mask support. */
		typedef typename MovingImageMaskSpatialObjectType::Pointer
			MovingImageMaskSpatialObjectPointer;

		/** Typedef's for mask support. */
		typedef __MaskFilePixelType	MaskFilePixelType; // defined at the top of this file
		typedef Image< MaskFilePixelType,
			itkGetStaticConstMacro( FixedImageDimension ) >			FixedMaskImageType;
		typedef Image< MaskFilePixelType,
			itkGetStaticConstMacro( MovingImageDimension ) >		MovingMaskImageType;
		typedef ImageFileReader< FixedMaskImageType >					FixedMaskImageReaderType;
		typedef ImageFileReader< MovingMaskImageType >				MovingMaskImageReaderType;
		typedef typename FixedMaskImageReaderType::Pointer		FixedMaskImageReaderPointer;
		typedef typename MovingMaskImageReaderType::Pointer		MovingMaskImageReaderPointer;

		/** Execute stuff before everything else:
		 * \li Check the appearance of masks.
		 */
		virtual int BeforeAllBase(void);
		/** Execute stuff before the actual registration:
		 * \li Read the masks.
		 */
		virtual void BeforeRegistrationBase(void);
		/** Execute stuff before each resolution:
		 * \li Update masks.
		 */
		virtual void BeforeEachResolutionBase(void);
		
		/**
		 * Force the metric to base its computation on a new subset of image samples.
		 * Not every metric may have implemented this.
		 */
		virtual void SelectNewSamples(void);

	protected:

		MetricBase();
		virtual ~MetricBase() {}

		/** Declaration of member variable, for mask support.*/
		FixedMaskImageReaderPointer		m_FixedMaskImageReader;
		/** Declaration of member variable, for mask support.*/
		MovingMaskImageReaderPointer	m_MovingMaskImageReader;

		/** Declaration of member variable, for mask support.*/
		typename FixedMaskImageType::Pointer		m_FixedMaskAsImage;
		/** Declaration of member variable, for mask support.*/
		typename MovingMaskImageType::Pointer		m_MovingMaskAsImage;

		/** Declaration of member variable, for mask support.*/
		FixedImageMaskSpatialObjectPointer			m_FixedMaskAsSpatialObject;
		/** Declaration of member variable, for mask support.*/
		MovingImageMaskSpatialObjectPointer			m_MovingMaskAsSpatialObject;

		/** Function to update masks. */
		void UpdateMasks( unsigned int level );

	private:

		MetricBase( const Self& );			// purposely not implemented
		void operator=( const Self& );	// purposely not implemented


	}; // end class MetricBase


} // end namespace elastix

#ifndef ITK_MANUAL_INSTANTIATION
#include "elxMetricBase.hxx"
#endif

#endif // end #ifndef __elxMetricBase_h

