// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "ClapPlugin.hpp"
#include <vector>

namespace body {

/// @brief Plugin registration system for CLAP
///
/// Plugins register themselves via registerPlugin(). The CLAP entry point
/// and factory use this registry to expose plugins to the host.
class ClapRegistry {
public:
    /// @brief Get the global singleton registry
    [[nodiscard]] static ClapRegistry& instance();

    /// @brief Register a plugin for CLAP hosting
    void registerPlugin(ClapPlugin::Registration registration);

    /// @brief Get all registered plugins
    [[nodiscard]] const std::vector<ClapPlugin::Registration>& getRegistrations() const noexcept;

private:
    ClapRegistry() = default;
    std::vector<ClapPlugin::Registration> registrations_;
};

} // namespace body

/// @brief Macro to register a BODY plugin for CLAP export
/// @param PluginClass The AudioProcessor subclass
/// @param pluginId Unique CLAP plugin ID string (e.g. "com.body.gain")
/// @param pluginName Display name
/// @param pluginVendor Vendor name
/// @param pluginVersion Version string
/// @param pluginDescription Short description
/// @param ... Feature strings (null-terminated array)
#define BODY_REGISTER_CLAP_PLUGIN(PluginClass, pluginId, pluginName, pluginVendor, \
                                  pluginVersion, pluginDescription, ...) \
    namespace { \
    static const char* const features_##PluginClass[] = {__VA_ARGS__, nullptr}; \
    struct Register_##PluginClass { \
        Register_##PluginClass() { \
            body::ClapPlugin::Registration reg; \
            reg.descriptor.clap_version = CLAP_VERSION; \
            reg.descriptor.id = pluginId; \
            reg.descriptor.name = pluginName; \
            reg.descriptor.vendor = pluginVendor; \
            reg.descriptor.url = ""; \
            reg.descriptor.manual_url = ""; \
            reg.descriptor.support_url = ""; \
            reg.descriptor.version = pluginVersion; \
            reg.descriptor.description = pluginDescription; \
            reg.descriptor.features = features_##PluginClass; \
            reg.factory = []() -> std::unique_ptr<body::AudioProcessor> { \
                return std::make_unique<PluginClass>(); \
            }; \
            body::ClapRegistry::instance().registerPlugin(std::move(reg)); \
        } \
    } register_instance_##PluginClass; \
    }
