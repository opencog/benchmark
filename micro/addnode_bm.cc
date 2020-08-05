/*
 * addnode_bm.cc
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

static void BM_AddNode(benchmark::State& state)
{
	AtomSpace* as = new AtomSpace();

	size_t seed = 0;
	const size_t number_of_nodes = state.range(0);
	std::vector<Handle> nodes(number_of_nodes);
	for (size_t i = 0; i < number_of_nodes; ++i)
		nodes[i] = createNode(CONCEPT_NODE, get_unique_name("barfology", seed));

	size_t i = 0;
	for (auto _ : state)
	{
		as->add_atom(nodes[i++ % number_of_nodes]);
	}
	delete as;
}
BENCHMARK(BM_AddNode)->Arg(2<<9)->Arg(2<<16)->Arg(2<<17)->Arg(2<<18)->Arg(2<<19);

static void BM_CreateAddNode(benchmark::State& state)
{
	AtomSpace* as = new AtomSpace();

	size_t seed = 0;
	const size_t number_of_names = state.range(0);
	std::vector<std::string> names(number_of_names);
	for (size_t i = 0; i < number_of_names; ++i)
		names[i] = get_unique_name("barfology", seed);

	size_t i = 0;
	for (auto _ : state)
	{
		// Make a copy so that move constructore works right.
		as->add_node(CONCEPT_NODE, std::string({names[i++ % number_of_names]}));
	}
	delete as;
}
BENCHMARK(BM_CreateAddNode)->Arg(2<<9)->Arg(2<<16)->Arg(2<<17)->Arg(2<<18)->Arg(2<<19);
