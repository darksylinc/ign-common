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

#ifndef IGNITION_COMMON_TEMPDIRECTORY_HH_
#define IGNITION_COMMON_TEMPDIRECTORY_HH_

#include <string>
#include <memory>

#include <ignition/common/Export.hh>
#include <ignition/common/Filesystem.hh>

namespace ignition
{
  namespace common
  {
    class TempDirectoryPrivate;

    /// \brief Return the path to a directory suitable for temporary files.
    ///
    /// Calls std::filesystem::temp_directory_path, refer to the standard
    /// documentation for your platform for behaviors.
    /// \return A directory suitable for temporary files.
    std::string IGNITION_COMMON_VISIBLE tempDirectoryPath();

    /// \brief Create a directory in the tempDirectoryPath by expanding
    /// a name template
    ///
    /// On execution, will create the directory:
    ///  "_parentPath"/"_baseName" + "XXXXXX", where XXXXXX will be filled
    /// out by an OS-appropriate method (eg mkdtmp/_mktemp_s)
    ///
    /// \param[in] _baseName String to be prepended to the expanded template
    /// \param[in] _parentPath Location to create the directory
    /// \param[in] _warningOp Allow or suppress filesystem warnings
    /// \return Path to newly-created temporary directory
    std::string IGNITION_COMMON_VISIBLE createTempDirectory(
        const std::string &_baseName,
        const std::string &_parentPath,
        const FilesystemWarningOp _warningOp = FSWO_LOG_WARNINGS);

    /// \class TempDirectory TempDirectory.hh ignitin/common/TempDirectory.hh
    /// \brief Create a temporary directory in the OS temp location.
    class TempDirectory
    {
      public: TempDirectory(const std::string &_prefix = "temp_dir",
                            const std::string &_subDir = "ignition",
                            bool _cleanup = true);

      public: ~TempDirectory();

      public: bool Valid() const;

      public: std::string Path() const;

      private: std::unique_ptr<TempDirectoryPrivate> dataPtr;
    };

  }  // namespace common
}  // namespace ignition
#endif  // IGNITION_COMMON_TEMPDIRECTORY_HH_

