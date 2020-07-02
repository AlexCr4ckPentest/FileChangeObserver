#include "../include/FileChangeObserver.h"

#include <thread>

namespace alex
{

FileChangeObserver::FileChangeObserver() noexcept
{}

FileChangeObserver::FileChangeObserver(const std::vector<boost_fs::path> &paths,
                                       const uint32_t sleep_timeout,
                                       const uint16_t update_iteration)
    : m_update_iteration {update_iteration}
    , m_sleep_timeout {sleep_timeout}
{
    add_paths(paths);
}



FileChangeObserver::~FileChangeObserver() noexcept
{}



void FileChangeObserver::add_path(const boost_fs::path &path)
{
    if (!boost_fs::exists(path)) {
        throw std::runtime_error(std::string{"No such directory: "} + path.string());
    }
    m_paths_to_dirs.emplace_back(path);
}



void FileChangeObserver::add_paths(const std::vector<boost_fs::path> &paths)
{
    if (paths.size() == 0) {
        throw std::runtime_error("No directories for watch!");
    }
    for (auto path : paths) {
        add_path(std::move(path));
    }
}



void FileChangeObserver::load_file_modified_dates() noexcept
{
    for (auto dir_path : m_paths_to_dirs) {
        for (boost_fs::directory_iterator it {dir_path}; it != boost_fs::directory_iterator {}; ++it) {
            if (it->status().type() != boost_fs::file_type::directory_file) {
                m_files_in_dir.emplace_back(it->path());
                m_last_modified_dates.emplace_back(boost_fs::last_write_time(it->path()));
            }
        }
    }
}



void FileChangeObserver::start(void (*file_changed_handler)(std::string_view)) noexcept
{
    uint16_t iter_counter {0};
    std::time_t last_modified_time {};

    boost::system::error_code err_code {};
    std::vector<std::time_t>::iterator file_date_iter {};
    std::vector<boost_fs::path>::const_iterator file_path_iter {};

    load_file_modified_dates();

    while (true) {
        if (iter_counter == m_update_iteration) {
            update_dates_list();
            iter_counter = 0;
        }

        file_date_iter = m_last_modified_dates.begin();
        file_path_iter = m_files_in_dir.cbegin();

        for (; file_path_iter != m_files_in_dir.cend() && file_date_iter != m_last_modified_dates.end();
            ++file_path_iter, ++file_date_iter)
        {
            last_modified_time = boost_fs::last_write_time(*file_path_iter, err_code);
            if (err_code) {
                continue;
            }
            if (*file_date_iter != last_modified_time) {
                *file_date_iter = last_modified_time;
                file_changed_handler(file_path_iter->string());
            }
        }

        iter_counter++;

        std::this_thread::sleep_for(std::chrono::microseconds(m_sleep_timeout));
    }
}

} // namespace alex
