#pragma once

struct Buffer {
	vk::Buffer handle;
	vk::DeviceMemory memory;
	void* memory_mapped = nullptr;
};