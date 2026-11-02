/*  Spike is universal dedicated module handler

    Copyright 2021-2023 Lukas Cone

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "spike/app/tmp_storage.hpp"
#include "spike/io/directory_scanner.hpp"
#include "spike/io/fileinfo.hpp"
#include "spike/io/stat.hpp"
#include "spike/master_printer.hpp"
#include <chrono>
#include <cinttypes>
#include <sstream>

#if defined(_WIN32)
// nftw/ftw.h is POSIX only; MinGW does not ship it either, so the whole
// Windows family uses std::filesystem.
#include <filesystem>
#else
#include <ftw.h>
#endif

static std::string localPath;

void InitTempStorage() {
  auto sample = es::GetTempFilename();
  AFileInfo sampleParts(sample);
  auto point = std::chrono::system_clock::now() + std::chrono::hours(2);
  std::stringstream str;
  str << sampleParts.GetFolder() << "spike/";
  es::mkdir(str.str());
  str << std::hex << std::chrono::system_clock::to_time_t(point) << '/';
  localPath = str.str();
  es::mkdir(localPath);
}

static void RemmoveAll(const std::string &path) {
#if defined(_WIN32)
  std::filesystem::remove_all(std::filesystem::u8path(path));
#else
  nftw(
      path.data(),
      [](const char *pathName, const struct stat *, int, struct FTW *) -> int {
        if (remove(pathName) < 0) {
          printerror("[Spike] Cannot remove: " << pathName);
          return -1;
        }
        return 0;
      },
      10, FTW_DEPTH | FTW_MOUNT | FTW_PHYS);
#endif
}

void CleanTempStorages() {
  auto sample = es::GetTempFilename();
  AFileInfo sampleParts(sample);
  std::string workDir(std::string(sampleParts.GetFolder()) + "spike/");
  DirectoryScanner sc;
  sc.ScanFolders(workDir);
  auto point = std::chrono::system_clock::now();

  for (auto &s : sc) {
    std::string_view sw(s);
    sw.remove_prefix(workDir.size());
    time_t time = std::strtoull(sw.data(), nullptr, 16);
    auto expiryTimePoint = std::chrono::system_clock::from_time_t(time);

    if (point >= expiryTimePoint) {
      RemmoveAll(s);
    }
  }
}

std::string RequestTempFile() {
  if (localPath.empty()) {
    throw std::runtime_error(
        "InitTempStorage() not called before RequestTempFile()!");
  }

  auto point = std::chrono::steady_clock::now();
  std::string retPath = localPath;
  char buffer[0x20];
  snprintf(buffer, sizeof(buffer), "%" PRIX64,
           point.time_since_epoch().count());

  return retPath + buffer;
}

void CleanCurrentTempStorage() {
  if (!localPath.empty()) {
    RemmoveAll(localPath);
  }
}
