#pragma once

/********* fontset-fontconfig.inl *********/

/// Turn on by default logic to throw away the trailing common subsequence between all font matches
/// Even if this is not enabled, you can still manually turn it on by calling `FontConfigResolver::ComputeExclusionList()`
//#define TXTVIEW_FONTCONFIG_DEFAULT_EXCLUSIONS
/// Turn on debug logging
//#define TXTVIEW_DEBUG_FONTCONFIG

/********* text.cpp *********/
#define TXTVIEW_TEXT_DOCUMENT_DEBUG
