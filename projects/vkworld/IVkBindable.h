#ifndef VKWORLD_IVKBINDINABLE_H
#define VKWORLD_IVKBINDINABLE_H

#include "VkWorldConfig.h"

namespace vk {
class DescriptorSet;
enum class DescriptorType;
} // namespace vk

namespace world {

/** Any class which gives access to a memory space on the GPU. */
class VKWORLD_EXPORT IVkBindable {
public:
    virtual ~IVkBindable() = default;

    virtual void registerTo(vk::DescriptorSet &descriptorSet,
                            vk::DescriptorType descriptorType, u32 id) = 0;
};

}; // namespace world

#endif // VKWORLD_IVKBINDINABLE_H