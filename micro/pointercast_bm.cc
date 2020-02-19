/*
 * pointercast_bm.cc
 *
 * Copyright (C) 2020 Linas Vepstas
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

#include <opencog/util/Logger.h>
#include <opencog/atoms/base/Atom.h>
#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atomspace/AtomSpace.h>

#include "benchmark.h"

using namespace opencog;

static Handle create_node(void)
{
	static int idx = 0;
	return createNode(CONCEPT_NODE, std::to_string(idx++));
}

static Handle create_link(void)
{
	return createLink(LIST_LINK, create_node(), create_node());
}

static void BM_NodeCast(benchmark::State& state)
{
	const size_t num_to_make = 5000;
	std::vector<Handle> atoms(num_to_make);
	for (size_t i = 0; i < num_to_make; ++i)
		atoms[i] = create_node();

	size_t i = 0;
	for (auto _ : state)
	{
		NodeCast(atoms[i++ % num_to_make]);
	}
}

BENCHMARK(BM_NodeCast);

static void BM_LinkCast(benchmark::State& state)
{
	const size_t num_to_make = 5000;
	std::vector<Handle> atoms(num_to_make);
	for (size_t i = 0; i < num_to_make; ++i)
		atoms[i] = create_link();

	size_t i = 0;
	for (auto _ : state)
	{
		LinkCast(atoms[i++ % num_to_make]);
	}
}

BENCHMARK(BM_LinkCast);
