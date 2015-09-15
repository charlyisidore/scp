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

#ifndef CHRONO_HPP
#define CHRONO_HPP

#include <sys/time.h>
#include <sys/resource.h>
#include <ctime>
#include <iostream>

#define _CHRONO_GETRUSAGE 0

struct chrono_time_point_clock;
struct chrono_time_point_getrusage;

/*
	Class: chrono

	A clock to measure execution time.
*/
struct chrono
{
#if _CHRONO_GETRUSAGE
	typedef chrono_time_point_getrusage time_point;
#else
	typedef chrono_time_point_clock time_point;
#endif

	static time_point now();
	static double diff( const time_point & end, const time_point & beg, double r = 1 );
};

// Using std::clock()
struct chrono_time_point_clock
{
	chrono_time_point_clock() {}

private:
	chrono_time_point_clock( std::clock_t t ) : _t( t ) {}
	std::clock_t _t;

	friend struct chrono;
};

// Using getrusage()
struct chrono_time_point_getrusage
{
	chrono_time_point_getrusage() {}

private:
	chrono_time_point_getrusage( const timeval & t ) : _t( t ) {}
	timeval _t;

	friend struct chrono;
};

double operator - ( const chrono::time_point & end, const chrono::time_point & beg );

////////////////////////////////////////////////////////////////////////////////

inline chrono::time_point chrono::now()
{
#if _CHRONO_GETRUSAGE
	rusage r;
	getrusage( RUSAGE_SELF, &r );
	return time_point( r.ru_utime );
#else
	return time_point( std::clock() );
#endif
}

inline double chrono::diff( const time_point & end, const time_point & beg, double r )
{
#if _CHRONO_GETRUSAGE
	return double( end._t.tv_sec - beg._t.tv_sec ) / r
	     + double( end._t.tv_usec - beg._t.tv_usec ) / r / 1e6;
#else
	return ( end._t - beg._t ) / r / CLOCKS_PER_SEC;
#endif
}

inline double operator - ( const chrono::time_point & end, const chrono::time_point & begin )
{
	return chrono::diff( end, begin );
}

#endif
