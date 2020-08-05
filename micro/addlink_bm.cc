/*
 * addlink_bm.cc
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
#include <opencog/atoms/base/Link.h>
#include <opencog/atomspace/AtomSpace.h>

#include "benchmark.h"

using namespace opencog;

static void BM_AddLink(benchmark::State& state)
{
	AtomSpace* as = new AtomSpace();

	// 101 and 233 are prime numbers. Thus, the links will interconnect
	// to form some kind of nice polytope.
	const size_t number_of_atoms = state.range(0);
	std::vector<Handle> atoms(number_of_atoms);
	for (size_t i = 0; i < number_of_atoms; ++i)
		atoms[i] = createLink(LIST_LINK,
			createNode(CONCEPT_NODE, "barfology" + std::to_string(i%101)),
			createNode(CONCEPT_NODE, "blingometry" + std::to_string(i%233)));

	size_t i = 0;
	for (auto _ : state)
	{
		as->add_atom(atoms[i++ % number_of_atoms]);
	}
	delete as;
}
BENCHMARK(BM_AddLink)->Arg(2<<9)->Arg(2<<16)->Arg(2<<17)->Arg(2<<18)->Arg(2<<19);

static void BM_CreateAddLink(benchmark::State& state)
{
	AtomSpace* as = new AtomSpace();

	std::vector<std::string> aname(101);
	std::vector<std::string> bname(233);
	for (size_t i = 0; i < 101; ++i)
		aname[i] = "barfology" + std::to_string(i);
	for (size_t i = 0; i < 233; ++i)
		bname[i] = "blingometry" + std::to_string(i);

	size_t i = 0;
	for (auto _ : state)
	{
		// Make a copy so that move constructore works right.
		as->add_link(LIST_LINK,
			as->add_node(CONCEPT_NODE, std::string({aname[i % 101]})),
			as->add_node(CONCEPT_NODE, std::string({bname[i % 233]})));
		i++;
	}
	delete as;
}
BENCHMARK(BM_CreateAddLink)->Arg(2<<9)->Arg(2<<16)->Arg(2<<17)->Arg(2<<18)->Arg(2<<19);
