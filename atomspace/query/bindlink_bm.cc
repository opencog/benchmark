/*
 * query/bindlink_bm.cc
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

#include <iostream>
#include <opencog/guile/SchemeEval.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/query/BindLinkAPI.h>

using namespace opencog;

AtomSpace* atomspace;
SchemeEval* scheme;

int iterations_count = 1;
std::string testcase = "animals";

void load_scheme()
{
    // Load some scheme for the setup
    scheme->eval("(use-modules (opencog))");
    scheme->eval("(add-to-load-path \".\")");
}

void load_scheme_atomspace(const std::string& testcase)
{
    std::string atomspace_filename = testcase + ".atomspace.scm";
    scheme->eval("(load-from-path \"" + atomspace_filename + "\")");
}

Handle load_scheme_query(const std::string& testcase)
{
    std::string query_filename = testcase + ".query.scm";
    return scheme->eval_h("(load-from-path \"" + query_filename + "\")");
}

void run_test(const std::string& testcase, int iterations_count)
{
    std::cout << "running testcase: " << testcase << ", number of iterations: " << iterations_count << std::endl;

    load_scheme_atomspace(testcase);
    Handle query = load_scheme_query(testcase);

    Handle result;
    for (int iteration = 0; iteration < iterations_count; iteration++)
    {
        result = bindlink(atomspace, query);
    }

    std::cout << "results are: " << result->to_string() << std::endl;
}

int main(int argc, char** argv)
{
    const char* description = "BindLink query benchmark tool\n"
        "Usage: bindlink_bm [-m <testcase>] [options]\n"
        "-m <testcase>\tTestcase to run: benchmark will use <testcase>.(atomspace|query).scm\n"
        "-r <int>  \tNumber of iterations; how many times a query is looped\n"
        "          \t(default: 1)\n";
    int c;

    if (argc == 1)
    {
        fprintf (stderr, "%s", description);
        return 0;
    }
    opterr = 0;
    while ((c = getopt (argc, argv, "t:r:")) != -1) {
       switch (c)
       {
           case 't':
             testcase = optarg;
             break;
           case 'r':
             iterations_count = (unsigned int) atoi(optarg);
             break;
           case '?':
             fprintf (stderr, "%s", description);
             return 0;
           default:
             fprintf (stderr, "Unknown option %c ", optopt);
             return 0;
       }
    }

    // Create the atomspace and scheme evaluator.
    atomspace = new AtomSpace();
    scheme = new SchemeEval(atomspace);

    // Load the scheme.
    load_scheme();

    // run the test
    run_test(testcase, iterations_count);

    return 0;
}
