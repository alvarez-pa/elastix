/*=========================================================================
 *
 *  Copyright UMC Utrecht and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef elxDeformationFieldTransform_hxx
#define elxDeformationFieldTransform_hxx

#include "elxDeformationFieldTransform.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkVectorNearestNeighborInterpolateImageFunction.h"
#include "itkVectorLinearInterpolateImageFunction.h"
#include "itkChangeInformationImageFilter.h"

namespace elastix
{

/**
 * ********************* Constructor ****************************
 */

template <class TElastix>
DeformationFieldTransform<TElastix>::DeformationFieldTransform()
{
  /** Initialize. */
  this->m_DeformationFieldInterpolatingTransform = DeformationFieldInterpolatingTransformType::New();
  this->SetCurrentTransform(this->m_DeformationFieldInterpolatingTransform);

  /** Make sure that the TransformBase::WriteToFile() does
   * not read the transformParameters in the file. */
  this->SetReadWriteTransformParameters(false);

  /** Initialize to identity. */
  this->m_OriginalDeformationFieldDirection.SetIdentity();

} // end Constructor


/**
 * ************************* ReadFromFile ************************
 */

template <class TElastix>
void
DeformationFieldTransform<TElastix>::ReadFromFile(void)
{
  // \todo Test this ReadFromFile function.

  /** Call the ReadFromFile from the TransformBase. */
  this->Superclass2::ReadFromFile();

  typedef itk::ChangeInformationImageFilter<DeformationFieldType> ChangeInfoFilterType;
  typedef typename ChangeInfoFilterType::Pointer                  ChangeInfoFilterPointer;

  /** Setup VectorImageReader. */
  typedef itk::ImageFileReader<DeformationFieldType> VectorReaderType;
  typename VectorReaderType::Pointer                 vectorReader = VectorReaderType::New();

  /** Read deformationFieldImage-name from parameter-file. */
  std::string fileName = "";
  this->m_Configuration->ReadParameter(fileName, "DeformationFieldFileName", 0);
  if (fileName.empty())
  {
    xl::xout["error"]
      << "ERROR: the entry (DeformationFieldFileName \"...\") is missing in the transform parameter file!" << std::endl;
    itkExceptionMacro(<< "Error while reading transform parameter file!");
  }

  /** Possibly overrule the direction cosines. */
  ChangeInfoFilterPointer infoChanger = ChangeInfoFilterType::New();
  DirectionType           direction;
  direction.SetIdentity();
  infoChanger->SetOutputDirection(direction);
  infoChanger->SetChangeDirection(!this->GetElastix()->GetUseDirectionCosines());
  infoChanger->SetInput(vectorReader->GetOutput());

  /** Read deformationFieldImage from file. */
  vectorReader->SetFileName(fileName);
  try
  {
    infoChanger->Update();
  }
  catch (itk::ExceptionObject & excp)
  {
    /** Add information to the exception. */
    excp.SetLocation("DeformationFieldTransform - ReadFromFile()");
    std::string err_str = excp.GetDescription();
    err_str += "\nError occured while reading the deformationField image.\n";
    excp.SetDescription(err_str);
    /** Pass the exception to an higher level. */
    throw excp;
  }

  /** Store the original direction for later use */
  this->m_OriginalDeformationFieldDirection = vectorReader->GetOutput()->GetDirection();

  /** Set the deformationFieldImage in the
   * itkDeformationFieldInterpolatingTransform.
   */
  this->m_DeformationFieldInterpolatingTransform->SetDeformationField(infoChanger->GetOutput());

  typedef typename DeformationFieldInterpolatingTransformType::DeformationFieldInterpolatorType  InterpolatorType;
  typedef itk::VectorNearestNeighborInterpolateImageFunction<DeformationFieldType, CoordRepType> NNInterpolatorType;
  typedef itk::VectorLinearInterpolateImageFunction<DeformationFieldType, CoordRepType>          LinInterpolatorType;

  typename InterpolatorType::Pointer interpolator; // default-constructed (null)
  unsigned int                       interpolationOrder = 0;
  this->m_Configuration->ReadParameter(interpolationOrder, "DeformationFieldInterpolationOrder", 0);
  if (interpolationOrder == 0)
  {
    interpolator = NNInterpolatorType::New();
  }
  else if (interpolationOrder == 1)
  {
    interpolator = LinInterpolatorType::New();
  }
  else
  {
    xl::xout["error"] << "Error while reading DeformationFieldInterpolationOrder from the parameter file" << std::endl;
    xl::xout["error"] << "DeformationFieldInterpolationOrder can only be 0 or 1!" << std::endl;
    itkExceptionMacro(<< "Invalid deformation field interpolation order selected!");
  }
  this->m_DeformationFieldInterpolatingTransform->SetDeformationFieldInterpolator(interpolator);

} // end ReadFromFile()


/**
 * ************************* WriteDerivedTransformDataToFile ************************
 *
 * Saves the TransformParameters as a vector and if wanted
 * also as a deformation field.
 */

template <class TElastix>
void
DeformationFieldTransform<TElastix>::WriteDerivedTransformDataToFile(void) const
{
  // \todo Finish and Test this function.

  typedef itk::ChangeInformationImageFilter<DeformationFieldType> ChangeInfoFilterType;

  /** Write the interpolation order to file */
  std::string interpolatorName =
    this->m_DeformationFieldInterpolatingTransform->GetDeformationFieldInterpolator()->GetNameOfClass();

  /** Possibly change the direction cosines to there original value */
  typename ChangeInfoFilterType::Pointer infoChanger = ChangeInfoFilterType::New();
  infoChanger->SetOutputDirection(this->m_OriginalDeformationFieldDirection);
  infoChanger->SetChangeDirection(!this->GetElastix()->GetUseDirectionCosines());
  infoChanger->SetInput(this->m_DeformationFieldInterpolatingTransform->GetDeformationField());

  /** Write the deformation field image. */
  typedef itk::ImageFileWriter<DeformationFieldType> VectorWriterType;
  typename VectorWriterType::Pointer                 writer = VectorWriterType::New();
  writer->SetFileName(TransformIO::MakeDeformationFieldFileName(*this));
  writer->SetInput(infoChanger->GetOutput());

  /** Do the writing. */
  try
  {
    writer->Update();
  }
  catch (itk::ExceptionObject & excp)
  {
    /** Add information to the exception. */
    excp.SetLocation("DeformationFieldTransform - WriteToFile()");
    std::string err_str = excp.GetDescription();
    err_str += "\nError while writing the deformationFieldImage.\n";
    excp.SetDescription(err_str);
    /** Print the exception. */
    xl::xout["error"] << excp << std::endl;
  }

} // end WriteDerivedTransformDataToFile()


/**
 * ************************* CustomizeTransformParametersMap ************************
 */

template <class TElastix>
auto
DeformationFieldTransform<TElastix>::CreateDerivedTransformParametersMap(void) const -> ParameterMapType
{
  const std::string interpolatorName =
    m_DeformationFieldInterpolatingTransform->GetDeformationFieldInterpolator()->GetNameOfClass();
  const auto interpolationOrder = (interpolatorName == "LinearInterpolateImageFunction") ? 1U : 0U;

  return { { "DeformationFieldFileName", { TransformIO::MakeDeformationFieldFileName(*this) } },
           { "DeformationFieldInterpolationOrder", { Conversion::ToString(interpolationOrder) } } };

} // end CustomizeTransformParametersMap()


} // end namespace elastix

#endif // end #ifndef elxDeformationFieldTransform_hxx
