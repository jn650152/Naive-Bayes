#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "target_statistics.h"
#include <math.h>
using namespace std;
int read_file(fstream &file, vector<string> &attribute_names,int indicator);
int print_attribute(vector<string> attribute_names);
void print_database();
map<string, statistics> get_target_probability(int target_num);
void print_map(map<string, statistics> target_statistics);
void get_conditional_probability(map<string,vector<map<string, double>>> &conditional_probability, map<string, statistics> target_statistics, int total_attribute_num);
void print_conditional_probability(map<string, vector<map<string, double>>> conditional_probability, vector<string> attribute_names);
void predict(map<string, statistics> target_statistics, fstream &output, map<string,vector<map<string, double>>> conditional_probability, int target_num,vector<string> attribute_names);
bool get_probability(map<string, statistics> target_statistics,double &sum, string target_value,vector<vector<string>>::iterator record, map<string,vector<map<string, double>>> conditional_probability, int target_num, vector<string> attribute_names);
string get_final_target_value(map<string, double> probabilities);
void write_to_file(vector<vector<string>>::iterator record,string result,fstream &output);
void write_title(fstream &output, vector<string> attribute_names);
#endif 
