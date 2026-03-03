// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "ClapEntry.hpp"
#include <cstring>

namespace body {

ClapRegistry& ClapRegistry::instance() {
    static ClapRegistry registry;
    return registry;
}

void ClapRegistry::registerPlugin(ClapPlugin::Registration registration) {
    registrations_.push_back(std::move(registration));
}

const std::vector<ClapPlugin::Registration>& ClapRegistry::getRegistrations() const noexcept {
    return registrations_;
}

} // namespace body

// --- CLAP Factory ---

static uint32_t factoryGetPluginCount(const clap_plugin_factory* /*factory*/) {
    return static_cast<uint32_t>(body::ClapRegistry::instance().getRegistrations().size());
}

static const clap_plugin_descriptor_t* factoryGetPluginDescriptor(
    const clap_plugin_factory* /*factory*/, uint32_t index) {
    auto& regs = body::ClapRegistry::instance().getRegistrations();
    if (index >= static_cast<uint32_t>(regs.size())) return nullptr;
    return &regs[index].descriptor;
}

static const clap_plugin_t* factoryCreatePlugin(
    const clap_plugin_factory* /*factory*/,
    const clap_host_t* host,
    const char* pluginId) {
    auto& regs = body::ClapRegistry::instance().getRegistrations();
    for (const auto& reg : regs) {
        if (strcmp(reg.descriptor.id, pluginId) == 0) {
            return body::ClapPlugin::create(reg, host);
        }
    }
    return nullptr;
}

static const clap_plugin_factory_t sFactory = {
    factoryGetPluginCount,
    factoryGetPluginDescriptor,
    factoryCreatePlugin
};

// --- CLAP Entry Point ---

static bool clapEntryInit(const char* /*pluginPath*/) {
    return true;
}

static void clapEntryDeinit() {
}

static const void* clapEntryGetFactory(const char* factoryId) {
    if (strcmp(factoryId, CLAP_PLUGIN_FACTORY_ID) == 0)
        return &sFactory;
    return nullptr;
}

extern "C" {
    CLAP_EXPORT const clap_plugin_entry_t clap_entry = {
        CLAP_VERSION,
        clapEntryInit,
        clapEntryDeinit,
        clapEntryGetFactory
    };
}
