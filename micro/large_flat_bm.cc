/*
 * large_flat_bm.cc
 * Copy of 
 * tests/persist/sql/multi-driver/LargeFlatUTest.cxxtest
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

// This is a verbatim copy of the business end of
//   tests/persist/sql/multi-driver/LargeFlatUTest.cxxtest
// This creates a "flat" Atomspace, where all links have a small
// incoming set.  Compare to Zipf, which distributes in a Zipfian
// way.
class LargeFlatUTest
{
	std::vector<NodePtr> n1;
	std::vector<NodePtr> n2;
	std::vector<NodePtr> n3;
	std::vector<NodePtr> n4;
	std::vector<LinkPtr> l;
	std::vector<LinkPtr> l2;
	std::vector<LinkPtr> l3;
	std::vector<Handle> h1;
	std::vector<Handle> h2;
	std::vector<Handle> h3;
	std::vector<Handle> h4;
	std::vector<Handle> hl;
	std::vector<Handle> hl2;
	std::vector<Handle> hl3;

public:
	LargeFlatUTest(size_t nchunks);
	void add_to_space(size_t idx, AtomSpace *as, std::string id);
	size_t filler_up(AtomSpace *space, size_t ichk);
};

LargeFlatUTest::LargeFlatUTest(size_t nchunks)
{
	for (size_t idx = 0; idx < nchunks; idx++)
	{
		n1.push_back(NodePtr());
		n2.push_back(NodePtr());
		n3.push_back(NodePtr());
		n4.push_back(NodePtr());
		l.push_back(LinkPtr());
		l2.push_back(LinkPtr());
		l3.push_back(LinkPtr());
		h1.push_back(Handle::UNDEFINED);
		h2.push_back(Handle::UNDEFINED);
		h3.push_back(Handle::UNDEFINED);
		h4.push_back(Handle::UNDEFINED);
		hl.push_back(Handle::UNDEFINED);
		hl2.push_back(Handle::UNDEFINED);
		hl3.push_back(Handle::UNDEFINED);
	}
}

void LargeFlatUTest::add_to_space(size_t idx, AtomSpace *as, std::string id)
{
	// Create an atom ...
	TruthValuePtr stv(SimpleTruthValue::createTV(0.11, 100+idx));
	h1[idx] = as->add_node(SCHEMA_NODE, id + "fromNode");
	h1[idx]->setTruthValue(stv);
	n1[idx] = NodeCast(h1[idx]);

	TruthValuePtr stv2(SimpleTruthValue::createTV(0.22, 200+idx));
	h2[idx] = as->add_node(SCHEMA_NODE, id + "toNode");
	h2[idx]->setTruthValue(stv2);
	n2[idx] = NodeCast(h2[idx]);

	TruthValuePtr stv3(SimpleTruthValue::createTV(0.33, 300+idx));
	h3[idx] = as->add_node(SCHEMA_NODE, id + "third wheel");
	h3[idx]->setTruthValue(stv3);
	n3[idx] = NodeCast(h3[idx]);

	// The NumberNode will go through the AtomTable clone factory
	// and should thus elicit any errors in clone uuid handling.
	char buf[40]; sprintf(buf, "%f", idx+0.14159265358979);
	h4[idx] = as->add_node(NUMBER_NODE, buf);
	TruthValuePtr stv4(SimpleTruthValue::createTV(0.44, 400+idx));
	h4[idx]->setTruthValue(stv4);
	n4[idx] = NodeCast(h4[idx]);

	HandleSeq hvec;
	hvec.push_back(h1[idx]);
	hvec.push_back(h2[idx]);
	hvec.push_back(h3[idx]);
	hvec.push_back(h4[idx]);

	// Note that SetLink is an unordered link.
	hl[idx] = as->add_link(SET_LINK, std::move(hvec));
	l[idx] = LinkCast(hl[idx]);

	hl2[idx] = as->add_link(LIST_LINK, hl[idx], h2[idx]);
	l2[idx] = LinkCast(hl2[idx]);

	hl3[idx] = as->add_link(EVALUATION_LINK, h1[idx], hl2[idx], h3[idx]);
	l3[idx] = LinkCast(hl3[idx]);
}

// ============================================================

size_t LargeFlatUTest::filler_up(AtomSpace* _as, size_t idx)
{
	std::string lbl = std::to_string(idx);
	add_to_space(idx++, _as, "AA-aa-wow " + lbl);
	add_to_space(idx++, _as, "BB-bb-wow " + lbl);
	add_to_space(idx++, _as, "CC-cc-wow " + lbl);
	add_to_space(idx++, _as, "DD-dd-wow " + lbl);
	add_to_space(idx++, _as, "EE-ee-wow " + lbl);

	/* Make sure UTF-8 works fine. */
	add_to_space(idx++, _as, "Попытка выбраться вызвала слабый стон " + lbl);
	add_to_space(idx++, _as, "はにがうりだそうであってるのかはち " + lbl);
	add_to_space(idx++, _as, "係拉丁字母" + lbl);

	return idx;
}

// ============================================================

static void BM_LargeFlat(benchmark::State& state)
{
	size_t num_adds = state.range(0);

	// The LargeFlatUTest create 56 atoms for each call to filler_up()
	// That's (4 nodes + 3 links) x 8 for each call.
	LargeFlatUTest* lfut = new LargeFlatUTest(num_adds/7+10);

	AtomSpace* as = new AtomSpace();
	size_t i=0;
	size_t j=0;
	for (auto _ : state)
	{
		if (j < num_adds)
		{
			if (j < as->get_size())
			{
				i = lfut->filler_up(as, i);
				i %= nchks/7;
			}
			j++;
		}
		else
		{
			delete lfut;
			delete as;
			as = new AtomSpace();
			lfut = new LargeFlatUTest(num_adds/7+10);
			i = 0;
			j = 0;
		}
	}
	delete as;
}

// Cannot go higher than 17 because the benchmark doesn't
// iterate enough times.
BENCHMARK(BM_LargeFlat)->Arg(2<<9)->Arg(2<<16)->Arg(2<<17);
