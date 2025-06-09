
#ifndef TESTMANAGER_H
#define TESTMANAGER_H

#include <functional>
#include <array>
#include <optional>

/**
 * @brief Manages a sequence of tests.
 *
 * This class allows you to add tests, run them in sequence, and handle success or failure conditions.
 * Tests can be specified with an enum value and accessed using a random access iterator.
 *
 * @tparam EnumType The enum type used to specify test types.
 * @tparam MaxEnumVal The maximum value of the enum type.
 */
template <typename EnumType, EnumType MaxEnumVal>
class TestManager {
public:
    /**
     * @brief Constructor.
     */
    TestManager() noexcept;

    /**
     * @brief Adds a test to the sequence.
     *
     * @param testType The type of the test.
     * @param test The test function, which returns a boolean indicating success.
     * @param continueOnFail Boolean indicating whether to continue if the test fails.
     */
    void AddTest(EnumType testType, const std::function<bool()>& test, bool continueOnFail) noexcept;

    /**
     * @brief Sets a hook function to be called after each test.
     *
     * @param hook The hook function, which takes the result of the test (true/false) and the test type.
     */
    void SetAfterTestHook(const std::function<void(bool, EnumType)>& hook) noexcept;

    /**
     * @brief Starts the test sequence from the start test type to the end test type (inclusive).
     *
     * @param start The starting test type.
     * @param end The ending test type.
     * @return true is successful, false otherwise.
     */
    bool Start(EnumType start, EnumType end) noexcept;

    /**
     * @brief Steps through the tests one by one.
     *
     * @return False if there are no more tests to run or if a test fails and should not continue, otherwise true.
     */
    bool Step() noexcept;

    /**
     * @brief Checks if the test sequence is still running.
     *
     * @return True if the test sequence is running, otherwise false.
     */
    bool IsRunning() const noexcept;

    /**
     * @brief Sets whether a test should continue on fail.
     *
     * @param testType The type of the test to modify.
     * @param continueOnFail Boolean indicating whether to continue if the test fails.
     */
    void setContinueOnFail(EnumType testType, bool continueOnFail) noexcept;

    /**
     * @brief Resets the test manager, clearing all tests.
     */
    void Reset() noexcept;

    /**
     * @brief Checks if a test is defined for a given type.
     *
     * @param testType The type of the test to check.
     * @return True if the test is defined, otherwise false.
     */
    bool IsTestDefined(EnumType testType) const noexcept;



    /**
     * @brief Gets a random access iterator to the tests.
     *
     * @return A random access iterator to the tests.
     */
    auto begin() noexcept { return tests_.begin(); }
    auto end() noexcept { return tests_.end(); }

    /**
     * @brief Runs a test directly by its type.
     *
     * @param testType The type of the test to run.
     * @return True if the test ran and passed, false if the test failed or was not found.
     */
    bool operator[](EnumType testType) noexcept;

private:
    static constexpr std::size_t MaxTests = static_cast<std::size_t>(MaxEnumVal) + 1;
    std::array<std::pair<std::function<bool()>, bool>, MaxTests> tests_; /**< The array of tests. */
    std::function<void(bool, EnumType)> afterTestHook_; /**< The hook function to be called after each test. */
    int currentIndex_; /**< The current index in the test sequence. */
    int endIndex_; /**< The end index in the test sequence. */
    bool running_; /**< Indicates whether the test sequence is running. */

    /**
     * @brief Converts an EnumType to its corresponding index.
     *
     * @param testType The test type to convert.
     * @return The index of the test type.
     */
    static constexpr std::size_t toIndex(EnumType testType) noexcept {
        return static_cast<std::size_t>(testType);
    }
};

template <typename EnumType, EnumType MaxEnumVal>
TestManager<EnumType, MaxEnumVal>::TestManager() noexcept : currentIndex_(-1), endIndex_(-1), running_(false) {
    /// Initialize all test slots with a default test that always passes
    for (auto& test : tests_) {
        test = std::make_pair([]() { return true; }, true);
    }
}

template <typename EnumType, EnumType MaxEnumVal>
void TestManager<EnumType, MaxEnumVal>::AddTest(EnumType testType, const std::function<bool()>& test, bool continueOnFail) noexcept {
    tests_[toIndex(testType)] = std::make_pair(test, continueOnFail);
}

template <typename EnumType, EnumType MaxEnumVal>
void TestManager<EnumType, MaxEnumVal>::SetAfterTestHook(const std::function<void(bool, EnumType)>& hook) noexcept {
    afterTestHook_ = hook;
}

template <typename EnumType, EnumType MaxEnumVal>
bool TestManager<EnumType, MaxEnumVal>::Start(EnumType start, EnumType end) noexcept {
    auto startIndex = toIndex(start);
    auto endIndex = toIndex(end);
    if (startIndex > endIndex || endIndex >= MaxTests) {
        return false;
    }
    currentIndex_ = startIndex;
    endIndex_ = endIndex;
    running_ = true;

    return true;
}

template <typename EnumType, EnumType MaxEnumVal>
bool TestManager<EnumType, MaxEnumVal>::Step() noexcept {
    if (!running_ || currentIndex_ > endIndex_) {
        return false;
    }

    auto& testPair = tests_[currentIndex_];
    bool result = testPair.first();

    if (afterTestHook_) {
        afterTestHook_(result, static_cast<EnumType>(currentIndex_));
    }

    if (!result && !testPair.second) {
        running_ = false;
        return false;
    }

    ++currentIndex_;
    if (currentIndex_ > endIndex_) {
        running_ = false;
    }

    return true;
}

template <typename EnumType, EnumType MaxEnumVal>
bool TestManager<EnumType, MaxEnumVal>::IsRunning() const noexcept {
    return running_;
}

template <typename EnumType, EnumType MaxEnumVal>
bool TestManager<EnumType, MaxEnumVal>::operator[](EnumType testType) noexcept {
    auto& testPair = tests_[toIndex(testType)];
    bool result = testPair.first();

    if (afterTestHook_) {
        afterTestHook_(result, testType);
    }

    return result;
}

template <typename EnumType, EnumType MaxEnumVal>
void TestManager<EnumType, MaxEnumVal>::setContinueOnFail(EnumType testType, bool continueOnFail) noexcept {
    tests_[toIndex(testType)].second = continueOnFail;
}

template <typename EnumType, EnumType MaxEnumVal>
void TestManager<EnumType, MaxEnumVal>::Reset() noexcept {
    for (auto& test : tests_) {
        test = std::make_pair([]() { return true; }, true);
    }
    currentIndex_ = -1;
    endIndex_ = -1;
    running_ = false;
}

template <typename EnumType, EnumType MaxEnumVal>
bool TestManager<EnumType, MaxEnumVal>::IsTestDefined(EnumType testType) const noexcept {
    const auto& testPair = tests_[toIndex(testType)];
    return testPair.first != nullptr;
}

#endif // TESTMANAGER_H
