// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#if defined(__APPLE__)

#import "AUPlugin.hpp"
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>

// Forward declaration of the BODYAudioUnit class defined in AUPlugin.mm
@class BODYAudioUnit;

namespace {

/// @brief Registers all BODY AU plugins with the AudioComponent system at load time
struct BODYAURegistrar {
    BODYAURegistrar() {
        auto& regs = body::AURegistry::instance().getRegistrations();
        for (const auto& reg : regs) {
            // Build AudioComponentDescription from 4-char codes
            AudioComponentDescription desc = {};
            desc.componentType = fourCharCode(reg.componentType);
            desc.componentSubType = fourCharCode(reg.componentSubType);
            desc.componentManufacturer = fourCharCode(reg.componentManufacturer);

            NSString* name = [NSString stringWithUTF8String:reg.name.c_str()];

            [AUAudioUnit registerSubclass:[BODYAudioUnit class]
                     asComponentDescription:desc
                                       name:name
                                    version:1];
        }
    }

private:
    static UInt32 fourCharCode(const std::string& s) {
        UInt32 code = 0;
        if (s.size() >= 4) {
            code |= static_cast<UInt32>(s[0]) << 24;
            code |= static_cast<UInt32>(s[1]) << 16;
            code |= static_cast<UInt32>(s[2]) << 8;
            code |= static_cast<UInt32>(s[3]);
        }
        return code;
    }
} gAURegistrar;

} // namespace

#endif // __APPLE__
