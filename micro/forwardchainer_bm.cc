/*
 * forwardchainer_bm.cc
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

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/ure/forwardchainer/ForwardChainer.h>
#include <opencog/atoms/atom_types/atom_names.h>

using namespace opencog;

static void BM_ForwardChainer_Basic(benchmark::State& state)
{
	AtomSpace as;

	HandleSeq atoms;
	for (int i = 0; i < 1024; i++) {
		atoms.push_back(Evaluation(Predicate("croaks"), Concept("Fritz" + std::to_string(i))));
	}
	Handle source = as.add_link(SET_LINK, std::move(atoms));

	as.add_atom(
			Define(
				DefinedSchema("if-croaks"),
				Bind(
					Variable("$X"),
					Present(Evaluation(Predicate("croaks"), Variable("$X"))),
					Evaluation(Predicate("eats_flies"), Variable("$X")))));
	as.add_atom(Member(DefinedSchema("if-croaks"), Concept("rules")));

	as.add_atom(
			Define(
				DefinedSchema("if-eats-flies"),
				Bind(
					Variable("$X"),
					Present(Evaluation(Predicate("eats_flies"), Variable("$X"))),
					Inheritance(Variable("$X"), Concept("frog")))));
	as.add_atom(Member(DefinedSchema("if-eats-flies"), Concept("rules")));

    as.add_atom(Execution(Schema("URE:jobs"),
                Concept("rules"), Number("1")));
    as.add_atom(Execution(Schema("URE:maximum-iterations"),
                Concept("rules"), Number("2")));

	Handle rules = Concept("rules");

	opencog::logger().set_level(opencog::Logger::NONE);

	for (auto _ : state)
	{
		ForwardChainer fc(as, rules, source);
		fc.do_chain();
	}
}
BENCHMARK(BM_ForwardChainer_Basic);
