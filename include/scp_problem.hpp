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

#ifndef SCP_PROBLEM_HPP
#define SCP_PROBLEM_HPP

#include <iostream>
#include <vector>
#include <set>

/*
	Class: scp_problem

	Set Cover Problem instance.
*/
struct scp_problem
{
	int num_elements() const;
	int num_sets() const;

	void clear();
	void reduce();

	double z( const std::set<int> & x ) const;

	bool check() const;
	bool check( const std::set<int> & x ) const;
	bool check( const std::set<int> & x, double obj, double epsilon = 1e-9 ) const;

	void read( std::istream & is );
	void read_rail( std::istream & is );
	void read_stn( std::istream & is );
	void write( std::ostream & os ) const;

	std::vector<double> c;
	std::vector< std::set<int> > S;
};

////////////////////////////////////////////////////////////////////////////////

inline int scp_problem::num_elements() const
{
	return c.size();
}

inline int scp_problem::num_sets() const
{
	return S.size();
}

inline void scp_problem::clear()
{
	c.clear();
	S.clear();
}

inline void scp_problem::reduce()
{
	int m = num_elements(), n = num_sets();
	std::set<int>::iterator it;

	for ( int i = 0; i < m; ++i )
	{
		bool found = false;

		for ( int j = 0; !found && j < n; ++j )
		{
			if ( S[j].find( i ) != S[j].end() )
			{
				found = true;
			}
		}

		if ( !found )
		{
			for ( int j = 0; !found && j < n; ++j )
			{
				it = S[j].lower_bound( i );

				if ( it != S[j].end() )
				{
					if ( *it == i )
						S[j].erase( it++ );
					else
						++it;

					while ( it != S[j].end() )
					{
						int k = *it;
						S[j].erase( it++ );
						S[j].insert( k-1 );
					}
				}
			}
		}
	}
}

inline double scp_problem::z( const std::set<int> & x ) const
{
	std::set<int>::const_iterator it;
	double obj = 0;

	for ( it = x.begin(); it != x.end(); ++it )
	{
		obj += c[*it];
	}
	return obj;
}

inline bool scp_problem::check() const
{
	int n = num_sets();

	for ( int j = 0; j < n; ++j )
	{
		if ( S[j].empty() )
		{
			return false;
		}
	}
	return true;
}

inline bool scp_problem::check( const std::set<int> & x ) const
{
	int n = num_sets();
	std::set<int>::const_iterator it;

	for ( int j = 0; j < n; ++j )
	{
		bool covered = false;

		for ( it = S[j].begin(); !covered && it != S[j].end(); ++it )
		{
			if ( x.find( *it ) != x.end() )
			{
				covered = true;
			}
		}

		if ( !covered )
		{
			return false;
		}
	}
	return true;
}

inline bool scp_problem::check( const std::set<int> & x, double obj, double epsilon ) const
{
	double diff = z( x ) - obj;
	return -epsilon <= diff && diff <= epsilon && check( x );
}

inline void scp_problem::read( std::istream & is )
{
	int m, n, p, l;

	is >> n >> m;

	clear();
	c.resize( m );
	S.resize( n );

	for ( int i = 0; i < m; ++i )
	{
		is >> c[i];
	}

	for ( int j = 0; j < n; ++j )
	{
		is >> p;

		for ( int k = 0; k < p; ++k )
		{
			is >> l;
			S[j].insert( l-1 );
		}
	}
}

inline void scp_problem::read_rail( std::istream & is )
{
	int m, n, p, l;

	is >> n >> m;

	clear();
	c.resize( m );
	S.resize( n );

	for ( int i = 0; i < m; ++i )
	{
		is >> c[i] >> p;

		for ( int k = 0; k < p; ++k )
		{
			is >> l;
			S[l-1].insert( i );
		}
	}
}

inline void scp_problem::read_stn( std::istream & is )
{
	int m, n, l;

	is >> m >> n;

	clear();
	c.resize( m, 1 );
	S.resize( n );

	for ( int j = 0; j < n; ++j )
	{
		for ( int k = 0; k < 3; ++k )
		{
			is >> l;
			S[j].insert( l-1 );
		}
	}
}

inline void scp_problem::write( std::ostream & os ) const
{
	int m = num_elements(),
	    n = num_sets();

	os << n << ' ' << m << std::endl;

	for ( int i = 0; i < m; ++i )
	{
		if ( i > 0 ) os << ' ';
		os << c[i];
	}
	os << std::endl;

	for ( int j = 0; j < n; ++j )
	{
		std::set<int>::const_iterator it;

		os << S[j].size() << std::endl;
		for ( it = S[j].begin(); it != S[j].end(); ++it )
		{
			os << ' ' << *it+1;
		}
		os << std::endl;
	}
}

inline std::istream & operator >> ( std::istream & is, scp_problem & p )
{
	p.read( is );
	return is;
}

inline std::ostream & operator << ( std::ostream & os, const scp_problem & p )
{
	p.write( os );
	return os;
}

#endif
