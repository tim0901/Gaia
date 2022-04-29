#include "RenderDocDebugging.h"

#if VULKAN_DEBUGGING

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan.hpp>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#endif // VULKAN_DEBUGGING

#include "../Dependencies/renderdoc_app.h"
RENDERDOC_API_1_1_2* rdoc_api = NULL;

PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTag = VK_NULL_HANDLE;
PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectName = VK_NULL_HANDLE;
PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBegin = VK_NULL_HANDLE;
PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEnd = VK_NULL_HANDLE;
PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsert = VK_NULL_HANDLE;

static bool active = false;
static bool extensionPresent = false;

void EnableDebug(VkPhysicalDevice physicalDevice, VkDevice logicalDevice) {


	if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI =
			(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&rdoc_api);
		assert(ret == 1);
	}

	// Check if the debug marker extension is present (which is the case if run from a graphics debugger)
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());
	for (VkExtensionProperties extension : extensions) {
		if (strcmp(extension.extensionName, VK_EXT_DEBUG_MARKER_EXTENSION_NAME) == 0) {
			extensionPresent = true;
			break;
		}
	}

	if (extensionPresent) {
		// The debug marker extension is not part of the core, so function pointers need to be loaded manually
		vkDebugMarkerSetObjectTag = (PFN_vkDebugMarkerSetObjectTagEXT)vkGetDeviceProcAddr(logicalDevice, "vkDebugMarkerSetObjectTagEXT");
		vkDebugMarkerSetObjectName = (PFN_vkDebugMarkerSetObjectNameEXT)vkGetDeviceProcAddr(logicalDevice, "vkDebugMarkerSetObjectNameEXT");
		vkCmdDebugMarkerBegin = (PFN_vkCmdDebugMarkerBeginEXT)vkGetDeviceProcAddr(logicalDevice, "vkCmdDebugMarkerBeginEXT");
		vkCmdDebugMarkerEnd = (PFN_vkCmdDebugMarkerEndEXT)vkGetDeviceProcAddr(logicalDevice, "vkCmdDebugMarkerEndEXT");
		vkCmdDebugMarkerInsert = (PFN_vkCmdDebugMarkerInsertEXT)vkGetDeviceProcAddr(logicalDevice, "vkCmdDebugMarkerInsertEXT");
		// Set flag if at least one function pointer is present
		active = (vkDebugMarkerSetObjectName != VK_NULL_HANDLE);
	}
	else {
		//std::cout << "Warning: " << VK_EXT_DEBUG_MARKER_EXTENSION_NAME << " not present, debug markers are disabled.";
		//std::cout << "Try running from inside a Vulkan graphics debugger (e.g. RenderDoc)" << std::endl;
	}
}

void SetObjectName(VkDevice device, uint64_t object, VkDebugReportObjectTypeEXT objectType, const char* name)
{
	// Check for valid function pointer (may not be present if not running in a debugging application)
	if (active)
	{
		VkDebugMarkerObjectNameInfoEXT nameInfo = {};
		nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
		nameInfo.objectType = objectType;
		nameInfo.object = object;
		nameInfo.pObjectName = name;
		vkDebugMarkerSetObjectName(device, &nameInfo);
	}
}

void SetObjectTag(VkDevice device, uint64_t object, VkDebugReportObjectTypeEXT objectType, uint64_t name, size_t tagSize, const void* tag) {

	// Check for valid function pointer (may not be present if not running in a debugging application)
	if (active)
	{
		VkDebugMarkerObjectTagInfoEXT tagInfo = {};
		tagInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_TAG_INFO_EXT;
		tagInfo.objectType = objectType;
		tagInfo.object = object;
		tagInfo.tagName = name;
		tagInfo.tagSize = tagSize;
		tagInfo.pTag = tag;
		vkDebugMarkerSetObjectTag(device, &tagInfo);
	}
}