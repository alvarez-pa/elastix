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

#include <itkElastixRegistrationMethod.h>
#include <elxParameterObject.h>
#include <Core/elxVersionMacros.h>
#include <itkImage.h>
#include <itkSize.h>
#include <cassert>

int
main(void)
{
  enum
  {
    imageSizeX = 5,
    imageSizeY = 6
  };

  std::cout << "Image registation example: estimating the translation between two images of " << imageSizeX << "x"
            << imageSizeY << " pixels\n\n"
            << "Linked to elastix library version " ELASTIX_VERSION_STRING "\nWeb site: https://elastix.lumc.nl"
               "\nSource code repository: https://github.com/SuperElastix/elastix"
               "\nDiscussion forum: https://groups.google.com/g/elastix-imageregistration\n\n";

  using ImageType = itk::Image<float>;

  const auto printImage = [](const ImageType & image) {
    for (int y{}; y < imageSizeY; ++y)
    {
      std::cout << '\t';

      for (int x{}; x < imageSizeX; ++x)
      {
        std::cout << image.GetPixel({ x, y }) << ' ';
      }
      std::cout << '\n';
    }
    std::cout << '\n';
  };

  const auto fixedImage = ImageType::New();
  fixedImage->SetRegions(itk::Size<>{ imageSizeX, imageSizeY });
  fixedImage->Allocate(true);
  fixedImage->SetPixel({ 1, 4 }, 4);
  fixedImage->SetPixel({ 2, 3 }, 3);
  fixedImage->SetPixel({ 2, 4 }, 5);
  std::cout << "Fixed image:\n";
  printImage(*fixedImage);

  const auto movingImage = ImageType::New();
  movingImage->SetRegions(itk::Size<>{ imageSizeX, imageSizeY });
  movingImage->Allocate(true);
  movingImage->SetPixel({ 2, 2 }, 4);
  movingImage->SetPixel({ 3, 1 }, 3);
  movingImage->SetPixel({ 3, 2 }, 5);
  std::cout << "Moving image:\n";
  printImage(*movingImage);

  const auto parameterObject = elx::ParameterObject::New();
  parameterObject->SetParameterMap(
    elx::ParameterObject::ParameterMapType{ { "ImageSampler", { "Full" } },
                                            { "MaximumNumberOfIterations", { "30" } },
                                            { "Metric", { "AdvancedNormalizedCorrelation" } },
                                            { "Optimizer", { "AdaptiveStochasticGradientDescent" } },
                                            { "Transform", { "TranslationTransform" } } });

  const auto filter = itk::ElastixRegistrationMethod<ImageType, ImageType>::New();
  assert(filter != nullptr);

  filter->SetFixedImage(fixedImage);
  filter->SetMovingImage(movingImage);
  filter->SetParameterObject(parameterObject);
  filter->Update();

  const auto * const transformParameterObject = filter->GetTransformParameterObject();
  assert(transformParameterObject != nullptr);

  const auto & transformParameterMaps = transformParameterObject->GetParameterMap();
  assert(!transformParameterMaps.empty());

  const auto & transformParameterMap = transformParameterMaps.front();
  const auto   found = transformParameterMap.find("TransformParameters");
  assert(found != transformParameterMap.cend());

  std::cout << "Estimated translation:\n\t";

  for (const auto & estimatedValue : found->second)
  {
    std::cout << estimatedValue << ' ';
  }
  std::cout << std::endl;

} // end main()
