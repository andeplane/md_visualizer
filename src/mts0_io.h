/*
mts0_io.cpp mts0_io.h
Created by Jørgen Trømborg and Anders Hafreager on 15.04.13.
Copyright (c) 2013 Universitetet i Oslo. All rights reserved.

load_atoms(nx, ny, nz, velocities, positions, atom_type, h_matrix, mts0_directory)
save_atoms(nx, ny, nz, velocities, positions, atom_type, h_matrix, mts0_directory)
nx, ny, nz     - number of cpus [int].
velocities     - atom velocities [vector<vector<double> >], dimension num_atoms x 3, units unknown
positions      - atom positions [vector<vector<double> >], dimension num_atoms x 3, units Ångström
atom_types     - atom types [vector<int>], dimension num_atoms, {1-Si,2-A,3-H,4-O,5-Na,6-Cl,7-X}
atom_ids       - atom ids [vector<int>], dimension num_atoms
h_matrix       - h-matrix [vector<vector<vector<double> > >], dimension 2 x (3 x 3)
mts0_directory - mts0-directory [string]
*/

#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

class Timestep {
public:
  int nx, ny, nz;
  static const double bohr = 0.5291772;
  vector<vector<int> > create_neighbor_list(const double &neighbor_list_radius);
  vector<vector<double> > positions;
  vector<vector<double> > velocities;
  vector<int> atom_ids;
  vector<int> atom_types;
  vector<vector<vector<double> > > h_matrix;
  vector<double> get_lx_ly_lz();
  int get_number_of_atoms();

  Timestep(string filename, int nx_, int ny_, int nz_);
  ~Timestep();
  void load_atoms(string filename);
  void read_data(ifstream *file, void *value);
  void read_mts(char *filename, vector<int> &atom_types_local, vector<int> &atom_ids_local, vector<vector<double> > &positions_local, vector<vector<double> > &velocities_local);
};

class Mts0_io {
private:
  char mts0_directory[5000];
  vector<double> system_size;
  int current_timestep;
  int max_timestep;
  bool preload;
  vector<Timestep*> timesteps;
  string foldername_base;

public:
	int nx, ny, nz;
  Mts0_io(int nx_, int ny_, int nz_, int max_timestep_, string foldername_base_, bool preload_);
  Timestep *get_next_timestep();

  void load_timesteps();
};
