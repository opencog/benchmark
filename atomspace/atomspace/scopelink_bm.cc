/*
 * scopelink_bm.cc
 *
 * Copyright (C) 2018 OpenCog Foundation
 * All Rights Reserved
 *
 * Created by Vitaly Bogdanov <vsbogd@gmail.com> May 28, 2018
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

#include <benchmark/benchmark.h>

#include <opencog/util/Logger.h>
#include <opencog/atoms/base/Atom.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/core/ScopeLink.h>

#include "benchmark.h"

using namespace opencog;

static Handle create_scope_link(AtomSpace& atomspace, size_t& seed)
{
	Handle X = atomspace.add_node(VARIABLE_NODE, get_unique_name("$X", seed));
	Handle Y = atomspace.add_node(VARIABLE_NODE, get_unique_name("$Y", seed));
	Handle P = atomspace.add_node(PREDICATE_NODE, get_unique_name("P", seed));
	Handle Q = atomspace.add_node(PREDICATE_NODE, get_unique_name("Q", seed));

	Handle variable_list = atomspace.add_link(VARIABLE_LIST, X, Y);
	Handle expression = atomspace.add_link(AND_LINK,
	        atomspace.add_link(EVALUATION_LINK, P, X),
	        atomspace.add_link(EVALUATION_LINK, Q, Y));
	return createLink(SCOPE_LINK, variable_list, expression);
}

static Handle create_scope_link(AtomSpace& atomspace)
{
	size_t seed = 0;
	return create_scope_link(atomspace, seed);
}

static void BM_CreateScopeLinkWithVariableList(benchmark::State& state)
{
	AtomSpace atomspace;

	Handle X = atomspace.add_node(VARIABLE_NODE, "$X");
	Handle Y = atomspace.add_node(VARIABLE_NODE, "$Y");
	Handle P = atomspace.add_node(PREDICATE_NODE, "P");
	Handle Q = atomspace.add_node(PREDICATE_NODE, "Q");

	Handle variable_list = atomspace.add_link(VARIABLE_LIST, X, Y);
	Handle expression = atomspace.add_link(AND_LINK,
	        atomspace.add_link(EVALUATION_LINK, P, X),
	        atomspace.add_link(EVALUATION_LINK, Q, Y));

	Handle h;
	for (auto _ : state)
	{
		h = createLink(SCOPE_LINK, variable_list, expression);
	}

	logger().fine(
	        "variables: " + ((ScopeLink&) (*h)).get_variables().to_string());
}
BENCHMARK(BM_CreateScopeLinkWithVariableList);

static void BM_CreateScopeLinkWithoutVariableList(benchmark::State& state)
{
	AtomSpace atomspace;

	Handle X = atomspace.add_node(VARIABLE_NODE, "$X");
	Handle Y = atomspace.add_node(VARIABLE_NODE, "$Y");
	Handle P = atomspace.add_node(PREDICATE_NODE, "P");
	Handle Q = atomspace.add_node(PREDICATE_NODE, "Q");

	Handle expression = atomspace.add_link(AND_LINK,
	        atomspace.add_link(EVALUATION_LINK, P, X),
	        atomspace.add_link(EVALUATION_LINK, Q, Y));

	Handle h;
	for (auto _ : state)
	{
		h = createLink(SCOPE_LINK, expression);
	}

	logger().fine(
	        "variables: " + ((ScopeLink&) (*h)).get_variables().to_string());
}
BENCHMARK(BM_CreateScopeLinkWithoutVariableList);

static void BM_CreateScopeLinkWithLambdaLink(benchmark::State& state)
{
	AtomSpace atomspace;

	Handle X = atomspace.add_node(VARIABLE_NODE, "$X");
	Handle Y = atomspace.add_node(VARIABLE_NODE, "$Y");
	Handle P = atomspace.add_node(PREDICATE_NODE, "P");
	Handle Q = atomspace.add_node(PREDICATE_NODE, "Q");

	Handle expression = atomspace.add_link(LAMBDA_LINK,
	        atomspace.add_link(AND_LINK,
	                atomspace.add_link(EVALUATION_LINK, P, X),
	                atomspace.add_link(EVALUATION_LINK, Q, Y)));

	Handle h;
	for (auto _ : state)
	{
		h = createLink(SCOPE_LINK, expression);
	}

	logger().fine(
	        "variables: " + ((ScopeLink&) (*h)).get_variables().to_string());
}
BENCHMARK(BM_CreateScopeLinkWithLambdaLink);

static void BM_AddSameScopeLink(benchmark::State& state)
{
	AtomSpace atomspace;
	Handle scopeLink = create_scope_link(atomspace);

	logger().fine("atomspace size before adding: %d", atomspace.get_size());

	for (auto _ : state)
	{
		atomspace.add_atom(scopeLink);
	}

	logger().fine("atomspace size after adding: %d", atomspace.get_size());
}
BENCHMARK(BM_AddSameScopeLink);

static void BM_AddScopeLink(benchmark::State& state)
{
	AtomSpace atomspace;

	const size_t number_of_links = state.range(0);
	std::vector<Handle> links(number_of_links);
	size_t seed = 0;
	for (size_t i = 0; i < number_of_links; ++i)
	{
		links[i] = create_scope_link(atomspace, seed);
	}

	logger().fine("atomspace size before adding: %d", atomspace.get_size());

	size_t i = 0;
	for (auto _ : state)
	{
		atomspace.add_atom(links[i++ % number_of_links]);
	}

	logger().fine("atomspace size after adding: %d", atomspace.get_size());
}
BENCHMARK(BM_AddScopeLink)->Range(2, 2<<13);

