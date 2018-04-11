/*
 * atomspace/query/query_benchmark.cc
 *
 * Copyright (C) 2016-2018 OpenCog Foundation
 *
 * Authors: Linas Vepstas 2016
 *          Vitaly Bogdanov 2018
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <string>
#include <vector>
#include <iostream>

#include <opencog/util/Config.h>
#include <opencog/guile/SchemeEval.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/query/BindLinkAPI.h>

#define DEFAULT_CONFIG_FILE_NAME "query_benchmark.conf"
#define BENCHMARKS_TO_RUN_PROPERTY "benchmarks_to_run"

using namespace opencog;

std::string working_dir = ".";
std::string config_file_name = DEFAULT_CONFIG_FILE_NAME;
std::string benchmarks_to_run = "";

Config configuration;

void load_scheme(SchemeEval& scheme)
{
    scheme.eval("(use-modules (opencog))");
    scheme.eval("(add-to-load-path \".\")");
}

void load_scheme_atomspace(SchemeEval& scheme,
                           const std::string& atomspace_filename)
{
    scheme.eval("(load-from-path \"" + atomspace_filename + "\")");
}

Handle load_scheme_query(SchemeEval& scheme, const std::string& query_filename)
{
    return scheme.eval_h("(load-from-path \"" + query_filename + "\")");
}

double duration_in_secs(clock_t start, clock_t end)
{
    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

void run_benchmark(const std::string& id)
{
    std::string atomspace_file = configuration.get(id + "_atomspace_file",
                                 id + ".atomspace.scm");
    std::string query_file = configuration.get(id + "_query_file",
                             id + ".query.scm");
    int iterations_count = configuration.get_int(id + "_iterations_count", 1);

    std::cout << "running benchmark id: " << id << std::endl;
    std::cout << "atomspace file: " << atomspace_file << std::endl;
    std::cout << "query file: " << query_file << std::endl;
    std::cout << "number of iterations: " << iterations_count << std::endl;

    AtomSpace atomspace;
    SchemeEval scheme(&atomspace);
    load_scheme(scheme);

    clock_t start = clock();
    load_scheme_atomspace(scheme, atomspace_file);
    Handle query = load_scheme_query(scheme, query_file);
    if (!query) {
        std::cerr << "could not load query, stopping test" << std::endl;
        return;
    }
    std::cout << "atomspace and query are loadded in: "
        << duration_in_secs(start, clock()) << " secs" << std::endl;

    Handle result;
    start = clock();
    for (int iteration = 0; iteration < iterations_count; iteration++) {
        result = satisfying_set(&atomspace, query);
    }
    std::cout << "query executed " << iterations_count << " time(s) in: "
        << duration_in_secs(start, clock()) << " secs" << std::endl;

    if (configuration.get_bool("print_results", true)) {
        std::cout << "results are: " << result->to_string() << std::endl;
    }
}

int parse_command_line(int argc, char** argv)
{
    const std::string description =
        "Query benchmark tool\n"
        "Usage: query_benchmark [-d <working_dir>] [-c <config>] [-t <benchmark_id>]\n"
        "Options:\n"
        "  -d <working_dir> - working dir, default: current dir\n"
        "\n"
        "  -c <config> - configuration file to load, default: query_benchmark.conf\n"
        "\n"
        "    Configuration file properties:\n"
        "      - guile_auto_compile=(true|false) # whether guile autocompilation should be enabled\n"
        "      - print_results=(true|false) # print query results after last execution\n"
        "      - benchmarks_to_run=benchmark1,benchmark2 # comma separated list of benchmarks to run\n"
        "      - <benchmark>_atomspace_file=<filename.scm> # scheme file describing atomspace to load\n"
        "      - <benchmark>_query_file=<filename.scm> # scheme file describing query to execute\n"
        "      - <benchmark>_iterations_count=<number> # number of times to execute query\n"
        "\n"
        "  -t <benchmark_id>,... - comma separated list of benchmarks to run,\n"
        "                          default: run all benchmarks from config\n";
    int c;

    opterr = 0;
    while ((c = getopt(argc, argv, "d:t:c:")) != -1)
    {
        switch (c) {
        case 'd':
            working_dir = optarg;
            break;
        case 't':
            benchmarks_to_run = optarg;
            break;
        case 'c':
            config_file_name = optarg;
            break;
        case '?':
            std::cerr << description;
            return -1;
        default:
            std::cerr << "Unknown option: " << optopt << std::endl;
            return -1;
        }
    }

    return 0;
}

void load_benchmarks_from_configuration(std::vector<std::string>& benchmarks_to_run)
{
    if (configuration.has(BENCHMARKS_TO_RUN_PROPERTY)) {
        std::istringstream istream(configuration.get(BENCHMARKS_TO_RUN_PROPERTY));

        while (istream.good()) {
            std::string benchmark_id;
            std::getline(istream, benchmark_id, ',');
            benchmarks_to_run.push_back(benchmark_id);
        }
    }
}

int main(int argc, char** argv)
{
    if (parse_command_line(argc, argv)) {
        return 0;
    }

    if (chdir(working_dir.c_str())) {
        std::cerr << "could not change dir to: " << working_dir << std::endl;
        return 0;
    }
    configuration.load(config_file_name.c_str(), false);
    if (!benchmarks_to_run.empty()) {
        configuration.set(BENCHMARKS_TO_RUN_PROPERTY, benchmarks_to_run);
    }

    // Switch Guile autocompilation off to prevent compilation on big data sets
    if (!configuration.get_bool("guile_auto_compile", true)) {
        setenv("GUILE_AUTO_COMPILE", "0", 1);
    }

    std::vector<std::string> benchmarks_to_run;
    load_benchmarks_from_configuration(benchmarks_to_run);

    if (benchmarks_to_run.empty()) {
        std::cerr << "no benchmarks to run" << std::endl;
        return 0;
    }

    for (const std::string& benchmark_id : benchmarks_to_run) {
        run_benchmark(benchmark_id);
    }

    return 0;
}
