/*
 * query/query_benchmark.cc
 *
 * Copyright (C) 2016 Linas Vepstas
 * All Rights Reserved
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

using namespace opencog;

std::string config_file_name = "query_benchmark.conf";
std::vector<std::string> benchmarks_to_run;

Config configuration;

void load_scheme(SchemeEval& scheme)
{
    scheme.eval("(use-modules (opencog))");
    scheme.eval("(add-to-load-path \".\")");
}

void load_scheme_atomspace(SchemeEval& scheme, const std::string& atomspace_filename)
{
    scheme.eval("(load-from-path \"" + atomspace_filename + "\")");
}

Handle load_scheme_query(SchemeEval& scheme, const std::string& query_filename)
{
    return scheme.eval_h("(load-from-path \"" + query_filename + "\")");
}

void run_test(const std::string& id)
{
    std::string atomspace_file = configuration.get(id + "_atomspace_file", id + ".atomspace.scm");
    std::string query_file = configuration.get(id + "_query_file", id + ".query.scm");
    int iterations_count = configuration.get_int(id + "_iterations_count", 1);

    std::cout << "running testcase id: " << id << std::endl;
    std::cout << "number of iterations: " << iterations_count << std::endl;
    std::cout << "atomspace file: " << atomspace_file << std::endl;
    std::cout << "query file: " << query_file << std::endl;

    AtomSpace atomspace;
    SchemeEval scheme(&atomspace);
    load_scheme(scheme);
    load_scheme_atomspace(scheme, atomspace_file);
    Handle query = load_scheme_query(scheme, query_file);

    Handle result;
    for (int iteration = 0; iteration < iterations_count; iteration++)
    {
        result = satisfying_set(&atomspace, query);
    }

    std::cout << "results are: " << result->to_string() << std::endl;
}

int parse_command_line(int argc, char** argv) {
    const std::string description = "Query benchmark tool\n"
        "Usage: query_benchmark -t <benchmark_id> [-c <config>]\n"
        "-c <config>\t\tConfiguration file to load (default: query_benchmark.conf)\n"
        "-t <benchmark_id>\tBenchmark id to run, \"all\" run all benchmarks\n";
    int c;

    if (argc == 1) {
        std::cout << description;
        return -1;
    }

    opterr = 0;
    while ((c = getopt(argc, argv, "t:c:")) != -1) {
        switch (c)
        {
            case 't':
                benchmarks_to_run.push_back(optarg);
                break;
            case 'c':
                config_file_name = optarg;
                break;
            case '?':
                std::cout << description;
                return -1;
            default:
                std::cout << "Unknown option: " << optopt << std::endl;
                return -1;
        }
    }

    return 0;
}

void load_all_benchmarks() {
    benchmarks_to_run.clear();
    for (int i = 0; i < configuration.get_int("number_of_benchmarks", 0); ++i) {
        const std::string& benchmark_id = configuration.get("benchmark_id_" + std::to_string(i));
        benchmarks_to_run.push_back(benchmark_id);
    }
}

int main(int argc, char** argv)
{
    if (parse_command_line(argc, argv)) {
        return 0;
    }

    configuration.load(config_file_name.c_str());

    // Switch Guile autocompilation off to prevent compilation on big data sets
    if (!configuration.get_bool("guile_auto_compile", true)) {
        setenv("GUILE_AUTO_COMPILE", "0", 1);
    }

    if (std::find(benchmarks_to_run.begin(), benchmarks_to_run.end(), "all") != benchmarks_to_run.end()) {
        load_all_benchmarks();
    }

    for (const std::string& benchmark_id : benchmarks_to_run) {
        run_test(benchmark_id);
    }

    return 0;
}
