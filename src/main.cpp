#include <memory>
#include <string>
#include <iostream>
#include <thread>
#include <vector>

#include "GameSocket.h"
#include "MoveManager.h"
#include "standardFunctions.h" // For make_unique

#ifdef __APPLE__
#include "OpenCL/opencl.h"
#else
#include "CL/cl.h"
#endif

int getNbCores()
{
	unsigned int nbCores = std::thread::hardware_concurrency();
	if (nbCores == 0)	// Couldn't determine the number of threads, use 2 by default
	{
		std::cerr << "[WARNING] Cannot find the real number of cores, using default 2" << std::endl;
		nbCores = 2;
	}
	return nbCores;
}

int main(int argc, char* argv[])
{
	// Parse the arguments
	std::vector<std::string> params(argv, argv + argc);

	// Get cores
	int nbCores = getNbCores();

	// If params is benchmark, do it
	/*if (((params.size() == 2) && params[1] == "benchmark") || params.size() == 1)
	{
		std::cout << "Benchmarking ..." << std::endl;
		GameState::setBoardSize(10, 15); // Set default board size
		std::shared_ptr<Node> root = std::make_shared<Node>(GameState()); // Empty Node
		MoveManager mm(root, nbCores);
		mm.benchmark();
		return 0;
	}
	// Check number of arguments
	if (params.size() != 3)
	{
		std::cout << "Usage: " << params.at(0) << " <ip> <port>" << std::endl;
		return -1;
	}
	// Number of arguments seems good, get ip and port
	std::string ip = params.at(1);
	int port = std::stoi(params.at(2));

	// First create a connetion to the server
	std::unique_ptr<GameSocket> socket = make_unique<GameSocket>();
	socket->connect(ip, port, "Edward");

	// Create the MoveManager with the root node
	MoveManager mm(nbCores);
	mm.setSocket(std::move(socket));

	mm.mainloop();*/

    cl_int error = 0;   // Used to handle error codes
    std::vector<cl_platform_id> platforms(2);
    cl_context context;
    cl_command_queue queue;
    cl_device_id device;

    // Platform
    error = clGetPlatformIDs(2, platforms.data(), nullptr);
    if (error != CL_SUCCESS) {
        std::cout << "Error getting platform id: " << error << std::endl;
        exit(error);
    }
    // Device
    error = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (error != CL_SUCCESS) {
        std::cout << "Error getting device ids: " << error << std::endl;
        exit(error);
    }
    // Context
    context = clCreateContext(0, 1, &device, NULL, NULL, &error);
    if (error != CL_SUCCESS) {
        std::cout << "Error creating context: " << error << std::endl;
        exit(error);
    }
    // Command-queue
    queue = clCreateCommandQueue(context, device, 0, &error);
    if (error != CL_SUCCESS) {
        std::cout << "Error creating command queue: " << error << std::endl;
        exit(error);
    }

    const int size = 10240000;
    float* src_a_h = new float[size];
    float* src_b_h = new float[size];
    float* res_h = new float[size];
    // Initialize both vectors
    for (int i = 0; i < size; i++) {
        src_a_h[i] = src_b_h[i] = (float)i;
    }

    const int mem_size = sizeof(float)*size;
    // Allocates a buffer of size mem_size and copies mem_size bytes from src_a_h
    cl_mem src_a_d = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, src_a_h, &error);
    cl_mem src_b_d = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, src_b_h, &error);
    cl_mem res_d = clCreateBuffer(context, CL_MEM_WRITE_ONLY, mem_size, NULL, &error);

    // Creates the program
    // Uses NVIDIA helper functions to get the code string and it's size (in bytes)
    const char* source = "__kernel void vector_add(__global const float* src_a, __global const float* src_b, __global float* res, const int num) { \
                              const int idx = get_global_id(0); \
                              if (idx < num) \
                                  res[idx] = src_a[idx] + src_b[idx]; \
                          }";
    size_t src_size = strlen(source);
    cl_program program = clCreateProgramWithSource(context, 1, &source, &src_size, &error);
    assert(error == CL_SUCCESS);

    // Builds the program
    error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    assert(error == CL_SUCCESS);

    // Shows the log
    char* build_log;
    size_t log_size;
    // First call to know the proper size
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    build_log = new char[log_size + 1];
    // Second call to get the log
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
    build_log[log_size] = '\0';
    std::cout << build_log << std::endl;
    delete[] build_log;

    // Extracting the kernel
    cl_kernel vector_add_kernel = clCreateKernel(program, "vector_add", &error);
    assert(error == CL_SUCCESS);

    // Enqueuing parameters
    // Note that we inform the size of the cl_mem object, not the size of the memory pointed by it
    error = clSetKernelArg(vector_add_kernel, 0, sizeof(cl_mem), &src_a_d);
    error |= clSetKernelArg(vector_add_kernel, 1, sizeof(cl_mem), &src_b_d);
    error |= clSetKernelArg(vector_add_kernel, 2, sizeof(cl_mem), &res_d);
    error |= clSetKernelArg(vector_add_kernel, 3, sizeof(size_t), &size);
    assert(error == CL_SUCCESS);

    // Launching kernel
    const size_t global_ws = 10240000;	// Total number of work-items
    error = clEnqueueNDRangeKernel(queue, vector_add_kernel, 1, NULL, &global_ws, nullptr, 0, NULL, NULL);
    assert(error == CL_SUCCESS);

    clEnqueueReadBuffer(queue, res_d, CL_TRUE, 0, mem_size, res_h, 0, NULL, NULL);

    return 0;
}