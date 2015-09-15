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

#include "scp_problem.hpp"
#include "scp_solver_cplex.hpp"
#include "scp_grasp.hpp"
#include "scp_local_search.hpp"
#include "chrono.hpp"
#include "gzfstream.hpp"
#include "docopt.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <ctime>

enum
{
	format_scp,
	format_rail,
	format_stn
};

void read_instance( int format, std::istream & is, scp_problem & instance )
{
	switch ( format )
	{
		case format_rail:
			instance.read_rail( is );
			break;
		case format_stn:
			instance.read_stn( is );
			break;
		case format_scp:
		default:
			instance.read( is );
			break;
	};
}

int main( int argc, char * argv[] )
{
	docopt::option_group options;
	docopt::parser opt_parser;

	int num_runs = 100, format = format_scp;
	double alpha = 0.9, epsilon = 1e-9;
	unsigned int seed = std::time( 0 );
	bool verbose = true, help = false;
	std::string format_str, filename;

	double z_opt = 0,
	       min_gap = std::numeric_limits<double>::infinity(),
	       max_gap = 0,
	       sum_gap = 0;

	scp_problem instance;
	scp_solver_cplex cplex;
	scp_grasp grasp( alpha, epsilon );
	scp_local_search local_search;

	chrono::time_point begin, end;

	// Initialize option parser

	options.name( "Options:" )
		( "alpha",   'a', "FLOAT",  "RCL threshold parameter (in [0,1])" )
		(            'n', "INT",    "Number of tries" )
		( "random",  'r', "INT",    "Random seed" )
		( "epsilon", 'e', "FLOAT",  "Tolerance" )
		( "format",  'f', "FORMAT", "Instance file format (scp, rail, stn)" )
		( "quiet",   'q',           "Don't produce any verbose output" )
		( docopt::option::help() );

	opt_parser
		.add( options )
		.bind( 'a', &alpha )
		.bind( 'n', &num_runs )
		.bind( 'r', &seed )
		.bind( 'e', &epsilon )
		.bind( 'f', &format_str )
		.bind<bool, false>( 'q', &verbose )
		.bind( '?', &help )
		.bind_arguments( &filename );

	opt_parser.parse( argc, argv );

	// Print help

	if ( help || filename.empty() )
	{
		std::cout
			<< "Usage: " << argv[0] << " [OPTIONS] FILE" << std::endl
			<< std::endl
			<< options << std::endl;
		return 0;
	}

	// Select format

	if ( format_str == "rail" )
	{
		format = format_rail;
	}
	else if ( format_str == "stn" )
	{
		format = format_stn;
	}

	// Print options

	if ( verbose )
	{
		switch ( format )
		{
			case format_rail:
				std::cout << "format  = rail" << std::endl;
				break;
			case format_stn:
				std::cout << "format  = stn" << std::endl;
				break;
			case format_scp:
			default:
				std::cout << "format  = scp" << std::endl;
				break;
		};
		std::cout
			<< "alpha   = " << alpha << std::endl
			<< "n       = " << num_runs << std::endl
			<< "random  = " << seed << std::endl
			<< "epsilon = " << epsilon << std::endl;
	}

	// Initialize random number generator

	std::srand( seed );

	// Initialize GRASP

	grasp.alpha = alpha;
	grasp.epsilon = epsilon;

	// Open and load the instance file

	if ( filename.substr( filename.find_last_of( "." ) + 1 ) == "gz" )
	{
		// Read gzip compressed instance
		gz::ifstream file( filename.c_str() );
		if ( !file.is_open() )
		{
			std::cerr << "Error opening '" << filename << "'" << std::endl;
			return 0;
		}
		read_instance( format, file, instance );
		file.close();
	}
	else if ( filename != "-" )
	{
		// Read uncompressed instance from file
		std::ifstream file( filename.c_str() );
		if ( !file.is_open() )
		{
			std::cerr << "Error opening '" << filename << "'" << std::endl;
			return 0;
		}
		read_instance( format, file, instance );
		file.close();
	}
	else
	{
		// Read uncompressed instance from terminal
		read_instance( format, std::cin, instance );
	}

	if ( verbose )
	{
		std::cout << "Number of elements: " << instance.num_elements() << std::endl;
		std::cout << "Number of sets: " << instance.num_sets() << std::endl;
	}

	// Load the instance in algorithms

	cplex.read( instance );
	grasp.read( instance );
	local_search.read( instance );

	// Compute an optimal solution using CPLEX solver

	if ( !cplex.run() )
	{
		std::cerr << "Error: CPLEX fail" << std::endl;
		return 0;
	}

	z_opt = cplex.z();

	// Display the results

	if ( verbose )
	{
		std::cout << "[*] CPLEX: " << z_opt << std::endl;
	}

	// Run the metaheuristics

	begin = chrono::now();

	try
	{
		for ( int k = 0; k < num_runs; ++k )
		{
			double z_g = 0, z_ls = 0, gap = 0;
			std::set<int> x;

			// Generate a solution using GRASP

			if ( !grasp.run() )
			{
				throw std::runtime_error( "GRASP fail" );
			}

			x = grasp.x();
			z_g = grasp.z();

			if ( !instance.check( x, z_g ) )
			{
				throw std::runtime_error( "GRASP check fail" );
			}

			local_search.read( x );

			// Find a better solution using a local search

			if ( !local_search.run() )
			{
				throw std::runtime_error( "Local search fail" );
			}

			x = local_search.x();
			z_ls = local_search.z();

			if ( !instance.check( x, z_ls ) )
			{
				throw std::runtime_error( "Local search check fail" );
			}

			gap = ( z_ls - z_opt ) / z_opt;

			if ( gap < min_gap ) min_gap = gap;
			if ( gap > max_gap ) max_gap = gap;
			sum_gap += gap;

			// Display the results

			if ( verbose )
			{
				std::cout << "[" << k+1 << "] GRASP: " << z_g
				          << " | GRASP+LS: " << z_ls
				          << " | gap = " << 100. * gap << "%" << std::endl;
			}
		}
	}
	catch ( std::exception & e )
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}

	end = chrono::now();

	// Display result

	if ( verbose )
	{
		std::cout << "Gap: min = " << 100. * min_gap << "%"
			  << " | avg = " << 100. * sum_gap / double( num_runs ) << "%"
			  << " | max = " << 100. * max_gap << "%" << std::endl;

		std::cout << "Time: avg = " << 1000. * ( end - begin ) / double( num_runs ) << " ms" << std::endl;
	}
	else
	{
		std::cout
			<< 100. * min_gap << " "
			<< 100. * sum_gap / double( num_runs ) << " "
			<< 100. * max_gap << " "
			<< 1000. * ( end - begin ) / double( num_runs ) << std::endl;
	}

	return 0;
}
