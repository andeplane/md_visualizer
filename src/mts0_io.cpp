#include <mts0_io.h>
#include <math.h>

Mts0_io::Mts0_io(int nx_, int ny_, int nz_) {
	nx = nx_;
	ny = ny_;
	nz = nz_;
	next_atom_id_is_known = false;
	did_remove_at_least_one_atom = false;
	atom_type_masses_is_known = false;
}

void Mts0_io::read_data(ifstream *file, void *value) {
	int N;
	file->read (reinterpret_cast<char*>(&N), sizeof(int));
	file->read (reinterpret_cast<char*>(value), N);
	file->read (reinterpret_cast<char*>(&N), sizeof(int));
}

void Mts0_io::write_data(ofstream *file, void *value, int &N) {
	file->write(reinterpret_cast<char*>(&N), sizeof(int));
	file->write(reinterpret_cast<char*>(value), N);
	file->write(reinterpret_cast<char*>(&N), sizeof(int));
}

void Mts0_io::read_mts(char *filename, vector<int> &atom_types_local, vector<int> &atom_ids_local, vector<vector<double> > &positions_local, vector<vector<double> > &velocities_local) {
	ifstream *file = new ifstream();
	file->open(filename, ios::in | ios::binary);
	if (!*file) {
		cout << "Error in Mts0_io::read_mts(): Failed to open file " << filename << endl;
		exit(1);
	}
	int num_atoms_local;
	read_data(file, &num_atoms_local);
	
	double *phase_space = new double[6*num_atoms_local];
	double *tmp_atom_data = new double[num_atoms_local];

	atom_types_local.resize(num_atoms_local);
	atom_ids_local.resize(num_atoms_local);
	positions_local.resize(num_atoms_local);
	velocities_local.resize(num_atoms_local);
	for(int i=0;i<num_atoms_local;i++) {
		positions_local[i].resize(3);
		velocities_local[i].resize(3);
	}

	read_data(file, tmp_atom_data);
	read_data(file, phase_space);

	for(int i=0;i<num_atoms_local;i++) {
		atom_types_local[i] = int(tmp_atom_data[i]);
		// Handle roundoff errors from 2 -> 1.99999999 -> 1
		atom_ids_local[i] = (tmp_atom_data[i]-atom_types_local[i])*1e11 + 1e-5;
		
		positions_local [i][0] = phase_space[3*i+0];
		positions_local [i][1] = phase_space[3*i+1];
		positions_local [i][2] = phase_space[3*i+2];
		velocities_local[i][0] = phase_space[3*num_atoms_local + 3*i+0];
		velocities_local[i][1] = phase_space[3*num_atoms_local + 3*i+1];
		velocities_local[i][2] = phase_space[3*num_atoms_local + 3*i+2];
	}

	double *tmp_h_matrix = new double[18];
	read_data(file,tmp_h_matrix);
	int count = 0;
	for(int k=0;k<2;k++) {
		for(int j=0;j<3;j++) {
			for(int i=0;i<3;i++) {
				h_matrix[k][i][j] = tmp_h_matrix[count++];
			}
		}
	}

	file->close();

	delete tmp_h_matrix;
	delete phase_space;
	delete tmp_atom_data;
	delete file;
}

void Mts0_io::write_mts(char *filename, vector<int> &atom_types_local, vector<int> &atom_ids_local, vector<vector<double> > &positions_local, vector<vector<double> > &velocities_local) {
	ofstream *file = new ofstream();
	file->open(filename, ios::out | ios::binary);
	if(!file->is_open())
	{
		cout << "Could not open " << filename << " for saving. Aborting!" << endl;
		exit(0);
	}
	int size_of_int = sizeof(int);
	int num_atoms_local = positions_local.size();

	write_data(file,&num_atoms_local,size_of_int);

	double *phase_space = new double[6*num_atoms_local];
	double *tmp_atom_data = new double[num_atoms_local];
	for(int i=0;i<num_atoms_local;i++) {
		tmp_atom_data[i] = atom_ids_local[i]*1e-11 + atom_types_local[i];

		phase_space[3*i+0] 			 = positions_local [i][0];
		phase_space[3*i+1] 			 = positions_local [i][1];
		phase_space[3*i+2] 			 = positions_local [i][2];
		phase_space[3*num_atoms_local + 3*i+0] = velocities_local[i][0];
		phase_space[3*num_atoms_local + 3*i+1] = velocities_local[i][1];
		phase_space[3*num_atoms_local + 3*i+2] = velocities_local[i][2];
	}

	int size_of_tmp_atom_data = num_atoms_local*sizeof(double);
	int size_of_phase_space = 6*num_atoms_local*sizeof(double);
	write_data(file, tmp_atom_data, size_of_tmp_atom_data);
	write_data(file, phase_space, size_of_phase_space);

	double *tmp_h_matrix = new double[18];

	int count = 0;
	for(int k=0;k<2;k++) {
		for(int j=0;j<3;j++) {
			for(int i=0;i<3;i++) {
				tmp_h_matrix[count++] = h_matrix[k][i][j];
			}
		}
	}

	int size_of_tmp_h_matrix = 18*sizeof(double);
	write_data(file,tmp_h_matrix, size_of_tmp_h_matrix);

	file->close();

	delete tmp_h_matrix;
	delete phase_space;
	delete tmp_atom_data;
	delete file;
}

void Mts0_io::load_atoms(string mts0_directory) {
	positions.clear();
	velocities.clear();
	atom_types.clear();
	atom_ids.clear();
	h_matrix.clear();

	h_matrix.resize(2);
	for(int i=0;i<2;i++) {
		h_matrix[i].resize(3);
		for(int j=0;j<3;j++) {
			h_matrix[i][j].resize(3);
		}
	}
	vector<vector<double> > positions_local;
	vector<vector<double> > velocities_local;
	vector<int> atom_types_local;
	vector<int> atom_ids_local;

	int num_nodes = nx*ny*nz;
	int num_atoms_local;
	vector<int> node_vector_index(3,0);
	vector<double> node_origin(3,0);
	vector<double> node_offset(3,0);

	char *filename = new char[1000];

	node_offset[0] = 1.0/nx;
	node_offset[1] = 1.0/ny;
	node_offset[2] = 1.0/nz;

	for(int node_id=0; node_id<num_nodes; node_id++) {
		node_vector_index[0] = node_id/(ny*nz);   // Node id in x-direction
		node_vector_index[1] = (node_id/nz) % ny; // Node id in y-direction
		node_vector_index[2] = node_id % nz; 	  // Node id in z-direction

		node_origin[0] = node_offset[0]*node_vector_index[0]; // Displacement in x-direction
		node_origin[1] = node_offset[1]*node_vector_index[1]; // Displacement in y-direction
		node_origin[2] = node_offset[2]*node_vector_index[2]; // Displacement in z-direction

		sprintf(filename,"%s/mt%04d",mts0_directory.c_str(), node_id);
		read_mts(filename, atom_types_local, atom_ids_local, positions_local, velocities_local);
		int num_atoms_local = positions_local.size();

		for(int j=0;j<num_atoms_local;j++) {
			for(int k=0;k<3;k++) {
				positions_local[j][k] += node_origin[k];
				positions_local[j][k] *= h_matrix[0][k][k]*bohr; // Possibly broken, we don't know how h_matrix really works
			}
		}

		positions.insert ( positions.end() , positions_local.begin() , positions_local.end()  );
		velocities.insert( velocities.end(), velocities_local.begin(), velocities_local.end() );
		atom_types.insert ( atom_types.end() , atom_types_local.begin() , atom_types_local.end()  );
		atom_ids.insert ( atom_ids.end() , atom_ids_local.begin() , atom_ids_local.end()  );
	}

	atom_is_removed.resize( get_number_of_atoms() ,false);
	did_remove_at_least_one_atom = false;

	is_all_atoms_inside_system();

	delete filename;
	atom_types_local.clear();
	atom_ids_local.clear();
	positions_local.clear();
	velocities_local.clear();

	node_vector_index.clear();
	node_origin.clear();
	node_offset.clear();
}

void Mts0_io::is_all_atoms_inside_system() {
	vector<double> system_size = get_lx_ly_lz();

	for(int n=0;n<get_number_of_atoms(); n++) {
		for(int i=0;i<3;i++) {
			if(positions[n][i] >= system_size[i]) {
				cout << "Error in Mts0_io::is_all_atoms_inside_system(): Atom " << atom_ids[n] << " at " << positions[n][0] << " " << positions[n][1] << " " << positions[n][2] << " is outside the system of size " << system_size[0] << " " << system_size[1] << " " << system_size[2] << ".\n";
				cout << "Check nx, ny, nz." << endl;
				exit(1);
			}
		}
	}
}

void Mts0_io::save_atoms(string mts0_directory) {
	if(did_remove_at_least_one_atom) rearrange_vectors_by_moving_atoms_from_end_to_locations_where_atoms_have_been_removed();

	int num_nodes = nx*ny*nz;
	int num_atoms = positions.size();
	vector<vector<vector<double> > > positions_in_nodes;
	vector<vector<vector<double> > > velocities_in_nodes;
	vector<vector<int> > atom_types_in_nodes;
	vector<vector<int> > atom_ids_in_nodes;
	positions_in_nodes.resize(num_nodes);
	velocities_in_nodes.resize(num_nodes);
	atom_types_in_nodes.resize(num_nodes);
	atom_ids_in_nodes.resize(num_nodes);

	vector<int> node_vector_index(3,0);
	for(int atom_index=0; atom_index<num_atoms; atom_index++) {
		double x = positions[atom_index][0]/(h_matrix[0][0][0]*bohr);
		double y = positions[atom_index][1]/(h_matrix[0][1][1]*bohr);
		double z = positions[atom_index][2]/(h_matrix[0][2][2]*bohr);

		node_vector_index[0] = x*nx;
		node_vector_index[1] = y*ny;
		node_vector_index[2] = z*nz;
		if(node_vector_index[0] >= nx || node_vector_index[1] >= ny || node_vector_index[2] >= nz) {
			cout << "\n\n\nError in save_atoms(). Please check that the specified node configuration (nx,ny,nz) is correct. Aborting! \n" << endl;
			exit(0);
		}

		int node_id = node_vector_index[0]*ny*nz + node_vector_index[1]*nz + node_vector_index[2];

		positions_in_nodes[node_id].push_back(positions[atom_index]);
		velocities_in_nodes[node_id].push_back(velocities[atom_index]);
		atom_types_in_nodes[node_id].push_back(atom_types[atom_index]);
		atom_ids_in_nodes[node_id].push_back(atom_ids[atom_index]);
	}

	vector<double> node_origin(3,0);
	vector<double> node_offset(3,0);
	char *filename = new char[1000];
	node_offset[0] = 1.0/nx;
	node_offset[1] = 1.0/ny;
	node_offset[2] = 1.0/nz;


	for(int node_id=0; node_id<num_nodes; node_id++) {
		node_vector_index[0] = node_id/(ny*nz);   // Node id in x-direction
		node_vector_index[1] = (node_id/nz) % ny; // Node id in y-direction
		node_vector_index[2] = node_id % nz; 	  // Node id in z-direction
		int num_atoms_local = atom_types_in_nodes[node_id].size();

		node_origin[0] = node_offset[0]*node_vector_index[0]; // Displacement in x-direction
		node_origin[1] = node_offset[1]*node_vector_index[1]; // Displacement in y-direction
		node_origin[2] = node_offset[2]*node_vector_index[2]; // Displacement in z-direction

		for(int j=0;j<num_atoms_local;j++) {
			for(int k=0;k<3;k++) {
				positions_in_nodes[node_id][j][k] /= h_matrix[0][k][k]*bohr;
				positions_in_nodes[node_id][j][k] -= node_origin[k];
			}
		}

		sprintf(filename,"%s/mt%04d",mts0_directory.c_str(), node_id);
		write_mts(filename, atom_types_in_nodes[node_id], atom_ids_in_nodes[node_id], positions_in_nodes[node_id], velocities_in_nodes[node_id]);
	}

	delete filename;
	atom_types_in_nodes.clear();
	atom_ids_in_nodes.clear();
	positions_in_nodes.clear();
	velocities_in_nodes.clear();
	node_vector_index.clear();
	node_origin.clear();
	node_offset.clear();
}

vector<vector<int> > Mts0_io::create_neighbor_list(const double &neighbor_list_radius) {
	/*
	Strategy: Divide system into boxes of size neighbor_list_radius/3. Assign all atoms into their boxes O(n).
		      Loop through all boxes. Every atom is the neighbor of every other atom in this and the 26 neighboring boxes.
	*/
	double neighbor_list_radius_squared = neighbor_list_radius*neighbor_list_radius;
	vector<vector<int> > neighbor_list;
	int num_atoms = get_number_of_atoms();

	neighbor_list.resize(num_atoms);
	double box_size[3];
	int box_id[3];
	int num_boxes_xyz[3];
	vector<double> length = get_lx_ly_lz();
	for(int i=0;i<3;i++) {
		box_size[i] = neighbor_list_radius;
		num_boxes_xyz[i] = length[i]/box_size[i];
		box_size[i] = length[i]/num_boxes_xyz[i];
	}

	int num_boxes = num_boxes_xyz[0]*num_boxes_xyz[1]*num_boxes_xyz[2];
	
	vector<vector<vector<vector<int> > > > boxes;

	boxes.resize(num_boxes_xyz[0]);
	for(int i=0;i<num_boxes_xyz[0];i++) {
		boxes[i].resize(num_boxes_xyz[1]);
		for(int j=0;j<num_boxes_xyz[1];j++) {
			boxes[i][j].resize(num_boxes_xyz[2]);
		}
	}

	// Assign atoms to boxes
	for(int atom_index=0; atom_index<num_atoms; atom_index++) {
		// +1e-5 makes sure that box_id does not reach num_boxes when positions[][i]==length[i]
		for(int i=0;i<3;i++) box_id[i] = int(positions[atom_index][i]/(length[i]+1e-5)*num_boxes_xyz[i]);
		vector<int> &box = boxes[ box_id[0] ][ box_id[1] ][ box_id[2] ];
		box.push_back(atom_index);
	}

	// Loop over all boxes
	for(int i=0;i<num_boxes_xyz[0];i++) {
		for(int j=0;j<num_boxes_xyz[1];j++) {
			for(int k=0;k<num_boxes_xyz[2];k++) {
				vector<int> &box = boxes[i][j][k];
				int num_atoms_in_this_box = box.size();

				for(int n=0;n<num_atoms_in_this_box;n++) {
					// This box
					for(int m=n+1;m<num_atoms_in_this_box;m++) {
						neighbor_list[ box[n] ].push_back(box[m]);
						neighbor_list[ box[m] ].push_back(box[n]);
					}

					// Neighboring boxes (periodic boundaries)
					for(int di=-1;di<=1;di++) {
						for(int dj=-1;dj<=1;dj++) {
							for(int dk=-1;dk<=1;dk++) {
								if(di==0 && dj==0 && dk==0) continue; // Skip self
								int neighbor_box_i = (i + di + num_boxes_xyz[0]) % num_boxes_xyz[0];
								int neighbor_box_j = (j + dj + num_boxes_xyz[1]) % num_boxes_xyz[1];
								int neighbor_box_k = (k + dk + num_boxes_xyz[2]) % num_boxes_xyz[2];
								vector<int> &neighbor_box = boxes[neighbor_box_i][neighbor_box_j][neighbor_box_k];
								int num_atoms_in_neighbor_box = neighbor_box.size();

								for(int m=0;m<num_atoms_in_neighbor_box;m++) {
									neighbor_list[ box[n] ].push_back(neighbor_box[m]);
								}
							}
						}
					}
				}
			}
		}
	}
	length.clear();

	return neighbor_list;
}

vector<int> Mts0_io::get_number_of_atoms_of_each_type() {
	int num_atoms = atom_types.size();
	vector<int> number_of_atoms_of_each_type;
	number_of_atoms_of_each_type.resize(100,0);
	for(int atom_index=0; atom_index<num_atoms; atom_index++) {
		number_of_atoms_of_each_type[ atom_types[atom_index] ]++;
	}

	return number_of_atoms_of_each_type;
}

int Mts0_io::add_atom(int atom_type, vector<double> position, vector<double> velocity) {
	atom_types.push_back(atom_type);
	atom_ids.push_back(get_next_atom_id());
	positions.push_back(position);
	velocities.push_back(velocity);
	atom_is_removed.push_back(false);
	int atom_index = get_number_of_atoms()-1;

	return atom_index;
}

void Mts0_io::remove_atom(int atom_to_be_removed_index) {
	did_remove_at_least_one_atom = true;
	atom_is_removed[atom_to_be_removed_index] = true;
}

void Mts0_io::remove_atoms(vector<int> &atoms_to_be_removed_indices) {
	for(int i=0;i<atoms_to_be_removed_indices.size(); i++) {
		remove_atom(atoms_to_be_removed_indices[i]);
	}
}

void Mts0_io::bring_removed_atoms_back(vector<int> &atoms_to_be_removed_indices) {
	for(int i=0;i<atoms_to_be_removed_indices.size(); i++) {
		atom_is_removed[i] = false;
	}
}

int Mts0_io::get_next_atom_id() {
	if(!next_atom_id_is_known) {
		next_atom_id = -1;
		int num_atoms = atom_ids.size();
		for(int n=0; n<num_atoms; n++) {
			if(atom_ids[n]>next_atom_id) next_atom_id = atom_ids[n];
		}

		next_atom_id_is_known = true;
	}
	
	return ++next_atom_id;
}

int Mts0_io::get_number_of_atoms() {
	return positions.size();
}

vector<double> Mts0_io::get_lx_ly_lz() {
	vector<double> system_size(3);
	for(int i=0;i<3;i++) {
		system_size[i] = h_matrix[0][i][i]*bohr;
	}

	return system_size;
}

double Mts0_io::get_volume() {
	vector<double> system_size = get_lx_ly_lz();
	return system_size[0]*system_size[1]*system_size[2];
}

void Mts0_io::rearrange_vectors_by_moving_atoms_from_end_to_locations_where_atoms_have_been_removed() {
	int num_atoms_including_removed_atoms = get_number_of_atoms();

	int remaining_atoms = 0;

	for(int n=0; n<num_atoms_including_removed_atoms; n++) {
		if(!atom_is_removed[n]) {
			atom_types[remaining_atoms] = atom_types[n];
			atom_ids[remaining_atoms] = atom_ids[n];
			positions[remaining_atoms] = positions[n];
			velocities[remaining_atoms] = velocities[n];
			atom_is_removed[remaining_atoms] = false;
			remaining_atoms++;
		}
	}

	atom_types.erase(atom_types.begin()+remaining_atoms, atom_types.end());
	atom_ids.erase  (atom_ids.begin()  +remaining_atoms, atom_ids.end());
	positions.erase (positions.begin() +remaining_atoms, positions.end());
	velocities.erase(velocities.begin()+remaining_atoms, velocities.end());
	atom_is_removed.erase(atom_is_removed.begin()+remaining_atoms, atom_is_removed.end());
	did_remove_at_least_one_atom = false;
}

void Mts0_io::set_atom_type_masses(vector<double> atom_type_masses_) {
	atom_type_masses = atom_type_masses_;
	atom_type_masses_is_known = true;
	cout << "Setting atom_type_masses: \n";
	for(int n=0; n<atom_type_masses.size(); n++) {
		cout << "\t" << n << ": " << atom_type_masses[n] << " amu." << endl;
	}

}

vector<double> Mts0_io::get_momentum_of_atom(int atom_index) {
	if(!atom_type_masses_is_known) {
		cout << "Error in Mts0_io::get_momentum_of_atom(): atom type masses are not set." << endl;
		exit(1);
	}

	if(atom_index >= atom_types.size()) {
		cout << "Error in Mts0_io::get_momentum_of_atom(): atom_index " << atom_index << "exceeds atom_types.size() " << atom_types.size() << endl;
		exit(1);
	}

	int atom_type = atom_types[atom_index];
	double mass = atom_type_masses[atom_type];
	vector<double> momentum(3,0);
	for(int i=0;i<3;i++) momentum[i] = mass*velocities[atom_index][i];

	return momentum;
}

vector<double> Mts0_io::get_momentum_of_atoms(vector<int> atom_indices) {
	vector<double> momentum(3,0);
	for(int n=0;n<atom_indices.size();n++) {
		int atom_index = atom_indices[n];
		vector<double> momentum_of_this_atom = get_momentum_of_atom(atom_index);
		for(int i=0;i<3;i++) momentum[i] += momentum_of_this_atom[i];
	}

	return momentum;
}

double Mts0_io::get_mass_of_atom(int atom_index) {
	if(!atom_type_masses_is_known) {
		cout << "Error in Mts0_io::get_mass_of_atom(): atom type masses are not set." << endl;
		exit(1);
	}

	if(atom_index >= atom_types.size()) {
		cout << "Error in Mts0_io::get_mass_of_atom(): atom_index " << atom_index << "exceeds atom_types.size() " << atom_types.size() << endl;
		exit(1);
	}

	int atom_type = atom_types[atom_index];
	if(atom_type>= atom_type_masses.size()) {
		cout << "Error in Mts0_io::get_mass_of_atom(): atom_type " << atom_type << "exceeds atom_type_masses.size() " << atom_type_masses.size() << endl;
		exit(1);
	}

	return atom_type_masses[atom_type];
}

double Mts0_io::get_squared_distance_between_atoms(int atom_index_0, int atom_index_1) {
	vector<double> system_size = get_lx_ly_lz();
	double Lx = system_size[0];
	double Ly = system_size[1];
	double Lz = system_size[2];

	double dx = positions[atom_index_0][0] - positions[atom_index_1][0];
	double dy = positions[atom_index_0][1] - positions[atom_index_1][1];
	double dz = positions[atom_index_0][2] - positions[atom_index_1][2];
	if(dx>Lx/2) dx -= Lx; if(dx< -Lx/2) dx += Lx;
	if(dy>Ly/2) dy -= Ly; if(dy< -Ly/2) dy += Ly;
	if(dz>Lz/2) dz -= Lz; if(dz< -Lz/2) dz += Lz;

	return dx*dx + dy*dy + dz*dz;
}

double Mts0_io::get_min_distance_between_atom_types(int wanted_atom_type_0, int wanted_atom_type_1, double max_min_distance) {
	double min_squared_distance = INFINITY;
	vector<vector<int> > neighbor_list = create_neighbor_list(max_min_distance);
	for(int atom_index_0 = 0; atom_index_0 < get_number_of_atoms(); atom_index_0++) {
		int atom_type_0 = atom_types[atom_index_0];
		if(wanted_atom_type_0 != atom_type_0) continue;

		for(int neighbor_index = 0; neighbor_index < neighbor_list[atom_index_0].size(); neighbor_index++) {
			int atom_index_1 = neighbor_list[atom_index_0][neighbor_index];
			int atom_type_1 = atom_types[atom_index_1];
			if(wanted_atom_type_1 != atom_type_1) continue;
			double dr2 = get_squared_distance_between_atoms(atom_index_0, atom_index_1);
			if(dr2 < min_squared_distance) {
				min_squared_distance = dr2;
			}
		}
	}

	neighbor_list.clear();

	return sqrt(min_squared_distance);
}