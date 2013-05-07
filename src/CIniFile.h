#pragma once

//using namespace std;


#include <string>
#include <vector>
#include <CUtil.h>
#include <CVector.h>
#include <iostream>



class CItem {
 public:
  string name, strval;
  double dval;
};




class CIniFile  {
 public:
  string filename;

  vector<CItem> items;

  void load(string fname) {
    CUtil::verify_file(fname);
    
    fstream f(fname.c_str(), ios::in);
    if (!f)
      throw string("Could not load file "+filename);

    char s[2000];
    string er = "";
    while(!f.eof()) {
      f.getline(s,2000);
      vector<string> tok;
      CUtil::Tokenize(s, tok,"=");
      if (tok.size()==2) {
	CItem it;
	it.dval = -1;
	it.name = CUtil::trim(tok[0]);
	it.strval = CUtil::trim(tok[1]);
	try {
	  it.dval  = strtod(tok.at(1).c_str(),0);
	} catch(...) {
	}
	items.push_back(it);
      }
    }
    f.close();     
  }

  string getstring(string name) {
    for (unsigned int i=0;i<items.size();i++) {
      if (items[i].name==CUtil::trim(name))
	return items[i].strval;
    }
    throw string("Could not find any parameter '" + name +"'");
  }

  unsigned char getchar(string name) {
    for (unsigned int i=0;i<items.size();i++) {
      if (items[i].name==CUtil::trim(name))
	return items[i].strval[0];
    }
    throw string("Could not find any parameter '" + name +"'");
  }
  

  bool getbool(string name) {
    for (unsigned int i=0;i<items.size();i++) {
      if (items[i].name==CUtil::trim(name)) {
	if (items[i].strval=="true")
	  return true;
	return false;
      }
    }
    throw string("Could not find any parameter '" + name +"'");
  }
  

  double getdouble(string name) {
    for (unsigned int i=0;i<items.size();i++) {
      if (items[i].name==CUtil::trim(name))
	return items[i].dval;
    }
    throw string("Could not find any parameter '" + name +"'");
  }

  int getint(string name) {
    for (unsigned int i=0;i<items.size();i++) {
      if (items[i].name==CUtil::trim(name))
  return items[i].dval;
    }
    throw string("Could not find any parameter '" + name +"'");
  }

  CVector getvector(string name) {
    CVector res(0,0,0);
    for (unsigned int i=0;i<items.size();i++) {
      if (items[i].name==CUtil::trim(name)) {
	string s = items[i].strval;
	vector<string> tok;
	CUtil::Tokenize(s, tok,",");
	if (tok.size()!=3) throw string("Error reading vector parameter: " +name+ " (not a vector)");
	try {
	  res.x = strtod (tok[0].c_str(),0);
	  res.y = strtod (tok[1].c_str(),0);
	  res.z = strtod (tok[2].c_str(),0);
	  
	} catch (...) {
	  throw string("Error reading vector parameter: " + name);
	}
	return res;
	
      }
    }
    throw string("Could not find parameter '" + name +"'");
  }


   

  CIniFile() { }
  ~CIniFile() { }


};
