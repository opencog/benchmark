/*
 * large_sflat_bm.cc
 * Copy of 
 * tests/persist/sql/multi-driver/LargeSimpleFlatUTest.cxxtest
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
#include <opencog/atoms/truthvalue/SimpleTruthValue.h>
#include <opencog/atomspace/AtomSpace.h>

#include "benchmark.h"

using namespace opencog;

// ============================================================

// This was a verbatim copy of the business end of
//   tests/persist/sql/multi-driver/LargeSimpleFlatUTest.cxxtest
// except it's been simplfied to remove the cruft.
// This creates a "flat" Atomspace, where all links have a small
// incoming set.  Compare to Zipf, which distributes in a Zipfian
// way.
class LargeSimpleFlatUTest
{
	AtomSpace* _as;
public:
	LargeSimpleFlatUTest(AtomSpace*);
	void add_to_space(size_t idx, const std::string& id);
	void filler_up(size_t idx);
};

LargeSimpleFlatUTest::LargeSimpleFlatUTest(AtomSpace* as)
{
	_as = as;
}

void LargeSimpleFlatUTest::add_to_space(size_t idx, const std::string& id)
{
	// Create an atom ...
	TruthValuePtr stv(SimpleTruthValue::createTV(0.11, 100+idx));
	Handle h1 = _as->add_node(SCHEMA_NODE, id + "fromNode");
	h1->setTruthValue(stv);

	TruthValuePtr stv2(SimpleTruthValue::createTV(0.22, 200+idx));
	Handle h2 = _as->add_node(SCHEMA_NODE, id + "toNode");
	h2->setTruthValue(stv2);

	TruthValuePtr stv3(SimpleTruthValue::createTV(0.33, 300+idx));
	Handle h3 = _as->add_node(SCHEMA_NODE, id + "third wheel");
	h3->setTruthValue(stv3);

	// The NumberNode will go through the AtomTable clone factory
	// and should thus elicit any errors in clone uuid handling.
	char buf[40]; sprintf(buf, "%f", idx+0.14159265358979);
	Handle h4 = _as->add_node(NUMBER_NODE, buf);
	TruthValuePtr stv4(SimpleTruthValue::createTV(0.44, 400+idx));
	h4->setTruthValue(stv4);

	HandleSeq hvec;
	hvec.push_back(h1);
	hvec.push_back(h2);
	hvec.push_back(h3);
	hvec.push_back(h4);

	// Note that SetLink is an unordered link.
	Handle hl = _as->add_link(SET_LINK, std::move(hvec));

	Handle hl2 = _as->add_link(LIST_LINK, hl, h2);
	_as->add_link(EVALUATION_LINK, h1, hl2, h3);
}

// ============================================================

void LargeSimpleFlatUTest::filler_up(size_t idx)
{
	std::string lbl = std::to_string(idx);
	add_to_space(idx, "AA-aa-wow " + lbl);
	add_to_space(idx, "BB-bb-wow " + lbl);
	add_to_space(idx, "CC-cc-wow " + lbl);
	add_to_space(idx, "DD-dd-wow " + lbl);
	add_to_space(idx, "EE-ee-wow " + lbl);

	/* Make sure UTF-8 works fine. */
	add_to_space(idx, "Попытка выбраться вызвала слабый стон " + lbl);
	add_to_space(idx, "はにがうりだそうであってるのかはち " + lbl);
	add_to_space(idx, "係拉丁字母" + lbl);
}

// ============================================================

static void BM_LargeSimpleFlat(benchmark::State& state)
{
	AtomSpace* as = new AtomSpace();

	// The LargeSimpleFlatUTest create 56 atoms for each call
	// to filler_up()  -- That's (4 nodes + 3 links) x 8 for each call.
	LargeSimpleFlatUTest* lfut = new LargeSimpleFlatUTest(as);

	size_t i=0;
	size_t j=0;
	for (auto _ : state)
	{
		if (j < as->get_size())
		{
			lfut->filler_up(i);
			i++;
		}
		j++;
	}
	delete as;
}
BENCHMARK(BM_LargeSimpleFlat)->Arg(2<<9)->Arg(2<<16)->Arg(2<<17)->Arg(2<<18)->Arg(2<<19);
