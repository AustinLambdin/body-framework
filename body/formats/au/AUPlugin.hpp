// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#if defined(__APPLE__)

#include "body/core/AudioProcessor.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace body {

/// @brief Registration info for an Audio Unit plugin
struct AURegistration {
    std::string componentType;        // e.g. "aufx" for effect
    std::string componentSubType;     // 4-char code e.g. "gain"
    std::string componentManufacturer; // 4-char code e.g. "Body"
    std::string name;                 // Display name "Vendor: PluginName"
    std::string version;
    AudioProcessorFactory factory;
};

/// @brief Singleton registry of AU plugins for this module
class AURegistry {
public:
    [[nodiscard]] static AURegistry& instance();
    void registerPlugin(AURegistration registration);
    [[nodiscard]] const std::vector<AURegistration>& getRegistrations() const noexcept;

private:
    AURegistry() = default;
    std::vector<AURegistration> registrations_;
};

} // namespace body

/// @brief Register a BODY plugin for Audio Unit export
/// @param PluginClass The AudioProcessor subclass
/// @param compType Component type (e.g. "aufx")
/// @param compSubType Component subtype (e.g. "gain")
/// @param compMfr Component manufacturer (e.g. "Body")
/// @param pluginName Display name (e.g. "BODY: Gain")
/// @param pluginVersion Version string
#define BODY_REGISTER_AU_PLUGIN(PluginClass, compType, compSubType, compMfr, \
                                 pluginName, pluginVersion) \
    namespace { \
    struct AURegister_##PluginClass { \
        AURegister_##PluginClass() { \
            body::AURegistration reg; \
            reg.componentType = compType; \
            reg.componentSubType = compSubType; \
            reg.componentManufacturer = compMfr; \
            reg.name = pluginName; \
            reg.version = pluginVersion; \
            reg.factory = []() -> std::unique_ptr<body::AudioProcessor> { \
                return std::make_unique<PluginClass>(); \
            }; \
            body::AURegistry::instance().registerPlugin(std::move(reg)); \
        } \
    } au_register_instance_##PluginClass; \
    }

#endif // __APPLE__
