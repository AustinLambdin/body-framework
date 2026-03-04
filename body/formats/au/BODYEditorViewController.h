// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#if defined(__APPLE__)

#import <AppKit/AppKit.h>
#include "body/gui/PluginEditor.hpp"
#include <memory>

@interface BODYEditorViewController : NSViewController
- (instancetype)initWithEditor:(std::unique_ptr<body::PluginEditor>)editor;
@end

#endif // __APPLE__
