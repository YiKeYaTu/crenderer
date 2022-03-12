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

    void useKernel(const std::string& path, const std::string& kernelName) {
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

        processErrors_(clBuildProgram(
            program_,
            0,
            NULL,
            "-I /Users/liangchen/CLionProjects/crenderer/gpu/opencl/kernels",
            NULL,
            NULL
        ), "Failed to build program.");

        cl_int *errcode_ret;
        kernel_ = clCreateKernel(program_, kernelName.c_str(), errcode_ret);
        if (kernel_ == NULL) {
            throw std::runtime_error("Failed to create kernel.");
            switch (*errcode_ret) {
                case CL_INVALID_PROGRAM:
                    throw std::runtime_error("CL_INVALID_PROGRAM");
                case CL_INVALID_PROGRAM_EXECUTABLE:
                    throw std::runtime_error("CL_INVALID_PROGRAM_EXECUTABLE");
                case CL_INVALID_KERNEL_NAME:
                    throw std::runtime_error("CL_INVALID_KERNEL_NAME");
                case CL_INVALID_KERNEL_DEFINITION:
                    throw std::runtime_error("CL_INVALID_KERNEL_DEFINITION");
                case CL_INVALID_VALUE:
                    throw std::runtime_error("CL_INVALID_VALUE");
                case CL_OUT_OF_HOST_MEMORY:
                    throw std::runtime_error("CL_OUT_OF_HOST_MEMORY");
            }
        }
    }

    template<typename T>
    void setKernelArguments(std::size_t pos, T* data, std::size_t dataSize) {
        memArgumentObjects_.push_back(clCreateBuffer(
            context_,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(T) * dataSize,
            data,
            NULL
        ));

        if (memArgumentObjects_.back() == NULL) {
            throw std::runtime_error("Error creating memory objects.");
        }

        clSetKernelArg(kernel_, pos, sizeof(cl_mem), &memArgumentObjects_.back());
    }

    template<typename T>
    void setReturnValues(std::size_t pos, T* data, std::size_t dataSize) {
        std::size_t totalSize = sizeof(T) * dataSize;
        memReturnObjects_.emplace_back(
            clCreateBuffer(
               context_,
               CL_MEM_WRITE_ONLY,
               totalSize,
               NULL,
               NULL
           ),
           reinterpret_cast<void*>(data),
           totalSize
        );

        if (memReturnObjects_.back().memObject == NULL) {
            throw std::runtime_error("Error creating memory objects.");
        }

        clSetKernelArg(kernel_, pos, sizeof(cl_mem), &memReturnObjects_.back().memObject);
    }

    void setWorkSize(size_t* globalWorkSize, size_t* localWorkSize) {
        globalWorkSize_ = globalWorkSize;
        localWorkSize_ = localWorkSize;
    }

    void execute() {
        if (globalWorkSize_ == nullptr || localWorkSize_ == nullptr) {
            throw std::runtime_error("globalWorkSize_ and localWorkSize_ must be set.");
        }

        processErrors_(clEnqueueNDRangeKernel(commandQueue_, kernel_, 1, NULL, globalWorkSize_, localWorkSize_, 0,
                                              NULL, NULL), "Failed to call clEnqueueNDRangeKernel.");

        for (const auto& item : memReturnObjects_) {
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
    struct MemObject {
        cl_mem memObject;
        void* data;
        std::size_t size;

        MemObject(const cl_mem& memObject, void *data, size_t size) : memObject(memObject), data(data), size(size) {}
    };

    cl_context context_;
    std::vector<cl_device_id> deviceIDs_;
    std::vector<cl_device_id> usedDeviceIDs_;

    cl_command_queue commandQueue_ = NULL;
    cl_program program_ = NULL;
    cl_kernel kernel_ = NULL;

    std::vector<cl_mem> memArgumentObjects_;
    std::vector<MemObject> memReturnObjects_;

    size_t* globalWorkSize_ = nullptr;
    size_t* localWorkSize_ = nullptr;

    OpenCLProgramContext(cl_context context, std::vector<cl_device_id>& deviceIDs)
        : context_(context), deviceIDs_(deviceIDs) {
        setup_();
    }

    void setup_() { useDevice( { 0 } ); }

    static void processErrors_(cl_int errNum, const std::string& errorMessage = "") {
        if (errNum != CL_SUCCESS) {
            switch (errNum) {
                case CL_INVALID_PROGRAM_EXECUTABLE:
                    throw std::runtime_error("CL_INVALID_PROGRAM_EXECUTABLE");
                case CL_INVALID_COMMAND_QUEUE:
                    throw std::runtime_error("CL_INVALID_COMMAND_QUEUE");
                case CL_INVALID_KERNEL:
                    throw std::runtime_error("CL_INVALID_KERNEL");
                case CL_INVALID_CONTEXT:
                    throw std::runtime_error("CL_INVALID_CONTEXT");
                case CL_INVALID_KERNEL_ARGS:
                    throw std::runtime_error("CL_INVALID_KERNEL_ARGS");
            }

        }
    }
};

#endif //CRENDERER_OPENCLPROGRAMCONTEXT_HPP
