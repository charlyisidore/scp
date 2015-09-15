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

#ifndef SCP_LOCAL_SEARCH_HPP
#define SCP_LOCAL_SEARCH_HPP

#include <vector>
#include <set>
#include <algorithm>
#include "scp_problem.hpp"

/*
	Class: scp_local_search

	Set Cover Problem solver using GRASP metaheuristic.
*/
struct scp_local_search
{
	enum
	{
		_1_0 = (1 << 0),
		_1_1 = (1 << 1),
		_2_1 = (1 << 2)
	};

	scp_local_search();

	void read( const scp_problem & instance );
	void read( const std::vector<double> & c );
	void read( const std::set<int> & x );
	bool run( int exchanges = (_1_0 | _1_1 | _2_1) );

	double z() const;
	std::set<int> x() const;

private:
	std::vector<double> _c;
	std::vector< std::set<int> > _S, _T;
	std::set<int> _x;
	double _z;

	void run_1_0( std::vector<int> & v );
	void run_1_1( std::vector<int> & v );
	void run_2_1( std::vector<int> & v );
};

////////////////////////////////////////////////////////////////////////////////

inline scp_local_search::scp_local_search() :
	_z( 0 )
{
}

inline void scp_local_search::read( const scp_problem & instance )
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

inline void scp_local_search::read( const std::vector<double> & c )
{
	_c = c;
}

inline void scp_local_search::read( const std::set<int> & x )
{
	_x = x;
}

inline bool scp_local_search::run( int exchanges )
{
	int n = _S.size();

	std::vector<int> v( n, 0 );
	std::set<int>::const_iterator it, jt;

	_z = 0;

	for ( it = _x.begin(); it != _x.end(); ++it )
	{
		_z += _c[*it];

		for ( jt = _T[*it].begin(); jt != _T[*it].end(); ++jt )
		{
			++v[*jt];
		}
	}

	if ( exchanges & _1_0 ) run_1_0( v );
	if ( exchanges & _1_1 ) run_1_1( v );
	if ( exchanges & _2_1 ) run_2_1( v );
	return true;
}

inline void scp_local_search::run_1_0( std::vector<int> & v )
{
	std::set<int>::iterator it, jt;

	// 1-0 exchange
	for ( it = _x.begin(); it != _x.end(); )
	{
		bool is_useless = true;

		for ( jt = _T[*it].begin(); is_useless && jt != _T[*it].end(); ++jt )
		{
			if ( v[*jt] <= 1 )
			{
				is_useless = false;
			}
		}

		if ( is_useless )
		{
			for ( jt = _T[*it].begin(); is_useless && jt != _T[*it].end(); ++jt )
			{
				--v[*jt];
			}

			_z -= _c[*it];
			_x.erase( it++ );
		}
		else
		{
			++it;
		}
	}
}

inline void scp_local_search::run_1_1( std::vector<int> & v )
{
	int m = _T.size();

	std::set<int>::iterator it, jt;
	bool improved = true;

	// 2-1 exchange
	while ( improved )
	{
		improved = false;
		for ( it = _x.begin(); !improved && it != _x.end(); ++it )
		{
			std::set<int> to_cover;

			for ( jt = _T[*it].begin(); jt != _T[*it].end(); ++jt )
			{
				if ( v[*jt] <= 1 )
				{
					to_cover.insert( *jt );
				}
			}

			for ( int i = 0; !improved && i < m; ++i )
			{
				if ( _c[i] - _c[*it] < 0            // Profitable exchange?
				     && _x.find( i ) == _x.end()    // Is the item not already selected?
				     && std::includes( _T[i].begin(), _T[i].end(), to_cover.begin(), to_cover.end() ) ) // Covers the subsets that need coverage?
				{
					for ( jt = _T[*it].begin(); jt != _T[*it].end(); ++jt )
					{
						--v[*jt];
					}

					for ( jt = _T[i].begin(); jt != _T[i].end(); ++jt )
					{
						++v[*jt];
					}

					_z += _c[i] - _c[*it];
					_x.erase( it );
					_x.insert( i );

					improved = true;
				}
			}
		}
	}
}

inline void scp_local_search::run_2_1( std::vector<int> & v )
{
	int m = _T.size(),
	    n = _S.size();

	std::set<int>::iterator it, jt, kt;
	bool improved = true;

	// 2-1 exchange
	while ( improved )
	{
		improved = false;
		for ( it = _x.begin(); !improved && it != _x.end(); ++it )
		{
			jt = it;
			for ( ++jt; !improved && jt != _x.end(); ++jt )
			{
				std::vector<int> v_ij = v;
				std::set<int> to_cover;

				for ( kt = _T[*it].begin(); kt != _T[*it].end(); ++kt )
				{
					--v_ij[*kt];
				}

				for ( kt = _T[*jt].begin(); kt != _T[*jt].end(); ++kt )
				{
					--v_ij[*kt];
				}

				for ( int j = 0; j < n; ++j )
				{
					if ( v_ij[j] <= 0 )
					{
						to_cover.insert( j );
					}
				}

				for ( int i = 0; !improved && i < m; ++i )
				{
					if ( _c[i] - _c[*it] - _c[*jt] < 0    // Profitable exchange?
					     && _x.find( i ) == _x.end()      // Is the item not already selected?
					     && std::includes( _T[i].begin(), _T[i].end(), to_cover.begin(), to_cover.end() ) ) // Covers the subsets that need coverage?
					{
						// v_ij is the new v vector
						for ( kt = _T[i].begin(); kt != _T[i].end(); ++kt )
						{
							++v_ij[*kt];
						}

						_z += _c[i] - _c[*it] - _c[*jt];
						_x.erase( it );
						_x.erase( jt );
						_x.insert( i );

						v = v_ij;
						improved = true;
					}
				}
			}
		}
	}
}

inline double scp_local_search::z() const
{
	return _z;
}

inline std::set<int> scp_local_search::x() const
{
	return _x;
}

#endif
