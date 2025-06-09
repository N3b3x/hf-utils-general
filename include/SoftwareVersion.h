/**
 * Nebula Tech Corporation
 *
 * Copyright © 2023 Nebula Tech Corporation. All rights reserved.
 * This file is part of HardFOC and is licensed under the GNU General Public License v3.0 or later.
 *
 * This file contains the declaration of a software version class.
 */

#ifndef SoftwareVersion_H
#define SoftwareVersion_H

#include <cstdint>
#include <string>

/**
 * @class SoftwareVersion
 * @brief Class to define the software version.
 *
 * This class encapsulates the software version information, including major, minor, and build numbers.
 * It provides methods to retrieve these values and a formatted version string.
 */
class SoftwareVersion
{
public:
    /**
     * @brief Constructs a new SoftwareVersion object.
     *
     * Initializes the software version with the specified major, minor, and build numbers.
     *
     * @param major The major version number.
     * @param minor The minor version number.
     * @param build The build number.
     */
    SoftwareVersion(uint8_t major, uint8_t minor, uint32_t build);

    /**
     * @brief Deleted copy constructor to avoid copying instances.
     */
    SoftwareVersion(const SoftwareVersion&) = delete;

    /**
     * @brief Deleted assignment operator to avoid copying instances.
     */
    SoftwareVersion& operator=(const SoftwareVersion&) = delete;

    /**
     * @brief Default destructor.
     *
     * De-initializes the singleton. It is not likely to be called in released products, but is implemented to support unit testing.
     */
    ~SoftwareVersion() = default;

    /**
     * @brief Returns the software version as a formatted string.
     *
     * The string is formatted as "v<major>.<minor>.<build>", for example, "v1.3.124".
     *
     * @return A constant character pointer to the formatted version string.
     */
    const char* GetString() const noexcept;

    /**
     * @brief Returns the major version number.
     *
     * @return The major version number.
     */
    uint8_t GetMajor() const noexcept;

    /**
     * @brief Returns the minor version number.
     *
     * @return The minor version number.
     */
    uint8_t GetMinor() const noexcept;

    /**
     * @brief Returns the build number.
     *
     * @return The build number.
     */
    uint32_t GetBuild() const noexcept;

private:
    static constexpr uint32_t MaxStringLength = 22; ///< Maximum length of the version string.
    const uint8_t major; ///< Major version number.
    const uint8_t minor; ///< Minor version number.
    const uint32_t build; ///< Build number.
    char text[MaxStringLength]; ///< Buffer to hold the formatted version string.
};

extern SoftwareVersion softwareVersion;


#endif /* SoftwareVersion_H */
