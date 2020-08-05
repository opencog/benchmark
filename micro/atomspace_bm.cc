/*
 * atomspace_bm.cc
 *
 * Copyright (C) 2020 OpenCog Foundation
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

#include <benchmark/benchmark.h>

#include <opencog/atoms/base/Node.h>
#include <opencog/atomspace/AtomSpace.h>

#include "benchmark.h"

using namespace opencog;

static void BM_AtomSpace_Basic(benchmark::State& state)
{
	AtomSpace* as;

	for (auto _ : state)
	{
		as = new AtomSpace();
		delete as;
	}
}
BENCHMARK(BM_AtomSpace_Basic);

static void BM_AtomSpace_OneNode(benchmark::State& state)
{
	AtomSpace* as;

	for (auto _ : state)
	{
		as = new AtomSpace();
		as->add_node(CONCEPT_NODE, "foobariffic");
		delete as;
	}
}
BENCHMARK(BM_AtomSpace_OneNode);

static void BM_AtomSpace_Nodes(benchmark::State& state)
{
	AtomSpace* as;

	size_t seed = 0;
	const size_t number_of_nodes = state.range(0);
	std::vector<Handle> nodes(number_of_nodes);
	for (size_t i = 0; i < number_of_nodes; ++i)
		nodes[i] = createNode(CONCEPT_NODE, get_unique_name("barfology", seed));

	for (auto _ : state)
	{
		as = new AtomSpace();
		for (size_t i=0; i< number_of_nodes; i++)
			as->add_atom(nodes[i]);
		delete as;
	}
}
BENCHMARK(BM_AtomSpace_Nodes)->Arg(2<<11)->Arg(2<<13)->Arg(2<<15);

static void BM_AtomSpace_CreateNodes(benchmark::State& state)
{
	AtomSpace* as;

	size_t seed = 0;
	const size_t number_of_names = state.range(0);
	std::vector<std::string> names(number_of_names);
	for (size_t i = 0; i < number_of_names; ++i)
		names[i] = get_unique_name("barfology", seed);

	for (auto _ : state)
	{
		as = new AtomSpace();
		for (size_t i=0; i< number_of_names; i++)
			as->add_node(CONCEPT_NODE, std::string({names[i]}));
		delete as;
	}
}
BENCHMARK(BM_AtomSpace_CreateNodes)->Arg(2<<11)->Arg(2<<13)->Arg(2<<15);
