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

#ifndef DOCOPT_HPP
#define DOCOPT_HPP

#include <iostream>
#include <iomanip>
#include <streambuf>
#include <sstream>
#include <string>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <utility>
#include <cctype>
#include <getopt.h>

namespace docopt
{

enum
{
	arg_optional = 1 << 0, // The argument is optional.
	hidden       = 1 << 1  // The option will not be shown in --help.
};

struct less_char
{
	bool operator () ( char c1, char c2 ) const;
};

struct less_string
{
	bool operator () ( const std::string & s1, const std::string & s2 ) const;
};

/*
	Class: option
*/
struct option
{
	typedef                               char   char_type;
	typedef                        std::string   string_type;
	typedef                                int   flag_type;
	typedef     std::set<char_type, less_char>   set_char_type;
	typedef std::set<string_type, less_string>   set_string_type;

	option( const string_type & doc = "", const string_type & arg = "", flag_type flags = 0 );

	const string_type & doc() const;
	const string_type & arg() const;
	flag_type flags() const;

	option & doc( const string_type & s );
	option & arg( const string_type & s );
	option & flags( flag_type flags );

	option & assign( char_type key );
	option & assign( const string_type & name );

	bool has( char_type key ) const;
	bool has( const string_type & name ) const;

	const set_char_type & keys() const;
	const set_string_type & names() const;

	char_type front() const;
	char_type front_key() const;
	string_type front_name() const;

	bool compare( const option & x ) const;

	// Alias of assign
	option & operator << ( char_type key );
	option & operator << ( const string_type & name );

	// Alias of has
	bool operator [] ( char_type key ) const;
	bool operator [] ( const string_type & name ) const;

	// Output
	void write( std::ostream & os, int key_col = 2, int name_col = 6, int doc_col = 29, int rmargin = 79 ) const;

	// Built-in options
	static option help();
	static option usage();
	static option version();

private:
	set_char_type _key;
	set_string_type _name;
	string_type _doc, _arg;
	int _flags, _index;
};

// Alias of compare
bool operator < ( const option & opt1, const option & opt2 );

// Alias of write
std::ostream & operator << ( std::ostream & os, const option & opt );

/*
	Class: option_group
*/
struct option_group
{
	typedef                                option   option_type;
	typedef                     option::char_type   char_type;
	typedef                   option::string_type   string_type;
	typedef std::set<option_type>::const_iterator   const_iterator;
	typedef                           std::size_t   size_type;
	typedef               std::list<option_group>   children_type;

	option_group( const string_type & name = "" );

	const string_type & name() const;
	option_group & name( const string_type & name );

	option_group & operator () ( const option_type & opt );
	option_group & operator () ( const option_group & grp );

	option_group & operator () ( char_type key, const string_type & doc, int flags = 0 );
	option_group & operator () ( char_type key, const string_type & arg, const string_type & doc, int flags = 0 );

	option_group & operator () ( const string_type & name, const string_type & doc, int flags = 0 );
	option_group & operator () ( const string_type & name, const string_type & arg, const string_type & doc, int flags = 0 );

	option_group & operator () ( const string_type & name, char_type key, const string_type & doc, int flags = 0 );
	option_group & operator () ( const string_type & name, char_type key, const string_type & arg, const string_type & doc, int flags = 0 );

	const_iterator begin() const;
	const_iterator end() const;

	size_type size() const;

	const children_type & children() const;

	// Output
	void write( std::ostream & os, int header_col = 1, int key_col = 2, int name_col = 6, int doc_col = 29, int rmargin = 79 ) const;

private:
	string_type _name;
	std::set<option_type> _options;
	children_type _children;
};

std::ostream & operator << ( std::ostream & os, const option_group & grp );

/*
	Class: parser
*/
struct parser
{
	typedef                                option   option_type;
	typedef                     option::char_type   char_type;
	typedef                   option::string_type   string_type;
	typedef                           std::size_t   size_type;
	typedef std::set<option_type>::const_iterator   const_iterator;
	typedef bool (*callback_type)(char, const char *, const char *, void *);

	parser();
	parser( callback_type cb, void * user = 0 );
	parser( const option_group & grp );
	parser( const option_group & grp, callback_type cb, void * user = 0 );

	parser & add( const option_type & opt );
	parser & add( const option_group & grp );

	parser & callback( callback_type cb, void * user = 0 );

	callback_type callback() const;
	void * userdata() const;

	template<typename T>
	parser & bind( char_type key, T * x );
	template<typename T>
	parser & bind( const string_type & name, T * x );

	template<typename T, T val>
	parser & bind( char_type key, T * x );
	template<typename T, T val>
	parser & bind( const string_type & name, T * x );

	parser & bind( char_type key, callback_type cb, void * user = 0 );
	parser & bind( const string_type & name, callback_type cb, void * user = 0 );
	parser & bind( const option_type & opt, callback_type cb, void * user = 0 );

	template<typename T>
	parser & bind_arguments( T * x );
	parser & bind_arguments( callback_type cb, void * user );

	size_type size() const;

	void clear();

	const_iterator begin() const;
	const_iterator end() const;

	const_iterator find( char_type key ) const;
	const_iterator find( const string_type & name ) const;

	bool parse( int argc, char * argv[] ) const;

	bool default_parser( char key, const char * name, const char * arg );

	static bool default_parser( char key, const char * name, const char * arg, void * user );
	static bool debug_parser( char key, const char * name, const char * arg, void * user );

	template<typename T>
	static bool assign( char key, const char * name, const char * arg, void * user );

	template<typename T, T val>
	static bool assign_value( char key, const char * name, const char * arg, void * user );

private:
	std::set<option_type> _options;
	callback_type _callback;
	void * _userdata;

	std::map< char_type, std::pair<callback_type, void *> > _bind_key;
	std::map< string_type, std::pair<callback_type, void *> > _bind_name;

	callback_type _callback_args;
	void * _userdata_args;
};

////////////////////////////////////////////////////////////////////////////////

/*
	Class: wrapbuf
*/
struct wrapbuf : std::streambuf
{
	typedef std::size_t size_type;
	typedef std::basic_string<char_type> string_type;

	wrapbuf( std::streambuf * s, size_type w );
	wrapbuf( std::streambuf * s, size_type indentw, size_type w );
	~wrapbuf();

	void indent( size_type indentw );
	size_type indent() const;

	void width( size_type w );
	size_type width() const;

	void first( int w );
	int first() const;

private:
	virtual int_type overflow( int_type c = traits_type::eof() );
	void _put_margin();

	static const int _tab_width = 8;
	std::streambuf * _sbuf;
	size_type _indent,
	          _width,
	          _count;
	string_type _buffer;
	int _first;
};

////////////////////////////////////////////////////////////////////////////////

/*
	Class: wrapstream

	A word wrap stream.
*/
struct wrapstream : std::ostream
{
	typedef std::size_t size_type;

	wrapstream( std::ostream & os, size_type w = 80 );
	wrapstream( std::ostream & os, size_type indentw, size_type w );

	wrapstream & indent( size_type indentw );
	size_type indent() const;

	wrapstream & wrap( size_type w );
	wrapstream & wrap( size_type indentw, size_type w );

	wrapstream & first( size_type w );

private:
	wrapbuf  _buf;
};

////////////////////////////////////////////////////////////////////////////////

inline bool less_char::operator () ( char c1, char c2 ) const
{
	if ( std::isalnum( c1 ) && std::isalnum( c2 ) )
	{
		return std::tolower( c1 ) < std::tolower( c2 )
		  || ( std::tolower( c1 ) == std::tolower( c2 ) && c2 < c1 );
	}
	else
	{
		return std::isalnum( c1 ) || ( !std::isalnum( c2 ) && c1 < c2 );
	}
}

inline bool less_string::operator () ( const std::string & s1, const std::string & s2 ) const
{
	return std::lexicographical_compare( s1.begin(), s1.end(), s2.begin(), s2.end(), less_char() );
}

////////////////////////////////////////////////////////////////////////////////

inline option::option( const string_type & doc, const string_type & arg, flag_type flags ) :
	_doc( doc ),
	_arg( arg ),
	_flags( flags )
{
}

inline const option::string_type & option::doc() const
{
	return _doc;
}

inline const option::string_type & option::arg() const
{
	return _arg;
}

inline option::flag_type option::flags() const
{
	return _flags;
}

inline option & option::doc( const string_type & s )
{
	_doc = s;
	return *this;
}

inline option & option::arg( const string_type & s )
{
	_arg = s;
	return *this;
}

inline option & option::flags( flag_type flags )
{
	_flags = flags;
	return *this;
}

inline option & option::assign( char_type key )
{
	_key.insert( key );
	return *this;
}

inline option & option::assign( const string_type & name )
{
	_name.insert( name );
	return *this;
}

inline bool option::has( char_type key ) const
{
	return _key.find( key ) != _key.end();
}

inline bool option::has( const string_type & name ) const
{
	return _name.find( name ) != _name.end();
}

inline const option::set_char_type & option::keys() const
{
	return _key;
}

inline const option::set_string_type & option::names() const
{
	return _name;
}

inline option::char_type option::front() const
{
	return !_key.empty() ? *_key.begin() : ( !_name.empty() ? (*_name.begin())[0] : 0 );
}

inline option::char_type option::front_key() const
{
	return !_key.empty() ? *_key.begin() : 0;
}

inline option::string_type option::front_name() const
{
	return !_name.empty() ? *_name.begin() : string_type();
}

inline bool option::compare( const option & x ) const
{
	option::char_type c1 = front(), c2 = x.front();
	if ( c1 == c2 )
	{
		bool h1 = has( c1 ), h2 = x.has( c1 );

		if ( h1 && h2 )
		{
			return false;
		}
		else if ( h1 || h2 )
		{
			return h1;
		}
		else
		{
			return less_string()( front_name(), x.front_name() );
		}
	}
	return less_char()( c1, c2 );
}

inline option & option::operator << ( char_type key )
{
	assign( key );
	return *this;
}

inline option & option::operator << ( const string_type & name )
{
	assign( name );
	return *this;
}

inline bool option::operator [] ( char_type key ) const
{
	return has( key );
}

inline bool option::operator [] ( const string_type & name ) const
{
	return has( name );
}

inline void option::write( std::ostream & os, int key_col, int name_col, int doc_col, int rmargin ) const
{
	std::ostringstream buf;
	wrapstream docwrap( os, doc_col, rmargin - doc_col );
	set_char_type::const_iterator k;
	set_string_type::const_iterator n;

	buf << string_type( _key.empty() ? name_col : key_col, ' ' );

	for ( k = _key.begin(); k != _key.end(); ++k )
	{
		if ( k != _key.begin() )
		{
			buf << ", ";
		}
		buf << "-" << char( *k );

		// Argument
		if ( !arg().empty() && _name.empty() )
		{
			if ( flags() & arg_optional )
			{
				buf << "[" << arg() << "]";
			}
			else
			{
				buf << " " << arg();
			}
		}
	}

	if ( !_key.empty() && !_name.empty() )
	{
		buf << ", ";
	}

	for ( n = _name.begin(); n != _name.end(); ++n )
	{
		if ( n != _name.begin() )
		{
			buf << ", ";
		}
		buf << "--" << *n;

		// Argument
		if ( !arg().empty() )
		{
			if ( flags() & arg_optional )
			{
				buf << "[=" << arg() << "]";
			}
			else
			{
				buf << "=" << arg();
			}
		}
	}

	os << std::left << std::setw( doc_col ) << buf.str();

	if ( int( buf.str().length() ) > doc_col-2 )
	{
		os << std::endl
		   << string_type( doc_col, ' ' );
	}

	docwrap.first( 0 ) << doc();
}

inline option option::help()
{
	option x( "Give this help list" );
	x.assign( '?' );
	x.assign( "help" );
	return x;
}

inline option option::usage()
{
	option x( "Give a short usage message" );
	x.assign( "usage" );
	return x;
}

inline option option::version()
{
	option x( "Print program version" );
	x.assign( 'V' );
	x.assign( "version" );
	return x;
}

inline bool operator < ( const option & opt1, const option & opt2 )
{
	return opt1.compare( opt2 );
}

inline std::ostream & operator << ( std::ostream & os, const option & opt )
{
	opt.write( os );
	return os;
}

////////////////////////////////////////////////////////////////////////////////

inline option_group::option_group( const string_type & name ) :
	_name( name )
{
}

inline const option_group::string_type & option_group::name() const
{
	return _name;
}

inline option_group & option_group::name( const string_type & name )
{
	_name = name;
	return *this;
}

inline option_group & option_group::operator () ( const option_type & opt )
{
	_options.insert( opt );
	return *this;
}

inline option_group & option_group::operator () ( const option_group & grp )
{
	_children.push_back( grp );
	return *this;
}

inline option_group & option_group::operator () ( char_type key, const string_type & doc, int flags )
{
	_options.insert( option_type( doc, "", flags ) << key );
	return *this;
}

inline option_group & option_group::operator () ( char_type key, const string_type & arg, const string_type & doc, int flags )
{
	_options.insert( option_type( doc, arg, flags ) << key );
	return *this;
}

inline option_group & option_group::operator () ( const string_type & name, const string_type & doc, int flags )
{
	_options.insert( option_type( doc, "", flags ) << name );
	return *this;
}

inline option_group & option_group::operator () ( const string_type & name, const string_type & arg, const string_type & doc, int flags )
{
	_options.insert( option_type( doc, arg, flags ) << name );
	return *this;
}

inline option_group & option_group::operator () ( const string_type & name, char_type key, const string_type & doc, int flags )
{
	_options.insert( option_type( doc, "", flags ) << key << name );
	return *this;
}

inline option_group & option_group::operator () ( const string_type & name, char_type key, const string_type & arg, const string_type & doc, int flags )
{
	_options.insert( option_type( doc, arg, flags ) << key << name );
	return *this;
}

inline option_group::const_iterator option_group::begin() const
{
	return _options.begin();
}

inline option_group::const_iterator option_group::end() const
{
	return _options.end();
}

inline option_group::size_type option_group::size() const
{
	return _options.size();
}

inline const option_group::children_type & option_group::children() const
{
	return _children;
}

inline void option_group::write( std::ostream & os, int header_col, int key_col, int name_col, int doc_col, int rmargin ) const
{
	std::set<option_type>::const_iterator opt;
	children_type::const_iterator grp;

	if ( !name().empty() )
	{
		os << string_type( header_col, ' ' ) << name() << std::endl;
	}

	for ( opt = _options.begin(); opt != _options.end(); ++opt )
	{
		opt->write( os, key_col, name_col, doc_col, rmargin );
	}

	for ( grp = _children.begin(); grp != _children.end(); ++grp )
	{
		if ( grp != _children.begin() || !_options.empty() )
		{
			os << std::endl;
		}
		grp->write( os, header_col, key_col, name_col, doc_col, rmargin );
	}
}

inline std::ostream & operator << ( std::ostream & os, const option_group & grp )
{
	grp.write( os );
	return os;
}

////////////////////////////////////////////////////////////////////////////////

inline parser::parser() :
	_callback( default_parser ),
	_userdata( this ),
	_callback_args( 0 ),
	_userdata_args( 0 )
{
}

inline parser::parser( callback_type cb, void * user ) :
	_callback( cb ),
	_userdata( user ),
	_callback_args( 0 ),
	_userdata_args( 0 )
{
}

inline parser::parser( const option_group & grp ) :
	_callback( default_parser ),
	_userdata( this ),
	_callback_args( 0 ),
	_userdata_args( 0 )
{
	add( grp );
}

inline parser::parser( const option_group & grp, callback_type cb, void * user ) :
	_callback( cb ),
	_userdata( user ),
	_callback_args( 0 ),
	_userdata_args( 0 )
{
	add( grp );
}

inline parser & parser::add( const option_group & grp )
{
	option_group::const_iterator opt;
	option_group::children_type::const_iterator g;

	for ( opt = grp.begin(); opt != grp.end(); ++opt )
	{
		add( *opt );
	}

	for ( g = grp.children().begin(); g != grp.children().end(); ++g )
	{
		add( *g );
	}
	return *this;
}

inline parser & parser::add( const option_type & opt )
{
	_options.insert( opt );
	return *this;
}

inline parser::size_type parser::size() const
{
	return _options.size();
}

inline parser & parser::callback( callback_type cb, void * user )
{
	_callback = cb;
	_userdata = user;
	return *this;
}

inline parser::callback_type parser::callback() const
{
	return _callback;
}

inline void * parser::userdata() const
{
	return _userdata;
}

template<typename T>
inline parser & parser::bind( char_type key, T * x )
{
	return bind( key, assign<T>, x );
}

template<typename T>
inline parser & parser::bind( const string_type & name, T * x )
{
	return bind( name, assign<T>, x );
}

template<typename T, T val>
inline parser & parser::bind( char_type key, T * x )
{
	return bind( key, assign_value<T,val>, x );
}

template<typename T, T val>
inline parser & parser::bind( const string_type & name, T * x )
{
	return bind( name, assign_value<T,val>, x );
}

inline parser & parser::bind( char_type key, callback_type cb, void * user )
{
	const_iterator opt = find( key );

	if ( opt != end() )
	{
		bind( *opt, cb, user );
	}
	return *this;
}

inline parser & parser::bind( const string_type & name, callback_type cb, void * user )
{
	const_iterator opt = find( name );

	if ( opt != end() )
	{
		bind( *opt, cb, user );
	}
	return *this;
}

inline parser & parser::bind( const option_type & opt, callback_type cb, void * user )
{
	option::set_char_type::const_iterator k;
	option::set_string_type::const_iterator n;

	for ( k = opt.keys().begin(); k != opt.keys().end(); ++k )
	{
		_bind_key[*k].first = cb;
		_bind_key[*k].second = user;
	}

	for ( n = opt.names().begin(); n != opt.names().end(); ++n )
	{
		_bind_name[*n].first = cb;
		_bind_name[*n].second = user;
	}
	return *this;
}

template<typename T>
inline parser & parser::bind_arguments( T * x )
{
	return bind_arguments( assign<T>, x );
}

inline parser & parser::bind_arguments( callback_type cb, void * user )
{
	_callback_args = cb;
	_userdata_args = user;
	return *this;
}

inline void parser::clear()
{
	_options.clear();
}

inline parser::const_iterator parser::begin() const
{
	return _options.begin();
}

inline parser::const_iterator parser::end() const
{
	return _options.end();
}

inline parser::const_iterator parser::find( char_type key ) const
{
	const_iterator opt;

	for ( opt = begin(); opt != end(); ++opt )
	{
		if ( opt->keys().find( key ) != opt->keys().end() )
		{
			return opt;
		}
	}
	return opt;
}

inline parser::const_iterator parser::find( const string_type & name ) const
{
	const_iterator opt;

	for ( opt = begin(); opt != end(); ++opt )
	{
		if ( opt->names().find( name ) != opt->names().end() )
		{
			return opt;
		}
	}
	return opt;
}

inline bool parser::parse( int argc, char * argv[] ) const
{
	typedef struct ::option getopt_option;
	std::vector<getopt_option> long_opts;
	std::ostringstream short_opts;
	const_iterator opt;
	int next = 0, long_index;
	bool ok = true;

	for ( opt = _options.begin(); opt != _options.end(); ++opt )
	{
		option::set_char_type::const_iterator key;
		option::set_string_type::const_iterator name;

		for ( key = opt->keys().begin(); key != opt->keys().end(); ++key )
		{
			short_opts << char( *key );
			if ( !opt->arg().empty() )
			{
				short_opts << ( ( opt->flags() & arg_optional ) ? "::" : ":" );
			}
		}

		for ( name = opt->names().begin(); name != opt->names().end(); ++name )
		{
			getopt_option lo;
			lo.name    = name->c_str();
			lo.has_arg = !opt->arg().empty()
				         ? ( ( opt->flags() & arg_optional )
				           ? (optional_argument)
				           : (required_argument))
				         : (no_argument);
			lo.flag    = 0;
			lo.val     = 0x80 + long_opts.size();
			long_opts.push_back( lo );
		}
	}

	// Final option
	{
		getopt_option lo = { 0, 0, 0, 0 };
		long_opts.push_back( lo );
	}

	while ( ok && next != -1 )
	{
		long_index = -1;

		next = getopt_long( argc, argv,
		                    short_opts.str().c_str(),
		                    &long_opts.front(),
		                    &long_index );

		if ( next != -1 )
		{
			if ( long_index != -1 )
			{
				ok = _callback( 0, long_opts[long_index].name, optarg, _userdata );
			}
			else
			{
				ok = _callback( next, 0, optarg, _userdata );
			}
		}
	}

	for ( int i = optind; ok && i < argc; ++i )
	{
		ok = _callback( 0, 0, argv[i], _userdata );
	}
	return ok;
}

inline bool parser::default_parser( char key, const char * name, const char * arg )
{
	typedef std::map<char_type, std::pair<callback_type, void *> > char_map;
	typedef std::map<string_type, std::pair<callback_type, void *> > string_map;

	if ( key )
	{
		char_map::iterator it = _bind_key.find( key );

		if ( it != _bind_key.end() && it->second.first )
		{
			callback_type call = it->second.first;
			void * user = it->second.second;

			return call( key, name, arg, user );
		}
	}
	else if ( name )
	{
		string_map::iterator it = _bind_name.find( name );

		if ( it != _bind_name.end() && it->second.first )
		{
			callback_type call = it->second.first;
			void * user = it->second.second;

			return call( key, name, arg, user );
		}
	}
	else if ( _callback_args )
	{
		return _callback_args( key, name, arg, _userdata_args );
	}
	return true;
}

inline bool parser::default_parser( char key, const char * name, const char * arg, void * user )
{
	return static_cast<parser *>( user )->default_parser( key, name, arg );
}

inline bool parser::debug_parser( char key, const char * name, const char * arg, void * user )
{
	if ( key )
	{
		if ( std::isprint( key ) )
		{
			std::cout << "'" << key << "'";
		}
		else
		{
			std::cout << "[" << int( key ) << "]";
		}
	}

	if ( name )
	{
		std::cout << '"' << name << '"';
	}

	if ( arg )
	{
		if ( key || name )
		{
			std::cout << " = ";
		}
		std::cout << '"' << arg << '"';
	}
	std::cout << std::endl;
	return true;
}

template<typename T>
inline bool parser::assign( char key, const char * name, const char * arg, void * user )
{
	if ( arg && user )
	{
		std::istringstream( arg ) >> *static_cast<T *>( user );
	}
	return true;
}

template<>
inline bool parser::assign<bool>( char key, const char * name, const char * arg, void * user )
{
	if ( user )
	{
		*static_cast<bool *>( user ) = true;
	}
	return true;
}

template<>
inline bool parser::assign<std::string>( char key, const char * name, const char * arg, void * user )
{
	if ( arg && user )
	{
		*static_cast<std::string *>( user ) = arg;
	}
	return true;
}

template<>
inline bool parser::assign< std::vector<std::string> >( char key, const char * name, const char * arg, void * user )
{
	if ( arg && user )
	{
		static_cast<std::vector<std::string> *>( user )->push_back( arg );
	}
	return true;
}

template<>
inline bool parser::assign< std::list<std::string> >( char key, const char * name, const char * arg, void * user )
{
	if ( arg && user )
	{
		static_cast<std::list<std::string> *>( user )->push_back( arg );
	}
	return true;
}

template<>
inline bool parser::assign< std::set<std::string> >( char key, const char * name, const char * arg, void * user )
{
	if ( arg && user )
	{
		static_cast<std::set<std::string> *>( user )->insert( arg );
	}
	return true;
}

template<typename T, T val>
inline bool parser::assign_value( char key, const char * name, const char * arg, void * user )
{
	if ( user )
	{
		*static_cast<T *>( user ) = val;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

inline wrapbuf::wrapbuf( std::streambuf * s, size_type w ) :
	_sbuf( s ),
	_indent( 0 ),
	_width( w ),
	_count( 0 ),
	_first( -1 )
{
}

inline wrapbuf::wrapbuf( std::streambuf * s, size_type indentw, size_type w ) :
	_sbuf( s ),
	_indent( indentw ),
	_width( w ),
	_count( 0 ),
	_first( -1 )
{
}

inline wrapbuf::~wrapbuf()
{
	overflow( '\n' );
}

inline void wrapbuf::indent( size_type indentw )
{
	_indent = indentw;
}

inline wrapbuf::size_type wrapbuf::indent() const
{
	return _indent;
}

inline void wrapbuf::width( size_type w )
{
	_width = w;
}

inline wrapbuf::size_type wrapbuf::width() const
{
	return _width;
}

inline void wrapbuf::first( int_type w )
{
	_first = w;
}

inline wrapbuf::int_type wrapbuf::first() const
{
	return _first;
}

inline wrapbuf::int_type wrapbuf::overflow( int_type c )
{
	if ( traits_type::eq_int_type( traits_type::eof(), c ) )
	{
		return traits_type::not_eof( c );
	}

	switch ( c )
	{
		case '\r':
		case '\n':
		{
			_buffer += c;
			_count = 0;
			_put_margin();
			int_type rc = _sbuf->sputn( _buffer.c_str(), _buffer.size() );
			_buffer.clear();
			return rc;
		}
		case '\a':
			return _sbuf->sputc( c );
		case '\t':
			_buffer += c;
			_count += _tab_width - _count % _tab_width;
			return c;
		default:
			if ( _count >= _width )
			{
				size_type wpos = _buffer.find_last_of( " \t" );
				_put_margin();
				if ( wpos != string_type::npos )
				{
					_sbuf->sputn( _buffer.c_str(), wpos );
					_count = _buffer.size() - wpos - 1;
					_buffer = string_type( _buffer, wpos + 1 );
				}
				else
				{
					_sbuf->sputn( _buffer.c_str(), _buffer.size() );
					_buffer.clear();
					_count = 0;
				}
				_sbuf->sputc( '\n' );
			}
			_buffer += c;
			++_count;
			return c;
	}
}

inline void wrapbuf::_put_margin()
{
	if ( _first < 0 )
	{
		_sbuf->sputn( string_type( _indent, ' ' ).c_str(), _indent );
	}
	else
	{
		_sbuf->sputn( string_type( _first, ' ' ).c_str(), _first );
		_first = -1;
	}
}

////////////////////////////////////////////////////////////////////////////////

inline wrapstream::wrapstream( std::ostream & os, size_type w ) :
	std::ostream( &_buf ),
	_buf( os.rdbuf(), w )
{
}

inline wrapstream::wrapstream( std::ostream & os, size_type indentw, size_type w ) :
	std::ostream( &_buf ),
	_buf( os.rdbuf(), indentw, w )
{
}

inline wrapstream & wrapstream::indent( size_type indentw )
{
	_buf.indent( indentw );
	return *this;
}

inline wrapstream::size_type wrapstream::indent() const
{
	return _buf.indent();
}

inline wrapstream & wrapstream::wrap( size_type w )
{
	_buf.width( w );
	return *this;
}

inline wrapstream & wrapstream::wrap( size_type indentw, size_type w )
{
	_buf.indent( indentw );
	_buf.width( w );
	return *this;
}

inline wrapstream & wrapstream::first( size_type w )
{
	_buf.first( w );
	return *this;
}

}

#endif
