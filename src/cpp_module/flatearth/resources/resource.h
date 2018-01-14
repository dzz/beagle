#ifndef RESOURCE_H
#define RESOURCE_H
#include <map>

enum ResourceType: int {
    WOOD,
    STONE
};

class StageResource {
    public:
        double get_resource(ResourceType type) {
            return resources[type];
        };
        void set_resource(ResourceType type, double amount) {
            resources[type] = amount;
        };
    private:
        std::map<ResourceType, double> resources;
};
#endif
