#ifndef RESOURCE_H
#define RESOURCE_H
#include <map>
#include <cstdio>

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
            puts("set resource");
            resources[type] = amount;
        };
        
        void add_resource(ResourceType type, double amount) {
            puts("add resource");
            resources[type] += amount;
        };

        void sub_resource(ResourceType type, double amount) {
            puts("sub resource");
            resources[type] -= amount;
        };
    private:
        std::map<ResourceType, double> resources;
};
#endif
