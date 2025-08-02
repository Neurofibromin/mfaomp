#include "BackEndEnum.h"

std::optional<BackEndManager::BackEnd> BackEndManager::fromString(const std::string& str) {
    const auto& map = getBackEndMetaDataMap();
    for (const auto& pair : map) {
        if (pair.second.getName() == str) {
            return pair.first;
        }
    }
    return std::nullopt;
}

std::vector<BackEndManager::BackEnd> BackEndManager::getAvailableRuntimeBackEnds() {
    std::vector<BackEnd> available;
    std::ranges::for_each(AllBackEnds, [&available](BackEnd backend) -> void {
        if (isBackendAvailableRuntime(backend)) {
            available.push_back(backend);
        }
    } );
    return available;
}