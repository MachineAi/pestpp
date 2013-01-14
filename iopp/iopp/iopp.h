#ifndef IOPP_H
#define IOPP_H

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>
#include <regex>
#include <map>
#include <algorithm>

using namespace std;

string string2upper(string& str);
string string2lower(string& str);



class ValueOverflowError : public runtime_error {
public:
	ValueOverflowError(const string &_message="") : runtime_error(_message), message(_message) 
		{message = "value too large for precision model: " + _message;}
	virtual ~ValueOverflowError() throw () {}	
	void raise() {throw *this;}
protected:
	string message;
};


class FixedWidthError : public runtime_error {
public:
	FixedWidthError(const string &_message="") : runtime_error(_message), message(_message) 
		{message = "FixedWidthError "+_message;}	
	FixedWidthError(const string &_message, const int &_width) : runtime_error(_message), message(_message) 
		{message = "not enough space in template file marker to represent number "+_message;}
	virtual ~FixedWidthError() throw () {}	
	void raise() {throw *this;}
protected:
	string message;
};

class TemplateParameterError : public runtime_error {
public:
	TemplateParameterError(const string &_message="") : runtime_error(_message), message(_message) {};
	virtual ~TemplateParameterError() throw () {}	
	void raise() {throw *this;}
protected:
	string message;
};

class TemplateFileError : public runtime_error {
public:
	TemplateFileError(const string &_message="") : runtime_error(_message), message(_message)
		{message = "TemplateFileError "+_message;}	
	virtual ~TemplateFileError() throw () {}	
	void raise() {throw *this;}
protected:
	string message;
};


class InputFileError : public runtime_error {
public:
	InputFileError(const string &_message="") : runtime_error(_message), message(_message)
		{message = "InputFileError "+_message;}	
	virtual ~InputFileError() throw () {}	
	void raise() {throw *this;}
protected:
	string message;
};

class SecondaryMarkerNotFound : public runtime_error {
public:
	SecondaryMarkerNotFound(const string &_message="") : runtime_error(_message), message(_message)
	{message = "SeconaryMarkerNotFound: "+_message;}
	virtual ~SecondaryMarkerNotFound() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};

class SecondaryMarkerReadError : public runtime_error {
public:
	SecondaryMarkerReadError(const string &_message="") : runtime_error(_message), message(_message)
	{message = "SecondaryMarkerReadError: "+_message;}
	virtual ~SecondaryMarkerReadError() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};


class Text2NumParseError : public runtime_error {
public:
	Text2NumParseError(const string &_message="") : runtime_error(_message), message(_message)
	{message = "Text2NumParseError: "+_message;}
	virtual ~Text2NumParseError() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};

class ReadLineError : public runtime_error {
public:
	ReadLineError(const string &_message="") : runtime_error(_message), message(_message)
	{message = "ReadLineError: "+_message;}
	virtual ~ReadLineError() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};

class RewindError : public runtime_error {
public:
	RewindError(const string &_message="") : runtime_error(_message), message(_message)
	{message = "RewindError: problem rewinding file "+_message;}
	virtual ~RewindError() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};


class PrimaryMarkerEOFError : public runtime_error {
public:
	PrimaryMarkerEOFError(const string &_message="") : runtime_error(_message), message(_message)
	{message = "PrimaryMarkerEOFError: end of file found with searching for primary marker "+_message;}
	virtual ~PrimaryMarkerEOFError() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};

class MarkerError : public runtime_error {
public:
	MarkerError(const string &_message="") : runtime_error(_message), message(_message)
	{message = "MarkerError: "+_message;}
	virtual ~MarkerError() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};

class FixedObsReadError : public runtime_error {
public:
	FixedObsReadError(const string &_message="") : runtime_error(_message), message(_message)
	{message = "FixedObsReadError: error reading observation "+_message;}
	virtual ~FixedObsReadError() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};


class SemiFixedObsReadError : public runtime_error {
public:
	SemiFixedObsReadError(const string &_message="") : runtime_error(_message), message(_message)
	{message = "SemiFixedObsReadError: error reading observation "+_message;}
	virtual ~SemiFixedObsReadError() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};

class NonFixedObsReadError : public runtime_error {
public:
	NonFixedObsReadError(const string &_message="") : runtime_error(_message), message(_message)
	{message = "NonFixedObsReadError: error reading observation "+_message;}
	virtual ~NonFixedObsReadError() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};

class NonFixedObsParseError : public runtime_error {
public:
	NonFixedObsParseError(const string &_message="") : runtime_error(_message), message(_message)
	{message = "NonFixedObsParseError: "+_message;}
	virtual ~NonFixedObsParseError() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};


class WhitespaceExecutionError : public runtime_error {
public:
	WhitespaceExecutionError(const string &_message="") : runtime_error(_message), message(_message)
	{message = "WhitespaceExecutionError: "+_message;}
	virtual ~WhitespaceExecutionError() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};


class InstructionLineError : public runtime_error {
public:
	InstructionLineError(const string &_message="") : runtime_error(_message), message(_message)
	{message = "InstructionLineError: "+_message;}
	virtual ~InstructionLineError() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};

class InstructionFileError : public runtime_error {
public:
	InstructionFileError(const string &_message="") : runtime_error(_message), message(_message)
	{message = "InstructionFileError: "+_message;}
	virtual ~InstructionFileError() throw () {}
	void raise() {throw *this;}
protected:
	string message;
};






//-------------------------------------------------------------------
//Instruction Classes
//-------------------------------------------------------------------


//lowest level ins class - commands to execute
class Instruction{
public:
	enum instruction_type{fixedObs,nonFixedObs,fixedLine,fixedTab,marker,whitespace};
	enum fixed_type {strict,flexible};
	enum read_type {line,tab};
	enum marker_type {primary,secondary};
	instruction_type itype;
	read_type rtype;
	fixed_type ftype;
	marker_type mtype;
	Instruction(){ins_string="defaul",marker_delim='a';}
	Instruction(string i_string,char m_delim);
	void execute(ifstream* out,int* lpos);
	double read(ifstream* out,int* lpos);
	void parse();
	int get_cursor(){return cursor_pos;}
	string get_name(){return obs_name;}
protected:
	string ins_string;
	string marker_string;
	string obs_name;
	char marker_delim;
	int cursor_pos;
	int number;
	int start;
	int end;
	streampos start_point;


	void parse_marker();
	void execute_marker(ifstream* out,int* lpos);

	void parse_fixedObs();
	double read_fixedObs(ifstream* out,int* lpos);

	void parse_nonFixedObs();
	double read_nonFixedObs(ifstream* out,int* lpos);

	void parse_fixed();
	void execute_fixed(ifstream* out,int* lpos);

	void parse_whitespace();
	void execute_whitespace(ifstream* out, int* lpos);
};


class InstructionLine{
public:
	InstructionLine(vector<Instruction> instructs);
	unordered_map<string,double> execute(ifstream* out);
	vector<string> get_observation_names();
private:
	vector<Instruction> instructions;
	vector<bool> satisfied;
	int lpos;
	bool markerFirst;
};

class InstructionFile{
public:
	InstructionFile();
	InstructionFile(string ins_filename);
	unordered_map<string,double> read(ifstream* out);
	vector<pair<int,int>> get_marker_indices(string line);
	vector<string> get_observation_names();
	void set_filename(string ins_file){instruction_filename=ins_file;}
private:
	string instruction_filename;
	vector<InstructionLine> instruction_lines;
	ifstream ins;
	char marker_delim;
	void build_instruction_set();
	void parse_instruction_line(string ins_line);
	string get_instruction_line();
	
};


class InstructionFiles{
public:
	InstructionFiles(vector<string> ins_files,vector<string>out_files,vector<string>onames);
	vector<double> readins();
	void check_obs_names(vector<string> onames);
private:
	vector<string> instruction_filenames;
	vector<string> output_filenames;
	vector<InstructionFile> instruction_files;
	vector<string> obs_names;
	vector<ifstream> out_streams;
};


//-------------------------------------------------------------------
//Template Classes
//-------------------------------------------------------------------
class FixedWidthValue{
public:
	FixedWidthValue(const bool isdDblPres,const bool forceRadix,const int wdth);
	string get_templatefile_string(const double val);
private:
	bool isDoublePrecision;	
	bool needSign;
	bool needEsign;
	bool needScientific;	
	bool needRadix;
	bool forceRadix;
	bool isRace;

	int max_exponent;
	int max_sig;
	int max_width;
	int text_width;
	int width;
	int exponent;
	int exp_digits;	
	double value;
	char base;
	char sign;
	char eSign;
	string max_str;
	vector<int>::size_type radix_pos;
	vector<int>::size_type sig_digits;
    vector<int> significand;
	vector<int> output_significand;
	
	double as_double();
	string as_string();
	
	void set_precision_components();	
	void shift_left();
	void shift_right();
	void prepare_output_significand();
	void update();
	void reduce_exponent();
	void drop_scientific();
	void drop_radix();
	void maximize_precision();

	long long digits_to_number(vector<int> iv);
	vector<int> pad_zeros(vector<int> vi,bool onLeft);
	vector<int> number_to_digits(long long val);
};



class TemplateParameter{
public:
	TemplateParameter() {name="",value=-1.0e+10,start_idx=-1,end_idx=-1,isDoublePrecision=true,forceRadix=false;}
	TemplateParameter(const string nm, const double val, const int start,const int end, const int lnum,const bool isDbl,const bool frcRad);
	bool operator==(const TemplateParameter &other) const;
	
	void set_value(const double val) {value = val;}
	void write_value(string &line);
	string get_name() {return name;}
private:
	bool isDoublePrecision;
	bool forceRadix;
	int start_idx;
	int end_idx;
	int line_num;
	double value;
	string name;
		
	string value_as_string();
	string value_as_string_pest();
	string build_parameter_format();
};


class TemplateFile{
public:
	TemplateFile() {template_filename = "", input_filename = "",isDouble=true,forceRadix=false,warn_width=5;}
	TemplateFile( const string tpl_filename, const string ipt_filename,const bool isDbl,const bool frcRad){forceRadix = frcRad, isDouble = isDbl, template_filename = tpl_filename, input_filename = ipt_filename;}
	void set_parameter_values(const map<string,double> parameter_map);	
	void write_inputfile();
	//bool check_templatefile();
	void read_templatefile();
	vector<string> get_parameter_names();
private:
	bool isDouble;
	bool forceRadix;
	int warn_width;
	char marker;
	string template_filetype;
	string template_filename;
	string input_filename;	
	map<int,vector<TemplateParameter>> parameter_line_index;
	vector<int> line_numbers;
	vector<string> warnings;
	vector<string> errors;
	
	vector<pair<int,int>> get_marker_indices(char marker,string line);
	void reduce_parameter_name(string &pname);
	vector<string> get_line_parameters(vector<pair<int,int>> indices,string line);
	string build_input_line(string line,vector<TemplateParameter> line_parameters);
	vector<double> get_line_values(vector<string> line_parameters);
};

class TemplateFiles{
public:	
	TemplateFiles(bool isDouble, bool forceRadix, vector<string> tpl_filenames,vector<string> ipt_filenames,vector<string> pnames);	
	int writtpl(vector<double> parameter_values);
	void check_parameter_names();
private:
	vector<string> parameter_names;	
	vector<string> template_filenames;
	vector<string> input_filenames;	   
	vector<TemplateFile> template_files;	
};

#endif
