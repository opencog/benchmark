/*
 * large_zipf_bm.cc
 * Copy of 
 * tests/persist/sql/multi-driver/LargeZipfUTest.cxxtest
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
#include <opencog/atoms/truthvalue/CountTruthValue.h>
#include <opencog/atoms/truthvalue/SimpleTruthValue.h>
#include <opencog/atomspace/AtomSpace.h>

#include "benchmark.h"

using namespace opencog;

// ============================================================

// This is a redesigned copy of the business end of
//   tests/persist/sql/multi-driver/LargeZipfUTest.cxxtest
// This creates an Atomspace with a Zipfian distribution of incoming
// sets.

// ============================================================

class Zipf
{
	// Emulate a Zipfian distribution.
	// Half of words linked once.  rpt = 1, wmax = NWORDS
	// quarter of words linked twice.  rpt = 2, wmax = NWORDS/2
	// eighth of words linked 4 times.  rpt = 4, wmax = NWORDS/4
	// 1/16 of words linked 8 times. rpt = 8, wmax = NWORDS/8
	//
	// I'm no longer sure the below works correctly, but whatever...
	// good enough, maybe?
	std::vector<Handle> hword;
	size_t nwords;
	size_t w1;
	size_t w2;
	size_t wmax;
	size_t npairs;

	// Emulate a word, with a spelling that is not long, not short ...
	std::string wrdbase;

	AtomSpace* _as;

public:
	Zipf(AtomSpace* as)
	{
		_as = as;

		w1 = 0;
		w2 = 0;
		npairs = 0;

		// Emulate a word, with a spelling that is not long, not short ...
		wrdbase = "Word-ishy ";

		Handle hw = as->add_node(CONCEPT_NODE, wrdbase + "foo");
		TruthValuePtr tv(CountTruthValue::createTV(1, 0, 0));
		hw->setTruthValue(tv);
		hword.emplace_back(hw);

		nwords = 1;
		wmax = 1;
	}
	size_t add_some();
};

// ============================================================

size_t Zipf::add_some()
{
	Handle hpair = _as->add_link(LIST_LINK, hword[w1], hword[w2]);
	TruthValuePtr tv = hpair->getTruthValue();
	size_t cnt = 0;
	if (COUNT_TRUTH_VALUE == tv->get_type())
		cnt = CountTruthValueCast(tv)->get_count();
	hpair->setTruthValue(CountTruthValue::createTV(1, 0, ++cnt));

	w2++;
	npairs++;
	if (wmax <= w2)
	{
		w2=0;
		w1++;
		if (nwords <= w1)
		{
			Handle hw = _as->add_node(CONCEPT_NODE,
				wrdbase + std::to_string(nwords));
			TruthValuePtr tv(CountTruthValue::createTV(1, 0, 0));
			hw->setTruthValue(tv);
			hword.emplace_back(hw);
			nwords++;
			w1 = 0;
		}

		wmax = nwords / (w1+1);
	}
	return nwords + npairs;
}

// ============================================================

static void BM_LargeZipf(benchmark::State& state)
{
	AtomSpace* as = new AtomSpace();
	Zipf* zp = new Zipf(as);

	size_t natoms = 1;
	size_t j=0;
	for (auto _ : state)
	{
		if (natoms < j)
		{
			natoms = zp->add_some();
		}
		j++;
	}
	delete as;
}
BENCHMARK(BM_LargeZipf)->Arg(2<<9)->Arg(2<<16)->Arg(2<<17)->Arg(2<<18)->Arg(2<<19);
