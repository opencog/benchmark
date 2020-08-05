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
