#ifndef MAIN_CPP_UTIL_FILE_H_
#define MAIN_CPP_UTIL_FILE_H_

/**
 * @file
 * Header file for file utils
 */

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace stride {
namespace util {

/**
 * Completes the file relative to root.
 * @param filename: filename to complete
 * @param root: default execution path.
 * @return the full path to the file if it exists
 */
const boost::filesystem::path completePath(const boost::filesystem::path& filename,
                                           const boost::filesystem::path& root = boost::filesystem::current_path());

/**
 * Checks if there is a file with "filename" relative to the exectution path.
 * @param filename: filename to check.
 * @param root: Default execution path.
 * @return the full path to the file if it exists
 * @throws runtime error if file doesn't exist
 */
const boost::filesystem::path checkFile(const boost::filesystem::path& filename,
                                        const boost::filesystem::path& root = boost::filesystem::current_path());

} // namespace util
} // namespace stride

#endif /* MAIN_CPP_UTIL_FILE_H_ */
