//
// Created by Raul Romero on 2024-01-16.
//

#ifndef WGPU_ATOM_H
#define WGPU_ATOM_H

#include <string>
#include "../Defines.h"

namespace wgpu
{

enum class FeatureName : uint32_t;
enum class ErrorType : uint32_t;
class Instance;
class Adapter;
class Device;
struct RequestAdapterOptions;
struct DeviceDescriptor;

namespace atom
{

std::string FeatureNameToString(FeatureName featureName);
std::string ErrorTypeToString(ErrorType errorType);
std::string ErrorTypeToString(i32 errorType);
Adapter RequestAdapter(const Instance& instance, const RequestAdapterOptions* options);
Device RequestDevice(Adapter adapter, const DeviceDescriptor* descriptor);

} // namespace atom

} // namespace wgpu

#endif //WGPU_ATOM_H
