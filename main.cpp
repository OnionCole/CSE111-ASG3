// $Id: main.cpp,v 1.13 2021-02-01 18:58:18-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <regex>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

str_str_map global_map;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

void coutkvp(string k, string v) {
   cout << k << " = " << v << endl;
}

void run(string fn, istream& in_stream) {
   // Uses some code from the official misc/matchlines.cpp

   regex comment_regex{ R"(^\s*(#.*)?$)" };
   regex key_value_regex{ R"(^\s*(.*?)\s*=\s*(.*?)\s*$)" };
   regex trimmed_regex{ R"(^\s*([^=]+?)\s*$)" };

   int n = 0;
   while (true) {
      string line;
      getline(in_stream, line);
      if (in_stream.eof()) { break; }  // EOF
      n++;

      cout << fn << ": " << n << ": " << line << endl;

      smatch result;
      if (regex_search(line, result, comment_regex)) {} // 
            // comment or empty line
      else if (regex_search(line, result, key_value_regex)) { //
            // '=' char present; meaning we might have a key and we
            // might have a value
         string k = result[0].str();
         string v = result[1].str();
         
         if (k.empty()) {  // no key (read from listmap)
            if (v.empty()) {  // no key, no value
               for (auto iter = global_map.begin();
                     iter != global_map.end(); ++iter) {
                  coutkvp(iter->first, iter->second);
               }
            } else {  // no key, yes value
               for (auto iter = global_map.begin();
                     iter != global_map.end(); ++iter) {
                  if (iter->second.compare(v) == 0) {
                     coutkvp(iter->first, iter->second);
                  }
               }
            }
         } else {  // yes key (write to listmap)
            auto key_loc_iterator = global_map.find(k);
            bool key_exists = key_loc_iterator != global_map.end();
            if (v.empty()) {  // yes key, no value
               // delete kvp
               if (key_exists) {
                  global_map.erase(key_loc_iterator);
               }
            } else {  // yes key, yes value
               // set value
               auto temp = global_map.insert(str_str_pair(k, v));
               coutkvp(temp->first, temp->second);
            }
         }
      } else if (regex_search(line, result, trimmed_regex)) { //
            // no '=' char, meaning we have a string of a possible key
         string k = result[0].str();

         auto key_loc_iterator = global_map.find(k);
         bool key_exists = key_loc_iterator != global_map.end();
         if (key_exists) {
            coutkvp(key_loc_iterator->first, key_loc_iterator->second);
         } else {
            cout << k << ": key not found" << endl;
         }
      } else {
         cerr << "Err in (Main)Run: No regex matched" << endl;
      }
   }
}

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   if (argc <= 1) {
      run("-", cin);  // cin run
   } else {
      for (int i = 1; i < argc; ++i) {
         string fn = argv[i];
         if (fn == "-") {
            run("-", cin);  // cin run
         } else {
            ifstream ifs = ifstream(fn);
            if (ifs.fail()) {
               cerr << "Given filename does not exist" << endl;
               sys_info::exit_status(1);
            } else {
               run(fn, ifs);
               ifs.close();
            }

         }
      }
   }

   return sys_info::exit_status();
}
