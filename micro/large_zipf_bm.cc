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

// This is a verbatim copy of the business end of
//   tests/persist/sql/multi-driver/LargeZipfUTest.cxxtest
// This creates a "flat" Atomspace, where all links have a small
// incoming set.  Compare to Zipf, which distributes in a Zipfian
// way.

// ============================================================

size_t count(size_t nwords)
{
	// Emulate a zipfian distribution.
	// Half of words linked once.  rpt = 1, wmax = NWORDS
	// quarter of words linked twice.  rpt = 2, wmax = NWORDS/2
	// eighth of words linked 4 times.  rpt = 4, wmax = NWORDS/4
	// 1/16 of words linked 8 times. rpt = 8, wmax = NWORDS/8
	size_t w1 = 0;
	size_t w2 = 0;
	size_t wmax = nwords;
	size_t rpt = 1;
	size_t again = 0;
	size_t p = 0;
	while (true)
	{
		w2++;
		p++;
		if (wmax <= w2)
		{
			w2=0;
			w1++;
			again ++;
			if (rpt <= again)
			{
				again = 0;
				rpt *= 2;
			}
			wmax = nwords / (w1 + 1);
			if (nwords <= w1) break;
		}
	}
	return p; // total number of pairs.
}


// ============================================================

size_t filler_up(AtomSpace *space, size_t nwords)
{
	std::vector<Handle> hword;

	// Emulate a word, with a spelling that is not long, not short ...
	std::string wrd = "Word-ishy ";
	for (size_t w=0; w<nwords; w++)
	{
		Handle hw = space->add_node(CONCEPT_NODE, wrd + std::to_string(w));
		TruthValuePtr tv(CountTruthValue::createTV(1, 0, nwords/(w+1)));
		hw->setTruthValue(tv);
		hword.emplace_back(hw);
	}

	// Emulate a zipfian distribution.
	// Half of words linked once.  rpt = 1, wmax = nwords
	// quarter of words linked twice.  rpt = 2, wmax = nwords/2
	// eighth of words linked 4 times.  rpt = 4, wmax = nwords/4
	// 1/16 of words linked 8 times. rpt = 8, wmax = nwords/8
	size_t w1 = 0;
	size_t w2 = 0;
	size_t wmax = nwords;
	size_t rpt = 1;
	size_t again = 0;
	size_t p = 0;
	while (true)
	{
		Handle hpair = space->add_link(LIST_LINK, hword[w1], hword[w2]);
		TruthValuePtr tv(CountTruthValue::createTV(1, 0, wmax));
		hpair->setTruthValue(tv);

		w2++;
		p++;
		if (wmax <= w2)
		{
			w2=0;
			w1++;
			again ++;
			if (rpt <= again)
			{
				again = 0;
				rpt *= 2;
			}
			wmax = nwords / (w1 + 1);
			if (nwords <= w1) break;
		}
	}
	return p;
}

// ============================================================

static void BM_LargeZipf(benchmark::State& state)
{
	size_t natoms = state.range(0);

	size_t nwords
	size_t npairs = count(nwords);
	// printf("nwords=%lu npairs=%lu\n", nwords, npairs);
	size_t loop = nwords + npairs;

	size_t j=0;
	for (auto _ : state)
	{
		if (j % loop == 0)
		{
			AtomSpace* as = new AtomSpace();
			filler_up(as, nwords);
			delete as;
		}
		j++;
	}
}
BENCHMARK(BM_LargeZipf)->Arg(2<<6)->Arg(2<<13)->Arg(2<<14)->Arg(2<<14)->Arg(2<<16);
