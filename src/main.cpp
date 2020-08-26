#include <iostream>

#include "../include/FileChangeObserver.h"

#ifdef _WIN32
#   include <windows.h>
#endif // _WIN32

#include <boost/program_options.hpp>

enum
{
    TIMER_INTERVAL_MIN = 100,
    UPDATE_FREQ_MIN = 5
};

int main(int argc, char** argv)
{
    boost::program_options::options_description opts_descr {"Available options"};
    uint32_t timer_interval {};
    uint32_t update_frequency {};

    opts_descr.add_options()
        ("help,h", "show help message")
        ("paths,P", boost::program_options::value<std::vector<boost::filesystem::path> >()->value_name("paths")->multitoken(), "paths to directories for monitoring")
        ("timer-interval,T", boost::program_options::value<uint32_t>(&timer_interval)->value_name("msec")->default_value(TIMER_INTERVAL_MIN * 10), "timer interval (in msec)")
        ("update-freq,U", boost::program_options::value<uint32_t>(&update_frequency)->value_name("freq")->default_value(UPDATE_FREQ_MIN * 6), "update fequency")
    ;

    try {
        boost::program_options::variables_map parsed_options {};

        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opts_descr), parsed_options);
        boost::program_options::notify(parsed_options);

        if (parsed_options.count("help") || parsed_options.size() < 1) {
            std::cout << opts_descr;
            return 0;
        }

        if (!parsed_options.count("paths")) {
            std::cerr << "Error: required option --paths (see help)\n";
            return 1;
        }

        if (timer_interval < TIMER_INTERVAL_MIN) {
            std::cerr << "Error: timer interval cannot be less than " << TIMER_INTERVAL_MIN << "\n";
            return 1;
        }

        if (update_frequency < UPDATE_FREQ_MIN) {
            std::cerr << "Error: update frequency cannot be less than " << TIMER_INTERVAL_MIN << "\n";
            return 1;
        }

        alex::FileChangeObserver file_change_observer {parsed_options["paths"].as<std::vector<boost::filesystem::path> >(),
                                                        timer_interval, update_frequency};

        file_change_observer.start([](const boost::filesystem::path& path)
        {
            std::cout << "File has been modified: " << path.string() << "\n";
#ifdef _WIN32
            MessageBeep(MB_ICONWARNING);
#endif // _WIN32
        });
    }
    catch (const boost::program_options::error& err) {
        std::cerr << "Error: " << err.what() << "\n";
        std::cout << opts_descr;
        return 1;
    }

    return 0;
}
