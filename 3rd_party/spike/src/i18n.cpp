/*  i18n implementation: runtime switchable Simplified Chinese / English UI

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0
*/

#include "spike/util/i18n.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#ifndef HK_DEFAULT_LANG_EN
#define HK_DEFAULT_LANG_EN 0
#endif

namespace es::i18n {
namespace {
struct Entry {
  const char *en;
  const char *zh;
};

// ~~~~~ translation table ~~~~~
// Sorted is not required; matching is greedy longest-first at build time of
// the index below. Entries may contain '\n' to cover multi-line blocks.
static const Entry table[] = {
    // ===== module headers =====
    {"Convert Havok files to GLTF", "将 Havok 文件转换为 GLTF"},
    {"Copyright (C)", "版权所有 (C)"},

    // ===== Spike: console / progress =====
    {"| APPLICATION HAVE CLOSED UNEXPECTEDLY, CODE: %.2i %s |\n",
     "| 程序意外退出，错误代码: %.2i %s |\n"},
    {"WARNING: ", "警告: "},
    {"ERROR: ", "错误: "},
    {"Thread[0x", "线程[0x"},
    {"Thread:", "线程:"},
    {"Total: ", "总计: "},
    {"Processing: ", "正在处理: "},
    {"Processed ", "已处理 "},
    {" files.", " 个文件。"},
    {"Folder id ", "文件夹编号 "},
    {"Processing extract stats.", "正在统计解包数据。"},

    // ===== Spike: CLI driver =====
    {"Simply drag'n'drop files/folders onto application or use as ",
     "把文件或文件夹直接拖到程序上，或按下面的格式运行 "},
    {" [options] path1 path2 ...\nTool can detect and scan folders and "
     "uncompressed zip archives.",
     " [选项] 路径1 路径2 ...\n程序可以自动识别并扫描文件夹以及未压缩的 zip "
     "压缩包。"},
    {"No parameters provided, entering scan mode and generating config.",
     "未提供任何参数，进入扫描模式并生成配置文件。"},
    {"Expexted template path!", "需要提供模板路径！"},
    {"Insufficient argument count, expected parameters.",
     "参数数量不足，需要提供参数。"},
    {"CLI option detected, config won't be loaded, all booleans set to false!",
     "检测到命令行选项，将不加载配置文件，所有布尔项已设为 false！"},
    {"Loading config: ", "正在加载配置: "},
    {"Expected group, got ", "预期为分组，实际得到 "},
    {"Expected path string, got ", "预期为路径字符串，实际得到 "},
    {" instead. Skipping input.", "。跳过该输入。"},
    {"Cache search hits: ", "缓存命中次数: "},
    {" search misses: ", " 未命中次数: "},

    {"Interface language: zh (Simplified Chinese) or en (English). Also "
     "settable with the -l switch or the HAVOK_LANG variable.",
     "界面语言: zh（简体中文）"
     "或 en（英文）。"
     "也可以用 -l 开关或 HAVOK_LANG 环境变量"
     "设置。"},
    {"language", "language"},

    // ===== setting group titles =====
    {"Options:", "选项:"},
    {"Values: ", "可选值: "},
    {"Invalid option: ", "无效的选项: "},
    {"common settings.", "通用设置。"},
    {" settings.", " 设置。"},
    {"Couldn't find module: ", "找不到模块: "},
    {"APPContext Error: ", "APPContext 错误: "},
    {"Module open: ", "已加载模块: "},
    {"Module context version mismatch!", "模块上下文版本不匹配！"},
    {"Module uses 2 or more contexts!", "模块使用了 2 个或更多上下文！"},
    {"Error while initializing context.", "初始化上下文时出错。"},
    {"Cannot access config. File is locked.", "无法访问配置，文件被占用。"},
    {"Will generate text log of console output inside application location.",
     "在程序所在目录生成控制台输出的文本日志。"},
    {"Prints more information per level.", "按级别输出更多信息。"},
    {"Output folder for processed files", "处理结果输出目录"},
    {"When extracting, create folder that uses input archive's name as output "
     "dir.",
     "解包时创建以输入压缩包名称命名的输出目录。"},
    {"Pack extracted files inside ZIP file named after input archive. Your HDD "
     "will thank you.",
     "把解出的文件打包成以输入压缩包命名的 ZIP 文件，你的硬盘会感谢你。"},
    {"Writes compressed data only when compression ratio is less than "
     "specified threshold [0 - 100]%",
     "仅当压缩率低于指定阈值 [0 - 100]% 时写入压缩数据。"},
    {"Files that are smaller than specified size won't be compressed.",
     "小于指定大小的文件不会被压缩。"},
    {"Specify output format for images", "指定图片的输出格式"},
    {"Convert cubemaps into equirectangular layout",
     "将立方体贴图转换为等距柱状投影布局"},
    {"Save only largest mipmap for each mipmap chain",
     "每条 mipmap 链只保存最大的一级"},

    // ===== Spike: texel enum descriptions =====
    {"Use legacy dds container but decode to uncompressed formats if "
     "necessary\nSupported legacy formats:\n  BC1, BC2, BC3, Grayscale, RGB8, "
     "RGBA8, RGB565, RGBA4\nOther formats will be converted to 8bit channeled "
     "format like:\n  BC4 -> Grayscale\n  BC5 -> RGX8\n  BC7 -> RGBA8 or "
     "RGB8\nSupport for mipmaps, cubemaps, volumetrics\nsnorm will be "
     "converted to unorm",
     "使用旧版 dds 容器，必要时解码为未压缩格式\n支持的旧版格式:\n  BC1, BC2, "
     "BC3, Grayscale, RGB8, RGBA8, RGB565, RGBA4\n其他格式会转换为 8 位通道格式，"
     "例如:\n  BC4 -> Grayscale\n  BC5 -> RGX8\n  BC7 -> RGBA8 或 RGB8\n支持 "
     "mipmap、立方体贴图、体积纹理\nsnorm 会被转换为 unorm"},
    {"dds, data format is unchanged\nPVRTC and ETC will be converted to "
     "RGBA8\nSupport for mipmaps, cubemaps, arrays, volumetrics",
     "dds，数据格式保持不变\nPVRTC 与 ETC 会转换为 RGBA8\n支持 mipmap、"
     "立方体贴图、数组纹理、体积纹理"},
    {"Decode to Quite OK Image format.\nOnly for RGX, RGB and RGBA\n2 channels "
     "will be converted to RGX8\nDecode to BMP for P8, P4, Grayscale,\nNo "
     "mipmap support",
     "解码为 Quite OK Image 格式。\n仅支持 RGX、RGB 与 RGBA\n2 通道会转换为 "
     "RGX8\nP8、P4、Grayscale 会解码为 BMP\n不支持 mipmap"},
    {"Decode to Quite OK Image format.\nNormal formats RGB and RGBA are kept "
     "instact\n2 channels will be converted to RGX8\n8bit (P8) and 4bit (P4) "
     "palletes will be expanded to RGB or RGBA\nGrayscale will be expanded to "
     "RGB\nNo mipmap support",
     "解码为 Quite OK Image 格式。\n常规的 RGB 与 RGBA 保持原样\n2 通道会转换为 "
     "RGX8\n8 位 (P8) 与 4 位 (P4) 调色板会展开为 RGB 或 RGBA\nGrayscale 会展开"
     "为 RGB\n不支持 mipmap"},

    // ===== Spike: context / batch / IO =====
    {"Class not registered!", "类未注册！"},
    {"Invalid call for uni::format codec!", "对 uni::format 编解码器的无效调用！"},
    {"Uncaught exception", "未捕获的异常"},
    {"Dupicate main file in batch group: ", "批处理组中主文件重复: "},
    {"Main file not provided for batch group", "批处理组未提供主文件"},
    {"Expected json bach, got: ", "预期为 json 批处理，实际得到: "},
    {"Invalid path: ", "无效的路径: "},
    {"Invalid call of SupplementalFiles, module is not for batch.",
     "SupplementalFiles 调用无效，该模块不支持批处理。"},
    {"Output path cannot be empty", "输出路径不能为空"},
    {"Generating cache.", "正在生成缓存。"},
    {"Maximum opened files reached!", "已达到可打开文件数上限！"},
    {"Too many files found for pattern: ", "匹配到过多文件，模式: "},
    {"Stream already freed.", "数据流已被释放。"},
    {"Requested stream not found!", "找不到请求的数据流！"},
    {"Cannot find ZIP central directory", "找不到 ZIP 中央目录"},
    {"Cannot find ZIPx64 central directory", "找不到 ZIPx64 中央目录"},
    {"ZIP cannot have encrypted files!", "ZIP 中不能包含加密文件！"},
    {"ZIP cannot have compressed files!", "ZIP 中不能包含压缩文件！"},
    {"ZIP local file's path must be specified!", "必须指定 ZIP 文件的本地路径！"},
    {"Invalid dir entry ", "无效的目录项 "},
    {" at ", " 位于 "},
    {"Cache header and zip checkup are different.", "缓存头与 zip 校验结果不一致。"},
    {"Failed loading cache: ", "加载缓存失败: "},
    {"Found zip cache: ", "找到 zip 缓存: "},
    {"Processing worker data", "正在处理工作线程数据"},
    {"Search end", "搜索结束"},
    {"Search done", "搜索完成"},
    {"Thread wait", "线程等待"},
    {"Thread begin", "线程开始"},
    {"Thread end", "线程结束"},
    {"Thread end wait", "线程结束等待"},
    {"Notifying workers", "正在通知工作线程"},
    {"Waiting for workers", "正在等待工作线程"},
    {"Failed to create wal file.", "创建 wal 文件失败。"},
    {"Failed to open wal file.", "打开 wal 文件失败。"},
    {"Failed to add file to WAL stream", "向 WAL 流添加文件失败"},
    {"NewFile path is empty", "NewFile 路径为空"},
    {"AddFolderPath not supported, use RequiresFolders to check.",
     "不支持 AddFolderPath，请先用 RequiresFolders 检查。"},
    {"GenerateFolders not supported, use RequiresFolders to check.",
     "不支持 GenerateFolders，请先用 RequiresFolders 检查。"},
    {"NX tile error, accessing block out of range",
     "NX 图块错误，访问的块超出范围"},
    {"Implement rgb decode", "需实现 rgb 解码"},
    {"Implement rgba decode", "需实现 rgba 解码"},
    {"Implement channel", "需实现通道处理"},
    {"Failed to decompress BC7 block", "BC7 块解压失败"},
    {"Writing image data beyond buffer's bounds", "写入的图像数据超出缓冲区边界"},
    {"Incomplete dds file", "dds 文件不完整"},
    {"Image format not supported", "不支持该图像格式"},
    {"[Spike] Cannot remove: ", "[Spike] 无法删除: "},
    {"InitTempStorage() not called before RequestTempFile()!",
     "调用 RequestTempFile() 之前未调用 InitTempStorage()！"},
    {"Cannot stat file ", "无法读取文件状态 "},
    {"Cannot map file ", "无法映射文件 "},
    {"Couldn't resize file, code: ", "无法调整文件大小，代码: "},
    {"WinApi call error ", "WinApi 调用错误 "},
    {"Couldn't load XML file <", "无法加载 XML 文件 <"},
    {"More than one root node in XML file", "XML 文件中存在多个根节点"},
    {"Too many bone weights for vertex, max is 8",
     "顶点的骨骼权重过多，最多为 8"},
    {"Floating point mismatch", "浮点数不一致"},
    {"Required field not found", "缺少必需字段"},
    {"Invalid buffer.uri value", "buffer.uri 值无效"},
    {"malformed base64", "base64 格式错误"},
    {"Unknown accessor.type value", "未知的 accessor.type 值"},
    {"Unknown accessor.componentType value", "未知的 accessor.componentType 值"},
    {"Unknown animation.sampler.interpolation value",
     "未知的 animation.sampler.interpolation 值"},
    {"Unknown camera.type value", "未知的 camera.type 值"},
    {"Unknown material.alphaMode value", "未知的 material.alphaMode 值"},
    {"Quota exceeded : number of buffers > MaxBufferCount",
     "超出配额: buffer 数量 > MaxBufferCount"},
    {"Quota exceeded : buffer.byteLength > MaxBufferByteLength",
     "超出配额: buffer.byteLength > MaxBufferByteLength"},
    {"Quota exceeded : file size > MaxFileSize", "超出配额: 文件大小 > MaxFileSize"},
    {"Invalid buffer.byteLength value : 0", "buffer.byteLength 值无效: 0"},
    {"Invalid buffer.byteLength value : does not match buffer.data size",
     "buffer.byteLength 值无效: 与 buffer.data 大小不一致"},
    {"Invalid GLB buffer data", "GLB 缓冲区数据无效"},
    {"Invalid GLB header", "GLB 文件头无效"},
    {"Invalid glTF document. A document must have at least 1 buffer.",
     "glTF 文档无效。文档至少需要 1 个 buffer。"},
    {"Invalid glTF document. Only 1 buffer, the very first, is allowed to have "
     "an empty buffer.uri field.",
     "glTF 文档无效。仅第一个 buffer 允许空 buffer.uri 字段。"},
    {"Invalid glTF document. No buffer found which can meet the criteria for "
     "saving to a .glb file.",
     "glTF 文档无效。没有符合保存为 .glb 文件条件的 buffer。"},
    {"Buffer size does not match number of bytes written!",
     "缓冲区大小与实际写入字节数不一致！"},
    {"Buffer's address must be 8 byte aligned.", "缓冲区地址必须 8 字节对齐。"},
    {"Buffer length expected to be multiple of 8.", "缓冲区长度必须是 8 的倍数。"},
    {"Specified layout was not found!", "找不到指定的布局！"},
    {"Provided stride for uni::format was less than format's stride!",
     "为 uni::format 提供的 stride 小于格式自身的 stride！"},
    {"Invalid component index", "无效的分量索引"},

    // ===== Spike: reflector =====
    {"[Reflector] Invalid value: ", "[Reflector] 无效的值: "},
    {"[Reflector] Integer out of range, got: ", "[Reflector] 整数超出范围，得到: "},
    {" for a signed ", " 而目标是有符号 "},
    {" for an unsigned ", " 而目标是无符号 "},
    {"bit number!", " 位数值！"},
    {"[Reflector] Applying ", "[Reflector] 正在把 "},
    {" to an unsigned integer, casting to: ", " 应用到无符号整数，转换为: "},
    {" to an unsigned float", " 应用到无符号浮点数"},
    {"[Reflector] Float out of range, got: ", "[Reflector] 浮点数超出范围，得到: "},
    {"[Reflector] Expected true/false, got: ",
     "[Reflector] 期望 true/false，实际得到: "},
    {"[Reflector] Enum value not found: ", "[Reflector] 找不到枚举值: "},
    {"[Reflector] Empty input for enum ", "[Reflector] 枚举输入为空 "},
    {"[Reflector] Empty input for enum flag ", "[Reflector] 枚举标志输入为空 "},
    {"[Reflector] Unregistered enum hash: ", "[Reflector] 未注册的枚举哈希: "},
    {" for value: ", " 对应值: "},
    {"[Reflector] Expected ", "[Reflector] 期望 "},
    {" not found.", " 未找到。"},
    {" was found before ", " 出现在 "},
    {" 之前。", " 之前。"},
    {"[Reflector] Empty array input.", "[Reflector] 数组输入为空。"},
    {"[Reflector] Array expected ", "[Reflector] 数组期望 "},
    {" but got ", " 个元素，实际得到 "},
    {"[Reflector] Too many array elements, ", "[Reflector] 数组元素过多，期望 "},
    {" expected.", " 个。"},
    {"[Reflector] Too many vector elements, ",
     "[Reflector] 向量元素过多，期望 "},
    {"[Reflector] Unhandled exception: PrintEnumValue",
     "[Reflector] 未处理的异常: PrintEnumValue"},
    {"[Reflector] Unhandled exception: PrintEnum",
     "[Reflector] 未处理的异常: PrintEnum"},

    // ===== Spike: unit testing =====
    {"Check failed ", "检查失败 "},
    {"Testing ", "正在测试 "},
    {" cases.", " 个用例。"},
    {"Testing result: ", "测试结果: "},
    {" out of ", " / "},
    {" successed.", " 通过。"},
    {"Method ", "方法 "},
    {" failed!", " 失败！"},
    {"Expected throw ", "期望抛出 "},

    // ===== test harness =====
    {"Testing file: ", "正在测试文件: "},
    {"Sample test finished for ", "样本测试完成，共 "},
    {" files with ", " 个文件，"},
    {" tracks.", " 条轨道。"},
    {"Sample tested ", "样本测试完成，共 "},
    {" files & ", " 个文件与 "},
    {" tracks.", " 条轨道。"},

    // ===== HavokLib core =====
    {"nullptr detected", "检测到空指针"},
    {"Unimplemented function", "功能尚未实现"},
    {"Unhandled int compression : 0xC0!", "未处理的整数压缩类型: 0xC0！"},
    {"[Havok] TNAM chunk read too much data, possible incorrect class map, "
     "proceeding to next chunk.",
     "[Havok] TNAM 数据块读取的数据过多，类映射可能有误，继续处理下一个数据块。"},
    {"File is missing type infos.", "文件缺少类型信息。"},
    {"no external compendium to be referenced", "没有可引用的外部索引文件"},
    {"external compendium signature mismatch", "外部索引文件签名不匹配"},
    {"[Havok] Unhandled tag chunk: ", "[Havok] 未处理的数据块标记: "},
    {"Cannot save loaded header! Use IhkPackFile::ToPackFile().",
     "已加载的文件头不能直接保存！请使用 IhkPackFile::ToPackFile()。"},
    {"[Havok] Cannot export unregistered class: ",
     "[Havok] 无法导出未注册的类: "},
    {"Number of blocks != number of block offsets",
     "块数量与块偏移数量不一致"},
    {"[Havok] Unknown layout rule littleEndian, expected [0,1], got: ",
     "[Havok] 未知的布局规则 littleEndian，期望 [0,1]，实际得到: "},
    {"[Havok] Unknown layout rule emptyBaseClassOptimization, expected [0,1], "
     "got: ",
     "[Havok] 未知的布局规则 emptyBaseClassOptimization，期望 [0,1]，实际得到: "},
    {"[Havok] Unknown layout rule reusePaddingOptimization, expected [0,1], "
     "got: ",
     "[Havok] 未知的布局规则 reusePaddingOptimization，期望 [0,1]，实际得到: "},
    {". Handling as true.", "。按 true 处理。"},
    {"hkaBone final was not found", "未找到 hkaBone final"},
    {"[Havok XML] Couldn't export variant: \"", "[Havok XML] 无法导出变体: \""},
    {"\" undefined class: ", "\" 未定义的类: "},

    // ===== Havok2GLTF: enum descriptions =====
    {"Ignore scale tracks.", "忽略缩放轨道。"},
    {"Input scale tracks does not affect child nodes positions.\n"
     "They also doesn't introduce shearing like in MATRICIAL case.\n"
     "They however require modified skeleton. See SkeletonGeneration.",
     "输入的缩放轨道不影响子节点位置。\n"
     "它们也不会像 MATRICIAL 情形那样引入切变。\n"
     "但它们需要修改过的骨骼，"
     "参见 SkeletonGeneration。"},
    {"Do not modify skeleton in any way.",
     "不对骨骼做任何修改。"},
    {"Automatically detect nodes with children where scale animation might "
     "occur.",
     "自动检测带子节点、可能出现缩放动画"
     "的节点。"},
    {"Manually select bones (in gen-cbones) where scale motion occurs and have "
     "child nodes.",
     "在 gen-cbones 中手动指定出现缩放运动、"
     "且带子节点的骨骼。"},

    // ===== Havok2GLTF module =====
    {"Scene UP axis cannot be same as FORWARD axis!",
     "场景的上轴不能与前向轴相同！"},
    {"Unhandled unit type", "未处理的单位类型"},
    {"Unhandled axis type", "未处理的轴向类型"},
    {"Extension pattern space exhausted (limit is 16). Skipping: ",
     "扩展名规则空间已耗尽（上限为 16）。跳过: "},
    {"Bone ", "骨骼 "},
    {" converted to control.", " 已转换为控制骨。"},
    {"Scale type set to NONE, but tool detected scale motions.",
     "缩放类型设置为 NONE，但工具检测到了缩放运动。"},
    {"Scale type set, but tool did not detected scale motions.",
     "已设置缩放类型，但工具没有检测到缩放运动。"},
    {" is not contol bone, scale skipped.", " 不是控制骨，已跳过缩放。"},
    {"No skeleton data found and no skeleton path provided.",
     "未找到骨骼数据，也未提供骨骼文件路径。"},
    {"Even provided skeleton file doesn't contain skeleton data.",
     "即使提供了骨骼文件，其中也不包含骨骼数据。"},
    {"Animation bindings count mismatch with animations. This should never "
     "happen but oh well. Some animations will be skipped.",
     "动画绑定数量与动画数量不一致。这本不该发生，但既然出现了，部分动画将被跳过。"},
    {"Animation bindings not found, animations might be incorrect.",
     "未找到动画绑定，动画结果可能有误。"},
    {"Override blend hint.", "覆盖混合提示。"},
    {"Specify frames per second for GLTF.", "指定 GLTF 的每秒帧数。"},
    {"Select desired processing mode for node scale tracks.",
     "选择节点缩放轨道的处理模式。"},
    {"Use filename for unnamed animations instead of Motion[]",
     "对未命名动画使用文件名代替 Motion[]"},
    {"Specify path to a skeleton hkx.", "指定骨骼 hkx 文件的路径。"},
    {"Set additional skeleton modifications.", "设置额外的骨骼修改。"},
    {"Comma separated bone names where should be applied scale tracks.",
     "以逗号分隔的骨骼名称，缩放轨道将应用到这些骨骼。"},
    {"Create visualization mesh for skeletons. (Enforces armature object for "
     "Blender)",
     "为骨骼创建可视化网格。（在 Blender 中强制生成骨架对象）"},
    {"Force create root node named after `root-node-name` setting. This node "
     "is automatically generated when there is multiple root nodes inside "
     "skeleton.(Separates root motion)",
     "按 `root-node-name` 设置强制创建根节点。当骨骼内部存在多个根节点时会自动"
     "生成该节点。（用于分离根运动）"},
    {"When create-root-node is active, generate root node name from specified "
     "pattern.\n`{SKELETON_NAME}` will be substitued by actual skeleton name. "
     "For example `root_{SKELETON_NAME}_bone` will generate "
     "`root_Refernce_bone` (skeleton name is Reference)",
     "启用 create-root-node 时，按指定模式生成根节点名称。\n"
     "`{SKELETON_NAME}` 会被替换为实际的骨骼名称。例如 "
     "`root_{SKELETON_NAME}_bone` 会生成 `root_Refernce_bone`"
     "（骨骼名称为 Reference）"},
    {"Input scene units.", "输入场景的单位。"},
    {"Set unit scale in case of CUSTOM units.", "使用 CUSTOM 单位时设置单位缩放。"},
    {"Input scene up axis.", "输入场景的上轴。"},
    {"Input scene forward axis.", "输入场景的前向轴。"},
    {"Input scene uses right handed coordiante system.",
     "输入场景使用右手坐标系。"},
    {"Specify extension patterns for file detecting separated by comma.",
     "以逗号分隔指定用于识别文件的扩展名规则。"},

    // ===== Documentation generator =====
    {"### Main file patterns: ", "### 主文件规则: "},
    {"### Secondary file patterns: ", "### 次文件规则: "},
    {"### Input file patterns: ", "### 输入文件规则: "},
    {"### Module command: ", "### 模块命令: "},
    {"**Default value:** ", "**默认值:** "},
    {"**Valid values:** ", "**可选值:** "},
    {"**CLI Long:** ", "**命令行长选项:** "},
    {"**CLI Short:** ", "**命令行短选项:** "},

    {"## ", "## "},
    {"### Module command: ", "### 模块命令: "},
    {"### Settings\n\n", "### 设置\n\n"},
    {"### Settings", "### 设置"},
    {"### Input file patterns: ", "### 输入文件规则: "},
    {"### Main file patterns: ", "### 主文件规则: "},
    {"### Secondary file patterns: ", "### 次文件规则: "},
    {"Module list", "模块列表"},
    {"**CLI Long:** ", "**命令行长选项:** "},
    {"**CLI Short:** ", "**命令行短选项:** "},
    {"**Default value:** ", "**默认值:** "},
    {"**Valid values:** ", "**可选值:** "},
    {"[[TOOLSET DESCRIPTION]]", "[[工具集说明]]"},
    {"[[MODULE DESCRIPTION]]", "[[模块说明]]"},
    {"[[MODULE CLASS NAME]]", "[[模块类名]]"},
};

struct Index {
  std::vector<const Entry *> entries;

  Index() {
    constexpr size_t count = sizeof(table) / sizeof(table[0]);
    entries.reserve(count);

    for (size_t i = 0; i < count; i++) {
      entries.push_back(&table[i]);
    }

    // Longest English literal first, so greedy matching prefers whole
    // sentences over the fragments they are built from.
    std::sort(entries.begin(), entries.end(), [](const Entry *a, const Entry *b) {
      return std::strlen(a->en) > std::strlen(b->en);
    });
  }
};

const Index &GetIndex() {
  static const Index index;
  return index;
}

// Group titles shown by --help and in generated config comments. Kept apart
// from the message table because these keys are also config element names and
// must stay verbatim wherever they are emitted as identifiers.
struct GroupEntry {
  const char *key;
  const char *zh;
};

static const GroupEntry groupTitles[] = {
    {"extract-settings", "解包设置"},
    {"compress-settings", "压缩设置"},
    {"texel-settings", "贴图设置"},
    {"animation", "动画"},
    {"skeleton", "骨骼"},
    {"scene", "场景"},
    {"common", "通用"},
    {"extract-settings", "解包设置"},
};

const Entry *FindExact(std::string_view english) {
  for (const auto *entry : GetIndex().entries) {
    if (english == entry->en) {
      return entry;
    }
  }

  return nullptr;
}
} // namespace

int &LangState() {
  static int state =
      HK_DEFAULT_LANG_EN ? int(Lang::English) : int(Lang::Chinese);
  return state;
}

Lang ParseLanguage(std::string_view name) {
  auto lower = [](std::string_view input) {
    std::string out;
    out.reserve(input.size());

    for (char c : input) {
      if (c >= 'A' && c <= 'Z') {
        c = char(c - 'A' + 'a');
      }
      out.push_back(c);
    }

    return out;
  };

  const std::string value = lower(name);

  if (value.empty()) {
    return Lang::Auto;
  }

  auto contains = [&value](const char *needle) {
    return value.find(needle) != std::string::npos;
  };

  if (contains("zh") || contains("cn") || contains("chs") ||
      contains("chinese") || value.find("\xe4\xb8\xad\xe6\x96\x87") !=
                                 std::string::npos ||
      value.find("\xe7\xae\x80\xe4\xbd\x93") != std::string::npos) {
    return Lang::Chinese;
  }

  if (value == "en" || contains("english") || contains("eng") ||
      value.find("\xe8\x8b\xb1\xe6\x96\x87") != std::string::npos ||
      value.find("\xe8\x8b\xb1\xe8\xaf\xad") != std::string::npos) {
    return Lang::English;
  }

  return Lang::Auto;
}

void InitFromEnvironment() {
  static const char *const vars[] = {"HAVOK_LANG", "HK_LANG", "LANG", nullptr};

  for (size_t i = 0; vars[i]; i++) {
    const char *value = std::getenv(vars[i]);

    if (!value || !*value) {
      continue;
    }

    if (Lang parsed = ParseLanguage(value); parsed != Lang::Auto) {
      SetLanguage(parsed);
      return;
    }
  }
}

std::string TranslateLine(const std::string &source) {
  if (!IsChinese() || source.empty()) {
    return source;
  }

  const Index &index = GetIndex();
  std::string result;
  result.reserve(source.size() + source.size() / 2);

  size_t pos = 0;

  while (pos < source.size()) {
    const Entry *hit = nullptr;

    for (const auto *entry : index.entries) {
      const size_t len = std::strlen(entry->en);
      const std::string_view slice(source.data() + pos, source.size() - pos);

      if (slice.compare(0, len, entry->en) == 0) {
        hit = entry;
        break;
      }
    }

    if (hit) {
      result += hit->zh;
      pos += std::strlen(hit->en);
    } else {
      result.push_back(source[pos]);
      pos++;
    }
  }

  return result;
}

void TranslateInPlace(std::string &text) {
  if (!IsChinese() || text.empty()) {
    return;
  }

  // Fast path: pure ASCII text without any registered fragment cannot be
  // translated, and files/dates/numbers dominate log traffic.
  if (TranslateLine(text) == text) {
    return;
  }

  text = TranslateLine(text);
}

const char *Tr(const char *english) {
  if (!english || !IsChinese()) {
    return english;
  }

  if (const Entry *entry = FindExact(english); entry) {
    return entry->zh;
  }

  return english;
}

const char *GroupTitle(const char *key) {
  if (!key || !IsChinese()) {
    return key;
  }

  for (const auto &entry : groupTitles) {
    if (std::strcmp(entry.key, key) == 0) {
      return entry.zh;
    }
  }

  return key;
}

size_t TranslationCount() { return sizeof(table) / sizeof(table[0]); }
} // namespace es::i18n
