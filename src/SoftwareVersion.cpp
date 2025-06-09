/**
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All rights reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 * This file contains the declaration of a software version class.
 */

#include<UTILITIES/common/SoftwareVersion.h>
#include <cstdio>
#include <cstring>

//==============================================================//
/// CURRENT SOFTWARE VERSION
//==============================================================//

/**
 * @brief Defines the software version.
 *
 * The software version is represented by three components: major, minor, and build numbers.
 * These components should be incremented based on the nature of the changes made to the software.
 *
 * - **Major Version**: Incremented for significant changes that may include backward-incompatible changes,
 *   major new features, or substantial improvements. For example, if the current version is 2.0.0001 and a major
 *   update is made, the new version should be 3.0.0000.
 *
 * - **Minor Version**: Incremented for backward-compatible changes that add functionality or improvements.
 *   This includes adding new features or enhancements that do not break existing functionality. For example,
 *   if the current version is 2.0.0001 and a minor update is made, the new version should be 2.1.0000.
 *
 * - **Build Number**: Incremented for small changes, bug fixes, or other minor updates that do not add new
 *   functionality or significantly alter existing functionality. This is typically incremented with each build
 *   or release. For example, if the current version is 2.0.0001 and a bug fix is made, the new version should be 2.0.0002.
 *
 * Example:
 * SoftwareVersion softwareVersion(2, 0, 0001);
 * - Major version: 2
 * - Minor version: 0
 * - Build number: 0001
 */
SoftwareVersion softwareVersion ( 3, 0, 0000 );

//==============================================================//
/// CLASS
//==============================================================//

/**
 * @brief Constructs a new SoftwareVersion object.
 *
 * Initializes the software version with the specified major, minor, and build numbers.
 *
 * @param major The major version number.
 * @param minor The minor version number.
 * @param build The build number.
 */
SoftwareVersion::SoftwareVersion(uint8_t majorArg, uint8_t minorArg, uint32_t buildArg) :
    major(majorArg),
    minor(minorArg),
    build(buildArg),
    text()
{
    snprintf(text, sizeof(text), "v%u.%u.%lu", major, minor, build);
}

/**
 * @brief Returns the software version as a formatted string.
 *
 * The string is formatted as "v<major>.<minor>.<build>", for example, "v1.3.124".
 *
 * @return A constant character pointer to the formatted version string.
 */
const char* SoftwareVersion::GetString() const noexcept
{
    return text; // Skip the 'v'
}

/**
 * @brief Returns the major version number.
 *
 * @return The major version number.
 */
uint8_t SoftwareVersion::GetMajor() const noexcept
{
    return major;
}

/**
 * @brief Returns the minor version number.
 *
 * @return The minor version number.
 */
uint8_t SoftwareVersion::GetMinor() const noexcept
{
    return minor;
}

/**
 * @brief Returns the build number.
 *
 * @return The build number.
 */
uint32_t SoftwareVersion::GetBuild() const noexcept
{
    return build;
}
