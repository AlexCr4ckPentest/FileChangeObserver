#include <vector>
#include <string_view>
#include <stdexcept>

#include <boost/filesystem.hpp>

#ifndef _FILE_CHANGE_OBSERVER_H_
#define _FILE_CHANGE_OBSERVER_H_

namespace boost_fs = boost::filesystem;

namespace my
{

constexpr uint32_t __DEFAULT_SLEEP_TIMEOUT      = 1500;
constexpr uint16_t __DEFAULT_UPDATE_ITERATION   = 35;

class FileChangeObserver
{
public:
    FileChangeObserver() noexcept;
    FileChangeObserver(const std::vector<boost_fs::path> &paths,
                       const uint32_t sleep_timeout = __DEFAULT_SLEEP_TIMEOUT,
                       const uint16_t update_iteration = __DEFAULT_UPDATE_ITERATION);
    ~FileChangeObserver() noexcept;

    FileChangeObserver(const FileChangeObserver&) = delete;
    FileChangeObserver& operator=(const FileChangeObserver&) = delete;

    void add_path(const boost_fs::path &path);
    void add_paths(const std::vector<boost_fs::path> &paths);

    inline void set_sleep_timeout(const uint32_t sleep_timeout) noexcept
    { m_sleep_timeout = sleep_timeout; }

    inline void set_update_iteration(const uint16_t update_iteration) noexcept
    { m_update_iteration = update_iteration; }

    void start(void (*file_changed_handler)(std::string_view)) noexcept;

private:
    void load_file_modified_dates() noexcept;

    inline void update_dates_list() noexcept
    {
        m_last_modified_dates.clear();
        m_files_in_dir.clear();
        load_file_modified_dates();
    }

private:
    uint16_t m_update_iteration {};
    uint32_t m_sleep_timeout {};

    std::vector<boost_fs::path> m_paths_to_dirs {};

    std::vector<std::time_t> m_last_modified_dates {};
    std::vector<boost_fs::path> m_files_in_dir {};
};

} // namespace my

#endif // _FILE_CHANGE_OBSERVER_H_
