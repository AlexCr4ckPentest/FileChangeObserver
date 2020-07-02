#include <iostream>

#include <boost/program_options.hpp>

#include "include/FileChangeObserver.h"

enum
{
    DEFAULT_UPDATE_ITERATION = 35,
    DEFAULT_SLEEP_TIMEOUT = 1500,

    MIN_UPDATE_ITERATION = 1,
    MIN_SLEEP_TIMEOUT = 100,

    MAX_UPDATE_ITERATION = 150,
    MAX_SLEEP_TIMEOUT = 5500
};

int main(int argc, char **argv)
{
    namespace po = boost::program_options;
    
    uint16_t update_iteration {0};
    uint32_t sleep_timeout {0};

    alex::FileChangeObserver file_watcher {};

    po::variables_map parsed_options {};
    po::options_description opts_descr {"Available options"};

    opts_descr.add_options()
        ("help,h", "show help message")
        ("update-iter,U", po::value<uint16_t>(&update_iteration)->value_name("iter")->default_value(DEFAULT_UPDATE_ITERATION), "iteration of updating files and dates lists")
        ("timeout,T", po::value<uint32_t>(&sleep_timeout)->value_name("timeout")->default_value(DEFAULT_SLEEP_TIMEOUT), "sleep timeout")
        ("paths,P", po::value<std::vector<boost_fs::path> >()->value_name("paths")->multitoken(), "paths to directories for monitoring")
    ;


    try {
        po::store(po::parse_command_line(argc, argv, opts_descr), parsed_options);
        po::notify(parsed_options);
    }
    catch (const boost::program_options::error &err) {
        std::cerr << "Error: " << err.what() << "\n";
        std::cout << opts_descr;
        return 1;
    }

    if (parsed_options.count("help")) {
        std::cout << opts_descr;
        return 0;
    }

    if (sleep_timeout < MIN_SLEEP_TIMEOUT || sleep_timeout > MAX_SLEEP_TIMEOUT) {
        std::cerr << "Sleep timeout is long or short!\n";
        return 1;
    }
    if (update_iteration < MIN_UPDATE_ITERATION || update_iteration > MAX_UPDATE_ITERATION) {
        std::cerr << "Update iteration is long or short!\n";
        return 1;
    }

    if (!parsed_options.count("paths")) {
        std::cerr << "No directories for monitoring! Required option --paths (see help)\n";
        return 1;
    }

    file_watcher.set_sleep_timeout(sleep_timeout);
    file_watcher.set_update_iteration(update_iteration);
    file_watcher.add_paths(parsed_options["paths"].as<std::vector<boost_fs::path> >());

    file_watcher.start([](std::string_view path)
                       {
                           std::cout << "File modified: " << path << "\n";
                       });

    return 0;
}
