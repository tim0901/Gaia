#include "VulkanViewport.h"

// Load file into byte array
std::vector<char> VulkanViewport::ReadShader(const std::string& filename) const {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

VulkanViewport::QueueFamilyIndices VulkanViewport::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) const {

    QueueFamilyIndices indices;

    // Find how many families there are
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    // Fetch their information
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // We need at least one family that supports VK_QUEUE_GRAPHICS BIT
    for (int i = 0; i < queueFamilies.size(); i++) {
        if (queueFamilies.at(i).queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport)
            indices.presentFamily = i;

        // We want a dedicated queue for transfer
        if ((queueFamilies.at(i).queueFlags & VK_QUEUE_TRANSFER_BIT) && ((queueFamilies.at(i).queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamilies.at(i).queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)) {
            indices.transferFamily = i;
        }

        // We also want a dedicated queue for Compute
        if (queueFamilies.at(i).queueFlags & VK_QUEUE_COMPUTE_BIT && ((queueFamilies.at(i).queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
            indices.computeFamily = i;
        }

        if (indices.IsComplete())
            break;
    }
    return indices;
}

VulkanViewport::SwapChainSupportDetails VulkanViewport::QuerySwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) const {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }
    return details;
}

bool VulkanViewport::CheckDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*> deviceExtensions) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    // Obtain a vector of the extensions supported by the device
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    // Remove each supported extension from the list of required extensions
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    // If there are no extensions left in requiredExtensions, then the device supports every extension that we need
    return requiredExtensions.empty();
}

void VulkanViewport::RecreateSwapChain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(logicalDevice);

    CleanupSwapChain();

    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateGraphicsPipeline();
    CreateFramebuffers();
}

uint32_t VulkanViewport::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type");
};

void VulkanViewport::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndicesArray[] = { queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.transferFamily.value() };

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    bufferInfo.queueFamilyIndexCount = 2;
    bufferInfo.pQueueFamilyIndices = queueFamilyIndicesArray;

    if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create vertex buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate vertex buffer memory");
    }

    if (vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("Failed to bind buffer memory");
    }
}

void VulkanViewport::CopyBuffer(VkBuffer sourceBuffer, VkBuffer destinationBuffer, VkDeviceSize size) {

    // Create temporary command buffer
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

    SetObjectName(logicalDevice, (uint64_t)commandBuffer, VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT, "copyCommandBuffer");

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, sourceBuffer, destinationBuffer, 1, &copyRegion);

    // Execute commands
    EndSingleTimeCommands(commandBuffer);
}

VkCommandBuffer VulkanViewport::BeginSingleTimeCommands() {
    VkCommandBufferAllocateInfo bufferAllocInfo{};
    bufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    bufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    bufferAllocInfo.commandPool = graphics.commandPool;
    bufferAllocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(logicalDevice, &bufferAllocInfo, &commandBuffer);

    // Start recording to command buffer
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // We only plan on using this once

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanViewport::EndSingleTimeCommands(VkCommandBuffer commandBuffer) {
    // End command buffer
    vkEndCommandBuffer(commandBuffer);

    // Execute command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphics.queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics.queue);

    // Cleanup
    vkFreeCommandBuffers(logicalDevice, graphics.commandPool, 1, &commandBuffer);
}

void VulkanViewport::TransitionImageLayout(VkImage img, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
    SetObjectName(logicalDevice, (uint64_t)commandBuffer, VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT, "transitionImageLayout command buffer");

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = img;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    EndSingleTimeCommands(commandBuffer);
}

void VulkanViewport::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    EndSingleTimeCommands(commandBuffer);
}

void VulkanViewport::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& img, VkDeviceMemory& imageMemory) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(width);
    imageInfo.extent.height = static_cast<uint32_t>(height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = 0;

    if (vkCreateImage(logicalDevice, &imageInfo, nullptr, &img) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalDevice, img, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(logicalDevice, img, imageMemory, 0);
}

VkImageView VulkanViewport::CreateImageView(VkImage img, VkFormat format) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = img;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(logicalDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
    return imageView;
}

VkDescriptorPoolSize VulkanViewport::CreateDescriptorPoolSize(const VkDescriptorType type, const uint32_t count) const {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = type;
    poolSize.descriptorCount = count;
    return poolSize;
}

VkWriteDescriptorSet VulkanViewport::CreateWriteDescriptorSet(const VkDescriptorSet descriptorSet, const VkDescriptorType descriptorType, const uint32_t binding, const VkDescriptorBufferInfo* bufferInfo, const uint32_t descriptorCount) const {
    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = descriptorSet;
    writeDescriptorSet.descriptorType = descriptorType;
    writeDescriptorSet.dstBinding = binding;
    writeDescriptorSet.pBufferInfo = bufferInfo;
    writeDescriptorSet.descriptorCount = descriptorCount;
    return writeDescriptorSet;
}

VkWriteDescriptorSet VulkanViewport::CreateWriteDescriptorSet(const VkDescriptorSet descriptorSet, const VkDescriptorType descriptorType, const uint32_t binding, const VkDescriptorImageInfo* imageInfo, const uint32_t descriptorCount) const {
    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = descriptorSet;
    writeDescriptorSet.descriptorType = descriptorType;
    writeDescriptorSet.dstBinding = binding;
    writeDescriptorSet.pImageInfo = imageInfo;
    writeDescriptorSet.descriptorCount = descriptorCount;
    return writeDescriptorSet;
}

void VulkanViewport::CreateInstance(const std::string applicationName) {

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = applicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    createInfo.enabledLayerCount = 0;
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        // Instance creation failed
        throw std::runtime_error("Failed to create Vulkan instance\n");
    }
}

void VulkanViewport::FindPhysicalDevice() {

    // Look for compatible devices
    uint32_t deviceCount = 0;

    // Fetch number of GPUs present
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPU with Vulkan support\n");
    }

    // Fetch GPUs
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());


    // Is this device suitable for our use?
    auto DeviceSuitable = [this](VkPhysicalDevice device) {

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        // Check that the device supports the families we need
        QueueFamilyIndices indices = FindQueueFamilies(device, surface);
        if (!indices.IsComplete())
            return false;

        // Check for swapchain support
        bool extensionsSupported = CheckDeviceExtensionSupport(device, deviceExtensions);
        if (extensionsSupported) {
            // Check further details of swapchain support
            bool swapChainAdequate = false;
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupportDetails(device, surface);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

            if (!swapChainAdequate)
                return false;
        }
        else {
            return false;
        }

        // If we can, we want to use the efficient iGPU in preference to the dGPU - we don't need a viewport to run at 1000000fps
        if (physicalDevice == VK_NULL_HANDLE) {
            // We haven't chosen a device yet
            return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU || deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        }
        else {
            // We've got a device already, so only return true if this device is the iGPU.
            return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
        }
    };

    // Find the best GPU for our use
    for (const VkPhysicalDevice device : devices) {
        if (DeviceSuitable(device)) {
            physicalDevice = device;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find GPU with vulkan support\n");
    }

    VkPhysicalDeviceProperties deviceProperties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    std::cout << "Chosen Vulkan device: " << deviceProperties.deviceName << "\n";

}

void VulkanViewport::CreateLogicalDevice() {

    // Now that we have our physical device, we need a logical device to interface with it

    // So first we need to describe the number of queues that we want for each queue family.
    queueFamilyIndices = FindQueueFamilies(physicalDevice, surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentFamily.value(), queueFamilyIndices.computeFamily.value(), queueFamilyIndices.transferFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    // Create logical device
    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS)
        throw std::runtime_error("Failed to create Vulkan logical device");

    // Fetch the handle for the queues
    vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphicsFamily.value(), 0, &graphics.queue);
    vkGetDeviceQueue(logicalDevice, queueFamilyIndices.presentFamily.value(), 0, &presentQueue);
    vkGetDeviceQueue(logicalDevice, queueFamilyIndices.computeFamily.value(), 0, &compute.queue);
    vkGetDeviceQueue(logicalDevice, queueFamilyIndices.transferFamily.value(), 0, &transfer.queue);
}

void VulkanViewport::CreateSwapChain() {

    // Create the swap chain
    // Vulkan doesn't have default render buffers - we need to create this infrastructure ourselves
    // This is known as the swap chain

    auto ChooseSwapSurfaceFormat = [](const std::vector<VkSurfaceFormatKHR>& availableFormats) {

        // Search through the available swap surface formats and return the desired combination
        for (const VkSurfaceFormatKHR& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_R16G16B16A16_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    };

    auto ChooseSwapPresentMode = [](const std::vector<VkPresentModeKHR>& availablePresentModes) {
        // We don't need this to run at 100fps - leave the framerate locked
        return VK_PRESENT_MODE_FIFO_KHR;
    };

    auto ChooseSwapExtent = [this](const VkSurfaceCapabilitiesKHR& capabilities) {
        // Swap extent is the resolution of the swap chain images

        // To account for resolution scaling, we use the frame buffer size provided by GLFW

        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D extent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return extent;
        }

    };

    // Fetch the desired properties for the swap chain
    SwapChainSupportDetails swapChainDetails = QuerySwapChainSupportDetails(this->physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainDetails.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainDetails.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainDetails.capabilities);

    uint32_t imageCount = swapChainDetails.capabilities.minImageCount + 1;

    if (swapChainDetails.capabilities.maxImageCount > 0 && imageCount > swapChainDetails.capabilities.maxImageCount) {
        imageCount = swapChainDetails.capabilities.maxImageCount;
    }

    // Fill the traditional massive struct
    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = surface;
    swapChainCreateInfo.minImageCount = imageCount;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = extent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndicesArray[] = { queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentFamily.value() };

    if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.presentFamily) {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndicesArray;
    }
    else {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    // We can perform operations eg flip on the swap chain. currentTransform means no change
    swapChainCreateInfo.preTransform = swapChainDetails.capabilities.currentTransform;

    // Should the alpha channel be used for blending with other windows in the window system?
    // No. 
    // VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR means we ignore alpha channel data
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.clipped = VK_TRUE; // Allow for pixel clipping, where pixels are not rendered if obscured by other windows
    //swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(logicalDevice, &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swap chain");
    }

    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}

void VulkanViewport::CreateImageViews() {

    swapChainImageViews.resize(swapChainImages.size());

    for (uint32_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] = CreateImageView(swapChainImages[i], swapChainImageFormat);
    }
}

void VulkanViewport::CreateRenderPass() {
    // We need to tell Vulkan about the framebuffer attachments that will be used
    // eg how many colour/depth buffers we're using, how their contents should be used

    VkAttachmentDescription colourAttachment{};
    colourAttachment.format = swapChainImageFormat;
    colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colourAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }

}

void VulkanViewport::CreateComputeDescriptorPool() {
    
    // We have a storage buffer for the pixel input, a uniform buffer for the image dimensions
    // and an image to be written to
    std::vector<VkDescriptorPoolSize> computePoolSizes{
        CreateDescriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1),
        CreateDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1),
        CreateDescriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1)
    };

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = static_cast<uint32_t>(computePoolSizes.size());
    createInfo.pPoolSizes = computePoolSizes.data();
    createInfo.maxSets = 3;

    if (vkCreateDescriptorPool(logicalDevice, &createInfo, nullptr, &compute.descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create compute descriptor pool");
    }
}

void VulkanViewport::CreateGraphicsDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo graphicsCreateInfo{};
    graphicsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    graphicsCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    graphicsCreateInfo.pPoolSizes = poolSizes.data();
    graphicsCreateInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(logicalDevice, &graphicsCreateInfo, nullptr, &graphics.descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics descriptor pool");
    }
}

void VulkanViewport::CreatePipelineCache() {
    VkPipelineCacheCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    if (vkCreatePipelineCache(logicalDevice, &createInfo, nullptr, &pipelineCache) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline cache");
    }
}

void VulkanViewport::CreateComputeDescriptorSets() {

}

void VulkanViewport::CreateComputePipeline() {
        
    CreateBuffer(static_cast<uint32_t>(compute.ubo.image_size), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, compute.buffers.input.buffer, compute.buffers.input.memory);

    // Load compute shader from file
    std::string computeShaderPath = "Vulkan/comp.spv";

    auto computeShader = ReadShader(computeShaderPath);

    // Take shader code and create a VkShaderModule from it
    auto CreateShaderModule = [this](const std::vector<char>& code, std::string shaderPath) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module");
        }

        return shaderModule;
    };

    // VkShaderModules are just wrappers for shaders
    compute.shaderModule = CreateShaderModule(computeShader, computeShaderPath);

    VkPipelineShaderStageCreateInfo shaderStage{};
    shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStage.module = compute.shaderModule;
    shaderStage.pName = "main";
    assert(shaderStage.module != VK_NULL_HANDLE);
    
    // A descriptor set is used to connect CPU data to the GPU
    // Each descriptor is a handle/pointer to a resource
    VkDescriptorSetLayoutBinding setLayoutBindingInputArray{};
    setLayoutBindingInputArray.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    setLayoutBindingInputArray.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    setLayoutBindingInputArray.binding = 0;
    setLayoutBindingInputArray.descriptorCount = 1;

    VkDescriptorSetLayoutBinding setLayoutBindingInputDimensions{};
    setLayoutBindingInputDimensions.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setLayoutBindingInputDimensions.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    setLayoutBindingInputDimensions.binding = 1;
    setLayoutBindingInputDimensions.descriptorCount = 1;

    VkDescriptorSetLayoutBinding setLayoutBindingOutput{};
    setLayoutBindingOutput.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    setLayoutBindingOutput.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    setLayoutBindingOutput.binding = 2;
    setLayoutBindingOutput.descriptorCount = 1;

    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
        setLayoutBindingInputArray, setLayoutBindingInputDimensions, setLayoutBindingOutput
    };

    // Define the layout of the descriptor set
    VkDescriptorSetLayoutCreateInfo setLayoutCreateInfo{};
    setLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    setLayoutCreateInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    setLayoutCreateInfo.pBindings = setLayoutBindings.data();
    
    // And create it
    if (vkCreateDescriptorSetLayout(logicalDevice, &setLayoutCreateInfo, nullptr, &compute.descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout");
    }

    // Time to create the compute pipeline
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &compute.descriptorSetLayout;

    if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutCreateInfo, nullptr, &compute.pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    SetObjectName(logicalDevice, (uint64_t)compute.pipelineLayout, VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT, "Compute pipeline layout");
    SetObjectName(logicalDevice, (uint64_t)compute.descriptorSetLayout, VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT, "Compute descriptor set layout");

    // Allocate memory for the descriptor sets
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = compute.descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &compute.descriptorSetLayout;
    
    if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, &compute.descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set");
    }

    compute.buffers.input.descriptor.buffer = compute.buffers.input.buffer;
    compute.buffers.input.descriptor.offset = 0;
    compute.buffers.input.descriptor.range = compute.ubo.image_size;

    compute.buffers.uniform.descriptor.buffer = compute.buffers.uniform.buffer;
    compute.buffers.uniform.descriptor.offset = 0;
    compute.buffers.uniform.descriptor.range = sizeof(compute.ubo);
    
    textureImageDescriptor.imageView = textureImageView;
    textureImageDescriptor.sampler = textureSampler;
    textureImageDescriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    std::vector<VkWriteDescriptorSet> computeWriteDescriptorSets = {
        CreateWriteDescriptorSet(compute.descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 0, &compute.buffers.input.descriptor),
        CreateWriteDescriptorSet(compute.descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &compute.buffers.uniform.descriptor),
        CreateWriteDescriptorSet(compute.descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 2, &textureImageDescriptor)
    };

    vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(computeWriteDescriptorSets.size()), computeWriteDescriptorSets.data(), 0, NULL);
    SetObjectName(logicalDevice, (uint64_t)compute.descriptorSet, VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT, "Compute descriptor set");

    // Finally create the compute pipeline
    VkComputePipelineCreateInfo computePipelineCreateInfo{};
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.layout = compute.pipelineLayout;
    computePipelineCreateInfo.flags = 0;
    computePipelineCreateInfo.stage = shaderStage;

    if (vkCreateComputePipelines(logicalDevice, pipelineCache, 1, &computePipelineCreateInfo, nullptr, &compute.pipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create compute pipeline");
    }
    
}

void VulkanViewport::CreateDescriptorSets() {
    
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, graphics.descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = graphics.descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    graphics.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    VkResult result = vkAllocateDescriptorSets(logicalDevice, &allocInfo, graphics.descriptorSets.data());
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate graphics descriptor sets");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        graphics.uniformBuffers[i].descriptor.buffer = graphics.uniformBuffers[i].buffer;
        graphics.uniformBuffers[i].descriptor.offset = 0;
        graphics.uniformBuffers[i].descriptor.range = sizeof(graphics.ubo);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        imageInfo.imageView = textureImageView;
        imageInfo.sampler = textureSampler;


        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
        descriptorWrites[0] = CreateWriteDescriptorSet(graphics.descriptorSets[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &graphics.uniformBuffers[i].descriptor, 1);
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[1] = CreateWriteDescriptorSet(graphics.descriptorSets[i], VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &imageInfo, 1);
        descriptorWrites[1].dstArrayElement = 0;

        vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void VulkanViewport::CreateDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &graphics.descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics descriptor set layout");
    }
}

void VulkanViewport::CreateGraphicsPipeline() {

    // Load shaders from file

    std::string vertShaderPath = "Vulkan/vert.spv";
    std::string fragShaderPath = "Vulkan/frag.spv";


    auto vertShader = ReadShader(vertShaderPath);
    auto fragShader = ReadShader(fragShaderPath);

    // Take shader code and create a VkShaderModule from it
    auto CreateShaderModule = [this](const std::vector<char>& code, std::string shaderPath) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module");
        }
        
        return shaderModule;
    };

    // VkShaderModules are just wrappers for shaders
    VkShaderModule vertShaderModule = CreateShaderModule(vertShader, vertShaderPath);
    VkShaderModule fragShaderModule = CreateShaderModule(fragShader, fragShaderPath);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };


    auto bindingDescription = Vertex::GetBindingDescription();
    auto attributeDescriptions = Vertex::GetAttributeDescriptions();

    // Describes the format in which vertex data will be supplied 
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    // Describes what type of geometry will be drawn from the vertices 
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Define the viewport
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0,0 };
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;

    // The rasteriser takes the geometry from the vertex shader and turns it into fragments to be coloured by the fragment shader
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // Multisampling is a way to perform anti-aliasing
    // Leave disabled for now
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;


    // We then need to add the colour we have obtained from the fragment shader to the framebuffer
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    // Some of the things we have defined here can be changed without recreating the graphics pipeline 
    // This code allows us to resize the viewport without recreating the graphics pipeline
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();



    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &graphics.descriptorSetLayout;

    if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &graphics.pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }

    // Create graphics pipeline object

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizer;
    pipelineCreateInfo.pMultisampleState = &multisampling;
    pipelineCreateInfo.pColorBlendState = &colorBlending;
    pipelineCreateInfo.pDynamicState = nullptr; 
    pipelineCreateInfo.layout = graphics.pipelineLayout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphics.pipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline");
    }

    // Destroy the shader modules - we don't need them anymore
    vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
    vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
}

void VulkanViewport::CreateFramebuffers() {
    swapChainFramebuffers.resize(swapChainImageViews.size());

    for (size_t i = 0; i < swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {
            swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }
}

void VulkanViewport::CreateCommandPools() {
    
    
    VkCommandPoolCreateInfo computePoolInfo{};
    computePoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    computePoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    computePoolInfo.queueFamilyIndex = queueFamilyIndices.computeFamily.value();

    if (vkCreateCommandPool(logicalDevice, &computePoolInfo, nullptr, &compute.commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create compute command pool");
    }

    VkCommandPoolCreateInfo graphicsPoolInfo{};
    graphicsPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    graphicsPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    graphicsPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(logicalDevice, &graphicsPoolInfo, nullptr, &graphics.commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics command pool");
    }

    VkCommandPoolCreateInfo transferPoolInfo{};
    transferPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    transferPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    transferPoolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();

    if (vkCreateCommandPool(logicalDevice, &transferPoolInfo, nullptr, &transfer.commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create transfer command pool");
    }
}

void VulkanViewport::CreateTexture() {
    // Create the texture during startup - we can then update it as we go

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(image->xDim);
    imageInfo.extent.height = static_cast<uint32_t>(image->yDim);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R16G16B16A16_UNORM;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;

    if (queueFamilyIndices.computeFamily.value() != queueFamilyIndices.graphicsFamily.value()) {
        std::vector<uint32_t> imageQueueFamilies = { queueFamilyIndices.computeFamily.value(), queueFamilyIndices.graphicsFamily.value()};
        imageInfo.queueFamilyIndexCount = imageQueueFamilies.size();
        imageInfo.pQueueFamilyIndices = imageQueueFamilies.data();
        imageInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    }

    if (vkCreateImage(logicalDevice, &imageInfo, nullptr, &textureImage) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(logicalDevice, textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(logicalDevice, textureImage, textureImageMemory, 0);


    // Fill in image parameters for GPU buffer
    compute.ubo.image_cols = image->xDim;
    compute.ubo.image_size = image->data.size() * 4;

    // This staging buffer is used to transfer the 3-vector image pixel array to GPU memory
    CreateBuffer(compute.ubo.image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    // Bind the texture to the staging buffer
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, (size_t)compute.ubo.image_size, 0, &stagingBufferDataPtr);

    SetObjectName(logicalDevice, (uint64_t)textureImage, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, "textureImage");

    TransitionImageLayout(textureImage, VK_FORMAT_R16G16B16A16_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    TransitionImageLayout(textureImage, VK_FORMAT_R16G16B16A16_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

}

void VulkanViewport::CreateTextureImageView() {
    textureImageView = CreateImageView(textureImage, VK_FORMAT_R16G16B16A16_UNORM);
}

void VulkanViewport::CreateTextureSampler() {
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(logicalDevice, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create texture sampler");
    }
}

void VulkanViewport::CreateVertexBuffer() {

    // Adjust vertices array to accomodate different image sizes
    float aspectRatio = static_cast<float>(image->xDim) / static_cast<float>(image->yDim);
    vertices[0].position = Vec2f(-aspectRatio, -1.0f);
    vertices[1].position = Vec2f(aspectRatio, -1.0f);
    vertices[2].position = Vec2f(aspectRatio, 1.0f);
    vertices[3].position = Vec2f(-aspectRatio, 1.0f);

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, graphics.vertexBuffer.buffer, graphics.vertexBuffer.memory);

    CopyBuffer(stagingBuffer, graphics.vertexBuffer.buffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void VulkanViewport::CreateIndexBuffer() {

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, graphics.indexBuffer.buffer, graphics.indexBuffer.memory);
    
    CopyBuffer(stagingBuffer, graphics.indexBuffer.buffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void VulkanViewport::CreateUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(Graphics::UBO);
    graphics.uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, graphics.uniformBuffers[i].buffer, graphics.uniformBuffers[i].memory);
        SetObjectName(logicalDevice, (uint64_t)graphics.uniformBuffers[i].buffer, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, "graphics.uniformBuffers[i].buffer");
        SetObjectName(logicalDevice, (uint64_t)graphics.uniformBuffers[i].memory, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, "graphics.uniformBuffers[i].memory");
    }

    // Compute
    CreateBuffer(sizeof(compute.ubo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, compute.buffers.uniform.buffer, compute.buffers.uniform.memory);
    SetObjectName(logicalDevice, (uint64_t)compute.buffers.uniform.buffer, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, "ComputeUniformBuffer");
    SetObjectName(logicalDevice, (uint64_t)compute.buffers.uniform.memory, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, "ComputeUniformBufferMemory");
}

void VulkanViewport::CreateCommandBuffers() {
    
    
    // Compute command buffer
    VkCommandBufferAllocateInfo computeAllocInfo{};
    computeAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    computeAllocInfo.commandPool = compute.commandPool;
    computeAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    computeAllocInfo.commandBufferCount = (uint32_t)2;

    if (vkAllocateCommandBuffers(logicalDevice, &computeAllocInfo, &compute.commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    SetObjectName(logicalDevice, (uint64_t)compute.commandBuffer, VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT, "compute.commandBuffer");

    // Graphics command buffer
    graphics.commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = graphics.commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) graphics.commandBuffers.size();

    if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, graphics.commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    SetObjectName(logicalDevice, (uint64_t)graphics.commandBuffers[0], VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT, "graphics.commandBuffer[0]");
    SetObjectName(logicalDevice, (uint64_t)graphics.commandBuffers[1], VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT, "graphics.commandBuffer[1]");
}

void VulkanViewport::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainExtent;

    VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    // We won't be changing the layout of the image
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageMemoryBarrier.image = textureImage;
    imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics.pipeline);

    VkBuffer vertexBuffers[] = { graphics.vertexBuffer.buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, graphics.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);


    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics.pipelineLayout, 0, 1, &graphics.descriptorSets[currentFrame], 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

}

void VulkanViewport::RecordComputeCommandBuffer() {
    
    // Flush queue to ensure it's not in use
    vkQueueWaitIdle(compute.queue);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;
    
    if (vkBeginCommandBuffer(compute.commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording compute command buffer");
    }

    // Pre-compute
    vkCmdBindDescriptorSets(compute.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute.pipelineLayout, 0, 1, &compute.descriptorSet, 0, 0);
    vkCmdBindPipeline(compute.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, compute.pipeline);

    // Post compute

    vkCmdDispatch(compute.commandBuffer, image->data.size(), 1, 1);

    vkEndCommandBuffer(compute.commandBuffer);
}

void VulkanViewport::CreateSyncObjects() {

    graphics.semaphores.imageAvailable.resize(MAX_FRAMES_IN_FLIGHT);
    graphics.semaphores.renderFinished.resize(MAX_FRAMES_IN_FLIGHT);
    graphics.inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &graphics.semaphores.imageAvailable[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice, &semaphoreCreateInfo, nullptr, &graphics.semaphores.renderFinished[i]) != VK_SUCCESS ||
            vkCreateFence(logicalDevice, &fenceInfo, nullptr, &graphics.inFlightFences[i]) != VK_SUCCESS) {

            SetObjectName(logicalDevice, (uint64_t)graphics.semaphores.imageAvailable[i], VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT, "graphics.semaphores.imageAvailable[i]");
            SetObjectName(logicalDevice, (uint64_t)graphics.semaphores.renderFinished[i], VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT, "graphics.semaphores.renderFinished[i]");
            SetObjectName(logicalDevice, (uint64_t)graphics.inFlightFences[i], VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT, "graphics.inFlightFences[i]");
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }

    VkSemaphoreCreateInfo computeSemaphoreCreateInfo{};
    computeSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if ((vkCreateSemaphore(logicalDevice, &computeSemaphoreCreateInfo, nullptr, &compute.semaphores.ready) != VK_SUCCESS) ||
        (vkCreateSemaphore(logicalDevice, &computeSemaphoreCreateInfo, nullptr, &compute.semaphores.complete) != VK_SUCCESS) ||
        vkCreateFence(logicalDevice, &fenceInfo, nullptr, &compute.inFlightFence) != VK_SUCCESS
        ) {
        throw std::runtime_error("Failed to create compute semaphore");
    }

    SetObjectName(logicalDevice, (uint64_t)compute.semaphores.ready, VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT, "compute.semaphores.ready");
    SetObjectName(logicalDevice, (uint64_t)compute.semaphores.complete, VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT, "compute.semaphores.complete");
}

void VulkanViewport::UpdateTexture() {
    // This function is called every frame to update the contents of the texture

    // Copy image array to video memory
    memcpy(stagingBufferDataPtr, image->data.data(), (size_t)compute.ubo.image_size);

    CopyBuffer(stagingBuffer, compute.buffers.input.buffer, compute.ubo.image_size);

    static bool firstCompute = true;
    if (firstCompute) {
        // Copy image parameters to video memory
        vkMapMemory(logicalDevice, compute.buffers.uniform.memory, 0, sizeof(compute.ubo), 0, &parametersDataPtr);
        memcpy(parametersDataPtr, &compute.ubo, sizeof(compute.ubo));
        vkUnmapMemory(logicalDevice, compute.buffers.uniform.memory);
    }

    // Dispatch compute
    vkWaitForFences(logicalDevice, 1, &compute.inFlightFence, VK_TRUE, UINT64_MAX);

    vkResetFences(logicalDevice, 1, &compute.inFlightFence);

    vkResetCommandBuffer(compute.commandBuffer, 0);
    RecordComputeCommandBuffer();

    VkSubmitInfo computeSubmitInfo{};
    computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags computeWaitDstStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    computeSubmitInfo.waitSemaphoreCount = 0;
    computeSubmitInfo.pWaitDstStageMask = &computeWaitDstStageMask;
    computeSubmitInfo.signalSemaphoreCount = 1;
    computeSubmitInfo.pSignalSemaphores = &compute.semaphores.complete;
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &compute.commandBuffer;

    VkResult result = vkQueueSubmit(compute.queue, 1, &computeSubmitInfo, compute.inFlightFence);

    if(result != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit compute command buffer");
    }

}

void VulkanViewport::DrawFrame() {
    vkWaitForFences(logicalDevice, 1, &graphics.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, graphics.semaphores.imageAvailable[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    // Only reset fence if we are submitting work
    vkResetFences(logicalDevice, 1, &graphics.inFlightFences[currentFrame]);

    vkResetCommandBuffer(graphics.commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    RecordCommandBuffer(graphics.commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { graphics.semaphores.imageAvailable[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    static bool firstDraw = true;
    if (!firstDraw) {
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
    }
    else {
        firstDraw = false;
    }

    UpdateTexture();

    UpdateUniformBuffer(currentFrame);


    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &graphics.commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = { graphics.semaphores.renderFinished[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphics.queue, 1, &submitInfo, graphics.inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer");
    }

    std::array<VkSemaphore, 2> presentWaitSemaphores = { graphics.semaphores.renderFinished[currentFrame], compute.semaphores.complete };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = presentWaitSemaphores.size();
    presentInfo.pWaitSemaphores = presentWaitSemaphores.data();

    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RecreateSwapChain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    currentFrame = (currentFrame + 1) % (MAX_FRAMES_IN_FLIGHT - 1);

}

Matrix44f Perspective(const float fovY, const float aspect, const float zNear, const float zFar) {

    assert(aspect != 0.0f);
    assert(zFar != zNear);

    const float rad = fovY;
    const float tanHalfFovY = tan(rad / 2.0f);

    Matrix44f result(0.0f);
    result[0][0] = 1.0f / (aspect * tanHalfFovY);
    result[1][1] = -1.0f / tanHalfFovY;
    result[2][2] = -(zFar + zNear) / (zFar - zNear);
    result[2][3] = -1.0f;
    result[3][2] = - (2.0f * zFar * zNear) / (zFar - zNear);
    return result;
}

Matrix44f LookAt(const Vec3f cameraPosition, const Vec3f cameraLookAt, const Vec3f cameraUp) {

    Vec3f f = unit_vector(cameraLookAt - cameraPosition);
    Vec3f s = unit_vector(cross_product(f, cameraUp));
    Vec3f u = cross_product(s, f);

    Matrix44f view(0.0f);
    view[0][0] = s.x();
    view[1][0] = s.y();
    view[2][0] = s.z();
    view[0][1] = u.x();
    view[1][1] = u.y();
    view[2][1] = u.z();
    view[0][2] = -f.x();
    view[1][2] = -f.y();
    view[2][2] = -f.z();
    view[3][0] = -dot(s, cameraPosition);
    view[3][1] = -dot(u, cameraPosition);
    view[3][2] = dot(f, cameraPosition);
    view[3][3] = 1.0f;

    return view;
}

float DegreesToRadians(const float& degrees) {
    return degrees / M_2PI;
}

float RadiansToDegrees(const float& radians) {
    return radians * M_2PI;
}

void VulkanViewport::UpdateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    float theta = time * DegreesToRadians(90.0f); // Degrees in radians

    Vec3f cameraPosition(0.0, 0.0, 1.78);
    Vec3f cameraLookAt(0.0, 0.0, 0.0);
    Vec3f cameraUp(0.0, 1.0, 0.0);
    Matrix44f view = LookAt(cameraPosition, cameraLookAt, cameraUp);
    Matrix44f proj = Perspective(DegreesToRadians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 10.0f);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            graphics.ubo.view.data[i][j] = view[i][j];
            graphics.ubo.proj.data[i][j] = proj[i][j];
        }
    }

    void* data;
    vkMapMemory(logicalDevice, graphics.uniformBuffers[currentImage].memory, 0, sizeof(graphics.ubo), 0, &data);
    memcpy(data, &graphics.ubo, sizeof(graphics.ubo));
    vkUnmapMemory(logicalDevice, graphics.uniformBuffers[currentImage].memory);
}
