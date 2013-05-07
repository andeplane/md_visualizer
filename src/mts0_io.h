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

#define SI_TYPE 1
#define A_TYPE 2
#define H_TYPE 3
#define O_TYPE 4
#define NA_TYPE 5
#define CL_TYPE 6
#define X_TYPE 7

class Timestep {
public:
  int nx, ny, nz;
  static const double bohr = 0.5291772;
  vector<vector<int> > create_neighbor_list(const float &neighbor_list_radius);
  vector<vector<float> > positions;
  vector<int> atom_ids;
  vector<int> atom_types;
  vector<bool> atom_is_removed;
  vector<vector<vector<float> > > h_matrix;
  vector<float> get_lx_ly_lz();
  int get_number_of_atoms();
  vector<int> get_number_of_atoms_of_each_type();
  void rearrange_vectors_by_moving_atoms_from_end_to_locations_where_atoms_have_been_removed();

  Timestep(string filename, int nx_, int ny_, int nz_);
  ~Timestep();
  void remove_water();
  void load_atoms(string filename);
  void read_data(ifstream *file, void *value);
  void read_mts(char *filename, vector<int> &atom_types_local, vector<int> &atom_ids_local, vector<vector<float> > &positions_local);
};

class Mts0_io {
private:
  char mts0_directory[5000];
  int current_timestep;
  int max_timestep;
  bool preload;
  vector<Timestep*> timesteps;
  string foldername_base;

public:
  vector<float> system_size;
	int nx, ny, nz;
  Mts0_io(int nx_, int ny_, int nz_, int max_timestep_, string foldername_base_, bool preload_);
  Timestep *get_next_timestep();

  void load_timesteps();
};
