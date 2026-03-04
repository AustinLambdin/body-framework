// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "Vst3Plugin.hpp"
#include "pluginterfaces/base/funknown.h"
#include <vector>

namespace body {

/// @brief Registration info for a VST3 plugin
struct Vst3Registration {
    Steinberg::FUID uid;
    std::string name;
    std::string vendor;
    std::string version;
    std::string subcategories;
    AudioProcessorFactory factory;
};

/// @brief Singleton registry of VST3 plugins for this module
class Vst3Registry {
public:
    [[nodiscard]] static Vst3Registry& instance();
    void registerPlugin(Vst3Registration registration);
    [[nodiscard]] const std::vector<Vst3Registration>& getRegistrations() const noexcept;

private:
    Vst3Registry() = default;
    std::vector<Vst3Registration> registrations_;
};

} // namespace body

/// @brief Register a BODY plugin for VST3 export
/// @param PluginClass The AudioProcessor subclass
/// @param uid1,uid2,uid3,uid4 FUID as 4 hex uint32 values
/// @param pluginName Display name
/// @param pluginVendor Vendor name
/// @param pluginVersion Version string
/// @param pluginSubcategories VST3 subcategory string (e.g. "Fx|Dynamics")
#define BODY_REGISTER_VST3_PLUGIN(PluginClass, uid1, uid2, uid3, uid4, \
                                   pluginName, pluginVendor, pluginVersion, \
                                   pluginSubcategories) \
    namespace { \
    struct Vst3Register_##PluginClass { \
        Vst3Register_##PluginClass() { \
            body::Vst3Registration reg; \
            reg.uid = Steinberg::FUID(uid1, uid2, uid3, uid4); \
            reg.name = pluginName; \
            reg.vendor = pluginVendor; \
            reg.version = pluginVersion; \
            reg.subcategories = pluginSubcategories; \
            reg.factory = []() -> std::unique_ptr<body::AudioProcessor> { \
                return std::make_unique<PluginClass>(); \
            }; \
            body::Vst3Registry::instance().registerPlugin(std::move(reg)); \
        } \
    } vst3_register_instance_##PluginClass; \
    }
