#!/bin/bash
# check_opencl.sh

echo "=== Проверка OpenCL на Arch Linux ==="
echo ""
echo "1. Проверка драйверов:"
ls /usr/lib/libIntel* 2>/dev/null || echo "Intel драйверы не найдены"
ls /usr/lib/libigc* 2>/dev/null || echo "IGC не найден"
ls /usr/lib/libigdfcl* 2>/dev/null || echo "FCL не найден"
echo ""

echo "2. Проверка файлов в /etc/OpenCL/vendors/:"
ls -la /etc/OpenCL/vendors/ 2>/dev/null || echo "Папка не существует"
echo ""

echo "3. Проверка всех платформ и устройств:"
cat > /tmp/test_opencl.c << 'EOF'
#include <stdio.h>
#include <CL/cl.h>

int main() {
    cl_uint num_platforms;
    clGetPlatformIDs(0, NULL, &num_platforms);
    printf("Платформ: %u\n\n", num_platforms);
    
    if (num_platforms == 0) return 0;
    
    cl_platform_id* platforms = malloc(num_platforms * sizeof(cl_platform_id));
    clGetPlatformIDs(num_platforms, platforms, NULL);
    
    for (cl_uint i = 0; i < num_platforms; i++) {
        char name[256], vendor[256];
        size_t size;
        
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 256, name, &size);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 256, vendor, &size);
        
        printf("Платформа %d: %s\n", i, name);
        printf("Вендор: %s\n", vendor);
        
        cl_uint num_devices;
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
        printf("Устройств: %u\n", num_devices);
        
        if (num_devices > 0) {
            cl_device_id* devices = malloc(num_devices * sizeof(cl_device_id));
            clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
            
            for (cl_uint j = 0; j < num_devices; j++) {
                char dev_name[256], dev_vendor[256];
                cl_device_type type;
                
                clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 256, dev_name, &size);
                clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, 256, dev_vendor, &size);
                clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(type), &type, &size);
                
                printf("  [%d] %s\n", j, dev_name);
                printf("      Тип: %s\n", (type & CL_DEVICE_TYPE_GPU) ? "GPU" : 
                                          (type & CL_DEVICE_TYPE_CPU) ? "CPU" : "Other");
                printf("      Вендор: %s\n", dev_vendor);
            }
            
            free(devices);
        }
        printf("\n");
    }
    
    free(platforms);
    return 0;
}
EOF

gcc -o /tmp/test_opencl /tmp/test_opencl.c -lOpenCL 2>/dev/null
if [ -f /tmp/test_opencl ]; then
    /tmp/test_opencl
else
    echo "Не удалось скомпилировать тестовую программу"
fi
echo ""

echo "4. Проверка через clinfo (если установлен):"
if command -v clinfo &> /dev/null; then
    clinfo | grep -E "Platform Name|Device Name|Device Type" | head -20
else
    echo "clinfo не установлен. Установите: sudo pacman -S clinfo"
fi
