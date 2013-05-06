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

class Mts0_io {
private:
	static const double bohr = 0.5291772;
	
	void read_data(ifstream *file, void *value);
	void write_data(ofstream *file, void *value, int &N);
	void read_mts(char *filename, vector<int> &atom_types_local, vector<int> &atom_ids_local, vector<vector<double> > &positions_local, vector<vector<double> > &velocities_local);
  void write_mts(char *filename, vector<int> &atom_types_local, vector<int> &atom_ids_local, vector<vector<double> > &positions_local, vector<vector<double> > &velocities_local);
  void is_all_atoms_inside_system();
  bool next_atom_id_is_known;
  bool atom_type_masses_is_known;
  bool did_remove_at_least_one_atom;
  int next_atom_id;
  vector<double> atom_type_masses;
  vector<vector<vector<double> > > h_matrix;

public:
	int nx, ny, nz;

  vector<vector<double> > positions;
  vector<vector<double> > velocities;
  vector<int> atom_ids;
  vector<int> atom_types;
  vector<bool> atom_is_removed;
  
  
  Mts0_io(int nx_, int ny_, int nz_);
  
  vector<vector<int> > create_neighbor_list(const double &neighbor_list_radius);
  int add_atom(int atom_type, vector<double> position, vector<double> velocity);
  void remove_atom(int atom_to_be_removed_index);
  void remove_atoms(vector<int> &atoms_to_be_removed_indices);
  void bring_removed_atoms_back(vector<int> &atoms_to_be_removed_indices);

  void rearrange_vectors_by_moving_atoms_from_end_to_locations_where_atoms_have_been_removed();
  
  void set_atom_type_masses(vector<double> atom_type_masses_);
  int get_number_of_atoms();
  vector<double> get_lx_ly_lz();
  double get_volume();
  vector<int> get_number_of_atoms_of_each_type();
  int get_next_atom_id();
  vector<double> get_momentum_of_atom(int atom_index);
  vector<double> get_momentum_of_atoms(vector<int> atom_indices);
  double get_mass_of_atom(int atom_index);
  double get_squared_distance_between_atoms(int atom_index_0, int atom_index_1);
  double get_min_distance_between_atom_types(int wanted_atom_type_0, int wanted_atom_type_1, double max_min_distance);

  
  void load_atoms(string mts0_directory);
  void save_atoms(string mts0_directory);
};
