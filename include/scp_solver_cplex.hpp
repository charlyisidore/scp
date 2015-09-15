/* -*- c++ -*-
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SCP_SOLVER_CPLEX_HPP
#define SCP_SOLVER_CPLEX_HPP

#include <ilcplex/ilocplex.h>
#include <vector>
#include <set>
#include "scp_problem.hpp"

/*
	Class: scp_solver_cplex

	Set Cover Problem solver.
*/
struct scp_solver_cplex
{
	scp_solver_cplex();

	void read( const scp_problem & instance );
	bool run();

	double z() const;
	std::set<int> x() const;

	void cut( const std::set<int> & x );

	void verbose();
	void quiet();

	void write_lp( const char * filename ) const;

	const IloCplex * ptr() const;
	      IloCplex * ptr();

private:
	IloEnv _env;
	IloCplex _cpx;
	IloModel _model;
	IloObjective _obj;
	IloNumVarArray _vars;
	IloRangeArray _cons;
};

#endif
