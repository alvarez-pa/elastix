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

#ifndef elxCoreMainGTestUtilities_h
#define elxCoreMainGTestUtilities_h

#include <itkImage.h>
#include <itkImageRegionRange.h>
#include <itkIndex.h>
#include <itkSize.h>

#include <algorithm> // For fill.
#include <map>
#include <initializer_list>
#include <iterator> // For begin and end.
#include <vector>

// GoogleTest header file:
#include <gtest/gtest.h>


namespace itk
{
namespace Experimental
{
// Workaround to allow using things that may be either in itk or in itk::Experimental.
}
} // namespace itk


namespace elastix
{
namespace CoreMainGTestUtilities
{

/// Simple exception class, to be used by unit tests.
class Exception : public std::exception
{
  const char * m_message = "";

public:
  explicit Exception(const char * const message)
    : m_message(message)
  {}

  const char *
  what() const noexcept override
  {
    return m_message;
  }
};


/// Dereferences the specified pointer. Throws an `Exception` instead, when the pointer is null.
template <typename T>
T &
Deref(T * ptr)
{
  if (ptr == nullptr)
  {
    throw Exception("Deref error: the pointer should not be null!");
  }
  return *ptr;
}


/// Returns a reference to the front of the specified container. Throws an
/// `Exception` instead, when the container is empty.
template <typename T>
decltype(T().front())
Front(T & container)
{
  if (container.empty())
  {
    throw Exception("Front error: the container should be non-empty!");
  }
  return container.front();
}


/// Fills the specified image region with pixel values 1.
template <typename TPixel, unsigned int VImageDimension>
void
FillImageRegion(itk::Image<TPixel, VImageDimension> & image,
                const itk::Index<VImageDimension> &   regionIndex,
                const itk::Size<VImageDimension> &    regionSize)
{
  // ImageRegionRange is to be moved from namespace itk::Experimental
  // to namespace itk with ITK version 5.2.
  using namespace itk;
  using namespace itk::Experimental;

  const ImageRegionRange<Image<TPixel, VImageDimension>> imageRegionRange{
    image, ImageRegion<VImageDimension>{ regionIndex, regionSize }
  };
  std::fill(std::begin(imageRegionRange), std::end(imageRegionRange), 1);
}


std::map<std::string, std::vector<std::string>> inline CreateParameterMap(
  std::initializer_list<std::pair<std::string, std::string>> initializerList)
{
  std::map<std::string, std::vector<std::string>> result;

  for (const auto & pair : initializerList)
  {
    [&pair, &result] { ASSERT_TRUE(result.insert({ pair.first, { pair.second } }).second); }();
  }
  return result;
}


template <unsigned VImageDimension>
std::map<std::string, std::vector<std::string>>
CreateParameterMap(std::initializer_list<std::pair<std::string, std::string>> initializerList)
{
  std::map<std::string, std::vector<std::string>> result = CreateParameterMap(initializerList);

  for (const auto & key : { "FixedImageDimension", "MovingImageDimension" })
  {
    [&key, &result] { ASSERT_TRUE(result.insert({ key, { std::to_string(VImageDimension) } }).second); }();
  }
  return result;
}


std::string
GetDataDirectoryPath();

} // namespace CoreMainGTestUtilities
} // namespace elastix


#endif
