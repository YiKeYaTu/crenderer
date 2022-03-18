//
// Created by lc on 2022/3/12.
//

#ifndef CRENDERER_OPENCLPROGRAMCONTEXT_HPP
#define CRENDERER_OPENCLPROGRAMCONTEXT_HPP

#include <OpenCL/opencl.h>
#include <string>
#include <vector>
#include <fstream>
#include "../../utils/util.hpp"
#include "./OpenCLKernel.hpp"

class OpenCLProgramContext {
public:
    static cl_uint getNumPlatforms() {
        cl_uint numPlatforms;
        processErrors_(clGetPlatformIDs(0, NULL, &numPlatforms), "Failed to query number of platforms.");
        return numPlatforms;
    }

    static std::vector<cl_platform_id> getPlatformIDs(cl_uint numPlatforms = 0) {
        if (numPlatforms == 0) {
            numPlatforms = getNumPlatforms();
        }
        cl_platform_id platformIDs[numPlatforms];
        processErrors_(clGetPlatformIDs(numPlatforms, platformIDs, &numPlatforms), "Failed to query id of platforms.");

        return std::vector<cl_platform_id>(platformIDs, platformIDs + numPlatforms);
    }

    static OpenCLProgramContext createGPUContext(cl_platform_id platformID) {
        cl_context_properties contextProperties[] = {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties) platformID,
            0
        };
        cl_int errNum;
        cl_context context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU, NULL, NULL, &errNum);
        processErrors_(errNum);

        size_t sizeDevices = -1;
        processErrors_(clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &sizeDevices));

        cl_device_id deviceIDs[sizeDevices / sizeof(cl_device_id)];
        processErrors_(clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeDevices, deviceIDs, NULL));

        std::vector<cl_device_id> deviceIDVector;
        for (cl_device_id* iter = deviceIDs; iter != deviceIDs + sizeDevices / sizeof(cl_device_id); ++iter) {
            deviceIDVector.push_back(*iter);
        }

        return OpenCLProgramContext(context, deviceIDVector);
    }

    void useDevice(std::size_t idx) {
        usedDeviceIDs_.clear();
        usedDeviceIDs_.push_back(deviceIDs_[idx]);
    }

    void useProgram(const std::string& path, const std::string& options) {
        commandQueue_ = clCreateCommandQueue(context_, usedDeviceIDs_[0], 0, NULL);
        if (commandQueue_ == NULL) {
            throw std::runtime_error("Failed to create commandQueue for device 0");
        }

        std::string srcStdStr = util::readFileString(path);
        const char *srcStr = srcStdStr.c_str();

        program_ = clCreateProgramWithSource(context_, 1, (const char**)&(srcStr), NULL, NULL);
        if (program_ == NULL) {
            throw std::runtime_error("Failed to create CL program from source.");
        }
        processBuildProgramErrors_(clBuildProgram(
            program_,
            1,
            &usedDeviceIDs_[0],
            options.c_str(),
            NULL,
            NULL
        ));
    }

    OpenCLKernel createKernel(const std::string& kernelName) {
        return OpenCLKernel(program_, context_, kernelName);
    }

    size_t* getMaxWorkItemSizes() {
        size_t param_value_size_ret;
        clGetDeviceInfo(usedDeviceIDs_[0], CL_DEVICE_MAX_WORK_ITEM_SIZES, 0, NULL, &param_value_size_ret);
        void* buffer = malloc(param_value_size_ret * sizeof(char));
        clGetDeviceInfo(usedDeviceIDs_[0], CL_DEVICE_MAX_WORK_ITEM_SIZES, param_value_size_ret, buffer, NULL);

        return static_cast<size_t*>(buffer);
    }

    cl_ulong getGlobalMemSize() {
        size_t param_value_size_ret;
        clGetDeviceInfo(usedDeviceIDs_[0], CL_DEVICE_GLOBAL_MEM_SIZE, 0, NULL, &param_value_size_ret);
        void* buffer = malloc(param_value_size_ret * sizeof(char));
        clGetDeviceInfo(usedDeviceIDs_[0], CL_DEVICE_GLOBAL_MEM_SIZE, param_value_size_ret, buffer, NULL);
        cl_ulong ret = *(static_cast<cl_ulong*>(buffer));

        free(buffer);

        return ret;
    }

    void execute(OpenCLKernel& openClKernel) {
        assert(openClKernel.globalWorkSize_.size() > 0 && openClKernel.localWorkSize_.size() > 0);
        processErrors_(clEnqueueNDRangeKernel(
            commandQueue_,
            openClKernel.kernel_,
            1,
            NULL,
            &openClKernel.globalWorkSize_[0],
            &openClKernel.localWorkSize_[0],
            0,
            NULL,
            NULL
        ), "Failed to call clEnqueueNDRangeKernel.");

        for (const auto& item : openClKernel.memOutputObjects_) {
            processErrors_(clEnqueueReadBuffer(
                commandQueue_,
                item.memObject,
                CL_TRUE,
                0,
                item.size,
                item.data,
                0,
                NULL,
                NULL)
            );
        }

    }

private:
    cl_context context_;
    std::vector<cl_device_id> deviceIDs_;
    std::vector<cl_device_id> usedDeviceIDs_;

    cl_command_queue commandQueue_ = NULL;
    cl_program program_ = NULL;

    OpenCLProgramContext(cl_context context, std::vector<cl_device_id>& deviceIDs)
        : context_(context), deviceIDs_(deviceIDs) {
        setup_();
    }

    void setup_() { useDevice( { 0 } ); }

    void processBuildProgramErrors_(cl_int errNum) {
        if (errNum == CL_SUCCESS) {
            return;
        }
        size_t len = 0;
        cl_int ret = CL_SUCCESS;
        ret = clGetProgramBuildInfo(program_, usedDeviceIDs_[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
        char *buffer = static_cast<char*>(calloc(len, sizeof(char)));
        ret = clGetProgramBuildInfo(program_, usedDeviceIDs_[0], CL_PROGRAM_BUILD_LOG, len, buffer, NULL);

        throw std::runtime_error(buffer);
    }

    static void processErrors_(cl_int errNum, const std::string& errorMessage = "") {
        std::string msg(errorMessage + " ");

        if (errNum != CL_SUCCESS) {
            switch (errNum) {
                case CL_INVALID_PROGRAM:
                    msg.append("CL_INVALID_PROGRAM");
                    break;
                case CL_INVALID_DEVICE:
                    msg.append("CL_INVALID_DEVICE");
                    break;
                case CL_INVALID_BINARY:
                    msg.append("CL_INVALID_BINARY");
                    break;
                case CL_INVALID_BUILD_OPTIONS:
                    msg.append("CL_INVALID_BUILD_OPTIONS");
                    break;
                case CL_INVALID_OPERATION:
                    msg.append("CL_INVALID_OPERATION");
                    break;
                case CL_COMPILER_NOT_AVAILABLE:
                    msg.append("CL_COMPILER_NOT_AVAILABLE");
                    break;
                case CL_BUILD_PROGRAM_FAILURE:
                    msg.append("CL_BUILD_PROGRAM_FAILURE");
                    break;
                case CL_OUT_OF_HOST_MEMORY:
                    msg.append("CL_OUT_OF_HOST_MEMORY");
                    break;
                case CL_INVALID_PROGRAM_EXECUTABLE:
                    msg.append("CL_INVALID_PROGRAM_EXECUTABLE");
                    break;
                case CL_INVALID_COMMAND_QUEUE:
                    msg.append("CL_INVALID_COMMAND_QUEUE");
                    break;
                case CL_INVALID_KERNEL:
                    msg.append("CL_INVALID_KERNEL");
                    break;
                case CL_INVALID_CONTEXT:
                    msg.append("CL_INVALID_CONTEXT");
                    break;
                case CL_INVALID_KERNEL_ARGS:
                    msg.append("CL_INVALID_KERNEL_ARGS");
                    break;
                case CL_INVALID_PLATFORM:
                    msg.append("CL_INVALID_PLATFORM");
                    break;
                case CL_INVALID_VALUE:
                    msg.append("CL_INVALID_VALUE");
                    break;
                case CL_DEVICE_NOT_AVAILABLE:
                    msg.append("CL_DEVICE_NOT_AVAILABLE");
                    break;
                case CL_DEVICE_NOT_FOUND:
                    msg.append("CL_DEVICE_NOT_FOUND");
                    break;
                case CL_INVALID_DEVICE_TYPE:
                    msg.append("CL_INVALID_DEVICE_TYPE");
                    break;
            }

            throw std::runtime_error(msg);
        }

    }
};

#endif //CRENDERER_OPENCLPROGRAMCONTEXT_HPP
