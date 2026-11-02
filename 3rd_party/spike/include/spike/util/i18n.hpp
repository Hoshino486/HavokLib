/*  i18n: process wide UI language switch for the Havok toolset

    Simplified Chinese (zh-CN) is the default; English is a first class
    alternative, so one binary serves both. Switching happens at runtime and
    takes effect immediately, including already buffered log lines.

    Language resolution, last writer wins:
      1. compile time default : -DHK_DEFAULT_LANG_EN=1 makes English default
      2. environment         : HAVOK_LANG, then HK_LANG, then LANG
      3. command line        : --language <zh|en>  (-l / --lang accepted)
      4. toolset config      : <language>zh</language> under <common>

    Translation strategy: every user visible message passes through the single
    printer sink, so translation is applied once, on the finished line, using
    an ordered table of English -> Chinese fragments. Messages that were built
    by stream concatenation are covered by matching their fragments; whole
    sentences get an exact entry and therefore win over fragment matches.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0
*/

#pragma once
#include "spike/util/settings.hpp"
#include <string>
#include <string_view>

namespace es::i18n {
enum class Lang : int { Auto = 0, Chinese = 1, English = 2 };

PC_EXTERN int &LangState();

inline void SetLanguage(Lang lang) {
  if (lang != Lang::Auto) {
    LangState() = static_cast<int>(lang);
  }
}

inline Lang GetLanguage() { return static_cast<Lang>(LangState()); }
inline bool IsChinese() { return LangState() == int(Lang::Chinese); }
inline bool IsEnglish() { return LangState() == int(Lang::English); }
inline const char *LangName() { return IsChinese() ? "zh-CN" : "en-US"; }

// zh, cn, chs, zh-CN, zh_CN, zh-Hans, chinese, 中文, 简体  ->  Chinese
// en, eng, en-US, en_GB, english, 英文, 英语            ->  English
// anything else                                       ->  Auto
PC_EXTERN Lang ParseLanguage(std::string_view name);

// HAVOK_LANG -> HK_LANG -> LANG; no-op when unset or unparsable.
PC_EXTERN void InitFromEnvironment();

// ~~~~~ message translation ~~~~~
// Translates one finished console line/fragment. Whole-line entries are tried
// first, then the longest matching fragments. Returns the input untouched in
// English mode or when nothing matches.
PC_EXTERN std::string TranslateLine(const std::string &source);

// In place variant used by the printer sinks.
PC_EXTERN void TranslateInPlace(std::string &text);

// Literal translation for reflected descriptions and other const char* data
// that is rendered at print time. Returns the source when no entry exists.
PC_EXTERN const char *Tr(const char *english);

inline const char *TrOr(const char *english, const char *fallback = "") {
  return english ? Tr(english) : fallback;
}

// Display title for a setting group key ("animation", "skeleton", "scene",
// "extract-settings", ...). Keys double as config file element names, so they
// must never be rewritten in place; only the printed title is localized.
PC_EXTERN const char *GroupTitle(const char *key);

// Diagnostics used by the localization self test.
PC_EXTERN size_t TranslationCount();

} // namespace es::i18n

// Shorthand. TR is the terse form used inside message macros and at render
// sites where the text is a single complete literal.
#define TR(en) ::es::i18n::Tr(en)

// Two literal form, for cases where Chinese word order differs too much to be
// produced by fragment translation.
#define I18N(zh, en) (::es::i18n::IsChinese() ? (zh) : (en))
