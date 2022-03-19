//
// Created by lc on 2022/3/14.
//

#ifndef CRENDERER_OPENCLKERNEL_HPP
#define CRENDERER_OPENCLKERNEL_HPP

#include <OpenCL/cl.h>
#include <vector>

class OpenCLProgramContext;

class OpenCLKernel {
    friend class OpenCLProgramContext;
public:
    OpenCLKernel(cl_program program, cl_context context, const std::string& kernelName)
        : program_(program), context_(context) {
        kernel_ = clCreateKernel(program_, kernelName.c_str(), NULL);
        if (kernel_ == NULL) {
            throw std::runtime_error("Failed to create kernel.");
        }
    }

    void setWorkSize(const std::vector<size_t>& globalWorkSize, const std::vector<size_t>& localWorkSize) {
        globalWorkSize_ = globalWorkSize;
        localWorkSize_ = localWorkSize;
    }

    cl_mem& setArguments(cl_mem_flags flags, std::size_t size, void* hostPtr, std::size_t pos) {
        memInputObjects_.push_back(clCreateBuffer(
                context_,
                flags,
                size,
                hostPtr,
                NULL
        ));

        if (memInputObjects_.back() == NULL) {
            throw std::runtime_error("Error creating memory objects.");
        }

        clSetKernelArg(kernel_, pos, sizeof(cl_mem), &memInputObjects_.back());
        return memInputObjects_.back();
    }

    template<typename T>
    void setInputArguments(std::size_t pos, T* data, std::size_t dataLen) {
        memInputObjects_.push_back(clCreateBuffer(
                context_,
                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                sizeof(T) * dataLen,
                data,
                NULL
        ));

        if (memInputObjects_.back() == NULL) {
            throw std::runtime_error("Error creating memory objects.");
        }

        clSetKernelArg(kernel_, pos, sizeof(cl_mem), &memInputObjects_.back());
    }
    template<typename T>
    void setInterArguments(std::size_t pos, T* data, std::size_t dataLen) {
        memInputObjects_.push_back(clCreateBuffer(
                context_,
                CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                sizeof(T) * dataLen,
                data,
                NULL
        ));

        if (memInputObjects_.back() == NULL) {
            throw std::runtime_error("Error creating memory objects.");
        }

        clSetKernelArg(kernel_, pos, sizeof(cl_mem), &memInputObjects_.back());
    }
    template<typename T>
    void setOutputArguments(std::size_t pos, T* data, std::size_t dataLen) {
        std::size_t totalSize = sizeof(T) * dataLen;
        memOutputObjects_.emplace_back(
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

        if (memOutputObjects_.back().memObject == NULL) {
            throw std::runtime_error("Error creating memory objects.");
        }

        clSetKernelArg(kernel_, pos, sizeof(cl_mem), &memOutputObjects_.back().memObject);
    }
private:
    struct MemObject {
        cl_mem memObject;
        void* data;
        std::size_t size;
        MemObject(const cl_mem& memObject, void *data, size_t size)
            : memObject(memObject), data(data), size(size) {}
    };

    const cl_program program_;
    const cl_context context_;
    cl_kernel kernel_;

    std::vector<cl_mem> memInputObjects_;
    std::vector<MemObject> memOutputObjects_;

    std::vector<size_t> globalWorkSize_ = { 1 };
    std::vector<size_t> localWorkSize_ = { 1 };
};

#endif //CRENDERER_OPENCLKERNEL_HPP
