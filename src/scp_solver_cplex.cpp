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

#include "scp_solver_cplex.hpp"
#include <sstream>

scp_solver_cplex::scp_solver_cplex() :
	_env(),
	_cpx( _env ),
	_model( _env ),
	_obj( _env ),
	_vars( _env ),
	_cons( _env )
{
	quiet();
}

void scp_solver_cplex::read( const scp_problem & instance )
{
	int m = instance.num_elements(),
	    n = instance.num_sets();

	_model = IloModel( _env );
	_vars = IloNumVarArray( _env, m );
	_cons = IloRangeArray( _env, n );
	_obj  = IloObjective( _env, 0, IloObjective::Minimize );

	for ( int i = 0; i < m; ++i )
	{
		std::ostringstream oss;

		oss << "x(" << i+1 << ")";

		_vars[i] = IloNumVar( _env, 0, 1, IloNumVar::Bool, oss.str().c_str() );
		_obj.setLinearCoef( _vars[i], instance.c[i] );
	}

	for ( int j = 0; j < n; ++j )
	{
		std::set<int>::const_iterator it;
		std::ostringstream oss;

		oss << "S(" << j+1 << ")";

		_cons[j] = IloRange( _env, 1, IloInfinity, oss.str().c_str() );

		for ( it = instance.S[j].begin(); it != instance.S[j].end(); ++it )
		{
			_cons[j].setLinearCoef( _vars[*it], 1 );
		}
	}

	_model.add( _vars );
	_model.add( _obj );
	_model.add( _cons );

	_cpx.extract( _model );
}

bool scp_solver_cplex::run()
{
	//_cpx.setParam( IloCplex::PopulateLim, 500 );
	//_cpx.setParam( IloCplex::SolnPoolGap, 1e-9 );
	//_cpx.setParam( IloCplex::SolnPoolAGap, 1e-9 );

	_cpx.solve();
	return _cpx.getStatus() == IloAlgorithm::Optimal;
}

double scp_solver_cplex::z() const
{
	return _cpx.getObjValue();
}

std::set<int> scp_solver_cplex::x() const
{
	int m = _vars.getSize();
	std::set<int> s;

	for ( int i = 0; i < m; ++i )
	{
		if ( _cpx.getValue( _vars[i] ) > 0.5 )
		{
			s.insert( i );
		}
	}
	return s;
}

void scp_solver_cplex::cut( const std::set<int> & x )
{
	std::set<int>::const_iterator it;

	IloRange cons( _env, -IloInfinity, x.size() - 1 );

	for ( it = x.begin(); it != x.end(); ++it )
	{
		cons.setLinearCoef( _vars[*it], 1 );
	}
	_model.add( cons );
}

void scp_solver_cplex::verbose()
{
	_cpx.setOut( std::clog );
}

void scp_solver_cplex::quiet()
{
	_cpx.setOut( _env.getNullStream() );
}

void scp_solver_cplex::write_lp( const char * filename ) const
{
	_cpx.exportModel( filename );
}

const IloCplex * scp_solver_cplex::ptr() const
{
	return &_cpx;
}

IloCplex * scp_solver_cplex::ptr()
{
	return &_cpx;
}
