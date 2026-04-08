/**
 * ForceACL - GPU Detector Implementation
 * Detailed GPU detection and identification
 */

#include <Lilu/kern_api.hpp>
#include <Lilu/kern_util.hpp>
#include <IOKit/pci/IOPCIDevice.h>
#include <libkern/libkern.h>
#include <cstring>

#include "ForceACL/GPUDetector.hpp"

// Device ID mappings - comprehensive list
const DeviceIDMapping GPUDetector::m_deviceMappings[] = {
    // Sandy Bridge (SNB) - 6 Series
    {0x0102, GPUGeneration::SandyBridge, "HD Graphics 2000", "Intel HD Graphics 2000"},
    {0x0106, GPUGeneration::SandyBridge, "HD Graphics 2000", "Intel HD Graphics 2000"},
    {0x010A, GPUGeneration::SandyBridge, "HD Graphics 2000", "Intel HD Graphics 2000"},
    {0x010B, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x010D, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x010E, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x010F, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x0110, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x0111, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x0112, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x0113, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x0120, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x0121, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x0122, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x0123, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x0125, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x0126, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    {0x0127, GPUGeneration::SandyBridge, "HD Graphics 3000", "Intel HD Graphics 3000"},
    
    // Ivy Bridge (IVB) - 7 Series
    {0x0152, GPUGeneration::IvyBridge, "HD Graphics 2500", "Intel HD Graphics 2500"},
    {0x0156, GPUGeneration::IvyBridge, "HD Graphics 2500", "Intel HD Graphics 2500"},
    {0x015A, GPUGeneration::IvyBridge, "HD Graphics 2500", "Intel HD Graphics 2500"},
    {0x015B, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    {0x015E, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    {0x0162, GPUGeneration::IvyBridge, "HD Graphics 2500", "Intel HD Graphics 2500"},
    {0x0166, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    {0x016A, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    {0x016B, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    {0x016E, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    {0x0172, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    {0x0176, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    {0x017A, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    {0x017B, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    {0x0182, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    {0x0186, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    {0x018A, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    {0x018B, GPUGeneration::IvyBridge, "HD Graphics 4000", "Intel HD Graphics 4000"},
    
    // Haswell (HSW) - 8 Series
    {0x0402, GPUGeneration::Haswell, "HD Graphics 4200", "Intel HD Graphics 4200"},
    {0x0406, GPUGeneration::Haswell, "HD Graphics 4200", "Intel HD Graphics 4200"},
    {0x040A, GPUGeneration::Haswell, "HD Graphics 4200", "Intel HD Graphics 4200"},
    {0x040B, GPUGeneration::Haswell, "HD Graphics 4400", "Intel HD Graphics 4400"},
    {0x040D, GPUGeneration::Haswell, "HD Graphics 4400", "Intel HD Graphics 4400"},
    {0x040E, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0412, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0413, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0414, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0415, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0416, GPUGeneration::Haswell, "HD Graphics 4400", "Intel HD Graphics 4400"},
    {0x0417, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0418, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0419, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x041A, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x041B, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x041E, GPUGeneration::Haswell, "HD Graphics 4200", "Intel HD Graphics 4200"},
    {0x041F, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0420, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0421, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0422, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0423, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0424, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0425, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0426, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0427, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0428, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0429, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x042A, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x042B, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x042C, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x042D, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x042E, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x042F, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0432, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0433, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0434, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0435, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0436, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0437, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0438, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0439, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x043A, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x043B, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x043C, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x043D, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x043E, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x043F, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0440, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0442, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0443, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0444, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0445, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0446, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0447, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0448, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0449, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x044A, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x044B, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x044C, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x044D, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x044E, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x044F, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A0A, GPUGeneration::Haswell, "HD Graphics 4400", "Intel HD Graphics 4400"},
    {0x0A0B, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0A0E, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0A0F, GPUGeneration::Haswell, "HD Graphics 4600", "Intel HD Graphics 4600"},
    {0x0A12, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0A13, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0A16, GPUGeneration::Haswell, "HD Graphics 5100", "Intel HD Graphics 5100"},
    {0x0A1A, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0A1B, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0A1E, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0A1F, GPUGeneration::Haswell, "HD Graphics 5000", "Intel HD Graphics 5000"},
    {0x0A20, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A21, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A22, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A23, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A2A, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A2B, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A2E, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A2F, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A30, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A31, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A32, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A33, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A34, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A35, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A36, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A37, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A3A, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A3B, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A3C, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A3D, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A3E, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    {0x0A3F, GPUGeneration::Haswell, "Iris Pro Graphics 5200", "Intel Iris Pro Graphics 5200"},
    
    // Broadwell (BDW) - 9 Series
    {0x1600, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x1601, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x1602, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x1603, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x1604, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x1605, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x1606, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x1607, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x1608, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x1609, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x160A, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x160B, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x160C, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x160D, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x160E, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x160F, GPUGeneration::Broadwell, "HD Graphics 5300", "Intel HD Graphics 5300"},
    {0x1610, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x1611, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x1612, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x1613, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x1614, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x1615, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x1616, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x1617, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x1618, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x1619, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x161A, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x161B, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x161C, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x161D, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x161E, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x161F, GPUGeneration::Broadwell, "HD Graphics 5500", "Intel HD Graphics 5500"},
    {0x1620, GPUGeneration::Broadwell, "HD Graphics 6000", "Intel HD Graphics 6000"},
    {0x1621, GPUGeneration::Broadwell, "HD Graphics 6000", "Intel HD Graphics 6000"},
    {0x1622, GPUGeneration::Broadwell, "HD Graphics 6000", "Intel HD Graphics 6000"},
    {0x1623, GPUGeneration::Broadwell, "HD Graphics 6000", "Intel HD Graphics 6000"},
    {0x1624, GPUGeneration::Broadwell, "HD Graphics 6000", "Intel HD Graphics 6000"},
    {0x1625, GPUGeneration::Broadwell, "HD Graphics 6000", "Intel HD Graphics 6000"},
    {0x1626, GPUGeneration::Broadwell, "HD Graphics 6000", "Intel HD Graphics 6000"},
    {0x1627, GPUGeneration::Broadwell, "HD Graphics 6000", "Intel HD Graphics 6000"},
    {0x1628, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1629, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x162A, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x162B, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x162C, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x162D, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x162E, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x162F, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1630, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1632, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1633, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1634, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1635, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1636, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1637, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1638, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1639, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x163A, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x163B, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x163C, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x163D, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x163E, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x163F, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1640, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1641, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1642, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x1643, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    {0x164A, GPUGeneration::Broadwell, "Iris Pro Graphics 6200", "Intel Iris Pro Graphics 6200"},
    
    // Skylake (SKL) - 9C Series
    {0x1902, GPUGeneration::Skylake, "HD Graphics 510", "Intel HD Graphics 510"},
    {0x1906, GPUGeneration::Skylake, "HD Graphics 510", "Intel HD Graphics 510"},
    {0x190A, GPUGeneration::Skylake, "HD Graphics 510", "Intel HD Graphics 510"},
    {0x190B, GPUGeneration::Skylake, "HD Graphics 515", "Intel HD Graphics 515"},
    {0x190D, GPUGeneration::Skylake, "HD Graphics 515", "Intel HD Graphics 515"},
    {0x190E, GPUGeneration::Skylake, "HD Graphics 515", "Intel HD Graphics 515"},
    {0x190F, GPUGeneration::Skylake, "HD Graphics 515", "Intel HD Graphics 515"},
    {0x1912, GPUGeneration::Skylake, "HD Graphics 520", "Intel HD Graphics 520"},
    {0x1913, GPUGeneration::Skylake, "HD Graphics 520", "Intel HD Graphics 520"},
    {0x1915, GPUGeneration::Skylake, "HD Graphics 520", "Intel HD Graphics 520"},
    {0x1916, GPUGeneration::Skylake, "HD Graphics 520", "Intel HD Graphics 520"},
    {0x1917, GPUGeneration::Skylake, "HD Graphics 520", "Intel HD Graphics 520"},
    {0x1918, GPUGeneration::Skylake, "HD Graphics 520", "Intel HD Graphics 520"},
    {0x1919, GPUGeneration::Skylake, "HD Graphics 520", "Intel HD Graphics 520"},
    {0x191A, GPUGeneration::Skylake, "HD Graphics 520", "Intel HD Graphics 520"},
    {0x191B, GPUGeneration::Skylake, "HD Graphics 520", "Intel HD Graphics 520"},
    {0x191D, GPUGeneration::Skylake, "HD Graphics 520", "Intel HD Graphics 520"},
    {0x191E, GPUGeneration::Skylake, "HD Graphics 520", "Intel HD Graphics 520"},
    {0x191F, GPUGeneration::Skylake, "HD Graphics 520", "Intel HD Graphics 520"},
    {0x1920, GPUGeneration::Skylake, "HD Graphics 530", "Intel HD Graphics 530"},
    {0x1921, GPUGeneration::Skylake, "HD Graphics 530", "Intel HD Graphics 530"},
    {0x1923, GPUGeneration::Skylake, "HD Graphics 530", "Intel HD Graphics 530"},
    {0x1924, GPUGeneration::Skylake, "HD Graphics 530", "Intel HD Graphics 530"},
    {0x1925, GPUGeneration::Skylake, "HD Graphics 530", "Intel HD Graphics 530"},
    {0x1926, GPUGeneration::Skylake, "HD Graphics 530", "Intel HD Graphics 530"},
    {0x1927, GPUGeneration::Skylake, "HD Graphics 530", "Intel HD Graphics 530"},
    {0x1928, GPUGeneration::Skylake, "HD Graphics 530", "Intel HD Graphics 530"},
    {0x1929, GPUGeneration::Skylake, "HD Graphics 530", "Intel HD Graphics 530"},
    {0x192A, GPUGeneration::Skylake, "HD Graphics 530", "Intel HD Graphics 530"},
    {0x192B, GPUGeneration::Skylake, "HD Graphics 530", "Intel HD Graphics 530"},
    {0x192D, GPUGeneration::Skylake, "HD Graphics 530", "Intel HD Graphics 530"},
    {0x1930, GPUGeneration::Skylake, "Iris Graphics 540", "Intel Iris Graphics 540"},
    {0x1931, GPUGeneration::Skylake, "Iris Graphics 540", "Intel Iris Graphics 540"},
    {0x1932, GPUGeneration::Skylake, "Iris Graphics 540", "Intel Iris Graphics 540"},
    {0x1933, GPUGeneration::Skylake, "Iris Graphics 540", "Intel Iris Graphics 540"},
    {0x1935, GPUGeneration::Skylake, "Iris Graphics 550", "Intel Iris Graphics 550"},
    {0x1936, GPUGeneration::Skylake, "Iris Graphics 550", "Intel Iris Graphics 550"},
    {0x1937, GPUGeneration::Skylake, "Iris Graphics 550", "Intel Iris Graphics 550"},
    {0x1938, GPUGeneration::Skylake, "Iris Graphics 550", "Intel Iris Graphics 550"},
    {0x1939, GPUGeneration::Skylake, "Iris Graphics 550", "Intel Iris Graphics 550"},
    {0x193A, GPUGeneration::Skylake, "Iris Graphics 550", "Intel Iris Graphics 550"},
    {0x193B, GPUGeneration::Skylake, "Iris Graphics 550", "Intel Iris Graphics 550"},
    {0x193D, GPUGeneration::Skylake, "Iris Pro Graphics 580", "Intel Iris Pro Graphics 580"},
    {0x193E, GPUGeneration::Skylake, "Iris Pro Graphics 580", "Intel Iris Pro Graphics 580"},
    {0x193F, GPUGeneration::Skylake, "Iris Pro Graphics 580", "Intel Iris Pro Graphics 580"},
    
    // And more... (Kaby Lake, Coffee Lake, etc.)
    // This is a partial list showing the structure
    
    // Kaby Lake (KBL) - U/Y Series
    {0x5906, GPUGeneration::KabyLake, "HD Graphics 615", "Intel HD Graphics 615"},
    {0x5907, GPUGeneration::KabyLake, "HD Graphics 615", "Intel HD Graphics 615"},
    {0x5908, GPUGeneration::KabyLake, "HD Graphics 615", "Intel HD Graphics 615"},
    {0x5909, GPUGeneration::KabyLake, "HD Graphics 617", "Intel HD Graphics 617"},
    {0x590A, GPUGeneration::KabyLake, "HD Graphics 617", "Intel HD Graphics 617"},
    {0x590B, GPUGeneration::KabyLake, "HD Graphics 620", "Intel HD Graphics 620"},
    {0x590C, GPUGeneration::KabyLake, "HD Graphics 620", "Intel HD Graphics 620"},
    {0x590D, GPUGeneration::KabyLake, "HD Graphics 620", "Intel HD Graphics 620"},
    {0x5912, GPUGeneration::KabyLake, "HD Graphics 620", "Intel HD Graphics 620"},
    {0x5913, GPUGeneration::KabyLake, "HD Graphics 620", "Intel HD Graphics 620"},
    {0x5914, GPUGeneration::KabyLake, "HD Graphics 620", "Intel HD Graphics 620"},
    {0x5915, GPUGeneration::KabyLake, "HD Graphics 620", "Intel HD Graphics 620"},
    {0x5916, GPUGeneration::KabyLake, "HD Graphics 620", "Intel HD Graphics 620"},
    {0x5917, GPUGeneration::KabyLake, "HD Graphics 620", "Intel HD Graphics 620"},
    {0x5918, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x5919, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x591A, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x591B, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x591C, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x591D, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x591E, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x591F, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x5920, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x5921, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x5922, GPUGeneration::KabyLake, "Iris Plus Graphics 640", "Intel Iris Plus Graphics 640"},
    {0x5923, GPUGeneration::KabyLake, "Iris Plus Graphics 640", "Intel Iris Plus Graphics 640"},
    {0x5924, GPUGeneration::KabyLake, "Iris Plus Graphics 640", "Intel Iris Plus Graphics 640"},
    {0x5925, GPUGeneration::KabyLake, "Iris Plus Graphics 650", "Intel Iris Plus Graphics 650"},
    {0x5926, GPUGeneration::KabyLake, "Iris Plus Graphics 650", "Intel Iris Plus Graphics 650"},
    {0x5927, GPUGeneration::KabyLake, "Iris Plus Graphics 650", "Intel Iris Plus Graphics 650"},
    
    // Kaby Lake - S/H Series
    {0x5912, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x5913, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x5914, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x5915, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x5916, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x5917, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x5918, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x5919, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x591A, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x591B, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x591C, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x591D, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x591E, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x591F, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x5920, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    {0x5921, GPUGeneration::KabyLake, "HD Graphics 630", "Intel HD Graphics 630"},
    
    // Coffee Lake (CFL)
    {0x3E90, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E91, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E92, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E93, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E94, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E95, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E96, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E97, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E98, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E99, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E9A, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E9B, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E9C, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E9D, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E9E, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E9F, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EA0, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EA1, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EA2, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EA3, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EA4, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EA5, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EA6, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EA7, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EA8, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EA9, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EAA, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EAB, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EAC, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EAD, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EAE, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EAF, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EB0, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EB1, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EB2, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EB3, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EB4, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EB5, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EB6, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EB7, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EB8, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EB9, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EBA, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EBB, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EBC, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EBD, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EBE, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EBF, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3ED0, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3ED1, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3ED2, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3ED3, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3ED4, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3ED5, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3ED6, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3ED7, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3ED8, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3ED9, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EDA, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EDB, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EDC, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EDD, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EDE, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EDF, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EE0, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EE1, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EE2, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EE3, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EE4, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EE5, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EE6, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EE7, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EE8, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EE9, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EEA, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EEB, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EEC, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EED, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EEE, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3EEF, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    
    // Coffee Lake - Higher end
    {0x3E71, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E72, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E73, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E74, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E75, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E76, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E77, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E78, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E79, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E7A, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E7B, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E7C, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E7D, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E7E, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x3E7F, GPUGeneration::CoffeeLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    
    // Comet Lake (10th Gen)
    {0x9B21, GPUGeneration::CometLake, "UHD Graphics 620", "Intel UHD Graphics 620"},
    {0x9B41, GPUGeneration::CometLake, "UHD Graphics 620", "Intel UHD Graphics 620"},
    {0x9B51, GPUGeneration::CometLake, "UHD Graphics 620", "Intel UHD Graphics 620"},
    {0x9B61, GPUGeneration::CometLake, "UHD Graphics 620", "Intel UHD Graphics 620"},
    {0x9BA1, GPUGeneration::CometLake, "UHD Graphics 630", "Intel UHD Graphics 630"},
    {0x9BA8, GPUGeneration::CometLake, "Iris Plus Graphics 655", "Intel Iris Plus Graphics 655"},
    
    // Ice Lake (10th Gen)
    {0x8A50, GPUGeneration::IceLake, "Iris Plus Graphics G1", "Intel Iris Plus Graphics G1"},
    {0x8A51, GPUGeneration::IceLake, "Iris Plus Graphics G1", "Intel Iris Plus Graphics G1"},
    {0x8A52, GPUGeneration::IceLake, "Iris Plus Graphics G4", "Intel Iris Plus Graphics G4"},
    {0x8A53, GPUGeneration::IceLake, "Iris Plus Graphics G4", "Intel Iris Plus Graphics G4"},
    {0x8A54, GPUGeneration::IceLake, "Iris Plus Graphics G7", "Intel Iris Plus Graphics G7"},
    {0x8A56, GPUGeneration::IceLake, "Iris Plus Graphics G7", "Intel Iris Plus Graphics G7"},
    {0x8A5A, GPUGeneration::IceLake, "Iris Plus Graphics G1", "Intel Iris Plus Graphics G1"},
    {0x8A5B, GPUGeneration::IceLake, "Iris Plus Graphics G1", "Intel Iris Plus Graphics G1"},
    {0x8A5C, GPUGeneration::IceLake, "Iris Plus Graphics G4", "Intel Iris Plus Graphics G4"},
    {0x8A5D, GPUGeneration::IceLake, "Iris Plus Graphics G4", "Intel Iris Plus Graphics G4"},
    
    // Tiger Lake (11th Gen)
    {0x9A40, GPUGeneration::TigerLake, "Iris Xe Graphics G7", "Intel Iris Xe Graphics G7"},
    {0x9A49, GPUGeneration::TigerLake, "Iris Xe Graphics G7", "Intel Iris Xe Graphics G7"},
    {0x9A59, GPUGeneration::TigerLake, "Iris Xe Graphics G7", "Intel Iris Xe Graphics G7"},
    {0x9A60, GPUGeneration::TigerLake, "Iris Xe Graphics G7", "Intel Iris Xe Graphics G7"},
    {0x9A68, GPUGeneration::TigerLake, "Iris Xe Graphics G7", "Intel Iris Xe Graphics G7"},
    {0x9A70, GPUGeneration::TigerLake, "Iris Xe Graphics G7", "Intel Iris Xe Graphics G7"},
    {0x9A78, GPUGeneration::TigerLake, "Iris Xe Graphics G7", "Intel Iris Xe Graphics G7"},
    
    // Rocket Lake (11th Gen)
    {0x4C61, GPUGeneration::RocketLake, "UHD Graphics 730", "Intel UHD Graphics 730"},
    {0x4C8A, GPUGeneration::RocketLake, "UHD Graphics 730", "Intel UHD Graphics 730"},
    {0x4C8B, GPUGeneration::RocketLake, "UHD Graphics 730", "Intel UHD Graphics 730"},
    {0x4C8C, GPUGeneration::RocketLake, "UHD Graphics 730", "Intel UHD Graphics 730"},
    
    // Alder Lake-S (12th Gen)
    {0x4680, GPUGeneration::AlderLakeS, "UHD Graphics 730", "Intel UHD Graphics 730"},
    {0x4682, GPUGeneration::AlderLakeS, "UHD Graphics 730", "Intel UHD Graphics 730"},
    {0x4690, GPUGeneration::AlderLakeS, "UHD Graphics 770", "Intel UHD Graphics 770"},
    {0x4692, GPUGeneration::AlderLakeS, "UHD Graphics 770", "Intel UHD Graphics 770"},
    {0x4693, GPUGeneration::AlderLakeS, "UHD Graphics 770", "Intel UHD Graphics 770"},
    
    // Alder Lake-P (12th Gen)
    {0x46A0, GPUGeneration::AlderLakeP, "Iris Xe Graphics G7", "Intel Iris Xe Graphics G7"},
    {0x46A1, GPUGeneration::AlderLakeP, "Iris Xe Graphics G7", "Intel Iris Xe Graphics G7"},
    {0x46A3, GPUGeneration::AlderLakeP, "Iris Xe Graphics G7", "Intel Iris Xe Graphics G7"},
    {0x46A6, GPUGeneration::AlderLakeP, "Iris Xe Graphics G7", "Intel Iris Xe Graphics G7"},
    {0x46A8, GPUGeneration::AlderLakeP, "Iris Xe Graphics G7", "Intel Iris Xe Graphics G7"},
    {0x46AA, GPUGeneration::AlderLakeP, "Iris Xe Graphics G7", "Intel Iris Xe Graphics G7"},
    
    // Meteor Lake (14th Gen)
    {0x7D40, GPUGeneration::MeteorLake, "Arc Graphics", "Intel Arc Graphics"},
    {0x7D45, GPUGeneration::MeteorLake, "Arc Graphics", "Intel Arc Graphics"},
    {0x7D55, GPUGeneration::MeteorLake, "Arc Graphics", "Intel Arc Graphics"},
    
    // Lunar Lake (15th Gen)
    {0x64A0, GPUGeneration::LunarLake, "Arc Graphics", "Intel Arc Graphics"},
    {0x64B0, GPUGeneration::LunarLake, "Arc Graphics", "Intel Arc Graphics"}
};

const size_t GPUDetector::m_mappingCount = sizeof(m_deviceMappings) / sizeof(m_deviceMappings[0]);

GPUDetector::GPUDetector() {
    GPU_DETECT_LOG("GPU Detector initialized");
}

GPUDetector::~GPUDetector() {
}

GPUGeneration GPUDetector::detectGeneration(uint16_t deviceId) {
    GPUGeneration gen = matchDeviceId(deviceId);
    
    GPU_DETECT_VERBOSE("Device 0x%04X -> Generation: %s", deviceId, generationToString(gen));
    
    return gen;
}

GPUGeneration GPUDetector::matchDeviceId(uint16_t deviceId) {
    for (size_t i = 0; i < m_mappingCount; i++) {
        if (m_deviceMappings[i].deviceId == deviceId) {
            return m_deviceMappings[i].generation;
        }
    }
    
    // Try to detect by device ID ranges
    // Haswell: 0x04xx
    if ((deviceId & 0xFF00) == 0x0400) {
        return GPUGeneration::Haswell;
    }
    // Broadwell: 0x16xx
    if ((deviceId & 0xFF00) == 0x1600) {
        return GPUGeneration::Broadwell;
    }
    // Skylake: 0x19xx
    if ((deviceId & 0xFF00) == 0x1900) {
        return GPUGeneration::Skylake;
    }
    // Kaby Lake: 0x59xx
    if ((deviceId & 0xFF00) == 0x5900) {
        return GPUGeneration::KabyLake;
    }
    // Coffee Lake: 0x3Exx
    if ((deviceId & 0xFF00) == 0x3E00) {
        return GPUGeneration::CoffeeLake;
    }
    // Comet Lake: 0x9Bxx
    if ((deviceId & 0xFF00) == 0x9B00) {
        return GPUGeneration::CometLake;
    }
    // Ice Lake: 0x8Axx
    if ((deviceId & 0xFF00) == 0x8A00) {
        return GPUGeneration::IceLake;
    }
    // Tiger Lake: 0x9Axx
    if ((deviceId & 0xFF00) == 0x9A00) {
        return GPUGeneration::TigerLake;
    }
    // Rocket Lake: 0x4Cxx
    if ((deviceId & 0xFF00) == 0x4C00) {
        return GPUGeneration::RocketLake;
    }
    // Alder Lake: 0x46xx / 0x4Dxx / 0x4Exx
    if ((deviceId & 0xFF00) == 0x4600 || (deviceId & 0xFF00) == 0x4D00 || (deviceId & 0xFF00) == 0x4E00) {
        return GPUGeneration::AlderLakeP;
    }
    // Meteor Lake: 0x7Dxx
    if ((deviceId & 0xFF00) == 0x7D00) {
        return GPUGeneration::MeteorLake;
    }
    // Lunar Lake: 0xB4xx
    if ((deviceId & 0xFF00) == 0xB400) {
        return GPUGeneration::LunarLake;
    }
    
    return GPUGeneration::Unknown;
}

const char* GPUDetector::generationToString(GPUGeneration gen) {
    switch (gen) {
        case GPUGeneration::SandyBridge: return "Sandy Bridge";
        case GPUGeneration::IvyBridge: return "Ivy Bridge";
        case GPUGeneration::Haswell: return "Haswell";
        case GPUGeneration::Broadwell: return "Broadwell";
        case GPUGeneration::Skylake: return "Skylake";
        case GPUGeneration::KabyLake: return "Kaby Lake";
        case GPUGeneration::CoffeeLake: return "Coffee Lake";
        case GPUGeneration::CometLake: return "Comet Lake";
        case GPUGeneration::IceLake: return "Ice Lake";
        case GPUGeneration::TigerLake: return "Tiger Lake";
        case GPUGeneration::RocketLake: return "Rocket Lake";
        case GPUGeneration::AlderLakeS: return "Alder Lake-S";
        case GPUGeneration::AlderLakeP: return "Alder Lake-P";
        case GPUGeneration::MeteorLake: return "Meteor Lake";
        case GPUGeneration::LunarLake: return "Lunar Lake";
        default: return "Unknown";
    }
}

const char* GPUDetector::getMarketingName(uint16_t deviceId) {
    for (size_t i = 0; i < m_mappingCount; i++) {
        if (m_deviceMappings[i].deviceId == deviceId) {
            return m_deviceMappings[i].marketingName;
        }
    }
    return "Unknown Intel GPU";
}

bool GPUDetector::isSupported(uint16_t deviceId) {
    return detectGeneration(deviceId) != GPUGeneration::Unknown;
}

bool GPUDetector::isHaswell(uint16_t deviceId) {
    return detectGeneration(deviceId) == GPUGeneration::Haswell;
}

bool GPUDetector::isIntel(uint16_t vendorId) {
    return vendorId == 0x8086;
}

uint16_t GPUDetector::getSubVendorId(IOPCIDevice* device) {
    if (!device) return 0;
    return device->configRead16(0x2C);
}

uint16_t GPUDetector::getSubDeviceId(IOPCIDevice* device) {
    if (!device) return 0;
    return device->configRead16(0x2E);
}

uint8_t GPUDetector::getRevisionId(IOPCIDevice* device) {
    if (!device) return 0;
    return device->configRead8(0x08);
}

void GPUDetector::dumpPCIDevice(IOPCIDevice* device) {
    if (!device) return;
    
    uint16_t vendorId = device->configRead16(0);
    uint16_t deviceId = device->configRead16(2);
    uint8_t classCode = device->configRead8(0x0B);
    uint8_t subclassCode = device->configRead8(0x0A);
    uint8_t progIF = device->configRead8(0x09);
    uint8_t revisionId = device->configRead8(0x08);
    uint16_t subVendorId = device->configRead16(0x2C);
    uint16_t subDeviceId = device->configRead16(0x2E);
    
    GPU_DETECT_VERBOSE("=== PCI Device Dump ===");
    GPU_DETECT_VERBOSE("Vendor: 0x%04X", vendorId);
    GPU_DETECT_VERBOSE("Device: 0x%04X", deviceId);
    GPU_DETECT_VERBOSE("Class: 0x%02X%02X%02X", classCode, subclassCode, progIF);
    GPU_DETECT_VERBOSE("Revision: 0x%02X", revisionId);
    GPU_DETECT_VERBOSE("SubVendor: 0x%04X", subVendorId);
    GPU_DETECT_VERBOSE("SubDevice: 0x%04X", subDeviceId);
    GPU_DETECT_VERBOSE("=======================");
}

const char* GPUDetector::lookupMarketingName(GPUGeneration gen) {
    switch (gen) {
        case GPUGeneration::SandyBridge: return "Intel HD Graphics (Sandy Bridge)";
        case GPUGeneration::IvyBridge: return "Intel HD Graphics (Ivy Bridge)";
        case GPUGeneration::Haswell: return "Intel HD Graphics 4600 (Haswell)";
        case GPUGeneration::Broadwell: return "Intel HD Graphics (Broadwell)";
        case GPUGeneration::Skylake: return "Intel HD Graphics (Skylake)";
        case GPUGeneration::KabyLake: return "Intel HD Graphics (Kaby Lake)";
        case GPUGeneration::CoffeeLake: return "Intel UHD Graphics 630 (Coffee Lake)";
        case GPUGeneration::CometLake: return "Intel UHD Graphics (Comet Lake)";
        case GPUGeneration::IceLake: return "Intel Iris Plus Graphics (Ice Lake)";
        case GPUGeneration::TigerLake: return "Intel Iris Xe Graphics (Tiger Lake)";
        case GPUGeneration::RocketLake: return "Intel UHD Graphics (Rocket Lake)";
        case GPUGeneration::AlderLakeS: return "Intel UHD Graphics (Alder Lake-S)";
        case GPUGeneration::AlderLakeP: return "Intel UHD Graphics (Alder Lake-P)";
        case GPUGeneration::MeteorLake: return "Intel Xe Graphics (Meteor Lake)";
        case GPUGeneration::LunarLake: return "Intel Xe Graphics (Lunar Lake)";
        default: return "Unknown Intel GPU";
    }
}

// Graphics acceleration detection methods
bool GPUDetector::isGraphicsAccelerationActive(IOPCIDevice* device) {
    if (!device) {
        GPU_DETECT_LOG_ERROR("isGraphicsAccelerationActive: null device");
        return false;
    }

    // Check if QE/CI is enabled
    if (!verifyQE_CI_Enabled(device)) {
        GPU_DETECT_LOG_VERBOSE("Graphics acceleration: QE/CI not enabled");
        return false;
    }

    // Check graphics memory access
    if (!testGraphicsMemoryAccess(device)) {
        GPU_DETECT_LOG_VERBOSE("Graphics acceleration: memory access failed");
        return false;
    }

    // Check if graphics device is ready
    if (!isGraphicsDeviceReady(device)) {
        GPU_DETECT_LOG_VERBOSE("Graphics acceleration: device not ready");
        return false;
    }

    // Check IOGraphics acceleration
    IOService* graphicsService = nullptr;
    if (IORegistryEntry* entry = device->getParentEntry(gIOServicePlane)) {
        graphicsService = OSDynamicCast(IOService, entry);
    }

    if (graphicsService && !checkIOGraphicsAcceleration(graphicsService)) {
        GPU_DETECT_LOG_VERBOSE("Graphics acceleration: IOGraphics check failed");
        return false;
    }

    GPU_DETECT_LOG_VERBOSE("Graphics acceleration: active and working");
    return true;
}

bool GPUDetector::checkIOGraphicsAcceleration(IOService* graphicsService) {
    if (!graphicsService) {
        return false;
    }

    // Check for IOGraphics properties that indicate acceleration
    OSObject* accelProperty = graphicsService->getProperty("IOGraphicsAcceleration");
    if (!accelProperty) {
        return false;
    }

    // Check if acceleration is enabled
    OSBoolean* accelEnabled = OSDynamicCast(OSBoolean, accelProperty);
    if (!accelEnabled || !accelEnabled->getValue()) {
        return false;
    }

    // Check for graphics family
    OSObject* familyProperty = graphicsService->getProperty("IOGraphicsFamily");
    if (!familyProperty) {
        return false;
    }

    GPU_DETECT_LOG_VERBOSE("IOGraphics acceleration: enabled");
    return true;
}

bool GPUDetector::verifyQE_CI_Enabled(IOPCIDevice* device) {
    if (!device) {
        return false;
    }

    // Read PCI config space to check for QE/CI capabilities
    // This is a simplified check - in real implementation would check specific registers
    uint32_t configSpace[16];
    for (int i = 0; i < 16; i++) {
        configSpace[i] = device->configRead32(i * 4);
    }

    // Check if device has basic graphics capabilities
    uint16_t command = configSpace[1] & 0xFFFF;
    if (!(command & 0x0002)) { // Memory space enabled
        return false;
    }

    if (!(command & 0x0001)) { // I/O space enabled
        return false;
    }

    GPU_DETECT_LOG_VERBOSE("QE/CI: basic capabilities enabled");
    return true;
}

bool GPUDetector::testGraphicsMemoryAccess(IOPCIDevice* device) {
    if (!device) {
        return false;
    }

    // Get graphics memory size
    uint32_t memorySize = getGraphicsMemorySize(device);
    if (memorySize == 0) {
        GPU_DETECT_LOG_VERBOSE("Graphics memory: no memory detected");
        return false;
    }

    // Check if memory is accessible
    IOMemoryMap* memoryMap = device->mapDeviceMemoryWithIndex(0);
    if (!memoryMap) {
        GPU_DETECT_LOG_VERBOSE("Graphics memory: cannot map memory");
        return false;
    }

    // Try to access memory (read first few bytes)
    volatile uint32_t* memoryPtr = (volatile uint32_t*)memoryMap->getVirtualAddress();
    if (!memoryPtr) {
        memoryMap->release();
        GPU_DETECT_LOG_VERBOSE("Graphics memory: invalid virtual address");
        return false;
    }

    // Simple read test
    uint32_t testValue = *memoryPtr;
    memoryMap->release();

    GPU_DETECT_LOG_VERBOSE("Graphics memory: accessible (%u MB)", memorySize / (1024 * 1024));
    return true;
}

uint32_t GPUDetector::getGraphicsMemorySize(IOPCIDevice* device) {
    if (!device) {
        return 0;
    }

    // Read BAR0 to get memory size
    uint32_t bar0 = device->configRead32(0x10);
    if (!(bar0 & 0x1)) { // Memory BAR
        uint32_t memorySize = ~(bar0 & ~0xF) + 1;
        return memorySize;
    }

    return 0;
}

bool GPUDetector::isGraphicsDeviceReady(IOPCIDevice* device) {
    if (!device) {
        return false;
    }

    // Check device status
    uint16_t status = device->configRead16(0x06);
    if (status & 0x0008) { // Master data parity error
        GPU_DETECT_LOG_VERBOSE("Graphics device: parity error detected");
        return false;
    }

    if (!(status & 0x0010)) { // Capabilities list
        GPU_DETECT_LOG_VERBOSE("Graphics device: no capabilities list");
        return false;
    }

    // Check if device is enabled
    uint16_t command = device->configRead16(0x04);
    if (!(command & 0x0002)) { // Bus master enabled
        GPU_DETECT_LOG_VERBOSE("Graphics device: bus master not enabled");
        return false;
    }

    GPU_DETECT_LOG_VERBOSE("Graphics device: ready");
    return true;
}
