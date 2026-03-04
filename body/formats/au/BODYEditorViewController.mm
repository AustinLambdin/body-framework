// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#if defined(__APPLE__)

#import "BODYEditorViewController.h"
#include "body/gui/platform/mac/PlatformViewMac.hpp"

@interface BODYEditorViewController () {
    std::unique_ptr<body::PlatformViewMac> _platformView;
}
@end

@implementation BODYEditorViewController

- (instancetype)initWithEditor:(std::unique_ptr<body::PluginEditor>)editor {
    self = [super initWithNibName:nil bundle:nil];
    if (self) {
        _platformView = std::make_unique<body::PlatformViewMac>(std::move(editor));
    }
    return self;
}

- (void)loadView {
    if (_platformView) {
        NSView* editorView = (__bridge NSView*)_platformView->getNativeView();
        self.view = editorView;
        self.preferredContentSize = NSMakeSize(_platformView->getWidth(),
                                                _platformView->getHeight());
    } else {
        self.view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 400, 300)];
    }
}

- (void)dealloc {
    _platformView.reset();
}

@end

#endif // __APPLE__
