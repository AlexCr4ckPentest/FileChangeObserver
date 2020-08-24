#include <vector>
#include <future>

#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#ifndef _FILE_CHANGE_OBSERVER_H_
#define _FILE_CHANGE_OBSERVER_H_

namespace alex
{

class FileChangeObserver
{
    using PathList = std::vector<boost::filesystem::path>;
    using DateList = std::vector<std::time_t>;

public:
    FileChangeObserver() noexcept;
    FileChangeObserver(const PathList& path_list, const uint32_t timer_interval_msec, const uint32_t update_frequency) noexcept;
    ~FileChangeObserver() noexcept;

    FileChangeObserver(const FileChangeObserver&) = delete;
    FileChangeObserver& operator=(const FileChangeObserver&) = delete;

    void add_path(const boost::filesystem::path& path) noexcept;
    void add_paths(const PathList& path_list) noexcept;

    inline void set_timer_interval(const uint32_t timer_interval_msec) noexcept
    { m_timer_interval_msec = timer_interval_msec; }

    inline void set_update_frequency(const uint32_t update_frequency)
    { m_update_frequency = update_frequency; }

    void start(const std::function<void(const boost::filesystem::path&)>& notifier) noexcept;

    inline auto async_start(const std::function<void(const boost::filesystem::path&)>& notifier) noexcept
    { return std::async(std::launch::async, std::bind(&FileChangeObserver::start, this, notifier)); }

private:
    static PathList m_working_dirs;
    static PathList m_files_list;
    static DateList m_date_list;

    static uint32_t m_timer_interval_msec;
    static uint32_t m_update_frequency;

    static void load_files_and_dates() noexcept;

    static inline void update_files_and_dates() noexcept
    {
        m_files_list.clear();
        m_date_list.clear();
        load_files_and_dates();
    }

    static void on_timer_update(const boost::system::error_code& error, boost::asio::deadline_timer* timer,
                                const std::function<void()>& run_observer) noexcept;

    static void start_observing(const std::function<void(const boost::filesystem::path&)>& notifier) noexcept;
};

} // namespace alex

#endif // _FILE_CHANGE_OBSERVER_H_
