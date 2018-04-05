/*
 * benchmark/profile_bindlink.cc
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

void load_scheme()
{
    // Load some scheme for the setup
    scheme->eval("(use-modules (opencog))");
    scheme->eval("(add-to-load-path \".\")");
}

void load_scheme_atomspace(const std::string& testcase) {
    std::string atomspace_filename = testcase + ".atomspace.scm";
    scheme->eval("(load-from-path \"" + atomspace_filename + "\")");
}

Handle load_scheme_query(const std::string& testcase) {
    std::string query_filename = testcase + ".query.scm";
    return scheme->eval_h("(load-from-path \"" + query_filename + "\")");
}

void run_test(const std::string& testcase, int iteration_count) {
    load_scheme_atomspace(testcase);
    Handle query = load_scheme_query(testcase);

    Handle result;
    for (int iteration = 0; iteration < iteration_count; iteration++ ) {
        result = bindlink(atomspace, query);
    }

    if (result->is_link())
    {
        size_t total_results = result->getOutgoingSet().size();
        std::cout << "total results = " << total_results << std::endl;
    }
    std::cout << "results are: " << result->to_string() << std::endl;
}

int main(void)
{
    // Create the atomspace and scheme evaluator.
    atomspace = new AtomSpace();
    scheme = new SchemeEval(atomspace);

    // Load the scheme.
    load_scheme();

    // run the test
    run_test("animals", 100000);

    return 0;
}
