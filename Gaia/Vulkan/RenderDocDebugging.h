#ifndef RENDERDOCDEBUGGING_H
#define RENDERDOCDEBUGGING_H

#include <string>
#include <assert.h>
#include <Windows.h>
#include <iostream>

#include <vector>
#include <vulkan/vulkan.h>

void EnableDebug(VkPhysicalDevice physicalDevice, VkDevice logicalDevice);

void SetObjectName(VkDevice device, uint64_t object, VkDebugReportObjectTypeEXT objectType, const char* name);

void SetObjectTag(VkDevice device, uint64_t object, VkDebugReportObjectTypeEXT objectType, uint64_t name, size_t tagSize, const void* tag);

#endif // !RENDERDOCDEBUGGING_H
