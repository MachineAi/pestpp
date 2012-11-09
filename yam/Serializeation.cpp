/*  
    � Copyright 2012, David Welter
    
    This file is part of PEST++.
   
    PEST++ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PEST++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PEST++.  If not, see<http://www.gnu.org/licenses/>.
*/

#include "network_wrapper.h"
#include <string>
#include <sstream>
#include <memory>
#include <cassert>
#include "Serialization.h"
#include "Transformable.h"
#include "utilities.h"

using namespace std;
using namespace pest_utils;


vector<char> Serialization::serialize(unsigned long data)
{
	vector<char> buf;
	unsigned buf_sz = sizeof(data);
    buf.resize(buf_sz);
	size_t i_start = 0;
	w_memcpy_s(&buf[i_start], buf_sz, &data, buf_sz);
	return buf;
}

unsigned long Serialization::unserialize(const vector<char> &buf, unsigned long &data, unsigned long start_loc)
{
	assert(buf.size()-start_loc >= sizeof(data));
	w_memcpy_s(&data, sizeof(data), &buf[start_loc], sizeof(data));
	unsigned bytes_read = sizeof(data);
	return bytes_read;
}


vector<char> Serialization::serialize(const Transformable &tr_data)
{
	vector<char> buf;
	unsigned long buf_sz = 0;
	unsigned long names_buf_sz = 0;;
	unsigned long data_buf_sz = 0;
	// calculate buffer size
 	for(auto &b : tr_data) 
	{
		names_buf_sz += b.first.size() + 1;
	}
	data_buf_sz = sizeof(double)*tr_data.size();
	buf_sz = sizeof(unsigned long) + names_buf_sz + sizeof(unsigned long) + data_buf_sz;
	// allocate space
	buf.resize(buf_sz,'\0');
	// build string with space deliminated names and array of numbers
	vector<char> names;
    names.reserve(names_buf_sz);
	vector<double> values;
	for (auto &b : tr_data)
	{
		names.insert(names.end(), b.first.begin(), b.first.end());
        names.push_back(' ');
		values.push_back(b.second);
	}
	unsigned long n_rec = values.size();
	//write information to buffer
	size_t i_start = 0;
	w_memcpy_s(&buf[i_start], buf_sz-i_start, &names_buf_sz, sizeof(names_buf_sz));
	i_start += sizeof(names_buf_sz);
	w_memcpy_s(&buf[i_start], buf_sz-i_start, names.data(), names_buf_sz);
	i_start +=names_buf_sz;
	w_memcpy_s(&buf[i_start], buf_sz-i_start, &n_rec, sizeof(n_rec));
	i_start +=sizeof(n_rec);
	w_memcpy_s(&buf[i_start], buf_sz-i_start, &values[0], data_buf_sz);
	return buf;
}

vector<char> Serialization::serialize(const vector< const Transformable*> tr_vec)
{
	vector<char> buf;
	for (auto i : tr_vec)
	{
		vector<char> serial_data = serialize(*i);
		buf.insert(buf.end(), serial_data.begin(), serial_data.end());
	}
	return buf;
}

vector<char> Serialization::serialize(const std::vector<Transformable*> &tr_vec)
{
	vector<const Transformable*> const_data_vec;
	for (auto &i : tr_vec)
	{
		const_data_vec.push_back(i);
	}
	return serialize(const_data_vec);
}

vector<char> Serialization::serialize(const Parameters &pars, const Observations &obs)
{
	 vector<const Transformable*> tr_vec;
	 tr_vec.push_back(&pars);
	 tr_vec.push_back(&obs);
	 return serialize(tr_vec);
}

vector<char> Serialization::serialize(const vector<string> &string_vec)
{
	vector<char> serial_data;
	for (auto &i : string_vec)
	{
		serial_data.insert(serial_data.end(), i.begin(), i.end());
		serial_data.push_back('\0');
	}
	return serial_data;
}

vector<char> Serialization::serialize(const vector<vector<string>*> &string_vec_vec)
{
	vector<char> serial_data;
	unsigned long buf_sz = 0;
	for (auto &i : string_vec_vec)
	{
		vector<char> buf_ser = serialize(*i);
		buf_sz = buf_ser.size();
		vector<char> buf_size = serialize(buf_sz);
		serial_data.insert(serial_data.end(), buf_size.begin(), buf_size.end());
		serial_data.insert(serial_data.end(), buf_ser.begin(), buf_ser.end());
	}
	return serial_data;
}


unsigned long Serialization::unserialize(const std::vector<char> &buf, Transformable &tr_data, unsigned long start_loc)
{
	// delete all existing items
	tr_data.clear();
	unsigned long names_arg_sz;
	unsigned long n_rec;
	unsigned long bytes_read;
	size_t i_start = 0;
	// get size of names record
	i_start = start_loc;
	w_memcpy_s(&names_arg_sz, sizeof(names_arg_sz), buf.data()+i_start, sizeof(names_arg_sz));
	i_start += sizeof(names_arg_sz);
	unique_ptr<char[]> names_buf(new char[names_arg_sz]);
	w_memcpy_s(names_buf.get(), sizeof(char)*names_arg_sz,  buf.data()+i_start, sizeof(char)*names_arg_sz);
	i_start += sizeof(char)*names_arg_sz;
	w_memcpy_s(&n_rec, sizeof(n_rec),  buf.data()+i_start, sizeof(n_rec));
	i_start += sizeof(n_rec);
	// build transformable data set
	double *value_ptr = (double *) ( buf.data()+i_start);
	vector<string> names_vec;
	tokenize(strip_cp(string(names_buf.get(), names_arg_sz)), names_vec);
	assert(names_vec.size() == n_rec);
	for (unsigned long i=0; i< n_rec; ++i)
	{
		tr_data.insert(names_vec[i], *(value_ptr+i));
	}
	bytes_read = i_start + n_rec * sizeof(double);
	return bytes_read;
}

unsigned long Serialization::unserialize(const std::vector<char> &ser_data, std::vector<Transformable*> &tr_vec, unsigned long start_loc)
{
	unsigned i_tr=start_loc;
	unsigned i_char=0;
	unsigned long bytes_read = 0;
	unsigned long total_bytes_read = 0;


	while(i_tr < tr_vec.size() && i_char < ser_data.size())
	{
		bytes_read = Serialization::unserialize(ser_data, *tr_vec[i_tr], i_char);
		i_char +=  bytes_read;
		++i_tr;
		total_bytes_read += bytes_read;
	}
	return total_bytes_read;
}

unsigned long Serialization::unserialize(const std::vector<char> &data, Parameters &pars, Observations &obs, unsigned long start_loc)
{
	 unsigned total_bytes_read = 0;
	 vector<Transformable*> tr_vec;
	 tr_vec.push_back(&pars);
	 tr_vec.push_back(&obs);
	 total_bytes_read = unserialize(data, tr_vec, start_loc);
	 return total_bytes_read;
}

unsigned long Serialization::unserialize(const vector<char> &ser_data, vector<string> &string_vec, unsigned long start_loc, unsigned long max_read_bytes)
{
	unsigned total_bytes_read = 0;
	assert(start_loc < ser_data.size());
	total_bytes_read = min(ser_data.size()-start_loc, max_read_bytes);
	string tmp_str(ser_data.begin()+start_loc, ser_data.begin()+start_loc+total_bytes_read);
	string delm;
	delm.push_back('\0');
	strip_ip(tmp_str, "both", delm);
	tokenize(tmp_str, string_vec, delm);
	return total_bytes_read;
}

unsigned long Serialization::unserialize(const vector<char> &ser_data, vector<vector<string>> &string_vec_vec)
{
	unsigned long bytes_read = 0;
	unsigned long total_bytes_read = 0;
	unsigned long vec_size = 0;

	// can recode to increase performance by eliminating repeated copying
	// but at this point it is not worth the effort
	while(total_bytes_read < ser_data.size())
	{
		bytes_read = unserialize(ser_data, vec_size, total_bytes_read);
		total_bytes_read += bytes_read;
		string_vec_vec.push_back(vector<string>());
		bytes_read =  unserialize(ser_data, string_vec_vec.back(), total_bytes_read, vec_size);
		total_bytes_read += bytes_read;
	}
	return total_bytes_read;
}