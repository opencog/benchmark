/*
 * variables_bm.cc
 *
 * Copyright (C) 2019 OpenCog Foundation
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
#include <opencog/atomspace/AtomSpace.h>

#include "benchmark.h"

using namespace opencog;

static void BM_ExtendVariablesNewVariable(benchmark::State& state)
{
	AtomSpace atomspace;
	Handle varX = atomspace.add_node(VARIABLE_NODE, "$X");
	Handle varY = atomspace.add_node(VARIABLE_NODE, "$Y");
	Handle varDeclA = atomspace.add_link(VARIABLE_LIST, VarX);
	Handle varDeclB = atomspace.add_link(VARIABLE_LIST, VarY);
	VariableList vl1(varDeclA), vl2(varDeclB);

	for (auto _ : state)
	{
		vl1.extend(vl2);
	}
}
BENCHMARK(BM_ExtendVariablesNewVariable);


