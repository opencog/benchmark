/*
 * values_bm.cc
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

#include <opencog/atoms/base/Node.h>
#include <opencog/atoms/base/Link.h>
#include <opencog/atoms/truthvalue/SimpleTruthValue.h>
#include <opencog/atoms/value/FloatValue.h>

#include "benchmark.h"

using namespace opencog;

static ValuePtr create_three_dee(void)
{
	const double rho=28.0, sigma=10.0, beta=8.0/3.0, delta=0.01;
	static double x = 0.0, y=0.0, z=0.0;
	x += delta * sigma * (y - x);
	y += delta * (x * (rho - z) - y);
	z += delta * (x * y - beta * z);
	return createFloatValue(std::vector<double>({x,y,z}));
}

static TruthValuePtr create_stv(void)
{
	static double strength = 0.0, confidence=0.0;
	strength += 0.001; confidence += 1.0e-4;
	return createSimpleTruthValue(strength, confidence);
}

static Handle create_node(void)
{
	static int idx = 0;
	return createNode(CONCEPT_NODE, std::to_string(idx++));
}

static void BM_CreateFloatValue(benchmark::State& state)
{
	for (auto _ : state)
	{
		create_three_dee();
	}
}

BENCHMARK(BM_CreateFloatValue);

static void BM_CreateTruthValue(benchmark::State& state)
{
	for (auto _ : state)
	{
		create_stv();
	}
}

BENCHMARK(BM_CreateTruthValue);

static void BM_SetValue(benchmark::State& state)
{
	Handle P = createNode(PREDICATE_NODE, "*-some key-*");
	const size_t num_to_make = 5000;
	std::vector<Handle> atoms(num_to_make);
	for (size_t i = 0; i < num_to_make; ++i)
		atoms[i] = create_node();

	size_t i = 0;
	for (auto _ : state)
	{
		atoms[i++ % num_to_make]->setValue(P, create_three_dee());
	}
}

BENCHMARK(BM_SetValue);

static void BM_GetValue(benchmark::State& state)
{
	Handle P = createNode(PREDICATE_NODE, "*-some key-*");
	const size_t num_to_make = 5000;
	std::vector<Handle> atoms(num_to_make);
	for (size_t i = 0; i < num_to_make; ++i)
	{
		atoms[i] = create_node();
		atoms[i]->setValue(P, create_three_dee());
	}

	size_t i = 0;
	for (auto _ : state)
	{
		atoms[i++ % num_to_make]->getValue(P);
	}
}

BENCHMARK(BM_GetValue);

static void BM_SetTruthValue(benchmark::State& state)
{
	const size_t num_to_make = 5000;
	std::vector<Handle> atoms(num_to_make);
	for (size_t i = 0; i < num_to_make; ++i)
		atoms[i] = create_node();

	size_t i = 0;
	for (auto _ : state)
	{
		atoms[i++ % num_to_make]->setTruthValue(create_stv());
	}
}

BENCHMARK(BM_SetTruthValue);

static void BM_GetTruthValue(benchmark::State& state)
{
	const size_t num_to_make = 5000;
	std::vector<Handle> atoms(num_to_make);
	for (size_t i = 0; i < num_to_make; ++i)
	{
		atoms[i] = create_node();
		atoms[i]->setTruthValue(create_stv());
	}

	size_t i = 0;
	for (auto _ : state)
	{
		atoms[i++ % num_to_make]->getTruthValue();
	}
}

BENCHMARK(BM_GetTruthValue);
