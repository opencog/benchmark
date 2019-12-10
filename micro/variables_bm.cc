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
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/atoms/core/Variables.h>
#include <opencog/atoms/core/VariableList.h>

#include "benchmark.h"

using namespace opencog;

static void BM_VariablesExtend_NewVariable(benchmark::State& state)
{
	AtomSpace atomspace;
	Handle varX = atomspace.add_node(VARIABLE_NODE, "$X");
	Handle varY = atomspace.add_node(VARIABLE_NODE, "$Y");
	Handle varDeclA = atomspace.add_link(VARIABLE_LIST, varX);
	Handle varDeclB = atomspace.add_link(VARIABLE_LIST, varY);
	Variables varsA(VariableList(varDeclA).get_variables());
	Variables varsB(VariableList(varDeclB).get_variables());

	for (auto _ : state)
	{
		varsA.extend(varsB);
	}
}
BENCHMARK(BM_VariablesExtend_NewVariable);

static void BM_VariablesExtend_SameVariableNoTypeRestrictions(benchmark::State& state)
{
	AtomSpace atomspace;
	Handle varX = atomspace.add_node(VARIABLE_NODE, "$X");
	Handle conceptNodeType = atomspace.add_node(TYPE_NODE, "ConceptNode");
	Handle varXTyped = atomspace.add_link(TYPED_VARIABLE_LINK, varX, conceptNodeType);
	Handle varDeclA = atomspace.add_link(VARIABLE_LIST, varXTyped);
	Handle varDeclB = atomspace.add_link(VARIABLE_LIST, varX);
	Variables varsA(VariableList(varDeclA).get_variables());
	Variables varsB(VariableList(varDeclB).get_variables());

	for (auto _ : state)
	{
		varsA.extend(varsB);
	}
}
BENCHMARK(BM_VariablesExtend_SameVariableNoTypeRestrictions);

static void BM_VariablesExtend_NewVariableNoTypeRestrictions(benchmark::State& state)
{
	AtomSpace atomspace;
	Handle varX = atomspace.add_node(VARIABLE_NODE, "$X");
	Handle varDeclA = atomspace.add_link(VARIABLE_LIST);
	Handle varDeclB = atomspace.add_link(VARIABLE_LIST, varX);
	Variables varsA(VariableList(varDeclA).get_variables());
	Variables varsB(VariableList(varDeclB).get_variables());

	for (auto _ : state)
	{
		varsA.extend(varsB);
	}
}
BENCHMARK(BM_VariablesExtend_NewVariableNoTypeRestrictions);

static void BM_VariablesExtend_SameVariableWithTypeRestrictions(benchmark::State& state)
{
	AtomSpace atomspace;
	Handle varX = atomspace.add_node(VARIABLE_NODE, "$X");
	Handle conceptNodeType = atomspace.add_node(TYPE_NODE, "ConceptNode");
	Handle varXTyped = atomspace.add_link(TYPED_VARIABLE_LINK, varX, conceptNodeType);
	Handle varDeclA = atomspace.add_link(VARIABLE_LIST, varXTyped);
	Handle varDeclB = atomspace.add_link(VARIABLE_LIST, varXTyped);
	Variables varsA(VariableList(varDeclA).get_variables());
	Variables varsB(VariableList(varDeclB).get_variables());

	for (auto _ : state)
	{
		varsA.extend(varsB);
	}
}
BENCHMARK(BM_VariablesExtend_SameVariableWithTypeRestrictions);


