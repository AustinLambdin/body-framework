// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "Vst3Entry.hpp"
#include "public.sdk/source/main/pluginfactory.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"

namespace body {

Vst3Registry& Vst3Registry::instance() {
    static Vst3Registry registry;
    return registry;
}

void Vst3Registry::registerPlugin(Vst3Registration registration) {
    registrations_.push_back(std::move(registration));
}

const std::vector<Vst3Registration>& Vst3Registry::getRegistrations() const noexcept {
    return registrations_;
}

} // namespace body

// --- Module init / deinit (required by VST3 SDK) ---

bool InitModule() { return true; }
bool DeinitModule() { return true; }

// --- VST3 Factory ---

BEGIN_FACTORY("BODY Framework",    // vendor
              "",                  // url
              "",                  // email
              Steinberg::PFactoryInfo::kUnicode)

    auto& regs = body::Vst3Registry::instance().getRegistrations();
    for (const auto& reg : regs) {
        // Create a Vst3Plugin that stores the factory from this registration
        auto createFunc = [](void* context) -> Steinberg::FUnknown* {
            auto* regPtr = static_cast<body::Vst3Registration*>(context);
            auto* plugin = new body::Vst3Plugin();
            plugin->setFactory(regPtr->factory);
            return static_cast<Steinberg::Vst::IAudioProcessor*>(plugin);
        };

        Steinberg::PClassInfo2 componentClass(
            reg.uid,
            Steinberg::PClassInfo::kManyInstances,
            kVstAudioEffectClass,
            reg.name.c_str(),
            0,                       // classFlags
            reg.subcategories.c_str(),
            reg.vendor.c_str(),
            reg.version.c_str(),
            kVstVersionString);

        // We need a stable pointer to the registration for the factory context
        // The registrations vector is stable once factory is called
        gPluginFactory->registerClass(
            &componentClass,
            createFunc,
            const_cast<void*>(static_cast<const void*>(&reg)));
    }

END_FACTORY
