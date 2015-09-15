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

#ifndef SCP_REPAIR_HPP
#define SCP_REPAIR_HPP

#include <vector>
#include <set>
#include <algorithm>
#include "scp_problem.hpp"

/*
	Class: scp_repair

	Set Cover Problem solution repair using heuristics.
*/
struct scp_repair
{
	scp_repair( double eps = 1e-9 );

	void read( const scp_problem & instance );
	void read( const std::vector<double> & c );
	void read( const std::set<int> & x );
	bool run();

	double z() const;
	std::set<int> x() const;

	double alpha,
	       epsilon;

private:
	std::vector<double> _c;
	std::vector< std::set<int> > _S, _T;
	std::set<int> _x;
	double _z;

	int _add( int i, std::vector<int> & u, std::vector<int> & v );
};

////////////////////////////////////////////////////////////////////////////////

inline scp_repair::scp_repair( double eps ) :
	alpha( 1 ), epsilon( eps ), _z( 0 )
{
}

inline void scp_repair::read( const scp_problem & instance )
{
	int m = instance.num_elements(),
	    n = instance.num_sets();

	std::set<int>::const_iterator it;

	_c.clear();
	_S.clear();
	_T.clear();

	_c = instance.c;
	_S = instance.S;
	_T.resize( m, std::set<int>() );

	for ( int j = 0; j < n; ++j )
	{
		for ( it = instance.S[j].begin(); it != instance.S[j].end(); ++it )
		{
			_T[*it].insert( j );
		}
	}
}

inline void scp_repair::read( const std::vector<double> & c )
{
	_c = c;
}

inline void scp_repair::read( const std::set<int> & x )
{
	_x = x;
}

inline bool scp_repair::run()
{
	int m = _T.size(), n = _S.size();

	std::vector<int> u( m, 0 ), v( n, 0 );
	std::set<int>::const_iterator it, jt, kt;
	int num_covered = 0;
	double c_min = std::numeric_limits<double>::infinity();

	_z = 0;

	for ( int i = 0; i < m; ++i )
	{
		u[i] = _T[i].size();

		// Cope with negative of null costs
		if ( _c[i] < c_min )
		{
			c_min = _c[i];
		}
	}

	for ( it = _x.begin(); it != _x.end(); ++it )
	{
		num_covered += _add( *it, u, v );
	}

	while ( num_covered < n )
	{
		std::vector<double> e( m, 0 );
		std::vector<int> rcl;
		double e_min = 0, e_max = 0, e_limit = 0;
		int k;

		for ( int i = 0; i < m; ++i )
		{
			if ( u[i] > 0 )
			{
				e[i] = double( u[i] ) / ( 1 + _c[i] - c_min );

				if ( i > 0 )
				{
					if ( e[i] < e_min ) e_min = e[i];
					if ( e[i] > e_max ) e_max = e[i];
				}
				else
				{
					e_min = e_max = e[i];
				}
			}
		}

		e_limit = e_min + alpha * ( e_max - e_min );

		for ( int i = 0; i < m; ++i )
		{
			if ( u[i] > 0 && e[i] + epsilon >= e_limit )
			{
				rcl.push_back( i );
			}
		}

		// Infeasible problem?
		if ( rcl.empty() ) return false;

		k = rcl[std::rand() % rcl.size()];
		_x.insert( k );
		num_covered += _add( k, u, v );
	}
	return true;
}

inline double scp_repair::z() const
{
	return _z;
}

inline std::set<int> scp_repair::x() const
{
	return _x;
}

inline int scp_repair::_add( int i, std::vector<int> & u, std::vector<int> & v )
{
	std::set<int>::const_iterator it, jt;
	int n = 0;

	_z += _c[i];

	for ( jt = _T[i].begin(); jt != _T[i].end(); ++jt )
	{
		if ( v[*jt] == 0 )
		{
			++n;

			for ( it = _S[*jt].begin(); it != _S[*jt].end(); ++it )
			{
				--u[*it];
			}
		}
		++v[*jt];
	}
	return n;
}

#endif
