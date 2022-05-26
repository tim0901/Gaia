#ifndef VULKAN_VIEWPORT_H
#define VULKAN_VIEWPORT_H

#include "../Core/Viewport.h"
//#include "../Maths/Vector.h"
//#include "../Maths/Matrix.h"
import sml;

#include "../Maths/Pi.h"
#include <array>
#include <assert.h>
#include <optional>
#include <chrono>
#include <algorithm>
#include <string>
#include <tuple>
#include <iostream>
#include <set>
#include <fstream>
#include "../Dependencies/stb_image.h"

#include "RenderDocDebugging.h"

class VulkanViewport : public Viewport {
public:
	VulkanViewport() {	};
	VulkanViewport(std::shared_ptr<Flags> f, std::shared_ptr<Image> i) : flags(f), image(i) {
		InitialiseViewport(); };
	~VulkanViewport() {

		if (supportCheck) {
			// We've only been testing for Vulkan support, so not everything is initialised
			vkDestroyInstance(instance, nullptr);
		}
		else if(initialised) {

			//Unmap Texture from staginf buffer
			vkUnmapMemory(logicalDevice, stagingBufferMemory);

			for (auto framebuffer : swapChainFramebuffers) {
				vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
			}

			vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

			for (VkImageView imageView : swapChainImageViews) {
				vkDestroyImageView(logicalDevice, imageView, nullptr);
			}
			vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);

			vkDestroySampler(logicalDevice, textureSampler, nullptr);
			vkDestroyImageView(logicalDevice, textureImageView, nullptr);

			vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
			vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);

			vkDestroyImage(logicalDevice, textureImage, nullptr);
			vkFreeMemory(logicalDevice, textureImageMemory, nullptr);

			compute.Destroy(logicalDevice);
			graphics.Destroy(logicalDevice);
			transfer.Destroy(logicalDevice);

			vkDestroyPipelineCache(logicalDevice, pipelineCache, nullptr);

			vkDestroyDevice(logicalDevice, nullptr);
			vkDestroySurfaceKHR(instance, surface, nullptr);
			vkDestroyInstance(instance, nullptr);

			glfwDestroyWindow(window);

			// glfw: terminate, clears all previously allocated GLFW resources.
			glfwTerminate();
		}
	}

	bool VulkanSupportCheck() {
		// To check for Vulkan support we attempt to make a Vulkan instance
		// If this fails, we know it is not supported.
		supportCheck = true;
		try
		{
			CreateInstance("TestApplication");
		}
		catch (const std::exception&)
		{
			return false;
		}
		return true;
	}

	void InitialiseViewport() {

		// Initialise window
		glfwInit();

		std::string applicationName = "Gaia - " + image->fileName;

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(image->xDim, image->yDim, applicationName.c_str(), nullptr, nullptr);
		SetWindowIcon();

		// Create Vulkan instance
		CreateInstance(applicationName);

		// Create a surface for our window;
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create GLFW window surface");

		// Look for a physical device to use
		FindPhysicalDevice();

		// Now that we have our physical device, we need a logical device to interface with it
		CreateLogicalDevice();
		EnableDebug(physicalDevice, logicalDevice);

		// Create the texture we render to
		CreateComputeDescriptorPool();

		// Create the swap chain
		CreateSwapChain();
		CreateImageViews();

		CreateRenderPass();

		CreatePipelineCache();
		CreateDescriptorSetLayout();

		CreateGraphicsPipeline();

		CreateFramebuffers();

		CreateCommandPools();
		CreateTexture();
		CreateTextureImageView();
		CreateTextureSampler();

		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateUniformBuffers();

		CreateComputePipeline();

		CreateGraphicsDescriptorPool();
		CreateDescriptorSets();

		CreateCommandBuffers();
		CreateSyncObjects();


		SetObjectName(logicalDevice, (uint64_t)compute.queue, VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT, "compute.queue");
		SetObjectName(logicalDevice, (uint64_t)presentQueue, VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT, "Graphics Presentqueue");
		
		// Draw the frame once to set things up
		DrawFrame();

		initialised = true;
	}

	// Call to update viewport contents
	void UpdateView(int argc, const char* argv[]) {
		if (!initialised)
			throw std::runtime_error("Attempting to update uninitialised viewport");

		while (!glfwWindowShouldClose(window)) {
			
			// Fetch latest image
			ProcessInput();
			glfwPollEvents();
			
			DrawFrame();
		}
		// Wait for device to finish all operations before cleanup
		vkDeviceWaitIdle(logicalDevice);
	}

private:

	// GLSL matrices are arrays of arrays
	// And so are incompatible with Matrix.h
	class GLSLMatrix44f {
	public:
		GLSLMatrix44f() {}
		GLSLMatrix44f(float n) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					data[i][j] = n;
				}
			}
		}
		float data[4][4];
	};

	struct Vertex {
		sml::Vec2f position;
		sml::Vec3f colour;
		sml::Vec2f texCoord;

		static VkVertexInputBindingDescription GetBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, position); 

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, colour);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
			return attributeDescriptions;
		}
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		std::optional<uint32_t> computeFamily;
		std::optional<uint32_t> transferFamily;

		bool IsComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value() && transferFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;

		bool IsAdequate() {
			return !formats.empty() && !presentModes.empty();
		}
	};

	// Process IO events
	void ProcessInput() {
		//Kills window on ESC
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
			std::cout << "ESCAPE" << std::endl;
			flags->cancelRender = true;
		}
	}

	// Support functions

	struct Icon {
		int width, height, channels;
		GLFWimage images[1];
		void Load(std::string location) {
			images[0].pixels = stbi_load(location.c_str(), &images[0].width, &images[0].height, 0, STBI_rgb_alpha);
		}
		void Free() {
			stbi_image_free(images[0].pixels);
		}
	}icon;

	void SetWindowIcon() {
		icon.Load("icon.png");
		glfwSetWindowIcon(window, 1, icon.images);
		icon.Free();
	}
	std::vector<char> ReadShader(const std::string& filename) const;
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) const;
	SwapChainSupportDetails QuerySwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) const;
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*> deviceExtensions);
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void CopyBuffer(VkBuffer sourceBuffer, VkBuffer destinationBuffer, VkDeviceSize size);
	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
	void TransitionImageLayout(VkImage img, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& img, VkDeviceMemory& imageMemory);
	VkImageView CreateImageView(VkImage img, VkFormat format);
	VkDescriptorPoolSize CreateDescriptorPoolSize(VkDescriptorType type, uint32_t count) const;
	VkWriteDescriptorSet CreateWriteDescriptorSet(const VkDescriptorSet descriptorSet, const VkDescriptorType descriptorType, const uint32_t binding, const VkDescriptorBufferInfo* bufferInfo, const uint32_t descriptorCount = 1) const;
	VkWriteDescriptorSet CreateWriteDescriptorSet(const VkDescriptorSet descriptorSet, const VkDescriptorType descriptorType, const uint32_t binding, const VkDescriptorImageInfo* imageInfo, const uint32_t descriptorCount = 1) const;

	void RenderDocInit();
	void RecreateSwapChain();
	void CreateInstance(const std::string applicationName);
	void FindPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();
	void CreateComputeDescriptorPool();
	void CreateGraphicsDescriptorPool();
	void CreateDescriptorSets();
	void CreateComputeDescriptorSets();
	void CreatePipelineCache();
	void CreateComputePipeline();
	void CreateDescriptorSetLayout();
	void CreateGraphicsPipeline();
	void CreateFramebuffers();
	void CreateCommandPools();
	void CreateTexture();
	void CreateTextureImageView();
	void CreateTextureSampler();
	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateUniformBuffers();
	void CreateCommandBuffers();
	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void RecordComputeCommandBuffer();
	void CreateSyncObjects();

	void DrawFrame();
	void UpdateTexture();
	void UpdateUniformBuffer(uint32_t currentImage);

	// This is called during RecreateSwapChain to reset certain resources
	void CleanupSwapChain() {
		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		}
		
		vkDestroyPipeline(logicalDevice, graphics.pipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, graphics.pipelineLayout, nullptr);

		vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

		for (VkImageView imageView : swapChainImageViews) {
			vkDestroyImageView(logicalDevice, imageView, nullptr);
		}
		vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
	}

	GLFWwindow* window = nullptr;
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice;
	VkQueue presentQueue;
	VkSwapchainKHR swapChain;

	void* stagingBufferDataPtr;
	void* parametersDataPtr;

	// These are the extensions that we need the device to support
	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
		//,VK_EXT_DEBUG_MARKER_EXTENSION_NAME
	};

	std::vector<Vertex> vertices = {
		{{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{-1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	QueueFamilyIndices queueFamilyIndices;

	struct ShaderBuffer {
		VkBuffer buffer;
		VkDeviceMemory memory;
		VkDescriptorBufferInfo descriptor{};
		void Destroy(VkDevice device) {
			vkFreeMemory(device, memory, nullptr);
			vkDestroyBuffer(device, buffer, nullptr);
		}
	};

	struct Compute {
		struct Buffers{
			ShaderBuffer input;
			ShaderBuffer uniform;
			ShaderBuffer output;
		} buffers;

		struct Semaphores {
			VkSemaphore ready;
			VkSemaphore complete;
		} semaphores;

		struct UBO {
			uint32_t image_size;
			uint32_t image_cols;
		} ubo;


		VkFence inFlightFence;

		VkShaderModule shaderModule;
		VkQueue queue;
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;

		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorSet descriptorSet;

		VkCommandBuffer commandBuffer;
		VkCommandPool commandPool;

		void Destroy(VkDevice device) {
			vkDestroyPipeline(device, pipeline, nullptr);
			vkDestroyShaderModule(device, shaderModule, nullptr);
			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
			vkDestroyDescriptorPool(device, descriptorPool, nullptr);
			vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
			vkDestroySemaphore(device, semaphores.ready, nullptr);
			vkDestroySemaphore(device, semaphores.complete, nullptr);
			vkDestroyCommandPool(device, commandPool, nullptr);
			
			buffers.input.Destroy(device);
			buffers.uniform.Destroy(device);
			buffers.output.Destroy(device);
		}
	} compute;

	struct Graphics {

		const int MAX_FRAMES_IN_FLIGHT = 2;
		
		struct UBO {
			GLSLMatrix44f view;
			GLSLMatrix44f proj;
		} ubo;

		struct Semaphores {
			std::vector<VkSemaphore> imageAvailable;
			std::vector<VkSemaphore> renderFinished;
		} semaphores;

		std::vector<VkFence> inFlightFences;

		ShaderBuffer indexBuffer;
		ShaderBuffer vertexBuffer;
		std::vector<ShaderBuffer> uniformBuffers;

		VkQueue queue;

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
		VkCommandPool commandPool;

		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		std::vector<VkDescriptorSet> descriptorSets;

		std::vector<VkCommandBuffer> commandBuffers;

		void Destroy(VkDevice device) {
			indexBuffer.Destroy(device);
			vertexBuffer.Destroy(device);
			for (ShaderBuffer ub : uniformBuffers) {
				ub.Destroy(device);
			}
			
			vkDestroyDescriptorPool(device, descriptorPool, nullptr);
			vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

				vkDestroySemaphore(device, semaphores.imageAvailable[i], nullptr);
				vkDestroySemaphore(device, semaphores.renderFinished[i], nullptr);
				vkDestroyFence(device, inFlightFences[i], nullptr);
			}

			vkDestroyPipeline(device, pipeline, nullptr);
			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
			vkDestroyCommandPool(device, commandPool, nullptr);
		}
	} graphics;

	struct Transfer {
		VkCommandPool commandPool;
		VkQueue queue;
		void Destroy(VkDevice device) {
			vkDestroyCommandPool(device, commandPool, nullptr);
		}
	} transfer;

	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	VkPipelineCache pipelineCache;

	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkRenderPass renderPass;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;

	VkImageView textureImageView;
	VkSampler textureSampler;
	VkDescriptorImageInfo textureImageDescriptor;

	const int MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t currentFrame = 0;

	std::shared_ptr<Flags> flags;
	std::shared_ptr<Image> image;
	bool initialised = false;
	bool supportCheck = false;

};

bool VulkanSupportCheck();

#endif // !VULKAN_VIEWPORT_H
