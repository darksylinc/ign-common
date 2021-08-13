/*
 * Copyright 2021 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <ignition/common/TempDirectory.hh>
#include <ignition/common/Console.hh>

#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <fileapi.h>
#include <io.h>
#endif

namespace fs = std::filesystem;

using namespace ignition;
using namespace common;

/////////////////////////////////////////////////
// Return true if success, false if error
inline bool fs_warn(const std::string &_fcn,
             const std::error_code &_ec,
             const FilesystemWarningOp &_warningOp = FSWO_LOG_WARNINGS)
{

  if (_ec)
  {
    if (FSWO_LOG_WARNINGS == _warningOp)
    {
      ignwarn << "Failed ignition::common::" << _fcn
        << " (ec: " << _ec << " " << _ec.message() << ")\n";
    }
    return false;
  }
  return true;
}

/////////////////////////////////////////////////
std::string ignition::common::tempDirectoryPath()
{
  std::error_code ec;
  auto ret = fs::temp_directory_path(ec);

  if (!fs_warn("tempDirectoryPath", ec))
  {
    ret = "";
  }

  return ret.string();
}

/////////////////////////////////////////////////
/// \brief Internal method for createTempDirectory
///
/// This is primarily to scope the "throw" behavior from when this
/// was copied from rclcpp.
std::string createTempDirectory(
    const std::string &_baseName,
    const std::string &_parentPath)
{
  fs::path parentPath(_parentPath);
  fs::path templatePath = _baseName + "XXXXXX";

  std::string fullTemplateStr = (parentPath / templatePath).string();
  if (!createDirectories(parentPath.string())) {
    std::error_code ec{errno, std::system_category()};
    errno = 0;
    throw std::system_error(ec, "could not create the parent directory");
  }

#ifdef _WIN32
  errno_t errcode = _mktemp_s(&fullTemplateStr[0], fullTemplateStr.size() + 1);
  if (errcode) {
    std::error_code ec(static_cast<int>(errcode), std::system_category());
    throw std::system_error(ec,
        "could not format the temp directory name template");
  }
  const fs::path finalPath{fullTemplateStr};
  if (!createDirectories(finalPath.string())) {
    std::error_code ec(static_cast<int>(GetLastError()),
        std::system_category());
    throw std::system_error(ec, "could not create the temp directory");
  }
#else
  const char * dirName = mkdtemp(&fullTemplateStr[0]);
  if (dirName == nullptr) {
    std::error_code ec{errno, std::system_category()};
    errno = 0;
    throw std::system_error(ec,
        "could not format or create the temp directory");
  }
  const fs::path finalPath{dirName};
#endif

  return finalPath.string();
}

/////////////////////////////////////////////////
std::string ignition::common::createTempDirectory(
    const std::string &_baseName,
    const std::string &_parentPath,
    const FilesystemWarningOp _warningOp)
{
  std::string ret;
  try {
    ret = ::createTempDirectory(_baseName, _parentPath);
  } catch (const std::system_error &ex) {
    ret = "";
    if(FSWO_LOG_WARNINGS == _warningOp)
    {
      ignwarn << "Failed to create temp directory: " << ex.what() << "\n";
    }
  }
  return ret;
}

class ignition::common::TempDirectoryPrivate
{
  /// \brief Current working directory before creation of temporary dir.
  public: std::string oldPath {""};

  /// \brief Path of the temporary directory
  public: std::string path {""};

  /// \brief True if the temporary directory exists
  public: bool isValid {false};

  /// \brief True if the temporary directory should be cleaned up from
  /// disk when the object goes out of scope.
  public: bool doCleanup {true};
};

/////////////////////////////////////////////////
TempDirectory::TempDirectory(const std::string &_prefix,
                             const std::string &_subDir,
                             bool _cleanup):
  dataPtr(std::make_unique<TempDirectoryPrivate>())
{

  this->dataPtr->oldPath = common::cwd();
  this->dataPtr->doCleanup = _cleanup;

  auto tempPath = common::tempDirectoryPath();
  if (!_subDir.empty())
  {
    tempPath = common::joinPaths(tempPath, _subDir);
  }

  this->dataPtr->path = common::createTempDirectory(_prefix, tempPath);
  std::cerr << "Created temporary directory: " << this->dataPtr->path << std::endl; 
  if (!this->dataPtr->path.empty())
  {
    this->dataPtr->isValid = true;
    common::chdir(this->dataPtr->path);
  }
}

/////////////////////////////////////////////////
TempDirectory::~TempDirectory()
{
  common::chdir(this->dataPtr->oldPath);
  if (this->dataPtr->isValid && this->dataPtr->doCleanup)
  {
    common::removeAll(this->dataPtr->path);
  }
}

/////////////////////////////////////////////////
bool TempDirectory::Valid() const
{
  return this->dataPtr->isValid;
}

/////////////////////////////////////////////////
void TempDirectory::Cleanup(bool _cleanup)
{
  this->dataPtr->doCleanup = _cleanup;
}

/////////////////////////////////////////////////
bool TempDirectory::Cleanup() const
{
  return this->dataPtr->doCleanup;
}

/////////////////////////////////////////////////
std::string TempDirectory::Path() const
{
  return this->dataPtr->path;
}
