/**************************************************************************
 *                                                                        *
 *   Author: Ivo Filot <i.a.w.filot@tue.nl>                               *
 *                                                                        *
 *   BRAMBLE is free software:                                            *
 *   you can redistribute it and/or modify it under the terms of the      *
 *   GNU General Public License as published by the Free Software         *
 *   Foundation, either version 3 of the License, or (at your option)     *
 *   any later version.                                                   *
 *                                                                        *
 *   BRAMBLE is distributed in the hope that it will be useful,           *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#include "card_manager.h"

CardManager::CardManager() {}

void CardManager::probe_cards() {
    int nDevices = 0;

	if(cudaGetDeviceCount(&nDevices) != cudaSuccess) {
        std::cout << "No CUDA-capable devices detected." << std::endl;
        return;
    }

	for (int i = 0; i < nDevices; i++) {
		cudaDeviceProp prop;
        int memory_clock_rate = 0;
        int memory_bus_width = 0;

		cudaGetDeviceProperties(&prop, i);
        cudaDeviceGetAttribute(&memory_clock_rate, cudaDevAttrMemoryClockRate, i);
        cudaDeviceGetAttribute(&memory_bus_width, cudaDevAttrGlobalMemoryBusWidth, i);

		std::cout << "Device Number: " << i << std::endl;
		std::cout << "  Device name: " << prop.name << std::endl;
		std::cout << "  Memory Clock Rate (KHz): " << memory_clock_rate << std::endl;
		std::cout << "  Memory Bus Width (bits): " << memory_bus_width << std::endl;
        std::cout << "  Total memory: " << prop.totalGlobalMem << std::endl;
		std::cout << "  Peak Memory Bandwidth (GB/s): " << 2.0*memory_clock_rate*(memory_bus_width/8)/1.0e6 << std::endl << std::endl;
	}
}

int CardManager::get_num_gpus() {
    int nDevices = 0;
    if(cudaGetDeviceCount(&nDevices) != cudaSuccess) {
        return 0;
    }

    return nDevices;
}

void CardManager::set_gpu_to_thread() {
    int id = omp_get_thread_num();
    cudaDeviceProp prop;
    if(cudaGetDeviceProperties(&prop, id) != cudaSuccess) {
        std::cout << "Could not set GPU " << id << " to thread " << id << std::endl;
        return;
    }

    std::cout << "Setting GPU " << id << " to thread " << id << std::endl;
    cudaSetDevice(id);
}

int64_t CardManager::get_memory_device(unsigned int device_id) {
    cudaDeviceProp prop;
    if(cudaGetDeviceProperties(&prop, device_id) == 0) {
        return prop.totalGlobalMem;
    } else {
        return -1;
    }
}
