#include "../include/FileChangeObserver.h"

#include <iostream>

namespace alex
{

uint32_t FileChangeObserver::m_timer_interval_msec {};
uint32_t FileChangeObserver::m_update_frequency {};

FileChangeObserver::PathList FileChangeObserver::m_working_dirs {};
FileChangeObserver::PathList FileChangeObserver::m_files_list {};
FileChangeObserver::DateList FileChangeObserver::m_date_list {};

static uint8_t tick_count {};



FileChangeObserver::FileChangeObserver() noexcept
{}



FileChangeObserver::FileChangeObserver(const FileChangeObserver::PathList& path_list,
                                       const uint32_t timer_interval_msec,
                                       const uint32_t update_frequency) noexcept
{
    add_paths(path_list);
    m_timer_interval_msec = timer_interval_msec;
    m_update_frequency = update_frequency;
}



FileChangeObserver::~FileChangeObserver() noexcept
{}



void FileChangeObserver::add_path(const boost::filesystem::path& path) noexcept
{
    if (!boost::filesystem::exists(path)) {
        std::cerr << "Error: no such directory: " + path.string() << "\n";
        std::terminate();
    }
    m_working_dirs.emplace_back(path);
}



void FileChangeObserver::add_paths(const std::vector<boost::filesystem::path>& path_list) noexcept
{
    if (path_list.empty()) {
        std::cerr << "Error: paths_list is empty!!\n";
        std::terminate();
    }
    for (auto path : path_list) {
        add_path(std::move(path));
    }
}



void FileChangeObserver::start_observing(const std::function<void(const boost::filesystem::path&)>& notifier) noexcept
{
    if (!m_files_list.empty()) {
        PathList::const_iterator file_iter {};
        DateList::iterator date_iter {};
        std::time_t last_modified_date {};
        boost::system::error_code error {};

        for (file_iter = m_files_list.cbegin(), date_iter = m_date_list.begin();
             file_iter != m_files_list.cend() && date_iter != m_date_list.end();
             ++file_iter, ++date_iter)
        {
            last_modified_date = boost::filesystem::last_write_time(*file_iter, error);
            if (error) { continue; }
            if (*date_iter != last_modified_date) {
                *date_iter = last_modified_date;
                notifier(*file_iter);
            }
        }
    }
}



void FileChangeObserver::load_files_and_dates() noexcept
{
    for (auto path : m_working_dirs) {
        boost::filesystem::directory_iterator dir_iter {path};
        for (; dir_iter != boost::filesystem::directory_iterator{}; ++dir_iter) {
            if (dir_iter->status().type() != boost::filesystem::file_type::directory_file) {
                m_files_list.emplace_back(dir_iter->path());
                m_date_list.emplace_back(boost::filesystem::last_write_time(dir_iter->path()));
            }
        }
    }
}



void FileChangeObserver::on_timer_update([[maybe_unused]] const boost::system::error_code& error,
                                        boost::asio::deadline_timer* timer,
                                        const std::function<void()>& run_observer) noexcept
{
    if (tick_count == m_update_frequency) {
        update_files_and_dates();
        tick_count = 0;
    }

    tick_count++;

    run_observer();
    timer->expires_from_now(boost::posix_time::milliseconds(m_timer_interval_msec));
    timer->async_wait(boost::bind(on_timer_update, boost::asio::placeholders::error, timer, run_observer));
}



void FileChangeObserver::start(const std::function<void(const boost::filesystem::path&)>& notifier) noexcept
{
    load_files_and_dates();

    boost::asio::io_context io_context {};
    boost::asio::deadline_timer timer {io_context};

    timer.expires_from_now(boost::posix_time::milliseconds(m_timer_interval_msec));
    timer.async_wait(boost::bind(on_timer_update, boost::asio::placeholders::error, &timer, std::bind(start_observing, notifier)));

    io_context.run();
}

} // namespace alex
