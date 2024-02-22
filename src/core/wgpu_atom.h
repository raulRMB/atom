//
// Created by Raul Romero on 2024-01-16.
//

#ifndef WGPU_ATOM_H
#define WGPU_ATOM_H

#include <string>
#include "../Defines.h"

#include <webgpu/webgpu_cpp.h>

namespace atom
{

std::string FeatureNameToString(wgpu::FeatureName featureName);
std::string ErrorTypeToString(wgpu::ErrorType errorType);
std::string ErrorTypeToString(i32 errorType);
::wgpu::Adapter RequestAdapter(const wgpu::Instance& instance, const wgpu::RequestAdapterOptions* options);
::wgpu::Device RequestDevice(wgpu::Adapter adapter, const wgpu::DeviceDescriptor* descriptor);

} // namespace atom

#endif //WGPU_ATOM_H
