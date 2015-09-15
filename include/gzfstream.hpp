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

#ifndef GZFSTREAM_HPP
#define GZFSTREAM_HPP

#include <ios>
#include <istream>
#include <ostream>
#include <streambuf>
#include <algorithm>
#include <cstdio>
#include <zlib.h>

namespace gz
{

/*
	Class: gz::filebuf

	A gzip file stream buffer.
*/
struct filebuf : std::streambuf
{
	filebuf();
	~filebuf();

	bool is_open() const;
	filebuf * open( const char * name, std::ios_base::openmode mode );
	filebuf * close();

protected:
	virtual int_type overflow( int_type c = traits_type::eof() );
	virtual int_type underflow();
	virtual int_type sync();

private:
	static const int        _size = 47+256;
	gzFile                  _file;
	char_type               _buffer[_size];
	bool                    _open;
	std::ios_base::openmode _mode;

	int_type _flush();
};

/*
	Class: gz::basic_fstream

	Base class for <gz::ifstream> and <gz::ofstream>.
*/
struct basic_fstream : virtual std::ios
{
	basic_fstream();
	basic_fstream( const char * name, std::ios_base::openmode mode );
	~basic_fstream();

	bool is_open();
	void open( const char * name, std::ios_base::openmode mode );
	void close();

	filebuf * rdbuf();

protected:
	filebuf _buf;
};

/*
	Class: gz::ifstream

	Gzip file input stream.
*/
struct ifstream : basic_fstream, std::istream
{
	ifstream();
	ifstream( const char * name, std::ios_base::openmode mode = std::ios::in );
	void open( const char * name, std::ios_base::openmode mode = std::ios::in );
};

/*
	Class: gz::ofstream

	Gzip file output stream.
*/
struct ofstream : basic_fstream, std::ostream
{
	ofstream();
	ofstream( const char * name, std::ios_base::openmode mode = std::ios::out );
	void open( const char * name, std::ios_base::openmode mode = std::ios::out );
};

////////////////////////////////////////////////////////////////////////////////

inline filebuf::filebuf() :
	_open( false )
{
	setp( _buffer, _buffer + ( _size - 1 ) );
	setg( _buffer + 4, _buffer + 4, _buffer + 4 );
}

inline filebuf::~filebuf()
{
	close();
}

inline bool filebuf::is_open() const
{
	return _open;
}

inline filebuf * filebuf::open( const char * name, std::ios_base::openmode mode )
{
	if ( is_open() )
	{
		return 0;
	}

	_mode = mode;

	if ( ( _mode & std::ios::ate )
	  || ( _mode & std::ios::app )
	  || ( ( _mode & std::ios::in ) && ( _mode & std::ios::out ) ) )
	{
		return 0;
	}

	char fmode[10];
	char * fmodeptr = fmode;

	if ( _mode & std::ios::in )
	{
		*fmodeptr++ = 'r';
	}
	else if ( _mode & std::ios::out )
	{
		*fmodeptr++ = 'w';
	}
	*fmodeptr++ = 'b';
	*fmodeptr = 0;

	if ( !( _file = gzopen( name, fmode ) ) )
	{
		return 0;
	}

	_open = true;
	return this;
}

inline filebuf * filebuf::close()
{
	if ( is_open() )
	{
		sync();
		_open = false;

		if ( gzclose( _file ) == Z_OK )
		{
			return this;
		}
	}
	return 0;
}

inline filebuf::int_type filebuf::overflow( int_type c )
{
	if ( !( _mode & std::ios::out ) || !_open )
	{
		return EOF;
	}

	if ( c != EOF )
	{
		*pptr() = c;
		pbump( 1 );
	}

	if ( _flush() == EOF )
	{
		return EOF;
	}
	return c;
}

inline filebuf::int_type filebuf::underflow()
{
	if ( gptr() && gptr() < egptr() )
	{
		return *reinterpret_cast<unsigned char *>( gptr() );
	}

	if ( !( _mode & std::ios::in ) || !_open )
	{
		return EOF;
	}

	int_type n_putback = gptr() - eback();

	if ( n_putback > 4 )
	{
		n_putback = 4;
	}

	std::copy( gptr() - n_putback, gptr(), _buffer + ( 4 - n_putback ) );

	int n = gzread( _file, _buffer + 4, _size - 4 );

	if ( n <= 0 )
	{
		return EOF;
	}

	setg( _buffer + ( 4 - n_putback ), _buffer + 4, _buffer + 4 + n );

	return *reinterpret_cast<unsigned char *>( gptr() );
}

inline filebuf::int_type filebuf::sync()
{
	if ( pptr() && pptr() > pbase() && _flush() == EOF )
	{
		return -1;
	}
	return 0;
}

inline filebuf::int_type filebuf::_flush()
{
	int_type w = pptr() - pbase();

	if ( gzwrite( _file, pbase(), w ) != w )
	{
		return EOF;
	}

	pbump( -w );
	return w;
}

////////////////////////////////////////////////////////////////////////////////

inline basic_fstream::basic_fstream() 
{
	init( &_buf );
}

inline basic_fstream::basic_fstream( const char * name, std::ios_base::openmode mode )
{
	init( &_buf );
	open( name, mode );
}

inline basic_fstream::~basic_fstream()
{
	_buf.close();
}

inline bool basic_fstream::is_open()
{
	return _buf.is_open();
}

inline void basic_fstream::open( const char * name, std::ios_base::openmode mode )
{
	if ( !_buf.open( name, mode ) )
	{
		clear( rdstate() | std::ios::badbit );
	}
}

inline void basic_fstream::close()
{
	if ( _buf.is_open() && !_buf.close() )
	{
		clear( rdstate() | std::ios::badbit );
	}
}

inline filebuf * basic_fstream::rdbuf()
{
	return &_buf;
}

////////////////////////////////////////////////////////////////////////////////

inline ifstream::ifstream() :
	basic_fstream(),
	std::istream( &_buf )
{
	init( &_buf );
}

inline ifstream::ifstream( const char * name, std::ios_base::openmode mode ) :
	basic_fstream( name, mode ),
	std::istream( &_buf )
{
}

inline void ifstream::open( const char * name, std::ios_base::openmode mode )
{
	if ( !_buf.open( name, mode ) )
	{
		clear( rdstate() | std::ios::badbit );
	}
}

////////////////////////////////////////////////////////////////////////////////

inline ofstream::ofstream() :
	basic_fstream(),
	std::ostream( &_buf )
{
	init( &_buf );
}

inline ofstream::ofstream( const char * name, std::ios_base::openmode mode ) :
	basic_fstream( name, mode ),
	std::ostream( &_buf )
{
}

inline void ofstream::open( const char * name, std::ios_base::openmode mode )
{
	if ( !_buf.open( name, mode ) )
	{
		clear( rdstate() | std::ios::badbit );
	}
}

}

#endif
