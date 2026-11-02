/*  Spike is universal dedicated module handler
    This source contains code for CLI master app

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

#include "nlohmann/json.hpp"
#include "project.h"
#include "spike/app/batch.hpp"
#include "spike/app/console.hpp"
#include "spike/app/tmp_storage.hpp"
#include "spike/io/binwritter.hpp"
#include "spike/io/stat.hpp"
#include "spike/master_printer.hpp"
#include "spike/type/tchar.hpp"
#include "spike/util/i18n.hpp"
#include "spike/util/pugiex.hpp"
#include <thread>

static const char appHeader0[] =
    "Simply drag'n'drop files/folders onto application or "
    "use as ";
static const char appHeader1[] =
    " [options] path1 path2 ...\nTool can detect and scan folders and "
    "uncompressed zip archives.";

static const char appHeader0Zh[] = "把文件或文件夹直接拖到程序上，或按下面的格式运行 ";
static const char appHeader1Zh[] =
    " [选项] 路径1 路径2 ...\n程序可以自动识别并扫描文件夹以及未压缩的 zip 压缩包。";

struct ProcessedFiles : LoadingBar, CounterLine {
  char buffer[128]{};

  ProcessedFiles() : LoadingBar({buffer, sizeof(buffer)}) {}
  void PrintLine() override {
      snprintf(buffer, sizeof(buffer),
               I18N("已处理 %4" PRIuMAX " 个文件。", "Processed %4" PRIuMAX " files."),
               curitem.load(std::memory_order_relaxed));
    LoadingBar::PrintLine();
  }
};

struct ExtractStats {
  std::map<JenHash, size_t> archiveFiles;
  size_t totalFiles = 0;
};

struct UILines {
  ProgressBar *totalProgress{nullptr};
  CounterLine *totalCount{nullptr};
  std::map<uint32, ProgressBar *> bars;
  std::mutex barsMutex;

  auto ChooseBar() {
    if (bars.empty()) {
      return (ProgressBar *)(nullptr);
    }

    auto threadId = std::this_thread::get_id();
    auto id = reinterpret_cast<const uint32 &>(threadId);
    auto found = bars.find(id);

    if (es::IsEnd(bars, found)) {
      std::lock_guard<std::mutex> lg(barsMutex);
      auto retVal = bars.begin()->second;
      bars.emplace(id, retVal);
      bars.erase(bars.begin());

      return retVal;
    }

    return found->second;
  };

  UILines(const ExtractStats &stats) {
    ModifyElements([&](ElementAPI &api) {
      const size_t minThreads =
          std::min(size_t(std::thread::hardware_concurrency()),
                   stats.archiveFiles.size());

      if (minThreads < 2) {
        return;
      }

      for (size_t t = 0; t < minThreads; t++) {
        auto progBar = std::make_unique<ProgressBar>(I18N("线程:", "Thread:"));
        auto progBarRaw = progBar.get();
        bars.emplace(t, progBarRaw);
        api.Append(std::move(progBar));
      }
    });

    auto prog = AppendNewLogLine<DetailedProgressBar>(I18N("总计: ", "Total: "));
    prog->ItemCount(stats.totalFiles);
    totalCount = prog;
  }

  UILines(size_t totalInputFiles) {
    totalCount = AppendNewLogLine<ProcessedFiles>();
    auto prog = AppendNewLogLine<DetailedProgressBar>(I18N("总计: ", "Total: "));
    prog->ItemCount(totalInputFiles);
    totalProgress = prog;
  }

  ~UILines() {
    ModifyElements([&](ElementAPI &api) {
      if (totalCount) {
        // Wait a little bit for internal queues to finish printing
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (totalProgress) {
          auto data = static_cast<ProcessedFiles *>(totalCount);
          data->Finish();
          api.Release(data);
        } else {
          auto data = static_cast<DetailedProgressBar *>(totalCount);
          api.Remove(data);
        }
      }
      api.Clean();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });
  }
};

bool ScanModules(const std::string &appFolder, const std::string &appName) {
  DirectoryScanner sc;
  sc.AddFilter(std::string_view(".spk$"));
  sc.Scan(appFolder);
  bool isOkay = true;

  for (auto &m : sc) {
    try {
      AFileInfo modulePath(m);
      auto moduleName = modulePath.GetFilename();
      const size_t firstDotPos = moduleName.find_first_of('.');
      std::string moduleNameStr(moduleName.substr(0, firstDotPos));
      APPContext ctx(moduleNameStr.data(), appFolder, appName);
      ctx.FromConfig();
    } catch (const std::runtime_error &e) {
      printerror(e.what());
      isOkay = false;
    }
  }

  return isOkay;
}

void GenerateDocumentation(const std::string &appFolder,
                           const std::string &appName,
                           const std::string &templatePath) {
  DirectoryScanner sc;
  sc.AddFilter(std::string_view(".spk$"));
  sc.Scan(appFolder);
  std::set<std::string> modules;
  pugi::xml_document doc;

  if (!templatePath.empty()) {
    doc = XMLFromFile(templatePath);
  }

  for (auto &m : sc) {
    try {
      AFileInfo modulePath(m);
      auto moduleName = modulePath.GetFilename();
      const size_t firstDotPos = moduleName.find_first_of('.');
      std::string moduleNameStr(moduleName.substr(0, firstDotPos));
      modules.emplace(moduleNameStr);
    } catch (const std::runtime_error &e) {
      printerror(e.what());
    }
  }

  BinWritter_t<BinCoreOpenMode::Text> wr(appFolder + "/README.md");

  const char *toolsetDescription = "[[TOOLSET DESCRIPTION]]";

  if (auto child = doc.child("toolset_description"); child) {
    toolsetDescription = child.text().as_string();
  }

  wr.BaseStream() << toolsetDescription << "<h2>"
                  << es::i18n::Tr("Module list") << "</h2>\n<ul>\n";
  std::stringstream str;

  for (auto &m : modules) {
    pugi::xml_node node = doc.child(m.data());
    APPContext ctx(m.data(), appFolder, appName);
    ctx.GetMarkdownDoc(str, node);
    std::string className = ctx.GetClassName(node);
    std::string classNameLink = className;
    std::replace_if(
        classNameLink.begin(), classNameLink.end(),
        [](char c) { return c == ' '; }, '-');

    wr.BaseStream() << "<li><a href=\"#" << classNameLink << "\">" << className
                    << "</a></li>\n";
  }

  wr.BaseStream() << "</ul>\n\n" << str.str() << "\n\n";

  if (auto child = doc.child("toolset_footer"); child) {
    wr.BaseStream() << child.text().as_string();
  }
}

void PackModeBatch(Batch &batch) {
  struct PackData {
    size_t index = 0;
    std::unique_ptr<AppPackContext> archiveContext;
    std::string pbarLabel;
    DetailedProgressBar *progBar = nullptr;
    std::string folderPath;
  };

  auto payload = std::make_shared<PackData>();

  batch.forEachFolder = [payload, ctx = batch.ctx](const std::string &path,
                                                   size_t numFiles) {
    payload->folderPath = path;
    payload->archiveContext.reset(ctx->NewArchive(path));
    payload->pbarLabel = std::string(I18N("文件夹编号 ", "Folder id ")) + std::to_string(payload->index++);
    payload->progBar =
        AppendNewLogLine<DetailedProgressBar>(payload->pbarLabel);
    payload->progBar->ItemCount(numFiles);
    uint8 consoleDetail = 1 | uint8(ctx->info->multithreaded) << 1;
    ConsolePrintDetail(consoleDetail);
    printline(I18N("正在处理: ", "Processing: ") << path);
  };

  batch.forEachFile = [payload](AppContextShare *iCtx) {
    if (iCtx->workingFile.GetFullPath().starts_with(payload->folderPath)) {
      int notSlash = !payload->folderPath.ends_with('/');
      payload->archiveContext->SendFile(
          iCtx->workingFile.GetFullPath().substr(payload->folderPath.size() +
                                                 notSlash),
          iCtx->GetStream());
    } else {
      payload->archiveContext->SendFile(iCtx->workingFile.GetFullPath(),
                                        iCtx->GetStream());
    }
    (*payload->progBar)++;
  };

  batch.forEachFolderFinish = [payload] {
    ConsolePrintDetail(1);
    payload->archiveContext->Finish();
    payload->archiveContext.reset();
    RemoveLogLines(payload->progBar);
  };
}

void MergePackModeBatch(Batch &batch, const std::string &folderPath,
                        AppPackContext *archiveContext) {
  uint8 consoleDetail = 1 | uint8(batch.ctx->info->multithreaded) << 1;
  ConsolePrintDetail(consoleDetail);
  printline(I18N("正在处理: ", "Processing: ") << folderPath);

  batch.forEachFile = [=](AppContextShare *iCtx) {
    if (iCtx->workingFile.GetFullPath().starts_with(folderPath)) {
      int notSlash = !folderPath.ends_with('/');
      archiveContext->SendFile(
          iCtx->workingFile.GetFullPath().substr(folderPath.size() + notSlash),
          iCtx->GetStream());
    } else {
      archiveContext->SendFile(iCtx->workingFile.GetFullPath(),
                               iCtx->GetStream());
    }
  };
}

auto ExtractStatBatch(Batch &batch) {
  struct ExtractStatsMaker : ExtractStats {
    std::mutex mtx;
    LoadingBar *scanBar;

    void Push(AppContextShare *ctx, size_t numFiles) {
      std::unique_lock<std::mutex> lg(mtx);
      archiveFiles.emplace(ctx->Hash(), numFiles);
      totalFiles += numFiles;
    }

    ~ExtractStatsMaker() { RemoveLogLines(scanBar); }
  };

  batch.keepFinishLines = false;
  auto sharedData = std::make_shared<ExtractStatsMaker>();
  uint8 consoleDetail = 1 | uint8(batch.ctx->info->multithreaded) << 1;
  ConsolePrintDetail(consoleDetail);
  sharedData->scanBar =
    AppendNewLogLine<LoadingBar>(I18N("正在统计解包数据。", "Processing extract stats."));

  batch.forEachFile = [payload = sharedData,
                       ctx = batch.ctx](AppContextShare *iCtx) {
    payload->Push(iCtx, ctx->ExtractStat(std::bind(
                            [&](size_t offset, size_t size) {
                              return iCtx->GetBuffer(size, offset);
                            },
                            std::placeholders::_1, std::placeholders::_2)));
  };

  return sharedData;
}

void ProcessBatch(Batch &batch, ExtractStats *stats) {
  uint8 consoleDetail = 1 | uint8(batch.ctx->info->multithreaded) << 1;
  ConsolePrintDetail(consoleDetail);
  batch.forEachFile = [payload = std::make_shared<UILines>(*stats),
                       archiveFiles =
                           std::make_shared<decltype(stats->archiveFiles)>(
                               std::move(stats->archiveFiles)),
                       ctx = batch.ctx](AppContextShare *iCtx) {
    auto currentBar = payload->ChooseBar();
    if (currentBar) {
      currentBar->ItemCount(archiveFiles->at(iCtx->Hash()));
    }

    iCtx->forEachFile = [=] {
      if (currentBar) {
        (*currentBar)++;
      }

      if (payload->totalCount) {
        (*payload->totalCount)++;
      }
    };

    printline(I18N("正在处理: ", "Processing: ") << iCtx->FullPath());
    ctx->ProcessFile(iCtx);
    if (payload->totalProgress) {
      (*payload->totalProgress)++;
    }
  };
}

void ProcessBatch(Batch &batch, size_t numFiles) {
  uint8 consoleDetail = 1 | uint8(batch.ctx->info->multithreaded) << 1;
  ConsolePrintDetail(consoleDetail);
  auto payload = std::make_shared<UILines>(numFiles);
  batch.forEachFile = [payload = payload,
                       ctx = batch.ctx](AppContextShare *iCtx) {
    printline(I18N("正在处理: ", "Processing: ") << iCtx->FullPath());
    ctx->ProcessFile(iCtx);
    if (payload->totalProgress) {
      (*payload->totalProgress)++;
    }
    if (payload->totalCount) {
      (*payload->totalCount)++;
    }
  };

  auto totalFiles = std::make_shared<size_t>(numFiles);
  batch.updateFileCount = [payload = payload,
                           totalFiles = totalFiles](size_t addedFiles) {
    *totalFiles.get() += addedFiles;
    payload->totalProgress->ItemCount(*totalFiles);
  };
}

int CreateContent(const std::string &moduleName, const std::string &appFolder,
                  const std::string &appName, APPContext &ctx) {
  try {
    ctx = APPContext(moduleName.c_str(), appFolder, appName);
  } catch (const std::exception &e) {
    printerror(e.what());
    return 2;
  }

  ConsolePrintDetail(0);

  printline(ctx.info->header);
  printline(I18N(appHeader0Zh, appHeader0)
            << appName << ' ' << moduleName
            << I18N(appHeader1Zh, appHeader1));

  return 0;
}

int LoadProject(const std::string &path, const std::string &appFolder,
                const std::string &appName) {
  std::ifstream str(path);
  nlohmann::json project(nlohmann::json::parse(str));

  APPContext ctx;
  std::string moduleName = project["module"];

  if (int ret = CreateContent(moduleName, appFolder, appName, ctx); ret != 0) {
    return ret;
  }

  ConsolePrintDetail(1);

  std::string outputDir;

  if (!project["output_dir"].is_null()) {
    std::string outputDir = project["output_dir"];
    ctx.ApplySetting("out", outputDir);
  }

  bool noConfig = !project["no_config"].is_null() && project["no_config"];

  if (!noConfig) {
    printinfo(I18N("正在加载配置: ", "Loading config: ") << appName << ".config");
    ctx.FromConfig();
  }

  nlohmann::json settings = project["settings"];

  for (auto &s : settings.items()) {
    std::string dumped = s.value().dump();
    if (dumped.front() == '"') {
      dumped.erase(0, 1);
    }
    if (dumped.back() == '"') {
      dumped.erase(dumped.size() - 1);
    }
    ctx.ApplySetting(s.key(), dumped);
  }

  nlohmann::json inputs = project["inputs"];

  InitTempStorage();
  ctx.SetupModule();
  std::unique_ptr<AppPackContext> archiveContext;

  {
    Batch batch(&ctx, ctx.info->multithreaded * 50);
    AFileInfo batchPath(path);
    std::string batchBase(batchPath.GetFolder());

    if (ctx.NewArchive) {
      std::string folder(batchPath.GetFolder());
      std::string archive(batchPath.GetFullPathNoExt());
      archiveContext.reset(batch.ctx->NewArchive(archive));
      MergePackModeBatch(batch, folder, archiveContext.get());
    } else {
      if (ctx.ExtractStat) {
        auto stats = ExtractStatBatch(batch);
        for (std::string input : inputs) {
          batch.AddFile(batchBase + input);
        }

        batch.FinishBatch();
        batch.Clean();
        stats.get()->totalFiles += inputs.size();
        ProcessBatch(batch, stats.get());
      } else {
        ProcessBatch(batch, inputs.size());
      }
    }

    if (ctx.info->batchControlFilters.size() > 0) {
      std::string pathDir(AFileInfo(path).GetFolder());
      for (nlohmann::json input : inputs) {
        if (input.is_array()) {
          batch.AddBatch(input, pathDir);
        } else {
          printwarning(I18N("预期为分组，实际得到 ", "Expected group, got ")
                       << input.type_name()
                       << I18N("。跳过该输入。", " instead. Skipping input."));
        }
      }
    } else {
      for (nlohmann::json input : inputs) {
        if (input.is_string()) {
          batch.AddFile(batchBase + std::string(input));
        } else {
          printwarning(I18N("预期为路径字符串，实际得到 ",
                            "Expected path string, got ")
                       << input.type_name()
                       << I18N("。跳过该输入。", " instead. Skipping input."));
        }
      }
    }

    batch.FinishBatch();
  }

  if (archiveContext) {
    ConsolePrintDetail(1);
    archiveContext->Finish();
  }

  if (ctx.FinishContext) {
    ctx.FinishContext();
  }

  return 0;
}

int Main(int argc, TCHAR *argv[]) {
  ConsolePrintDetail(1);
  AFileInfo appLocation(std::to_string(*argv));
  std::string appFolder(appLocation.GetFolder());
  std::string appName(appLocation.GetFilename());
  es::SetDllRunPath(appFolder + "lib");

  // Language resolution order: environment -> --language/-l switch -> saved
  // default. The switch always wins over the environment.
  es::i18n::InitFromEnvironment();

  // Consume language switches from anywhere on the command line, then drop
  // them from argv so the rest of the driver (and the reflector) never sees
  // them. This makes "toolset -l en hk_to_gltf --help" work.
  {
    int write = 1;

    for (int read = 1; read < argc; read++) {
      const std::string arg = std::to_string(argv[read]);

      if ((arg == "--language" || arg == "--lang" || arg == "-l") &&
          read + 1 < argc) {
        const std::string langArg = std::to_string(argv[read + 1]);

        if (auto lang = es::i18n::ParseLanguage(langArg);
            lang != es::i18n::Lang::Auto) {
          es::i18n::SetLanguage(lang);
        }

        read++;
        continue;
      }

      // Also accept the joined form: --language=en / -l=en
      const size_t eq = arg.find('=');

      if (eq != std::string::npos) {
        const std::string key = arg.substr(0, eq);

        if (key == "--language" || key == "--lang" || key == "-l") {
          if (auto lang = es::i18n::ParseLanguage(arg.substr(eq + 1));
              lang != es::i18n::Lang::Auto) {
            es::i18n::SetLanguage(lang);
          }

          continue;
        }
      }

      argv[write++] = argv[read];
    }

    argc = write;
  }

  if (argc < 2) {
    printwarning(I18N("未提供任何参数，进入扫描模式并生成配置文件。",
                      "No parameters provided, entering scan mode and "
                      "generating config."));
    return !ScanModules(appFolder, appName);
  }

  auto moduleName = std::to_string(argv[1]);

  if (moduleName == "--make-doc") {
    std::string templatePath;

    if (argc < 3) {
      printwarning(I18N("需要提供模板路径！", "Expexted template path!"));
    } else {
      templatePath = std::to_string(argv[2]);
    }

    GenerateDocumentation(appFolder, appName, templatePath);
    return 0;
  } else if (moduleName.ends_with(".json")) {
    return LoadProject(moduleName, appFolder, appName);
  }

  APPContext ctx;

  if (int ret = CreateContent(moduleName, appFolder, appName, ctx); ret != 0) {
    return ret;
  }

  if (argc >= 3 && IsHelp(argv[2])) {
    ctx.PrintCLIHelp();
    return 0;
  }

  if (argc < 3) {
    printerror(I18N("参数数量不足，需要提供参数。",
                    "Insufficient argument count, expected parameters."));
    return 1;
  }

  ConsolePrintDetail(1);
  bool dontLoadConfig = false;
  std::vector<bool> markedFiles(size_t(argc), false);
  size_t totalFiles = 0;

  // Handle cli options and switches
  for (int a = 2; a < argc; a++) {
    auto opt = argv[a];

    if (opt[0] == '-') {
      auto optStr = std::to_string(opt);
      std::string_view optsw(optStr);

      // Language switches are consumed before this loop runs; skip them here
      // so they never reach the reflector as unknown settings.
      if (optsw == "--language" || optsw == "--lang" || optsw == "-l") {
        a++;
        continue;
      }

      if (optsw != "--out") {
        // We won't use config file, reset all booleans to false,
        // so we can properly use cli switches
        [&] {
          if (dontLoadConfig) {
            return;
          }

          printinfo("CLI option detected, config won't be loaded, all booleans "
                    "set to false!");
          ctx.ResetSwitchSettings();
        }();
        dontLoadConfig = true;
      }
      optsw.remove_prefix(1);

      if (opt[0] == '-') {
        optsw.remove_prefix(1);
      }

      // A switch may legitimately be the last argument (boolean flags such as
      // --visualize). Reading argv[a + 1] unconditionally walks past the end
      // of the vector and faults, so probe the array bound first.
      const std::string valStr =
          a + 1 < argc ? std::to_string(argv[a + 1]) : std::string();

      if (auto retVal = ctx.ApplySetting(optsw, valStr); retVal > 0) {
        if (a + 1 < argc) {
          a++;
        }
      }

    } else {
      markedFiles[a] = true;
      totalFiles++;
    }
  }

  if (!dontLoadConfig) {
    printinfo(I18N("正在加载配置: ", "Loading config: ") << appName << ".config");
    ctx.FromConfig();
  }

  InitTempStorage();
  ctx.SetupModule();
  {
    Batch batch(&ctx, ctx.info->multithreaded * 50);

    if (ctx.NewArchive) {
      PackModeBatch(batch);
    } else {
      if (ctx.ExtractStat) {
        auto stats = ExtractStatBatch(batch);
        for (int a = 2; a < argc; a++) {
          if (!markedFiles.at(a)) {
            continue;
          }
          batch.AddFile(std::to_string(argv[a]));
        }

        batch.FinishBatch();
        batch.Clean();
        stats.get()->totalFiles += totalFiles;
        ProcessBatch(batch, stats.get());
      } else {
        ProcessBatch(batch, totalFiles);
      }
    }

    for (int a = 2; a < argc; a++) {
      if (!markedFiles.at(a)) {
        continue;
      }
      batch.AddFile(std::to_string(argv[a]));
    }

    batch.FinishBatch();
  }

  if (ctx.FinishContext) {
    ctx.FinishContext();
  }

  return 0;
}

int _tmain(int argc, TCHAR *argv[]) {
  es::SetupWinApiConsole();
  InitConsole();
  CleanTempStorages();

  int retVal = Main(argc, argv);

  CleanCurrentTempStorage();

#ifndef NDEBUG
  auto cacheStats = CacheGenerator::GlobalMetrics();
  PrintInfo("Cache search hits: ", cacheStats.numSearchHits,
            " search misses: ", cacheStats.numSearchMisses);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
  TerminateConsole();
  return retVal;
}
