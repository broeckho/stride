#include "File.h"

/**
 * @file
 * Implementation of file utils.
 */

using namespace boost;

namespace stride {
namespace util {

const filesystem::path completePath(const filesystem::path& filename, const filesystem::path& root)
{
        return complete(filename, root);
}

const filesystem::path checkFile(const filesystem::path& filename, const filesystem::path& root)
{
        const boost::filesystem::path file_path = canonical(completePath(filename, root));
        if (!is_regular_file(file_path)) {
                throw std::runtime_error(std::string(__func__) + ">File " + file_path.string() +
                                         " not present. Aborting.");
        }
        return file_path;
}

} // namespace util
} // namespace stride
